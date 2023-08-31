///*
#include <iostream>
#include <algorithm>
#include <variant>
#include "thread/threadMain.h"
#include "thread/mutexMain.h"

int main(int argc, char** argv)
{
    std::cout << "ccwindows main exec begin ...\n";
    threadDemo::testMain();
    //mutexDemo::testMain();
    std::cout << "ccwindows main exec end ...\n";
    return EXIT_SUCCESS;
}
//*/
/*
#include <iostream>
#include <string>
//#include <iostream>
//#include <algorithm>
//#include <variant>
//#include "thread/threadMain.h"
//#include "thread/mutexMain.h"
int data_arr[32768] = {1, 2, 3, 4, 5, 6, 7, 8};
//int data_arr[32768] = {2};
//int data_arr[32768] = {0,0,0,0,0,0};// 15k
//int data_arr[32768];// 15k
volatile const static int Major_version = 22;
volatile const float      Minor_Version = 17;
std::string               base_str_0      = "sssssAAAAA00000";
int                       parseSignal(int signal)
{
    static int  baseSignal = 1013;
    std::string base_str   = "sssssAAAAA11111";
    if (signal > 15)
    {
        base_str += "sssssAAAAA22222" + base_str_0;
        signal *= base_str.size();
    }
    return signal * baseSignal;
}
int main(int argc, char** argv)
{
    //std::cout << "ccwindows main exec begin ...\n";
    //threadDemo::testMain();
    ////mutexDemo::testMain();
    //std::cout << "ccwindows main exec end ...\n";
    //return EXIT_SUCCESS;
    //auto t = sizeof(data_arr);
    //size_t addr = (size_t)data_arr;
    //std::cout << "addr: " << addr << std::endl;
    data_arr[1] += 11;
    std::cout << "data_arr[1]: " << data_arr[1] << std::endl;
    return data_arr[1] + (Major_version << argc) + Minor_Version * argc + parseSignal(argc >> 1);
}
//*/