// ..\..\..\..\dev\webwasm\emsdk\emsdk_env.bat
// emcmake cmake ..
// cmake --build .

// ..\..\..\..\..\dev\webwasm\emsdk\emsdk_env.bat
// emcmake "C:/Program Files/CMake/bin/cmake.exe" -B build -G Ninja
// cmake --build build
//
// http-server -p 666 --cors

#include <cstdint>
#include <cstdio>
#include <chrono>

#ifdef __EMSCRIPTEN__
#    include <emscripten/bind.h>
#    include <emscripten/emscripten.h>
#    include <emscripten/heap.h>
#    include <emscripten/val.h>
#    include <wasm_simd128.h>
#endif


#include "Voxol/Base/SlabArena.h"
#include "Voxol/Base/BaseComponentStorage.h"
#include "Voxol/Base/ComponentWorld.h"
#include "Voxol/Base/EntityIDFactory.h"
#include "Voxol/Base/Component.h"
#include "Voxol/Base/EntityObjectBuilder.h"
#include "Voxol/System/RenderSystem.h"

#include "Voxol/Test/TestRenderer.h"



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
#ifdef __EMSCRIPTEN__

Voxol::Test::TestRenderer renderer{};
uint8_t data[256];

// 加法：result = a + b
void add_f32x4(const float* a, const float* b, float* result)
{
    v128_t va   = wasm_v128_load(a);
    v128_t vb   = wasm_v128_load(b);
    v128_t vsum = wasm_f32x4_add(va, vb);
    wasm_v128_store(result, vsum);
}

// 乘法：result = a * b
void mul_f32x4(const float* a, const float* b, float* result)
{
    v128_t va   = wasm_v128_load(a);
    v128_t vb   = wasm_v128_load(b);
    v128_t vmul = wasm_f32x4_mul(va, vb);
    wasm_v128_store(result, vmul);
}

// 打印4个 float
void print_f32x4(const float* vec, const char* label)
{
    printf("%s: [%.2f, %.2f, %.2f, %.2f]\n", label, vec[0], vec[1], vec[2],
           vec[3]);
}

// 示例主函数（仅供本地调试或测试）
void testSimd()
{

    printf("testSimd() begin ...\n");

    alignas(16) float a[4] = {1.0f, 2.0f, 3.0f, 4.0f};
    alignas(16) float b[4] = {5.0f, 6.0f, 7.0f, 8.0f};
    alignas(16) float r[4];

    add_f32x4(a, b, r);
    print_f32x4(r, "add");

    mul_f32x4(a, b, r);
    print_f32x4(r, "mul");

    printf("testSimd() end ...\n");
}

extern "C"
{

    EMSCRIPTEN_KEEPALIVE
    int get_heap_size_bytes()
    {
        return emscripten_get_heap_size(); // 更轻量、无 Embind 依赖
    }

    // 获取数据的起始地址
    EMSCRIPTEN_KEEPALIVE
    uint8_t* get_buffer_ptr() { return data; }

    // 写入数据
    EMSCRIPTEN_KEEPALIVE
    void set_buffer_value(int index, uint8_t value)
    {
        if (index >= 0 && index < 256)
            data[index] = value;
    }
    EMSCRIPTEN_KEEPALIVE
    void startup()
    {
        printf("voxol main startup() ...\n");
        // testSimd();
        // testMemoryManage();
        renderer.startup();
    }

    EMSCRIPTEN_KEEPALIVE
    void setGPUCtxSize(int w, int h)
    {

        printf("voxol main setGPUCtxSize size(w=%d, h=%d)\n", w, h);
        calcProfileTest();

        renderer.setGPUCtxSize(w, h);
        renderer.render();
    }

    EMSCRIPTEN_KEEPALIVE
    void setMouseParams(float x, float y, int type, int flag)
    {

        printf("voxol main setMouseParams mouseParam(x=%f, y=%f, type=%d, flag=:%d)\n", x, y, type, flag);

        renderer.setMouseXY(x, y);
        renderer.render();
    }
}
#else
int main()
{
    testMemoryManage();
    calcProfileTest();
    return 1;
}
#endif
