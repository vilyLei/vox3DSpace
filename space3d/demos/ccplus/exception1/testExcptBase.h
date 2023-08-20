#include <iostream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace exception1::excptBase
{
namespace excptTest_01
{
void handle_eptr(std::exception_ptr eptr) // passing by value is ok
{
    try
    {
        if (eptr)
            std::rethrow_exception(eptr);
    }
    catch (const std::exception& e)
    {
        std::cout << "Caught exception: '" << e.what() << "'\n";
    }
}
void testMain2()
{
    try
    {
        std::cout << "Throwing an integer exception...\n";
        throw 42;
    }
    //catch (std::string i)
    catch (int i)
    {
        std::cout << " the integer exception was caught, with value: " << i << '\n';
    }

    try
    {
        std::cout << "Creating a vector of size 5... \n";
        std::vector<int> v(5);
        std::cout << "Accessing the 11th element of the vector...\n";
        std::cout << v.at(10); // vector::at() throws std::out_of_range
    }
    catch (const std::exception& e) // caught by reference to base
    {
        std::cout << " a standard exception was caught, with message '"
                  << e.what() << "'\n";
    }
}
void testMain()
{
    std::exception_ptr eptr;

    try
    {
        std::string().at(1); // this generates a std::out_of_range
    }
    catch (...)
    {
        eptr = std::current_exception(); // capture
    }

    handle_eptr(eptr);

} // destructor for std::out_of_range called here, when the eptr is destructed
}
void testMain()
{
    std::cout << "exception1::excptBase::testMain() begin.\n";
    excptTest_01::testMain();
    std::cout << "-----------  ----------------  ----------------\n";
    excptTest_01::testMain2();
    std::cout << "exception1::excptBase::testMain() end.\n";
}
} // namespace exception1::excptBase