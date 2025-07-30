#include "TestConcurrent.h"

#include <cstdio>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>
#include <chrono>
#include <shared_mutex>
#include <optional>
#include <random>
#include <latch>

namespace Voxol::Test
{
///*
namespace Thread
{
using Matrix = std::vector<std::vector<float>>;

Matrix multiply(const Matrix& A, const Matrix& B)
{
    size_t m = A.size(), n = B[0].size(), p = B.size();
    Matrix result(m, std::vector<float>(n, 0));
    for (size_t i = 0; i < m; ++i)
        for (size_t j = 0; j < n; ++j)
            for (size_t k = 0; k < p; ++k)
                result[i][j] += A[i][k] * B[k][j];
    return result;
}

namespace Demo01
{
Matrix                  resultMatrix;
bool                    done = false;
std::mutex              mtx;
std::condition_variable cv;

void matrixWorker(const Matrix& A, const Matrix& B)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(800));
    Matrix result = multiply(A, B);

    {
        std::lock_guard<std::mutex> lock(mtx);
        resultMatrix = std::move(result);
        done         = true;
    }
    cv.notify_one();
}

void resultConsumer()
{
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [] { return done; });

    std::cout << "Result matrix:\n";
    for (const auto& row : resultMatrix)
    {
        for (float v : row) std::cout << v << " ";
        std::cout << "\n";
    }
}
void resultConsumerNonBlocking()
{
    while (true)
    {
        printf("Thread::Demo01::resultConsumerNonBlocking() wait ...\n");
        {
            std::lock_guard<std::mutex> lock(mtx);
            if (done) break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    // 数据已就绪，再次获取
    std::lock_guard<std::mutex> lock(mtx);
    std::cout << "Result matrix:\n";
    for (const auto& row : resultMatrix)
    {
        for (float v : row) std::cout << v << " ";
        std::cout << "\n";
    }
}

int main()
{

    printf("Thread::Demo01::main() begin ...\n");

    Matrix A = {
        {1, 2},
        {3, 4}};
    Matrix B = {
        {5, 6},
        {7, 8}};

    std::thread t1(matrixWorker, std::ref(A), std::ref(B));
    //std::thread t2(resultConsumer);
    std::thread t2(resultConsumerNonBlocking);

    t1.join();
    t2.join();

    printf("Thread::Demo01::main() end ...\n");

    return 0;
}
} // namespace Demo01

namespace Demo02
{
class ResultNotifier
{
public:
    void setCallback(std::function<void(const Matrix&)> cb)
    {
        std::lock_guard<std::mutex> lock(mtx_);
        callback_ = std::move(cb);
        if (ready_)
        {
            callback_(result_);
        }
    }

    void setResult(Matrix result)
    {
        std::lock_guard<std::mutex> lock(mtx_);
        result_ = std::move(result);
        ready_  = true;
        if (callback_)
        {
            callback_(result_);
        }
    }

private:
    std::mutex                         mtx_;
    std::function<void(const Matrix&)> callback_;
    Matrix                             result_;
    bool                               ready_ = false;
};

int main()
{

    printf("Thread::Demo02::main() begin ...\n");

    Matrix A = {{1, 2}, {3, 4}};
    Matrix B = {{5, 6}, {7, 8}};

    ResultNotifier notifier;

    // 注册回调：在结果准备好时调用
    notifier.setCallback([](const Matrix& result) {
        std::cout << "Received result matrix:\n";
        for (const auto& row : result)
        {
            for (float v : row) std::cout << v << " ";
            std::cout << "\n";
        }
    });

    // 异步计算线程
    std::thread t([&notifier, A, B]() {
        Matrix result = multiply(A, B);
        notifier.setResult(std::move(result)); // 计算完成，通知回调
    });

    t.join();

    printf("Thread::Demo02::main() end ...\n");
    return 0;
}

} // namespace Demo02
namespace Demo03
{
constexpr int BUFFER_COUNT = 2;

class SharedMatrixBuffers
{
public:
    SharedMatrixBuffers(size_t rows, size_t cols)
    {
        for (int i = 0; i < BUFFER_COUNT; ++i)
            buffers[i] = Matrix(rows, std::vector<float>(cols, 0));
    }

    void write(const Matrix& data)
    {
        int writeIndex      = 1 - readIndex.load(std::memory_order_acquire);
        buffers[writeIndex] = data;
        readIndex.store(writeIndex, std::memory_order_release);
    }

    const Matrix& read() const
    {
        int index = readIndex.load(std::memory_order_acquire);
        return buffers[index];
    }

private:
    Matrix           buffers[BUFFER_COUNT];
    std::atomic<int> readIndex{0};
};

int main()
{
    printf("Thread::Demo03::main() begin ...\n");
    Matrix A = {{1, 2}, {3, 4}};
    Matrix B = {{5, 6}, {7, 8}};

    SharedMatrixBuffers shared(2, 2);

    std::thread producer([&]() {
        while (true)
        {
            auto result = multiply(A, B);
            shared.write(result);
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    });

    std::thread consumer([&]() {
        while (true)
        {
            const Matrix& mat = shared.read();
            std::cout << "Using matrix:\n";
            for (const auto& row : mat)
            {
                for (float v : row) std::cout << v << " ";
                std::cout << "\n";
            }
            std::cout << "----\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(800));
        }
    });

    producer.join();
    consumer.join();

    printf("Thread::Demo03::main() end ...\n");
    return 0;
}

} // namespace Demo03
namespace Demo04
{
using TimePoint = std::chrono::steady_clock::time_point;

constexpr int BUFFER_SIZE = 4; // 环形缓冲区大小

struct MatrixSlot
{
    Matrix    data;
    TimePoint timestamp;
};

class MatrixRingBuffer
{
public:
    MatrixRingBuffer(size_t rows, size_t cols)
    {
        for (int i = 0; i < BUFFER_SIZE; ++i)
        {
            buffers[i].data      = Matrix(rows, std::vector<float>(cols, 0));
            buffers[i].timestamp = TimePoint::min();
        }
    }

    void write(const Matrix& m)
    {
        int index = writeIndex.fetch_add(1, std::memory_order_relaxed) % BUFFER_SIZE;

        {
            std::unique_lock lock(bufferLocks[index]); // 排他写锁
            buffers[index].data      = m;
            buffers[index].timestamp = std::chrono::steady_clock::now();
        }

        // 更新最新数据索引
        latestIndex.store(index, std::memory_order_release);
    }

    // 读线程尝试读取最新数据（可多个线程同时读）
    std::optional<Matrix> readIfUpdated(TimePoint& lastTimeSeen) const
    {
        int              index = latestIndex.load(std::memory_order_acquire);
        std::shared_lock lock(bufferLocks[index]); // 共享读锁

        if (buffers[index].timestamp > lastTimeSeen)
        {
            lastTimeSeen = buffers[index].timestamp;
            return buffers[index].data;
        }
        return std::nullopt;
    }

private:
    MatrixSlot                buffers[BUFFER_SIZE];
    mutable std::shared_mutex bufferLocks[BUFFER_SIZE]; // 支持多读单写
    std::atomic<int>          writeIndex{0};            // 写入推进
    std::atomic<int>          latestIndex{0};           // 最新数据指针
};

int main()
{

    printf("Thread::Demo04::main() begin ...\n");

    Matrix           A = {{1, 2}, {3, 4}};
    Matrix           B = {{5, 6}, {7, 8}};
    MatrixRingBuffer shared(2, 2);

    // 写线程
    std::thread producer([&]() {
        while (true)
        {
            shared.write(multiply(A, B));
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    });

    // 多个读线程（如主渲染 + 分析线程）
    auto readerTask = [&](int id) {
        TimePoint lastSeen = TimePoint::min();
        while (true)
        {
            auto result = shared.readIfUpdated(lastSeen);
            if (result)
            {
                std::cout << "[Reader " << id << "] Updated matrix:\n";
                for (const auto& row : *result)
                {
                    for (float v : row) std::cout << v << " ";
                    std::cout << "\n";
                }
                std::cout << "----\n";
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(800));
        }
    };

    std::thread reader1(readerTask, 1);
    std::thread reader2(readerTask, 2);

    producer.join();
    reader1.join();
    reader2.join();

    printf("Thread::Demo04::main() end ...\n");

    return 0;
}

}

namespace Demo05
{
using TimePoint           = std::chrono::steady_clock::time_point;
constexpr int BUFFER_SIZE = 4;

struct MatrixSlot
{
    std::optional<Matrix> data;
    TimePoint             timestamp = TimePoint::min();
};

class MatrixRingBuffer
{
public:
    MatrixRingBuffer(size_t rows, size_t cols)
    {
        for (int i = 0; i < BUFFER_SIZE; ++i)
        {
            buffers[i].data = Matrix(rows, std::vector<float>(cols, 0));
        }
    }

    void write(const Matrix& m)
    {
        int index = writeIndex.fetch_add(1, std::memory_order_relaxed) & (BUFFER_SIZE - 1);
        {
            std::unique_lock lock(bufferLocks[index]);
            buffers[index].data      = m;
            buffers[index].timestamp = std::chrono::steady_clock::now();
        }
        latestIndex.store(index, std::memory_order_release);
    }

    std::optional<Matrix> readIfUpdated(TimePoint& lastSeen) const
    {
        int              index = latestIndex.load(std::memory_order_acquire);
        std::shared_lock lock(bufferLocks[index]);
        if (buffers[index].timestamp > lastSeen && buffers[index].data.has_value())
        {
            lastSeen = buffers[index].timestamp;
            return buffers[index].data;
        }
        return std::nullopt;
    }

    void cleanupOldData(std::chrono::milliseconds maxAge)
    {
        TimePoint now = std::chrono::steady_clock::now();
        for (int i = 0; i < BUFFER_SIZE; ++i)
        {
            std::unique_lock lock(bufferLocks[i]);
            if (buffers[i].data.has_value() && (now - buffers[i].timestamp > maxAge))
            {
                buffers[i].data.reset();
            }
        }
    }

private:
    MatrixSlot                buffers[BUFFER_SIZE];
    mutable std::shared_mutex bufferLocks[BUFFER_SIZE];
    std::atomic<int>          writeIndex{0};
    std::atomic<int>          latestIndex{0};
};

Matrix generateRandomMatrix(size_t rows, size_t cols)
{
    Matrix                                m(rows, std::vector<float>(cols));
    static std::mt19937                   gen(std::random_device{}());
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    for (auto& row : m)
        for (auto& v : row)
            v = dist(gen);
    return m;
}

void render(const Matrix& m, int readerId)
{
    std::cout << "[Reader " << readerId << "] Matrix updated:\n";
    for (const auto& row : m)
    {
        for (float v : row)
            std::cout << v << " ";
        std::cout << "\n";
    }
    std::cout << "---\n";
}

void renderBlank(int readerId)
{
    std::cout << "[Reader " << readerId << "] No update, rendering blank frame.\n---\n";
}

int main()
{
    MatrixRingBuffer buffer(2, 2);

    // Producer thread: generates new matrix every 10ms
    std::thread producer([&]() {
        while (true)
        {
            Matrix m = generateRandomMatrix(2, 2);
            buffer.write(m);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    });

    // Reader threads
    auto readerFunc = [&](int id) {
        TimePoint lastSeen = TimePoint::min();
        while (true)
        {
            auto now    = std::chrono::steady_clock::now();
            auto result = buffer.readIfUpdated(lastSeen);
            if (result)
            {
                render(*result, id);
            }
            else if (now - lastSeen > std::chrono::milliseconds(800))
            {
                renderBlank(id);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    };

    std::thread reader1(readerFunc, 1);
    std::thread reader2(readerFunc, 2);

    // Cleanup thread: clears old buffers every 1 second
    std::thread cleaner([&]() {
        while (true)
        {
            buffer.cleanupOldData(std::chrono::seconds(2));
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    });

    producer.join();
    reader1.join();
    reader2.join();
    cleaner.join();

    return 0;
}
class SynchronizedCounter
{
    std::atomic<int> count{0};

public:
    void increment()
    {
        count.fetch_add(1, std::memory_order_release);
    }

    // 用于需要同步场景，如在某个阈值时执行某些操作
    int get() const
    {
        return count.load(std::memory_order_acquire);
    }
};
struct SimpleRingBuffer
{
    std::atomic<size_t> head = 0;
    std::atomic<size_t> tail = 0;
    int                 data[1024];

    void push(int v)
    {
        size_t h       = head.load(std::memory_order_relaxed);
        data[h % 1024] = v;
        head.store(h + 1, std::memory_order_release);
    }

    std::optional<int> pop()
    {
        size_t t = tail.load(std::memory_order_relaxed);
        size_t h = head.load(std::memory_order_acquire);
        if (t < h)
        {
            int v = data[t % 1024];
            tail.store(t + 1, std::memory_order_relaxed);
            return v;
        }
        return std::nullopt;
    }
};
} // namespace Demo05
namespace Demo06
{
std::atomic<bool> ready_flag(false);

void worker_thread()
{
    while (!ready_flag.load())
    {
        std::this_thread::yield(); // 让出CPU时间片
    }
    std::cout << "Worker thread running" << std::endl;
}

std::latch task_done{6};

void latchCountDown()
{
    for (auto i = 0; i < 6; ++i)
    {
        printf("latchCountDown() i: %d \n", i);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        task_done.count_down();
    }
}

void latchWaitForZero()
{
    printf("latchWaitForZero() begin ...\n");
    for (auto i = 0; i < 3; ++i)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        printf("latchWaitForZero() i: %d \n", i);
        if (i == 1)
        {
            printf("latchWaitForZero() wait begin, i: %d \n", i);
            task_done.wait();
            printf("latchWaitForZero() wait end, i: %d \n", i);
        }
    }
    printf("latchWaitForZero() end ...\n");
}

int main()
{
    printf("Demo06::main() begin ...\n");

    //std::thread worker(worker_thread);
    //std::this_thread::sleep_for(std::chrono::seconds(1));
    //ready_flag.store(true);
    //worker.join();

    std::thread worker01(latchCountDown);
    std::thread worker02(latchWaitForZero);
    worker01.join();
    worker02.join();

    printf("Demo06::main() end ...\n");
    return 0;
}
}

} // namespace Thread
//*/
void TestConcurrent::init()
{
    //Thread::Demo01::main();
    //Thread::Demo02::main();
    //Thread::Demo03::main();
    //Thread::Demo04::main();
    //Thread::Demo05::main();
    //Thread::Demo06::main();

    LockFree::main();
}
} // namespace Voxol::Test