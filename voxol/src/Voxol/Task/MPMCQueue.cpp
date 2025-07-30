#include "MPMCQueue.h"
#include <cstdio>
#include <thread>
#include <random>
namespace Voxol::Task::Cocurrent
{
namespace MultiTask
{

constexpr int MATRIX_SIZE    = 4;
constexpr int QUEUE_CAPACITY = 8;

struct Matrix4x4
{
    float m[MATRIX_SIZE][MATRIX_SIZE];

    Matrix4x4()
    {
        memset(m, 0, sizeof(m));
    }

    static Matrix4x4 random()
    {
        Matrix4x4                                    mat;
        static thread_local std::mt19937             gen(std::random_device{}());
        static std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
        for (int i = 0; i < MATRIX_SIZE; ++i)
            for (int j = 0; j < MATRIX_SIZE; ++j)
                mat.m[i][j] = dist(gen);
        return mat;
    }

    static Matrix4x4 multiply(const Matrix4x4& a, const Matrix4x4& b)
    {
        Matrix4x4 result;
        for (int i = 0; i < MATRIX_SIZE; ++i)
        {
            for (int j = 0; j < MATRIX_SIZE; ++j)
            {
                for (int k = 0; k < MATRIX_SIZE; ++k)
                {
                    result.m[i][j] += a.m[i][k] * b.m[k][j];
                }
            }
        }
        return result;
    }

    void print() const
    {
        for (int i = 0; i < MATRIX_SIZE; ++i)
        {
            for (int j = 0; j < MATRIX_SIZE; ++j)
                std::cout << m[i][j] << ' ';
            std::cout << '\n';
        }
        std::cout << "------------------------\n";
    }
};

MPMCQueue<Matrix4x4, QUEUE_CAPACITY> inputQueue;
MPMCQueue<Matrix4x4, QUEUE_CAPACITY> outputQueue;
std::atomic<bool>                    done = false;

void producerThread(int id)
{
    while (!done)
    {
        Matrix4x4 mat = Matrix4x4::random();
        inputQueue.try_enqueue(mat); // 若 MPMCQueue 支持非阻塞，替换为 try_enqueue + sleep
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
}

void workerThread(int id)
{
    while (!done)
    {
        Matrix4x4 a, b;
        // if (inputQueue.try_dequeue(a) && inputQueue.try_dequeue(b)) {
        if (inputQueue.try_dequeue_pair(a, b))
        {
            Matrix4x4 result = Matrix4x4::multiply(a, b);
            outputQueue.try_enqueue(result);
        }
        else
        {
            std::this_thread::yield();
        }
    }
}

void mainLoop()
{
    while (!done)
    {
        auto result = outputQueue.try_dequeue();
        if (result.has_value())
        {
            // has calc result
            result.value().print();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
}

int main()
{

    printf("\nVoxol::Task::Cocurrent::MultiTask::main() begin ...\n");
    std::vector<std::thread> threads;
    for (int i = 0; i < 2; ++i)
        threads.emplace_back(producerThread, i);
    for (int i = 0; i < 2; ++i)
        threads.emplace_back(workerThread, i);

    std::thread mainT(mainLoop);

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    // std::this_thread::sleep_for(std::chrono::seconds(5));
    done = true;

    for (auto& t : threads) t.join();
    mainT.join();
    printf("\nVoxol::Task::Cocurrent::MultiTask::main() end ...\n");
    return 0;
}

} // namespace MultiTask
void init()
{
    printf("\n\nVoxol::Task::Cocurrent::init() begin ...\n");
    MultiTask::main();
    printf("\nVoxol::Task::Cocurrent::init() end ...\n");
}
} // namespace Voxol::Task::Cocurrent