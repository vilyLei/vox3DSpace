// vox3DSpace.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
//#include "./src/demo/voxengineDemo/math/mathDemo.h"
#include "voxengine/data/ctm/ctmDemo.h"
#include "voxengine/data/stream/streamDemo.h"
//#include "./src/model/stream/Int8Array.h"
//#include "./src/model/stream/baseDef.h"


//#define DELETE_OBJ_NAKED_PTRT(objPtr) \
//    if (objPtr != nullptr)           \
//    {                                \
//        delete objPtr;               \
//        objPtr = nullptr;            \
//    }

#include <cstdlib>
#include <iostream>

int main()
{
	std::cout << "Hello VOX 3D Space!\n";

	//demo::voxengineDemo::math::testBase();
	//demo::voxengineDemo::math::testMatrix();
	//demo::voxengineDemo::math::testCamera();


	voxengine::data::ctm::test();
	//voxengine::data::stream::test();
	//typedef voxengine::data::stream::Int8Array Int8Array;
	//auto u8Arr = std::make_shared<Int8Array>(8);

	return EXIT_SUCCESS;
}
