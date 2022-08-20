// vox3DSpace.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
//#include "./src/demo/voxengineDemo/math/mathDemo.h"
#include "./src/voxengine/data/stream/streamDemo.h"
#include "./src/voxengine/math/Float.h"

#include <cstdlib>
#include <iostream>

int main()
{
    std::cout << "Hello VOX 3D Space!\n";

    std::boolalpha(std::cout);
    std::cout << "isZero(0.001f): "<< isZero(0.001f) << std::endl;
    std::cout << "isGreaterPositiveZero(0.001f): "<< isGreaterPositiveZero(0.001f) << std::endl;

    //demo::voxengineDemo::math::testBase();
    //demo::voxengineDemo::math::testCamera();
    voxengine::data::stream::test();


    return EXIT_SUCCESS;
}
