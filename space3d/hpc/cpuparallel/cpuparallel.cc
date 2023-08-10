
#include <iostream>
#include "sseavxChecker.h"
#include "sseavxBaseTest.h"
#include "sseavxBaseTest2.h"

int main(int argc, char** argv)
{
    std::cout << "cpuparallel::main() begin.\n";
    std::cout << "__alignof(double): " << __alignof(double) << std::endl;
    std::cout << "alignof(double): " << alignof(double) << std::endl;
    std::cout << "alignof(std::max_align_t): " << alignof(std::max_align_t) << '\n';
    //sseavx::config::sseavxCheck();
    //sseavx::test::testBase();
    //sseavx::test::testBase2();
    //sseavx::test::test_sqrt_calc();
    //sseavx::test::test_matrix_calc();
    sseavx::testwithsoa::test_sqrt_calc();
    std::cout << "\ncpuparallel::main() end.\n";
    return EXIT_SUCCESS;
}
