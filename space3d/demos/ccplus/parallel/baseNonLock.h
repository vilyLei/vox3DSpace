#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <condition_variable>
#include <future>
#include <queue>
#include <vector>

namespace parallel::baseNonLock
{
namespace baseNonLock_02
{
static int qi_uuid = 0;
struct QInfo
{
    int index;
    int total;
    int uuid;
    //QInfo(int i, int t) :
    //    index(i), total(t), uuid(qi_uuid++)
    //{
    //    std::cout << " \tQInfo construct() uuid: " << uuid << ".\n";
    //}
    //~QInfo()
    //{
    //    std::cout << " \tQInfo destruct() uuid: " << uuid << ".\n";
    //}
};
template <typename T>
class LockFreeStack
{
public:
    void push(T const& data)
    {
        node* const new_node = new node(data);
        new_node->next       = head.load();
        while (!head.compare_exchange_weak(new_node->next, new_node))
        {
            std::this_thread::yield();
        };
    }
    std::shared_ptr<T> pop(int flag)
    {
        ++threadsTotalInPop;

        node* old_head = head.load();
        int         blockCycles = 0;
        std::string info         = " \t *** pop(),threadsTotalInPop: " + std::to_string(threadsTotalInPop) + ", flag: " + std::to_string(flag) + ", old_head!=nullptr: " + std::to_string((old_head!=nullptr)) + " ...\n ";
        std::cout << info;
        while (old_head &&
               !head.compare_exchange_weak(old_head, old_head->next))
        {
            ++blockCycles;
            std::this_thread::yield();
        };

        std::string info1 = " \t *** pop(),call on, blockCycles: " + std::to_string(blockCycles) + ", flag: " + std::to_string(flag) + ", old_head!=nullptr: " + std::to_string((old_head != nullptr)) + " ...\n ";
        std::cout << info1;

        std::shared_ptr<T> res;
        if (old_head)
        {
            res.swap(old_head->data);
        }
        try_reclaim(old_head, flag);
        return res;
    }

private:
    struct node
    {
        std::shared_ptr<T> data;
        node*              next;
        node(T const& data_) :
            data(std::make_shared<T>(data_)), next(nullptr)
        {}
    };
    std::atomic<unsigned> threadsTotalInPop;
    std::atomic<node*>    to_be_deleted;
    std::atomic<node*>    head;
    static void delete_nodes(node* nodes, int flag)
    {
        while (nodes)
        {
            node* next = nodes->next;
            std::cout << " \t\t $$$ delete_nodes() *** delete a node ...\n";
            delete nodes;
            nodes = next;
        }

        std::string info = " \t $$$ delete_nodes() *** delete all nodes,flag: " + std::to_string(flag) + " ...\n";
        std::cout << info;
        //std::cout << " \t $$$ delete_nodes() *** delete all nodes ...\n";
    }
    void try_reclaim(node* old_head, int flag)
    {
        if (threadsTotalInPop == 1) // 1
        {
            node* nodes_to_delete = to_be_deleted.exchange(nullptr); // 2 声明“可删除”列表
            if (!--threadsTotalInPop)                                   // 3 是否只有一个线程调用pop()？
            {
                delete_nodes(nodes_to_delete, flag); // 4
            }
            else if (nodes_to_delete) // 5
            {
                chain_pending_nodes(nodes_to_delete, flag); // 6
            }
            if (old_head)
            {
                auto infoT = " \t $$$ try_reclaim() *** delete old_head,flag: " + std::to_string(flag) + " ...\n";
                std::cout << infoT;
                delete old_head; // 7
            }
        }
        else
        {
            chain_pending_node(old_head, flag); // 8
            --threadsTotalInPop;
        }
        if (threadsTotalInPop < 1)
        {
            std::string info = " \n\t MMMMM try_reclaim() threadsTotalInPop: " + std::to_string(threadsTotalInPop) + ",flag: " + std::to_string(flag) + " ...\n";
            std::cout << info;
        }
    }
    void chain_pending_nodes(node* nodes, int flag)
    {
        node* last = nodes;
        while (node* const next = last->next) // 9 让next指针指向链表的末尾
        {
            last = next;
        }
        chain_pending_nodes(nodes, last, flag);
    }
    void chain_pending_nodes(node* first, node* last, int flag)
    {
        last->next = to_be_deleted;                  // 10
        while (!to_be_deleted.compare_exchange_weak( // 11 用循环来保证last->next的正确性
            last->next,
            first))
            ;
    }
    void chain_pending_node(node* n, int flag)
    {
        if (n)
        {
            chain_pending_nodes(n, n, flag); // 12
        }
    }
};
LockFreeStack<QInfo> stack01{};
void popAndShowStackNode(int index)
{
    auto qi = stack01.pop(index);
    if (qi)
    {
        std::string info = " \t >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> popAndShowStackNode() A flag: " + std::to_string(index) + " qi.index: " + std::to_string(qi->index) + "\n";
        std::cout << info;
    }
    else
    {
        std::string info = " \t popAndShowStackNode() B flag: " + std::to_string(index) + " qi is nullptr. \n";
        std::cout << info;
    }
}
void testMain()
{
    std::cout << " \tLockFreeStack::testMain() begin.\n";
    /*
    QInfo qi_0 = {.index = 0, .total = 101};
    QInfo qi_1 = {.index = 1, .total = 102};
    //std::cout << " \t qi_1.index: " << qi_1.index << "\n";
    stack01.push(qi_0);
    stack01.push(qi_1);
    //*/
    for (auto i = 0; i < 5; ++i)
    {
        QInfo qi_data = {.index = i, .total = i+1, .uuid = qi_uuid++};
        //QInfo qi_data{i, i+1};
        //qi_data.index = i;
        //qi_data.total = i+1;

        stack01.push(qi_data);
    }
    std::vector<std::thread> pthreads;
    for (auto i = 0; i < 2;++i)
    {
        pthreads.emplace_back(std::thread(popAndShowStackNode, i));
    }
    for (auto& t : pthreads)
    {
        t.join();
    }
    

    //auto qi = stack01.pop();
    //std::cout << " \t qi.index: " << qi->index << "\n";
    //qi = stack01.pop();
    //std::cout << " \t qi.index: " << qi->index << "\n";


    //for (auto i = 0; i < 2; ++i)
    //{
    //    stack01.pop();
    //    std::cout << " \t LockFreeStack::testMain() B ...\n";
    //    auto qi = stack01.pop();
    //    if (qi)
    //    {
    //        std::cout << " \t LockFreeStack::testMain() B qi.index: " << qi->index << "\n";
    //    }
    //    else
    //    {
    //        std::cout << " \t qi is nullptr. \n";
    //    }
    //}


    std::cout << " \tLockFreeStack::testMain() end.\n";

}
} // namespace baseNonLock_02
namespace baseNonLock_01
{
template <typename T>
class LockFreeStack
{
private:
    struct node
    {
        std::shared_ptr<T> data; // 1 指针获取数据
        node*              next;
        node(T const& data_) :
            data(std::make_shared<T>(data_)) // 2 让std::shared_ptr指向新分配出来的T
        {}
    };
    std::atomic<node*> head;

public:
    void push(T const& data)
    {
        node* const new_node = new node(data);
        new_node->next       = head.load();
        while (!head.compare_exchange_weak(new_node->next, new_node))
            ;
    }
    std::shared_ptr<T> pop()
    {
        node* old_head = head.load();
        while (old_head && // 3 在解引用前检查old_head是否为空指针
               !head.compare_exchange_weak(old_head, old_head->next))
            ;
        return old_head ? old_head->data : std::shared_ptr<T>(); // 4
    }
};
void testMain()
{
}
} // namespace baseNonLock_01
void testMain()
{
    std::cout << "\nparallel::baseNonLock::testMain() begin.\n";
    //parallel::baseNonLock::baseNonLock_01::testMain();
    parallel::baseNonLock::baseNonLock_02::testMain();
    std::cout << "parallel::baseNonLock::testMain() end.\n";
}
} // namespace parallel::baseNonLock