#pragma once
#include <iostream>
#include "../../../voxengine/math/mathDefine.h"
#include "../../../voxengine/math/vec3.h"
#include "../../../voxengine/math/Matrix4.h"

namespace demo
{
namespace voxengineDemo
{
namespace math
{

void testBase()
{
    std::cout << "testBase() ...\n";

    std::cout << std::boolalpha;
    using Vec3Type = voxengine::math::Vec3<float>;
    using Mat4Type = voxengine::math::Matrix4<float>;
    
    using Vec3HPType = voxengine::math::Vec3<double>;
    using Mat4HPType = voxengine::math::Matrix4<double>;

    Vec3Type pos0;
    Vec3Type pos1(0.5,0.5,0.5);
    pos0[0] += 1.5f;
    std::cout << "pos0: " << pos0.toString() << std::endl;
    pos0.setXYZ(0.5,0.5,0.501);
    Vec3HPType hPos0(0.5,0.5,0.5);

    std::cout << "pos0.equalsXYZ(pos1): " << pos0.equalsXYZ(pos1) << std::endl;
    
    return;
    Vec3Type vf{};
    vf.setXYZ(0.5f, 0.6f, 0.78f);
    vf.scaleBy(0.5f);
    std::cout << "vf.x: " << vf.x << std::endl;
    std::cout << "vf.w: " << vf.w << std::endl;
    std::cout << "vf: " << vf.toString() << std::endl;



    Mat4Type mf{};
    mf.identity();
    Mat4Type mf2{};
    mf2.identity();
    mf.copyFrom(mf2);
    std::cout << "mf: " << mf.toString() << std::endl;
}
} // namespace math
} // namespace voxengine
} // namespace demo