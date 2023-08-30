//#include <iostream>
//#include <algorithm>
//#include <variant>
//#include "thread/threadMain.h"
//#include "thread/mutexMain.h"
int data_arr[32768] = {1, 2, 3, 4, 5, 6};
//int data_arr[32768];
int main()
{
    //std::cout << "ccwindows main exec begin ...\n";
    //threadDemo::testMain();
    ////mutexDemo::testMain();
    //std::cout << "ccwindows main exec end ...\n";
    //return EXIT_SUCCESS;
    auto t = sizeof(data_arr);
    data_arr[0] += 1;
    return data_arr[0];
}