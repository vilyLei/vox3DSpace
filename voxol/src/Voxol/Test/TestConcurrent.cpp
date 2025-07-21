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
}

namespace Demo02
{

}

}

void TestConcurrent::init()
{
    Thread::Demo01::main();
}
} // namespace Voxol::Test