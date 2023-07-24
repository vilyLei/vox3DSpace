
#if defined(__GNUC__)
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#elif defined(_MSC_VER)
#    pragma warning(disable : 4996)
#endif
#include <iostream>
#include <string>
#include <tinyObj/include/tiny_obj_loader.h>

int main()
{
    std::cout<<"tiny obj loader test ... \n";
    tinyobj::float3 f3a{1.0f,2.0f,3.0f};
    tinyobj::float3 f3b{3.0f,2.0f,3.0f};
    tinyobj::float3 f3c{f3a, f3b};
    
    return EXIT_SUCCESS;
}