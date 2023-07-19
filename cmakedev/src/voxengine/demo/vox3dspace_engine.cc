// vox3DSpace.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
//#include "./src/demo/voxengineDemo/math/mathDemo.h"

// #include "./src/common/math/vec4.h"
// #include "./src/common/math/mat2.h"
// #include "./src/common/math/box.h"
// #include "./src/common/math/Sphere.h"

#include "./src/voxengine/data/stream/streamDemo.h"
#include "./src/voxengine/math/Float.h"
#include "./src/voxengine/math/OBB.h"
// #include "./src/voxengine/text/BaseText.h"

#include <cstdlib>
#include <iostream>

template <typename T, std::size_t N>
void useFixLengthArr(const T (&arr)[N])
{
    std::cout << "useFixLengthArr N: " << N << std::endl;
}
int main()
{
    std::cout << "Hello VOX 3D Space!\n";

    // std::cout << "\ntest base math:\n";
    // baseMath::vec4<float> fv4A{0.0f};
    // baseMath::mat2f       mat2fA{0.0f};
    // baseMath::Box         boxA{0.0f};
    // baseMath::Sphere      sphA{baseMath::Float3(), 10.0f};

    std::cout << "\ntest engine base objects:\n";

    voxengine::math::OBB<double> obbA{};
    auto&                        obb = obbA;
    obb.center.setXYZ(-22.798999786376953, 33.0, -15.0);
    obb.extent.setXYZ(75.00000218686336, 75.0000005694148, 75.00000168327009);
    auto axes = obb.axes;
    axes[0].setXYZ(0.08246776228180698, 0.8724176040717958, -0.4817536634939675);
    axes[1].setXYZ(-0.995561961746394, 0.09410834353909094, 0.0);
    axes[2].setXYZ(0.045337035055944344, 0.4796156160661552, 0.8763066895087692);
    obb.update();


    voxengine::math::OBB<double> obbB{};
    auto&                        obb1 = obbB;
    obb1.center.setXYZ(134.0, 0.0, 0.0);
    obb1.extent.setXYZ(50.00000082180265, 49.99999978489287, 49.99999764021434);
    auto axes1 = obb1.axes;
    axes1[0].setXYZ(0.7269230010572669, 0.6802424902319316, -0.09409094014280595);
    axes1[1].setXYZ(-0.5265063722183034, 0.6400393343308274, 0.5595897519816394);
    axes1[2].setXYZ(0.4408786410630717, -0.35723919598373893, 0.8234113071286251);
    obb1.update();

    auto intersectionFlag = obb.intersect(obb1);
    std::cout << "\ntwo obb objects intersectionFlag: " << intersectionFlag << std::endl;

    float(*arr)[3]     = new float[3][3];
    float(*arr3)[3][3] = new float[3][3][3];

    voxengine::math::Vec3<float> v3;
    v3.setXYZ(-22.798999786376953, 33.0, -15.0);

    std::cout << "\ntest engine module:\n";
    std::boolalpha(std::cout);
    std::cout << "\nisZero(0.001f): " << isZero(0.001f) << std::endl;
    std::cout << "isGreaterPositiveZero(0.001f): " << isGreaterPositiveZero(0.001f) << std::endl;

    //demo::voxengineDemo::math::testBase();
    //demo::voxengineDemo::math::testCamera();
    voxengine::data::stream::test();

    std::cout << "\n-------------------------  ------------------------  ------------------------------\n";
    useFixLengthArr("dfdfdf");
    float fls[3]{0.0f, 0.1f, 0.2f};
    useFixLengthArr(fls);
    std::cout << "\n-------------------------  ------------------------  ------------------------------\n";
    //BaseText
    //tatic constexpr auto add3 = parse(",>+++<[->+<]>.");
    // static constexpr auto add3         = voxengine::text::parse(",>+++<[->+<]>.");
    // unsigned char         memory[1024] = {};
    // voxengine::text::execute(add3, memory);

    return EXIT_SUCCESS;
}
