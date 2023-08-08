
//#include "./src/demo/voxengineDemo/math/mathDemo.h"

#include "./src/common/math/vec4.h"
#include "./src/common/math/mat2.h"
#include "./src/common/math/box.h"
#include "./src/common/math/Sphere.h"

#include "./src/voxengine/data/stream/streamDemo.h"
#include "./src/voxengine/data/stream/BaseTypeArray.h"
#include "./src/voxengine/math/Float.h"
#include "./src/voxengine/math/OBB.h"
#include "./src/voxengine/math/vec2.h"
#include "./src/voxengine/math/Matrix4.h"
#include "./src/voxengine/text/BaseText.h"

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

    std::cout << "\ntest common math:\n";
    baseMath::vec4<float> fv4A{0.0f};
    baseMath::mat2f       mat2fA{0.0f};
    baseMath::Box         boxA{0.0f};
    baseMath::Sphere      sphA{baseMath::Float3(), 10.0f};

    std::cout << "\ntest engine base objects:\n";

    
    std::cout << "\n-------------------------  ------------------------  ------------------------------\n";
    voxengine::data::stream::BaseTypeArray<UINT32> uint32Array(6);
    std::cout << "uint32Array.getByteStride(): " << uint32Array.getByteStride() << ", uint32Array.getByteLength(): " << uint32Array.getByteLength() << std::endl;
    voxengine::data::stream::BaseTypeArray<Float32> float32Array(6);
    float32Array[2] = 0.5f;
    std::cout << "float32Array.getByteStride(): " << float32Array.getByteStride() << ", float32Array.getByteLength(): " << float32Array.getByteLength() << std::endl;
    std::cout << "float32Array[2]: " << float32Array[2] << std::endl;
    // 
    //voxengine::data::stream::BaseTypeArray<UINT32*> stringArray(6);   // has some errors
    //voxengine::data::stream::BaseTypeArray<std::string> stringArray(6); // has some errors
    //stringArray[2] = "df";
    //std::cout << "stringArray[2]: " << stringArray[2] << std::endl;
    std::cout << "\n-------------------------  ------------------------  ------------------------------\n";
    voxengine::math::Matrix4<double> mat4_01{};
    std::cout << "mat4_01: " << mat4_01.toString() << std::endl;
    mat4_01.setRotationEulerAngle(0.5, 0.0, 1.0);
    std::cout << "mat4_01: " << mat4_01.toString() << std::endl;

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
    v3.setXYZ(-22.7989f, 33.6f, -15.3f);
    v3.data[0] = 15.7768f;
    v3.x += 15.0f;
    voxengine::math::UVec3 uv3;
    uv3.setXYZ(325,3,5);
    std::cout << "v3.x: " << v3.x << std::endl;
    std::cout << "uv3.x: " << uv3.x << std::endl;

    std::cout << "\nXXXXXXXX      <<<\n";
    std::cout << "Vec3<float> v3, sizeof(v3): " << sizeof(v3) << std::endl;
    std::cout << "v3.toString(): " << v3.toString() << std::endl;
    float float_vs4[4]{};
    v3.toArray4(float_vs4, 0);
    voxengine::math::Vec3<float> v3_01;
    v3_01.fromArray4(float_vs4);
    std::cout << "v3_01.toString(): " << v3_01.toString() << std::endl;
    v3_01.negate();

    
    float float_vs3[3]{};
    v3_01.toArray3(float_vs3);
    v3.fromArray3(float_vs3);
    std::cout << "v3.toString(): " << v3.toString() << std::endl;
    v3_01.setXYZ(1.3f, 5.6f, 0.7f);
    v3.crossBy(v3_01);
    std::cout << "v3::crossBy(), v3.toString(): " << v3.toString() << std::endl;
    v3.normalize();
    std::cout << "v3::normalize(), v3.toString(): " << v3.toString() << std::endl;
    v3_01.copyFrom(v3);
    std::cout << "v3_01.toString(): " << v3_01.toString() << std::endl;
    voxengine::math::Vec3<float> v3_02;
    std::memcpy(&v3_02, &v3_01, sizeof(v3_01));
    std::cout << "std::memcpy(&v3_02, &v3_01, sizeof(v3_01)), v3_02.toString(): " << v3_02.toString() << std::endl;

    voxengine::math::Vec3<float> v3_03(v3_02);
    std::cout << "v3_03.toString(): " << v3_03.toString() << std::endl;
    v3_02.negate();
    v3_03 = v3_02;
    std::cout << "v3_03.toString(): " << v3_03.toString() << std::endl;
    std::cout << "XXXXXXXX      >>>\n\n";

    
    voxengine::math::Vec2<float> v2;
    v2.setXY(33.17f, -15.1f);
    voxengine::math::UVec2 uv2;
    uv2.setXY(125, 8);
    std::cout << "v2.x: " << v2.x << std::endl;
    std::cout << "uv2.x: " << uv2.x << ", uv2.y: " << uv2.y << std::endl;
    uv2.z = 319;
    std::cout << "uv2.toString(): " << uv2.toString() << std::endl;

    voxengine::math::UVec2 uv2b;
    uv2b = uv2;
    std::cout << "uv2b.x: " << uv2b.x << ", uv2b.y: " << uv2b.y << std::endl;

    voxengine::math::Vec2<char> v2_char;
    v2_char.setXY(15, 12);
    std::cout << "v2_char.x: " << (int)v2_char.x << ", v2_char.y: " << (int)v2_char.y << std::endl;

    voxengine::math::Vec2<short> v2_short;
    v2_short.setXY(65537, 12);
    std::cout << "v2_short.x: " << v2_short.x << ", v2_short.y: " << v2_short.y << std::endl;

    
    voxengine::math::Vec2<int> v2_int;
    v2_int.setXY(65537, 12);
    std::cout << "v2_int.x: " << v2_int.x << ", v2_int.y: " << v2_int.y << std::endl;

    auto& axis_const = voxengine::math::Vec2<int>::X_AXIS;
    std::cout << "vec2 const x_axis.toString(): " << axis_const.toString() << std::endl;
    auto axis = voxengine::math::Vec2<int>::X_AXIS;
    std::cout << "vec2 x_axis.toString(): " << axis.toString() << std::endl;
    axis = voxengine::math::Vec2<int>::Y_AXIS;
    std::cout << "vec2 y_axis.toString(): " << axis.toString() << std::endl;
    axis = voxengine::math::Vec2<int>::ZERO;
    std::cout << "vec2 zero.toString(): " << axis.toString() << std::endl;
    axis = voxengine::math::Vec2<int>::ONE.clone();
    std::cout << "vec2 ONE.toString(): " << axis.toString() << std::endl;

    
    
    auto v3_axis = voxengine::math::Vec3<int>::Y_AXIS;
    std::cout << "vec3 y_axis.toString(): " << v3_axis.toString() << std::endl;

    //voxengine::math::Vec2<unsigned short> v2_u_short;
    //v2_u_short.setXY(65537, 12);
    //std::cout << "v2_u_short.x: " << v2_u_short.x << ", v2_short.y: " << v2_u_short.y << std::endl;

    std::cout << "\ntest engine module:\n";
    std::boolalpha(std::cout);
    std::cout << "\nisZero(0.001f): " << voxengine::math::isZero(0.001f) << std::endl;
    std::cout << "isGreaterPositiveZero(0.001f): " << voxengine::math::isGreaterPositiveZero(0.001f) << std::endl;

    //demo::voxengineDemo::math::testBase();
    //demo::voxengineDemo::math::testCamera();
    voxengine::data::stream::test();

    std::cout << "\n-------------------------  ------------------------  ------------------------------\n";
    useFixLengthArr("dfdfdf");
    float fls[3]{0.0f, 0.1f, 0.2f};
    useFixLengthArr(fls);
    std::cout << "\n-------------------------  ------------------------ text test begin ------------------------------\n";
    // BaseText
    static constexpr auto add3         = voxengine::text::parse(",>+++<[->+<]>.");
    unsigned char         memory[1024] = {};
    voxengine::text::execute(add3, memory);
    std::cout << "\n-------------------------  ------------------------ text test end ------------------------------\n";

    return EXIT_SUCCESS;
}
