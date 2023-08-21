#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <condition_variable>
#include <future>
#include <queue>

namespace parallel::baseNonLock
{
namespace baseNonLock_02
{
template <typename T>
class lock_free_stack
{
private:
    struct node
    {
        std::shared_ptr<T> data;
        node*              next;
        node(T const& data_) :
            data(std::make_shared<T>(data_))
        {}
    };
    std::atomic<unsigned> threads_in_pop;
    std::atomic<node*>    to_be_deleted;
    std::atomic<node*>    head;
    std::shared_ptr<T> pop()
    {
        ++threads_in_pop;
        node* old_head = head.load();
        while (old_head &&
               !head.compare_exchange_weak(old_head, old_head->next))
            ;
        std::shared_ptr<T> res;
        if (old_head)
        {
            res.swap(old_head->data);
        }
        try_reclaim(old_head);
        return res;
    }
    static void delete_nodes(node* nodes)
    {
        while (nodes)
        {
            node* next = nodes->next;
            delete nodes;
            nodes = next;
        }
    }
    void try_reclaim(node* old_head)
    {
        if (threads_in_pop == 1) // 1
        {
            node* nodes_to_delete = to_be_deleted.exchange(nullptr); // 2 声明“可删除”列表
            if (!--threads_in_pop)                                   // 3 是否只有一个线程调用pop()？
            {
                delete_nodes(nodes_to_delete); // 4
            }
            else if (nodes_to_delete) // 5
            {
                chain_pending_nodes(nodes_to_delete); // 6
            }
            delete old_head; // 7
        }
        else
        {
            chain_pending_node(old_head); // 8
            --threads_in_pop;
        }
    }
    void chain_pending_nodes(node* nodes)
    {
        node* last = nodes;
        while (node* const next = last->next) // 9 让next指针指向链表的末尾
        {
            last = next;
        }
        chain_pending_nodes(nodes, last);
    }
    void chain_pending_nodes(node* first, node* last)
    {
        last->next = to_be_deleted;                  // 10
        while (!to_be_deleted.compare_exchange_weak( // 11 用循环来保证last->next的正确性
            last->next,
            first))
            ;
    }
    void chain_pending_node(node* n)
    {
        chain_pending_nodes(n, n); // 12
    }
};
} // namespace baseNonLock_02
namespace baseNonLock_01
{
template <typename T>
class lock_free_stack
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
    std::cout << "parallel::baseNonLock::testMain() begin.\n";
    parallel::baseNonLock::baseNonLock_01::testMain();
    std::cout << "parallel::baseNonLock::testMain() end.\n";
}
} // namespace parallel::baseNonLock