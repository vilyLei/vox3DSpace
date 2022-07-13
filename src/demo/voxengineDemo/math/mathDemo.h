#pragma once
#include <iostream>
#include "../../../voxengine/math/mathDefine.h"
#include "../../../voxengine/math/vec3.h"
#include "../../../voxengine/math/Matrix4.h"
#include "../../../voxengine/view/camera.h"

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

void testCamera()
{
    std::cout << "testCamera() ...\n";


    using NumberType = double;

    // 构建摄像机计算过程

    NumberType viewWidth  = 800.0;
    NumberType viewHeight = 600.0;
    //
    using Vec3DoubleType = voxengine::math::Vec3<NumberType>;
    Vec3DoubleType                          camPos(500.0, 500.0, 500.0);
    Vec3DoubleType                          lookAtPos(0.0, 0.0, 0.0);
    Vec3DoubleType                          axisUp(0.0, 1.0, 0.0);
    voxengine::math::Camera<NumberType> camera{};
    camera.lookAtRH(camPos, lookAtPos, axisUp);
    camera.perspectiveRH(voxengine::math::degreeToRadian(45.0), viewWidth / viewHeight, 10.1, 2000.0);
    camera.setViewXY(0.0, 0.0);
    camera.setViewSize(viewWidth, viewHeight);
    camera.update();
    // 转换3d空间坐标到屏幕坐标
    Vec3DoubleType pos3D(100.0, 100.0, 0.0);
    Vec3DoubleType posResult{};
    camera.calcScreenNormalizeXYByWorldPos(pos3D, posResult);
    std::cout << "screen normalization 2d position(x=" << posResult.x << ", y=" << posResult.y << ")" << std::endl;
    camera.worldPosToScreen(pos3D);
    std::cout << "screen 2d position(x=" << pos3D.x << ", y=" << pos3D.y << ")" << std::endl;
}
} // namespace math
} // namespace voxengineDemo
} // namespace demo