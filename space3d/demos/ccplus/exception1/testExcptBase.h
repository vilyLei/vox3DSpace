#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <string>
#include <system_error>
#include <thread>

namespace exception1::excptBase
{
namespace excptTest_02
{
void print_error(const std::string& details, std::error_code error_code)
{
    std::string value_name;
    if (error_code == std::errc::invalid_argument)
        value_name = "std::errc::invalid_argument";
    if (error_code == std::errc::no_such_file_or_directory)
        value_name = "std::errc::no_such_file_or_directory";
    if (error_code == std::errc::is_a_directory)
        value_name = "std::errc::is_a_directory";
    if (error_code == std::errc::permission_denied)
        value_name = "std::errc::permission_denied";

    std::cout << details << ":\n  "
              << std::quoted(error_code.message())
              << " (" << value_name << "), " << error_code
              << " \n";
}

void print_errno(const std::string& details, int errno_value = errno)
{
    print_error(details, std::make_error_code(std::errc(errno_value)));
}

void testMain()
{
    // Detaching a not-a-thread
    try
    {
        std::thread().detach();
    }
    catch (const std::system_error& e)
    {
        print_error("Error detaching empty thread", e.code());
    }

    // Opening nonexistent file
    {
        std::ifstream nofile("nonexistent-file");
        if (!nofile.is_open())
        {
            print_errno("Error opening nonexistent file for reading");
        }
    }

    // Reading from directory as a file
    {
        std::filesystem::create_directory("testDir");
        std::ifstream         dir_stream("testDir");
        [[maybe_unused]] char c = dir_stream.get();
        if (!dir_stream.good())
        {
            print_errno("Error reading data from directory");
        }
    }

    // Open readonly file for writing
    {
        {
            std::fstream new_file("readonly-file", std::ios::out);
        }
        std::filesystem::permissions("readonly-file", std::filesystem::perms::owner_read);
        std::fstream write_readonly("readonly-file", std::ios::out);
        if (!write_readonly.is_open())
        {
            print_errno("Error opening readonly file for writing");
        }
    }
}
}
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
    //excptTest_01::testMain();
    std::cout << "-----------  ----------------  ----------------\n";
    //excptTest_01::testMain2();
    std::cout << "-----------  ----------------  ----------------\n";
    excptTest_02::testMain();
    std::cout << "exception1::excptBase::testMain() end.\n";
}
} // namespace exception1::excptBase