// vox3DSpace.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
//#include "./src/demo/voxengineDemo/math/mathDemo.h"
#include "./src/voxengine/data/stream/streamDemo.h"

#include <cstdlib>
#include <iostream>

int main()
{
    std::cout << "Hello VOX 3D Space!\n";

    //demo::voxengineDemo::math::testBase();
    //demo::voxengineDemo::math::testCamera();
    voxengine::data::stream::test();


    return EXIT_SUCCESS;
}
