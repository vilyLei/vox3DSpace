
如何写一个 lock-free 的 ring buffer？
如何用 std::atomic 替代原始锁？
C++ 内存序的每一种 memory_order 行为可视化？

多生产者 / 多消费者 ring buffer；
lock-free 栈、队列；
多原子变量之间的顺序如何构建 happens-before；
如何使用 memory_order_acq_rel / consume；

加上等待机制（如条件变量 + busy-loop fallback）
如何用原子 spin 等待计数器归零
支持可变大小（带 GC）