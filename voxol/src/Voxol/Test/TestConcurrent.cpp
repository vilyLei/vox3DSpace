#include "TestConcurrent.h"

#include <cstdio>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace Voxol::Test
{
    
namespace Thread
{

using Matrix = std::vector<std::vector<float>>;

// 简单矩阵乘法函数
Matrix multiply(const Matrix& A, const Matrix& B) {
    size_t m = A.size(), n = B[0].size(), p = B.size();
    Matrix result(m, std::vector<float>(n, 0));
    for (size_t i = 0; i < m; ++i)
        for (size_t j = 0; j < n; ++j)
            for (size_t k = 0; k < p; ++k)
                result[i][j] += A[i][k] * B[k][j];
    return result;
}

// 全局共享数据
Matrix resultMatrix;
bool done = false;
std::mutex mtx;
std::condition_variable cv;

// 线程 A：生产者，计算矩阵乘法
void matrixWorker(const Matrix& A, const Matrix& B) {
    Matrix result = multiply(A, B);

    {
        std::lock_guard<std::mutex> lock(mtx);
        resultMatrix = std::move(result);
        done = true;
    }
    cv.notify_one(); // 通知消费者线程
}

// 线程 B：消费者，等待计算完成后处理数据
void resultConsumer() {
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [] { return done; });

    // 使用结果
    std::cout << "Result matrix:\n";
    for (const auto& row : resultMatrix) {
        for (float v : row) std::cout << v << " ";
        std::cout << "\n";
    }
}

int main01() {

    printf("main01() begin ...\n");

    // 示例矩阵
    Matrix A = {
        {1, 2},
        {3, 4}
    };
    Matrix B = {
        {5, 6},
        {7, 8}
    };

    std::thread t1(matrixWorker, std::ref(A), std::ref(B));
    std::thread t2(resultConsumer);

    t1.join();
    t2.join();

    printf("main01() end ...\n");

    return 0;
}
}

void TestConcurrent::init()
{
    Thread::main01();
}
} // namespace Voxol::Test