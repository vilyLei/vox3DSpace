#include "TestLockFree.h"
#include <cstdio>
#include <atomic>
namespace Voxol::Test::LockFree
{
namespace Demo1
{
class SeqCstCounter
{
    std::atomic<int> count{0};

public:
    void increment()
    {
        count.fetch_add(1); // 默认 memory_order_seq_cst
    }

    int get() const
    {
        return count.load(); // 保证全序一致
    }
};
} // namespace Demo1
void main()
{
    printf("Voxol::Test::LockFree::main() begin ...\n");
    printf("Voxol::Test::LockFree::main() end ...\n");
}
} // namespace Voxol::Test::LockFree