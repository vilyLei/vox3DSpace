// ..\..\..\..\dev\webwasm\emsdk\emsdk_env.bat
// emcmake cmake ..
// cmake --build .

// ..\..\..\..\dev\webwasm\emsdk\emsdk_env.bat
// emcmake "C:/Program Files/CMake/bin/cmake.exe" -B build -G Ninja
// cmake --build build
//
// http-server -p 666 --cors

/*

glew32d.lib
glfw3.lib
opengl32.lib
kernel32.lib
user32.lib
gdi32.lib

*/

#include <iostream>
#include <cstdint>
#include <cstdio>
#include <chrono>

#include "Voxol/Base/SlabArena.h"
#include "Voxol/Base/BaseComponentStorage.h"
#include "Voxol/Base/ComponentWorld.h"
#include "Voxol/Base/EntityIDFactory.h"
#include "Voxol/Base/Component.h"
#include "Voxol/Base/EntityObjectBuilder.h"
#include "Voxol/System/RenderSystem.h"

// #include "Voxol/Render/RenderCmdWorld.h"
#include "Voxol/Motion/RenderCmdWorld.h"
#include "Voxol/Test/OglRenderer.h"

void testMemoryManage()
{
    // printf("testMemoryManage() begin 01...\n");
    // {
    //     using namespace Voxol::Base;
    //     SlabArena arena{};
    //     using TestCompStorage = BaseComponentStorage<Rect, SlabPool<Rect>>;
    //     SlabPool<Rect>  slabPool(arena);
    //     TestCompStorage rectCompStore(std::move(slabPool));
    //     rectCompStore.add(1, {0, 20, 100, 70});
    // }
    // printf("testMemoryManage() end 01...\n");
    printf("testMemoryManage() begin 02...\n");
    {
        using namespace Voxol::Base;
        auto        worldPtr   = EntityObjectBuilder::make();
        VoxolEntity rootEntity = VoxolEntity_None;
        rootEntity             = worldPtr->createEntity("root");
        printf("testMemoryManage() end 02 rootEntity: %d\n", rootEntity);
        auto e01 = worldPtr->createRectFillEntity("rect_01", {15, 25}, {0, 0, 100, 130}, {0xff00aa00}, rootEntity);
        auto e02 = worldPtr->createRectFillGradientBlurEntity("rect_gradient_02", {15, 25}, {0, 0, 100, 130}, {0xff0000aa, 0xffaa0000}, {15}, rootEntity);


        Voxol::System::RenderSystem Renderer;
        Renderer(*worldPtr);

        worldPtr->removeEntity(e01);
    }
    printf("testMemoryManage() end 02...\n");

    printf("testMemoryManage() end ...\n");
}

void calcProfileTest()
{
    auto tot = 10000 * 100;
    printf("calcProfileTest() tot: %d\n", tot);
#ifdef __EMSCRIPTEN__
    double start = emscripten_get_now();
#else
    auto start = std::chrono::high_resolution_clock::now();
#endif

    using namespace Voxol::Math;

    Mat33 mat0(10, 20, 33.5f, -8.2f, 0.3f);
    Mat33 mat1(180, -120, 313.5f, 8.2f, 0.77f);
    Mat33 mat2(90, -120, 313.5f, 8.2f, 10.77f);
    for (auto i = 0; i < tot; i++)
    {
        mat0.prepend(mat1);
        // mat0.append(mat1);
        //mat2 = mat0 * mat1;
    }

#ifdef __EMSCRIPTEN__
    double end      = emscripten_get_now();
    auto   lossTime = end - start;
    printf("calcProfileTest() elapsed time: %fms\n", lossTime);
#else
    auto end      = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    printf("calcProfileTest() elapsed time: %llums\n", duration.count());
#endif
}

int main()
{
    // testMemoryManage();
    // calcProfileTest();

    Voxol::Motion::RenderCmdWorld rcmdWorld{};
    Voxol::Test::OglRenderer      renderer{};

    rcmdWorld.run();

    renderer.cmdBuf                   = rcmdWorld.buffer;
    renderer.init();
    return 1;
}
