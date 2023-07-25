#include <iostream>
#include <thread>
#include <boost/algorithm/string.hpp>
#include <boost/thread/thread.hpp>
using namespace std;
using namespace boost;

int main1()
{
    std::cout << "... boost library test ..." << std::endl;
    std::string s = "a boost Test";
    std::cout << boost::algorithm::to_upper_copy(s) << std::endl;
    std::cout << "Hello World!\n";
    return EXIT_SUCCESS;
}
void hello()
{
    std::cout << "Hello world, I'm a thread!" << std::endl;
    int total = 5;
    for (auto i = 0; i < total; ++i)
    {
        std::cout << ">>>>> progress: " << i << "/" << total << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}
int main()
{
    boost::thread thrd(&hello);
    thrd.join();
    std::cout << "main thread end!\n";
    return EXIT_SUCCESS;
}