#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <condition_variable>
#include <future>
#include <queue>
#include <vector>

namespace parallel::baseNonLock2
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
unsigned const max_hazard_pointers = 100;
struct hazard_pointer
{
    std::atomic<std::thread::id> id;
    std::atomic<void*>           pointer;
};
hazard_pointer hazard_pointers[max_hazard_pointers];
class hp_owner
{
    hazard_pointer* hp;

public:
    hp_owner(hp_owner const&)           = delete;
    hp_owner operator=(hp_owner const&) = delete;
    hp_owner() :
        hp(nullptr)
    {
        for (unsigned i = 0; i < max_hazard_pointers; ++i)
        {
            std::thread::id old_id;
            if (hazard_pointers[i].id.compare_exchange_strong( // 6 尝试声明风险指针的所有权
                        old_id,
                    std::this_thread::get_id()))
            {
                hp = &hazard_pointers[i];
                break; // 7
            }
        }
        if (!hp) // 1
        {
            throw std::runtime_error("No hazard pointers available");
        }
    }
    std::atomic<void*>& get_pointer()
    {
        return hp->pointer;
    }
    ~hp_owner() // 2
    {
        hp->pointer.store(nullptr);      // 8
        hp->id.store(std::thread::id()); // 9
    }
};
std::atomic<void*>& get_hazard_pointer_for_current_thread() // 3
{
    thread_local static hp_owner hazard; // 4 每个线程都有自己的风险指针
    return hazard.get_pointer(); // 5
}
bool outstanding_hazard_pointers_for(void* p)
{
    for (unsigned i = 0; i < max_hazard_pointers; ++i)
    {
        if (hazard_pointers[i].pointer.load() == p)
        {
            return true;
        }
    }
    return false;
}
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
        std::atomic<void*>&
              hp       = get_hazard_pointer_for_current_thread();
        node* old_head = head.load();
        do
        {
            node* temp;
            do // 1 直到将风险指针设为head指针
            {
                temp = old_head;
                hp.store(old_head);
                old_head = head.load();
            } while (old_head != temp);
        } while (old_head &&
                 !head.compare_exchange_strong(old_head, old_head->next));
        hp.store(nullptr); // 2 当声明完成，清除风险指针
        std::shared_ptr<T> res;
        if (old_head)
        {
            res.swap(old_head->data);
            //if (outstanding_hazard_pointers_for(old_head)) // 3 在删除之前对风险指针引用的节点进行检查
            //{
            //    reclaim_later(old_head); // 4
            //}
            //else
            //{
            //    delete old_head; // 5
            //}
            //delete_nodes_with_no_hazards(); // 6
        }
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
            node* nodes_to_delete = to_be_deleted.exchange(nullptr); // 2 ��������ɾ�����б�
            if (!--threadsTotalInPop)                                   // 3 �Ƿ�ֻ��һ���̵߳���pop()��
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
        while (node* const next = last->next) // 9 ��nextָ��ָ��������ĩβ
        {
            last = next;
        }
        chain_pending_nodes(nodes, last, flag);
    }
    void chain_pending_nodes(node* first, node* last, int flag)
    {
        last->next = to_be_deleted;                  // 10
        while (!to_be_deleted.compare_exchange_weak( // 11 ��ѭ������֤last->next����ȷ��
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
        std::shared_ptr<T> data; // 1 ָ���ȡ����
        node*              next;
        node(T const& data_) :
            data(std::make_shared<T>(data_)) // 2 ��std::shared_ptrָ���·��������T
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
        while (old_head && // 3 �ڽ�����ǰ���old_head�Ƿ�Ϊ��ָ��
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
    std::cout << "\nparallel::baseNonLock2::testMain() begin.\n";
    std::thread::id thread_old_id;
    std::cout << "thread_old_id: " << thread_old_id << "\n";
    //parallel::baseNonLock2::baseNonLock_01::testMain();
    parallel::baseNonLock2::baseNonLock_02::testMain();
    std::cout << "parallel::baseNonLock2::testMain() end.\n";
}
} // namespace parallel::baseNonLock2