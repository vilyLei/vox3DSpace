#include <iostream>
#include <algorithm>
#include <variant>
#include "thread/threadMain.h"

int main()
{
    std::cout << "ccwindows main exec begin ...\n";
    threadDemo::testMain();
    std::cout << "ccwindows main exec end ...\n";
    return EXIT_SUCCESS;
}