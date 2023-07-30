#include "voxengine/data/ctm/ctmDemo.h"
#include "voxengine/data/stream/streamDemo.h"
//#include “voxengine/data/stream/Int8Array.h"
//#include “voxengine/data/stream/baseDef.h"


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


	voxengine::data::ctm::test();
	//voxengine::data::stream::test();
	//typedef voxengine::data::stream::Int8Array Int8Array;
	//auto u8Arr = std::make_shared<Int8Array>(8);

	return EXIT_SUCCESS;
}
