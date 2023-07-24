#include <iostream>
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
}
int main()
{
    boost::thread thrd(&hello);
    thrd.join();
    std::cout << "main thread end!\n";
    return EXIT_SUCCESS;
}