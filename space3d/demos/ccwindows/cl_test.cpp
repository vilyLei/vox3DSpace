#include <iostream>

int main()
{
    std::cout << "main exec begin ...\n";
    std::cout << "main exec end ...\n";
    return EXIT_SUCCESS;
}
// https://learn.microsoft.com/en-us/cpp/build/reference/compiler-options?view=msvc-170
// https://learn.microsoft.com/en-us/cpp/build/reference/compiler-command-line-syntax?view=msvc-170
// https://learn.microsoft.com/en-us/cpp/build/reference/compiler-options-listed-alphabetically?view=msvc-170
// https://learn.microsoft.com/en-us/cpp/build/reference/eh-exception-handling-model?view=msvc-170

// compile command: cl /EHsc cl_test.cpp