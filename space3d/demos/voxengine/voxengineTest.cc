
//#include "./src/demo/voxengineDemo/math/mathDemo.h"

#include "./src/common/math/vec4.h"
#include "./src/common/math/mat2.h"
#include "./src/common/math/box.h"
#include "./src/common/math/Sphere.h"
#include "./src/common/math/mat4.h"

#include "./src/voxengine/data/stream/streamDemo.h"
#include "./src/voxengine/data/stream/BaseTypeArray.h"
#include "./src/voxengine/math/Float.h"
#include "./src/voxengine/math/OBB.h"
#include "./src/voxengine/math/vec2.h"
#include "./src/voxengine/math/Matrix4.h"
#include "./src/voxengine/view/Camera.h"
#include "./src/voxengine/text/BaseText.h"

#include <cstdlib>
#include <iostream>

template <typename T, std::size_t N>
void useFixLengthArr(const T (&arr)[N])
{
    std::cout << "useFixLengthArr N: " << N << std::endl;
}
void testSharedPtr()
{
    //{
    //    using namespace std::literals;
    //    std::shared_ptr<voxengine::data::stream::BaseTypeArray<Float32>> fa_ptr_0(new voxengine::data::stream::BaseTypeArray<Float32>(6));
    //    std::shared_ptr<voxengine::data::stream::BaseTypeArray<Float32>> fa_ptr_1 = fa_ptr_0->getSharedPtr();
    //    std::cout << "(fa_ptr_0.get() == fa_ptr_1.get()): " << (fa_ptr_0.get() == fa_ptr_1.get()) << std::endl;
    //    std::cout << fa_ptr_0.use_count() << std::endl;
    //    std::cout << fa_ptr_1.use_count() << std::endl;
    //}
    std::cout << "\n-------------------------  ------------- ------- 01 ------ -----------  ------------------------------\n";
    {
        using namespace std::literals;
        voxengine::data::stream::BaseTypeArray<Float32>                  fa_0(6);
        std::shared_ptr<voxengine::data::stream::BaseTypeArray<Float32>> fa_ptr_1 = fa_0.getSharedPtr();
        //std::cout << "(fa_ptr_0.get() == fa_ptr_1.get()): " << (fa_ptr_0.get() == fa_ptr_1.get()) << std::endl;
        //std::cout << fa_ptr_0.use_count() << std::endl;
        //std::cout << fa_ptr_1.use_count() << std::endl;
    }
    std::cout << "\n-------------------------  ------------- testSharedPtr end -----------  ------------------------------\n";
}
int main()
{
    std::cout << "Hello VOX 3D Space!\n";

    //testSharedPtr();
    //return 1;

    float mat16Data[16]{0.540302, 0.841471, -0.000000, 0.000000, -0.738460, 0.474160, 0.479426, 0.000000, 0.403423, -0.259035, 0.877583, 0.000000, 0.0, 0.000000, 0.000000, 1.000000};

    std::cout << "\ntest common math:\n";
    baseMath::vec4<float> fv4A{0.0f};
    baseMath::mat2f       mat2fA{0.0f};
    baseMath::Box         boxA{0.0f};
    baseMath::Sphere      sphA{baseMath::Float3{}, 10.0f};
    baseMath::mat4f       mat4f_0{1.0f};
    baseMath::mat4f       mat4f_1;
    std::cout << "sizeof(mat4f_1): " << sizeof(mat4f_1) << std::endl;
    std::cout << "sizeof(mat16Data): " << sizeof(mat16Data) << std::endl;
    std::memcpy(&mat4f_1, mat16Data, sizeof(mat16Data));

    fv4A.xyz = {1.0, 2.0, 3.0};
    auto new_mat1 = mat4f_0.rotation(0.7, fv4A.xyz);
    fv4A.xyz     = {11.0, 12.0, 13.0};
    auto new_mat  = new_mat1 * mat4f_0.translation(fv4A.xyz);
    std::cout << "mat4f_0: " << mat4f_0 << std::endl;
    std::cout << "new_mat: " << new_mat << std::endl;

    fv4A.w = 0.0;
    fv4A.xyz     = {3.0, 2.0, 1.0};
    auto new_pv0 = new_mat * fv4A;
    baseMath::vec3<float> fv3B{0.0f};
    fv3B = {3.0, 2.0, 1.0};

    new_mat.deltaTransformV3Self(fv3B);

    std::cout << "new_pv0: " << new_pv0 << std::endl;
    std::cout << "fv3B: " << fv3B << std::endl;
    std::cout << "\ntest engine base objects:\n";

    
    //return 1;
    std::cout << "\n-------------------------  ------------------------  ------------------------------\n";
    voxengine::data::stream::BaseTypeArray<UINT32> uint32Array(6);
    std::cout << "uint32Array.getByteStride(): " << uint32Array.getByteStride() << ", uint32Array.getByteLength(): " << uint32Array.getByteLength() << std::endl;
    voxengine::data::stream::BaseTypeArray<Float32> float32Array(6);
    float32Array[2] = 0.5f;
    std::cout << "float32Array.getByteStride(): " << float32Array.getByteStride() << ", float32Array.getByteLength(): " << float32Array.getByteLength() << std::endl;
    std::cout << "float32Array[2]: " << float32Array[2] << std::endl;
    // 不可以这么写(float32Array是普通对象), float32Array必须是shared_ptr指针对象,下面这句代码获得的shared_prt指针才有意义
    auto fa_ptr_0 = float32Array.getSharedPtr();
    if (fa_ptr_0)
    {
        std::cout << "XXXTTTT float32Array[2]: " << (*fa_ptr_0)[2] << std::endl;
    }
    //
    //voxengine::data::stream::BaseTypeArray<UINT32*> stringArray(6);   // has some errors
    //voxengine::data::stream::BaseTypeArray<std::string> stringArray(6); // has some errors
    //stringArray[2] = "df";
    //std::cout << "stringArray[2]: " << stringArray[2] << std::endl;
    std::cout << "\n-------------------------  ------------------------  ------------------------------\n";
    using Camera = voxengine::view::Camera<double>;
    Camera cam01{};
    std::cout << "\n-------------------------  ------------------------  ------------------------------\n";
    using Mat4 = voxengine::math::Matrix4<double>;
    Mat4 mat4_01{};
    std::cout << "mat4_01: " << mat4_01.toString() << std::endl;
    mat4_01.setRotationEulerAngle(0.5, 0.0, 1.0);
    std::cout << "mat4_01: " << mat4_01.toString() << std::endl;
    Mat4 mat4_02{};
    Mat4 mat4_03{};
    mat4_02.setRotationEulerAngle(30.0, 5.0, 0.0);
    mat4_03.multiply(mat4_01, mat4_02);
    mat4_03.multiply(Mat4{});
    std::cout << "mat4_03: " << mat4_03.toString() << std::endl;

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
    uv3.setXYZ(325, 3, 5);
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

    constexpr voxengine::math::Vec3<float> v3_ce_f_1{1.0f, 1.0f, 1.0f, 1.0f};
    constexpr voxengine::math::Vec2<float> v2_ce_f_1{1.0f, 1.0f, 1.0f};

    auto v3_f_axis = voxengine::math::Vec3<float>::X_AXIS;
    v3_f_axis.x    = 0.3f;
    std::cout << "voxengine::math::Vec3<float>::X_AXIS.toString(): " << voxengine::math::Vec3<float>::X_AXIS.toString() << std::endl;
    std::cout << "v3_f_axis.toString(): " << v3_f_axis.toString() << std::endl;

    std::cout << "\n";
    union UnionVec2
    {
        voxengine::math::Vec2<float>  vf2[2]{};
        voxengine::math::Vec2<double> vd;
        ~UnionVec2() = default;
    };
    UnionVec2 u_v2_0 = {{{0.1, 0.2, 0.3}, {0.1, 0.2, 0.3}}};
    UnionVec2 u_v2_1 = {{{0.1, 0.2}, {0.1, 0.2}}};
    UnionVec2 u_v2_2 = {{{0.1, 0.2}, {0.1, 0.2}}};
    UnionVec2 u_v2   = {{{0.1, 0.2, 0.3}}};
    u_v2.vd          = {0.1, 0.2, 0.3};
    std::cout << "UnionVec2, u_v2.vf2[0].toString(): " << u_v2.vf2[0].toString() << std::endl;
    std::cout << "UnionVec2, u_v2.vd.toString(): " << u_v2.vd.toString() << std::endl;
    union UnionVec3
    {
        voxengine::math::Vec3<float>  vf2[2]{};
        voxengine::math::Vec3<double> vd;
        ~UnionVec3() = default;
    };
    UnionVec3 u_v3_0 = {{{0.1, 0.2, 0.3}, {0.1, 0.2, 0.3}}};
    UnionVec3 u_v3   = {{{0.1, 0.2, 0.3}}};
    u_v3.vd          = {0.1, 0.2, 0.3};
    std::cout << "UnionVec3, u_v3.vf2[0].toString(): " << u_v3.vf2[0].toString() << std::endl;
    std::cout << "UnionVec3, u_v3.vd.toString(): " << u_v3.vd.toString() << std::endl;

    std::cout << "\n";
    v3_f_axis = {0.1f, 0.2f, 0.3f};
    voxengine::math::Vec3<float> v3_05{};
    v3_05 = {2.1f, 2.2f, 2.3f};
    std::cout << "v3_f_axis.toString(): " << v3_f_axis.toString() << std::endl;
    std::cout << "v3_05.toString(): " << v3_05.toString() << std::endl;


    voxengine::math::Vec3<double> v3_d_0{};
    v3_d_0 = {7.1f, 7.2f, 7.3f};

    voxengine::math::Vec3<double>&& temp_d_v3_01{1.0f, 1.0f, 2.0f};
    v3_d_0.normalize();
    temp_d_v3_01.normalize();
    auto value_v3_0 = v3_d_0.dot(temp_d_v3_01);
    std::cout << "v3_d_0.dot(temp_d_v3_01): " << value_v3_0 << std::endl;
    std::cout << "v3_d_0.dot(voxengine::math::Vec3<double>()): " << v3_d_0.dot(voxengine::math::Vec3<double>(0.5, 0.7, 0.8)) << std::endl;
    v3_d_0.scaleVector(std::forward<voxengine::math::Vec3<double>>(temp_d_v3_01));
    v3_d_0.scaleVector(std::move(temp_d_v3_01));
    std::cout << "std::move(temp_d_v3_01), temp_d_v3_01.toString(): " << temp_d_v3_01.toString() << std::endl;
    v3_d_0.scaleVector(voxengine::math::Vec3<double>(0.5, 0.7, 0.8));
    std::cout << "\n-------------------------  ------------------------  ------------------------------\n";
    v3_d_0.normalize();
    temp_d_v3_01.normalize();
    v3_d_0 += temp_d_v3_01;
    v3_d_0 += voxengine::math::Vec3<double>(0.5, 0.7, 0.8);
    std::cout << "v3_d_0: " << v3_d_0.toString() << std::endl;
    v3_d_0 += v3_d_0;
    std::cout << "v3_d_0: " << v3_d_0.toString() << std::endl;

    std::cout << "\n-------------------------  ------------------------  ------------------------------\n";
    voxengine::math::Vec3<double> v3a_01{1.0, 2.0, 3.0};
    voxengine::math::Vec3<double> v3a_02{10.0, 20.0, 30.0};
    voxengine::math::Vec3<double> v3_add_mul_01 = (v3a_01 + v3a_02) * voxengine::math::Vec3<double>{3.0, 2.0, 1.5};
    std::cout << "v3_add_mul_01: " << v3_add_mul_01.toString() << std::endl;
    auto                           buf_v3_d = new double[4]{16.1, 17.1, 18.1, 0.9};
    voxengine::math::Vec3<double>* ptr_v3   = (voxengine::math::Vec3<double>*)buf_v3_d;
    std::cout << "ptr_v3: " << ptr_v3->toString() << std::endl;
    v3a_01 += *ptr_v3;
    std::cout << "v3a_01: " << v3a_01.toString() << std::endl;
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
