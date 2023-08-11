
#include <iostream>
#include <thread>
#include "sseavxChecker.h"
#include "sseavxBaseTest.h"
#include "sseavxBaseTestSOA.h"

int main(int argc, char** argv)
{
    std::cout << "cpuparallel::main() begin.\n";
    std::cout << "hardware_concurrency: " << std::thread::hardware_concurrency() << std::endl;
    std::cout << "__alignof(double): " << __alignof(double) << std::endl;
    std::cout << "alignof(double): " << alignof(double) << std::endl;
    std::cout << "alignof(std::max_align_t): " << alignof(std::max_align_t) << '\n';
    //sseavx::config::sseavxCheck();
    //sseavx::test::testBase();
    //sseavx::test::testBase2();
    sseavx::test::testBase3();
    //sseavx::test::test_matrix_calc();
    //sseavx::test::test_sqrt_calc();
    //sseavx::testwithsoa::test_sqrt_calc();
    std::cout << "\ncpuparallel::main() end.\n";
    //std::system("pause");
    return EXIT_SUCCESS;
}
