#include <cassert>
#include <cmath>
#include <iostream>
#include "mathDefine.h"
#include "Matrix4.h"
namespace voxengine
{
namespace math
{
template <typename NumberType>
const NumberType Matrix4<NumberType>::s_initData[16]{
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f};
template <typename NumberType>
Vec3<typename NumberType> Matrix4<typename NumberType>::s_v3{};
template <typename NumberType>
Matrix4<typename NumberType> Matrix4<typename NumberType>::s_mat{};
template <typename NumberType>
const NumberType Matrix4<typename NumberType>::s_minv = getPositiveMinValue<NumberType>();


template <typename NumberType>
Matrix4<NumberType>::Matrix4(NumberType* pfs, unsigned int index) noexcept
    :
    m_fs(pfs),
    m_index(index),
    m_localFSBytesTotal(sizeof(NumberType) << 4)
{
    assert(pfs != nullptr);
    m_localFS = pfs + index;
    m_type    = 1;
}

template <typename NumberType>
Matrix4<NumberType>::Matrix4()
{
    m_fs = new NumberType[16]{
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0};
    m_index             = 0;
    m_localFS           = m_fs;
    m_type              = 0;
    m_localFSBytesTotal = sizeof(NumberType) << 4;
}
template <typename NumberType>
Matrix4<NumberType>::~Matrix4()
{
    if (m_type == 0)
    {
        delete[] m_fs;
    }
    m_localFS = nullptr;
    m_fs      = nullptr;
    m_index   = 0;
    m_type    = 0;
}
template <typename NumberType>
void Matrix4<NumberType>::identity()
{
    std::memcpy(m_localFS, &s_initData, m_localFSBytesTotal);
}

template <typename NumberType>
NumberType* Matrix4<NumberType>::getLocalFS() const
{
    return m_localFS;
}
template <typename NumberType>
void Matrix4<NumberType>::copyFrom(const Matrix4<NumberType>& mat)
{
    //std::memcpy(m_localFS, reinterpret_cast<char*>( mat.getLocalFS()), m_localFSBytesTotal);
    std::memcpy(m_localFS, mat.getLocalFS(), m_localFSBytesTotal);
}

template <typename NumberType>
void Matrix4<NumberType>::copyRowFrom(unsigned int row_index, const Vec3<NumberType>& v3)
{
    switch (row_index)
    {
        case 0:
        {
            m_localFS[0]  = v3.x;
            m_localFS[4]  = v3.y;
            m_localFS[8]  = v3.z;
            m_localFS[12] = v3.w;
        }
        break;
        case 1:
        {
            m_localFS[1]  = v3.x;
            m_localFS[5]  = v3.y;
            m_localFS[9]  = v3.z;
            m_localFS[13] = v3.w;
        }
        break;
        case 2:
        {
            m_localFS[2]  = v3.x;
            m_localFS[6]  = v3.y;
            m_localFS[10] = v3.z;
            m_localFS[14] = v3.w;
        }
        break;
        case 3:
        {
            m_localFS[3]  = v3.x;
            m_localFS[7]  = v3.y;
            m_localFS[11] = v3.z;
            m_localFS[15] = v3.w;
        }
        break;
        default:
            break;
    }
}

template <typename NumberType>
void Matrix4<NumberType>::copyColumnTo(unsigned int column_index, Vec3<NumberType>& outV3) const
{
    column_index <<= 2;
    outV3.x = m_localFS[column_index];
    outV3.y = m_localFS[1 + column_index];
    outV3.z = m_localFS[2 + column_index];
    outV3.w = m_localFS[3 + column_index];
}

template <typename NumberType>
void Matrix4<NumberType>::transformVector3Self(Vec3<NumberType>& v3)
{
    NumberType x  = v3.x;
    NumberType y  = v3.y;
    NumberType z  = v3.z;
    auto       fs = m_localFS;
    v3.x          = x * fs[0] + y * fs[4] + z * fs[8] + fs[12];
    v3.y          = x * fs[1] + y * fs[5] + z * fs[9] + fs[13];
    v3.z          = x * fs[2] + y * fs[6] + z * fs[10] + fs[14];
}
template <typename NumberType>
void Matrix4<NumberType>::transformVector4Self(Vec3<NumberType>& v3)
{
    NumberType x  = v3.x;
    NumberType y  = v3.y;
    NumberType z  = v3.z;
    auto       fs = m_localFS;
    v3.set(
        x * fs[0] + y * fs[4] + z * fs[8] + fs[12],
        x * fs[1] + y * fs[5] + z * fs[9] + fs[13],
        x * fs[2] + y * fs[6] + z * fs[10] + fs[14],
        x * fs[3] + y * fs[7] + z * fs[11] + fs[15]);
}


template <typename NumberType>
NumberType Matrix4<NumberType>::determinant()
{
    auto lfs = m_localFS;
    return (lfs[0] * lfs[5] - lfs[4] * lfs[1]) *
        (lfs[10] * lfs[15] - lfs[14] * lfs[11]) -
        (lfs[0] * lfs[9] - lfs[8] * lfs[1]) *
        (lfs[6] * lfs[15] - lfs[14] * lfs[7]) +
        (lfs[0] * lfs[13] - lfs[12] * lfs[1]) *
        (lfs[6] * lfs[11] - lfs[10] * lfs[7]) +
        (lfs[4] * lfs[9] - lfs[8] * lfs[5]) *
        (lfs[2] * lfs[15] - lfs[14] * lfs[3]) -
        (lfs[4] * lfs[13] - lfs[12] * lfs[5]) *
        (lfs[2] * lfs[11] - lfs[10] * lfs[3]) +
        (lfs[8] * lfs[13] - lfs[12] * lfs[9]) *
        (lfs[2] * lfs[7] - lfs[6] * lfs[3]);
}

template <typename NumberType>
Matrix4<NumberType>* Matrix4<NumberType>::multiplyMatrices(Matrix4<NumberType>* a, Matrix4<NumberType>* b)
{
    auto ae = a->m_localFS;
    auto be = b->m_localFS;
    auto te = m_localFS;

    auto a11 = ae[0], a12 = ae[4], a13 = ae[8], a14 = ae[12];
    auto a21 = ae[1], a22 = ae[5], a23 = ae[9], a24 = ae[13];
    auto a31 = ae[2], a32 = ae[6], a33 = ae[10], a34 = ae[14];
    auto a41 = ae[3], a42 = ae[7], a43 = ae[11], a44 = ae[15];

    auto b11 = be[0], b12 = be[4], b13 = be[8], b14 = be[12];
    auto b21 = be[1], b22 = be[5], b23 = be[9], b24 = be[13];
    auto b31 = be[2], b32 = be[6], b33 = be[10], b34 = be[14];
    auto b41 = be[3], b42 = be[7], b43 = be[11], b44 = be[15];

    te[0]  = a11 * b11 + a12 * b21 + a13 * b31 + a14 * b41;
    te[4]  = a11 * b12 + a12 * b22 + a13 * b32 + a14 * b42;
    te[8]  = a11 * b13 + a12 * b23 + a13 * b33 + a14 * b43;
    te[12] = a11 * b14 + a12 * b24 + a13 * b34 + a14 * b44;

    te[1]  = a21 * b11 + a22 * b21 + a23 * b31 + a24 * b41;
    te[5]  = a21 * b12 + a22 * b22 + a23 * b32 + a24 * b42;
    te[9]  = a21 * b13 + a22 * b23 + a23 * b33 + a24 * b43;
    te[13] = a21 * b14 + a22 * b24 + a23 * b34 + a24 * b44;

    te[2]  = a31 * b11 + a32 * b21 + a33 * b31 + a34 * b41;
    te[6]  = a31 * b12 + a32 * b22 + a33 * b32 + a34 * b42;
    te[10] = a31 * b13 + a32 * b23 + a33 * b33 + a34 * b43;
    te[14] = a31 * b14 + a32 * b24 + a33 * b34 + a34 * b44;

    te[3]  = a41 * b11 + a42 * b21 + a43 * b31 + a44 * b41;
    te[7]  = a41 * b12 + a42 * b22 + a43 * b32 + a44 * b42;
    te[11] = a41 * b13 + a42 * b23 + a43 * b33 + a44 * b43;
    te[15] = a41 * b14 + a42 * b24 + a43 * b34 + a44 * b44;

    return this;
}
template <typename NumberType>
Matrix4<NumberType>* Matrix4<NumberType>::multiply(Matrix4<NumberType>* ma, Matrix4<NumberType>* mb)
{
    if (ma != nullptr && mb != nullptr)
    {
        return multiplyMatrices(ma, mb);
    }
    else if (ma != nullptr)
    {
        return multiplyMatrices(this, ma);
    }
    return this;
}

template <typename NumberType>
Matrix4<NumberType>* Matrix4<NumberType>::premultiply(Matrix4<NumberType>* m)
{
    if (m != this && m != nullptr)
    {
        return multiplyMatrices(m, this);
    }
    return this;
}

template <typename NumberType>
void Matrix4<NumberType>::append(const Matrix4& lhs)
{
    auto lfs = lhs.getLocalFS();
    auto sfs = m_localFS;

    auto m111 = sfs[0];
    auto m121 = sfs[4];
    auto m131 = sfs[8];
    auto m141 = sfs[12];
    auto m112 = sfs[1];
    auto m122 = sfs[5];
    auto m132 = sfs[9];
    auto m142 = sfs[13];
    auto m113 = sfs[2];
    auto m123 = sfs[6];
    auto m133 = sfs[10];
    auto m143 = sfs[14];
    auto m114 = sfs[3];
    auto m124 = sfs[7];
    auto m134 = sfs[11];
    auto m144 = sfs[15];
    auto m211 = lfs[0];
    auto m221 = lfs[4];
    auto m231 = lfs[8];
    auto m241 = lfs[12];
    auto m212 = lfs[1];
    auto m222 = lfs[5];
    auto m232 = lfs[9];
    auto m242 = lfs[13];
    auto m213 = lfs[2];
    auto m223 = lfs[6];
    auto m233 = lfs[10];
    auto m243 = lfs[14];
    auto m214 = lfs[3];
    auto m224 = lfs[7];
    auto m234 = lfs[11];
    auto m244 = lfs[15];

    sfs[0]  = m111 * m211 + m112 * m221 + m113 * m231 + m114 * m241;
    sfs[1]  = m111 * m212 + m112 * m222 + m113 * m232 + m114 * m242;
    sfs[2]  = m111 * m213 + m112 * m223 + m113 * m233 + m114 * m243;
    sfs[3]  = m111 * m214 + m112 * m224 + m113 * m234 + m114 * m244;
    sfs[4]  = m121 * m211 + m122 * m221 + m123 * m231 + m124 * m241;
    sfs[5]  = m121 * m212 + m122 * m222 + m123 * m232 + m124 * m242;
    sfs[6]  = m121 * m213 + m122 * m223 + m123 * m233 + m124 * m243;
    sfs[7]  = m121 * m214 + m122 * m224 + m123 * m234 + m124 * m244;
    sfs[8]  = m131 * m211 + m132 * m221 + m133 * m231 + m134 * m241;
    sfs[9]  = m131 * m212 + m132 * m222 + m133 * m232 + m134 * m242;
    sfs[10] = m131 * m213 + m132 * m223 + m133 * m233 + m134 * m243;
    sfs[11] = m131 * m214 + m132 * m224 + m133 * m234 + m134 * m244;
    sfs[12] = m141 * m211 + m142 * m221 + m143 * m231 + m144 * m241;
    sfs[13] = m141 * m212 + m142 * m222 + m143 * m232 + m144 * m242;
    sfs[14] = m141 * m213 + m142 * m223 + m143 * m233 + m144 * m243;
    sfs[15] = m141 * m214 + m142 * m224 + m143 * m234 + m144 * m244;
}

template <typename NumberType>
void Matrix4<NumberType>::append3x3(const Matrix4& lhs)
{
    auto lfs = lhs.getLocalFS();
    auto sfs = m_localFS;

    auto m111 = sfs[0];
    auto m121 = sfs[4];
    auto m131 = sfs[8];
    auto m112 = sfs[1];
    auto m122 = sfs[5];
    auto m132 = sfs[9];
    auto m113 = sfs[2];
    auto m123 = sfs[6];
    auto m133 = sfs[10];
    auto m211 = lfs[0];
    auto m221 = lfs[4];
    auto m231 = lfs[8];
    auto m212 = lfs[1];
    auto m222 = lfs[5];
    auto m232 = lfs[9];
    auto m213 = lfs[2];
    auto m223 = lfs[6];
    auto m233 = lfs[10];
    sfs[0]    = m111 * m211 + m112 * m221 + m113 * m231;
    sfs[1]    = m111 * m212 + m112 * m222 + m113 * m232;
    sfs[2]    = m111 * m213 + m112 * m223 + m113 * m233;
    sfs[4]    = m121 * m211 + m122 * m221 + m123 * m231;
    sfs[5]    = m121 * m212 + m122 * m222 + m123 * m232;
    sfs[6]    = m121 * m213 + m122 * m223 + m123 * m233;
    sfs[8]    = m131 * m211 + m132 * m221 + m133 * m231;
    sfs[9]    = m131 * m212 + m132 * m222 + m133 * m232;
    sfs[10]   = m131 * m213 + m132 * m223 + m133 * m233;
}

template <typename NumberType>
void Matrix4<NumberType>::appendRotationPivot(NumberType radian, const Vec3<NumberType>& axis, Vec3<NumberType>* pivotPoint)
{
    s_mat.identity();
    s_mat.getAxisRotation(axis.x, axis.y, axis.z, radian);
    //std::cout << "appendRotationPivot 1 : " << s_mat.toString() << std::endl;
    if (pivotPoint == nullptr)
    {
        //std::cout << "appendRotationPivot A.\n";
        auto& pv = Vec3<NumberType>::Z_AXIS;
        s_mat.appendTranslationXYZ(pv.x, pv.y, pv.z);
    }
    else
    {
        //std::cout << "appendRotationPivot B.\n";
        auto& pv = (*pivotPoint);
        s_mat.appendTranslationXYZ(pv.x, pv.y, pv.z);
    }
    //std::cout << "appendRotationPivot 2 : " << s_mat.toString() << std::endl;
    append(s_mat);
    //std::cout << "appendRotationPivot 3 : " << this->toString() << std::endl;
}
template <typename NumberType>
void Matrix4<NumberType>::appendRotation(NumberType radian, const Vec3<NumberType>& axis)
{
    s_mat.identity();
    s_mat.getAxisRotation(axis.x, axis.y, axis.z, radian);
    append(s_mat);
}

template <typename NumberType>
void Matrix4<NumberType>::appendRotationX(NumberType radian)
{
    s_mat.rotationX(radian);
    append3x3(s_mat);
}

template <typename NumberType>
void Matrix4<NumberType>::appendRotationY(NumberType radian)
{
    s_mat.rotationY(radian);
    append3x3(s_mat);
}
template <typename NumberType>
void Matrix4<NumberType>::appendRotationZ(NumberType radian)
{
    s_mat.rotationZ(radian);
    append3x3(s_mat);
}
// 用欧拉角形式旋转(heading->pitch->bank) => (y->x->z)
template <typename NumberType>
void Matrix4<NumberType>::appendRotationEulerAngle(NumberType radianX, NumberType radianY, NumberType radianZ)
{
    s_mat.rotationY(radianY);
    append3x3(s_mat);
    s_mat.rotationX(radianX);
    append3x3(s_mat);
    s_mat.rotationZ(radianZ);
    append3x3(s_mat);
}

template <typename NumberType>
Matrix4<NumberType>* Matrix4<NumberType>::setScale(const Vec3<NumberType>& v3)
{
    auto sfs = m_localFS;
    sfs[0]   = v3.x;
    sfs[5]   = v3.y;
    sfs[10]  = v3.z;
    return this;
}

template <typename NumberType>
void Matrix4<NumberType>::setScaleXYZ(NumberType xScale, NumberType yScale, NumberType zScale)
{
    auto sfs = m_localFS;
    sfs[0]   = xScale;
    sfs[5]   = yScale;
    sfs[10]  = zScale;
}

template <typename NumberType>
void Matrix4<NumberType>::getScale(Vec3<NumberType>& outV3)
{
    auto sfs = m_localFS;
    outV3.x  = sfs[0];
    outV3.y  = sfs[5];
    outV3.z  = sfs[10];
}

template <typename NumberType>
void Matrix4<NumberType>::setRotationEulerAngle(NumberType radianX, NumberType radianY, NumberType radianZ)
{
    auto sfs = m_localFS;
    //auto& sx = sfs[0];
    //auto& sy = sfs[5];
    //auto& sz = sfs[10];

    auto cosX = std::cos(radianX);
    auto sinX = std::sin(radianX);
    auto cosY = std::cos(radianY);
    auto sinY = std::sin(radianY);
    auto cosZ = std::cos(radianZ);
    auto sinZ = std::sin(radianZ);

    auto cosZsinY   = cosZ * sinY;
    auto sinZsinY   = sinZ * sinY;
    auto cosYscaleX = cosY * sfs[0];
    auto sinXscaleY = sinX * sfs[5];
    auto cosXscaleY = cosX * sfs[5];
    auto cosXscaleZ = cosX * sfs[10];
    auto sinXscaleZ = sinX * sfs[10];

    sfs[1]  = sinZ * cosYscaleX;
    sfs[2]  = -sinY * sfs[0];
    sfs[0]  = cosZ * cosYscaleX;
    sfs[4]  = cosZsinY * sinXscaleY - sinZ * cosXscaleY;
    sfs[8]  = cosZsinY * cosXscaleZ + sinZ * sinXscaleZ;
    sfs[5]  = sinZsinY * sinXscaleY + cosZ * cosXscaleY;
    sfs[9]  = sinZsinY * cosXscaleZ - cosZ * sinXscaleZ;
    sfs[6]  = cosY * sinXscaleY;
    sfs[10] = cosY * cosXscaleZ;
}

template <typename NumberType>
void Matrix4<NumberType>::setRotationEulerAngleWithTriFunc(NumberType cosX, NumberType sinX, NumberType cosY, NumberType sinY, NumberType cosZ, NumberType sinZ)
{
    auto sfs = m_localFS;

    auto cosZsinY   = cosZ * sinY;
    auto sinZsinY   = sinZ * sinY;
    auto cosYscaleX = cosY * sfs[0];
    auto sinXscaleY = sinX * sfs[5];
    auto cosXscaleY = cosX * sfs[5];
    auto cosXscaleZ = cosX * sfs[10];
    auto sinXscaleZ = sinX * sfs[10];

    sfs[1]  = sinZ * cosYscaleX;
    sfs[2]  = -sinY * sfs[0];
    sfs[0]  = cosZ * cosYscaleX;
    sfs[4]  = cosZsinY * sinXscaleY - sinZ * cosXscaleY;
    sfs[8]  = cosZsinY * cosXscaleZ + sinZ * sinXscaleZ;
    sfs[5]  = sinZsinY * sinXscaleY + cosZ * cosXscaleY;
    sfs[9]  = sinZsinY * cosXscaleZ - cosZ * sinXscaleZ;
    sfs[6]  = cosY * sinXscaleY;
    sfs[10] = cosY * cosXscaleZ;
}

template <typename NumberType>
void Matrix4<NumberType>::getAxisRotation(NumberType x, NumberType y, NumberType z, NumberType radian)
{
    radian    = -radian;
    auto sfs  = m_localFS;
    auto s    = std::sin(radian);
    auto c    = std::cos(radian);
    auto t    = 1.0f - c;
    sfs[0]    = c + x * x * t;
    sfs[5]    = c + y * y * t;
    sfs[10]   = c + z * z * t;
    auto tmp1 = x * y * t;
    auto tmp2 = z * s;
    sfs[4]    = tmp1 + tmp2;
    sfs[1]    = tmp1 - tmp2;
    tmp1      = x * z * t;
    tmp2      = y * s;
    sfs[8]    = tmp1 - tmp2;
    sfs[2]    = tmp1 + tmp2;
    tmp1      = y * z * t;
    tmp2      = x * s;
    sfs[9]    = tmp1 + tmp2;
    sfs[6]    = tmp1 - tmp2;
}

template <typename NumberType>
void Matrix4<NumberType>::rotationX(NumberType radian)
{
    auto s = std::sin(radian);
    auto c = std::cos(radian);

    m_localFS[0]  = 1.0f;
    m_localFS[1]  = 0.0f;
    m_localFS[2]  = 0.0f;
    m_localFS[4]  = 0.0f;
    m_localFS[5]  = c;
    m_localFS[6]  = s;
    m_localFS[8]  = 0.0f;
    m_localFS[9]  = -s;
    m_localFS[10] = c;
}
template <typename NumberType>
void Matrix4<NumberType>::rotationY(NumberType radian)
{
    auto s        = std::sin(radian);
    auto c        = std::cos(radian);
    m_localFS[0]  = c;
    m_localFS[1]  = 0.0f;
    m_localFS[2]  = -s;
    m_localFS[4]  = 0.0f;
    m_localFS[5]  = 1.0f;
    m_localFS[6]  = 0.0f;
    m_localFS[8]  = s;
    m_localFS[9]  = 0.0f;
    m_localFS[10] = c;
}
template <typename NumberType>
void Matrix4<NumberType>::rotationZ(NumberType radian)
{
    auto s        = std::sin(radian);
    auto c        = std::cos(radian);
    m_localFS[0]  = c;
    m_localFS[1]  = s;
    m_localFS[2]  = 0.0f;
    m_localFS[4]  = -s;
    m_localFS[5]  = c;
    m_localFS[6]  = 0.0f;
    m_localFS[8]  = 0.0f;
    m_localFS[9]  = 0.0f;
    m_localFS[10] = 1.0f;
}

template <typename NumberType>
Matrix4<NumberType>* Matrix4<NumberType>::extractRotation(const Matrix4& m)
{

    // this method does not support reflection matrices

    auto  te = m_localFS;
    auto  me = m.getLocalFS();
    auto& v3 = s_v3;
    m.copyColumnTo(0, v3);
    auto scaleX = 1.0f / v3.getLength();
    m.copyColumnTo(1, v3);
    auto scaleY = 1.0f / v3.getLength();
    m.copyColumnTo(2, v3);
    auto scaleZ = 1.0f / v3.getLength();

    te[0] = me[0] * scaleX;
    te[1] = me[1] * scaleX;
    te[2] = me[2] * scaleX;
    te[3] = 0.0f;

    te[4] = me[4] * scaleY;
    te[5] = me[5] * scaleY;
    te[6] = me[6] * scaleY;
    te[7] = 0.0f;

    te[8]  = me[8] * scaleZ;
    te[9]  = me[9] * scaleZ;
    te[10] = me[10] * scaleZ;
    te[11] = 0.0f;

    te[12] = 0.0f;
    te[13] = 0.0f;
    te[14] = 0.0f;
    te[15] = 1.0f;

    return this;
}

template <typename NumberType>
Matrix4<NumberType>* Matrix4<NumberType>::copyTranslation(const Matrix4& m)
{
    auto te = m_localFS;
    auto me = m.getLocalFS();

    te[12] = me[12];
    te[13] = me[13];
    te[14] = me[14];

    return this;
}
template <typename NumberType>
void Matrix4<NumberType>::setTranslationXYZ(NumberType px, NumberType py, NumberType pz)
{
    m_localFS[12] = px;
    m_localFS[13] = py;
    m_localFS[14] = pz;
}
template <typename NumberType>
void Matrix4<NumberType>::setTranslation(const Vec3<NumberType>& v3)
{
    m_localFS[12] = v3.x;
    m_localFS[13] = v3.y;
    m_localFS[14] = v3.z;
}
template <typename NumberType>
void Matrix4<NumberType>::appendScaleXYZ(NumberType xScale, NumberType yScale, NumberType zScale)
{
    auto& sys = m_localFS;
    sys[0] *= xScale;
    sys[1] *= xScale;
    sys[2] *= xScale;
    sys[3] *= xScale;
    sys[4] *= yScale;
    sys[5] *= yScale;
    sys[6] *= yScale;
    sys[7] *= yScale;
    sys[8] *= zScale;
    sys[9] *= zScale;
    sys[10] *= zScale;
    sys[11] *= zScale;
}

template <typename NumberType>
void Matrix4<NumberType>::appendScaleXY(NumberType xScale, NumberType yScale)
{
    m_localFS[0] *= xScale;
    m_localFS[1] *= xScale;
    m_localFS[2] *= xScale;
    m_localFS[3] *= xScale;
    m_localFS[4] *= yScale;
    m_localFS[5] *= yScale;
    m_localFS[6] *= yScale;
    m_localFS[7] *= yScale;
}

template <typename NumberType>
void Matrix4<NumberType>::appendTranslationXYZ(NumberType px, NumberType py, NumberType pz)
{
    m_localFS[12] += px;
    m_localFS[13] += py;
    m_localFS[14] += pz;
}

template <typename NumberType>
void Matrix4<NumberType>::appendTranslation(const Vec3<NumberType>& v3)
{
    m_localFS[12] += v3.x;
    m_localFS[13] += v3.y;
    m_localFS[14] += v3.z;
}


template <typename NumberType>
bool Matrix4<NumberType>::invert()
{
    auto d = determinant();
    bool invertable = std::abs(d) > s_minv;
    if (invertable)
    {
        auto sfs = m_localFS;
        d        = 1.0f / d;
        auto m11 = sfs[0];
        auto m21 = sfs[4];
        auto m31 = sfs[8];
        auto m41 = sfs[12];
        auto m12 = sfs[1];
        auto m22 = sfs[5];
        auto m32 = sfs[9];
        auto m42 = sfs[13];
        auto m13 = sfs[2];
        auto m23 = sfs[6];
        auto m33 = sfs[10];
        auto m43 = sfs[14];
        auto m14 = sfs[3];
        auto m24 = sfs[7];
        auto m34 = sfs[11];
        auto m44 = sfs[15];
        sfs[0]   = d * (m22 * (m33 * m44 - m43 * m34) - m32 * (m23 * m44 - m43 * m24) + m42 * (m23 * m34 - m33 * m24));
        sfs[1]   = -d * (m12 * (m33 * m44 - m43 * m34) - m32 * (m13 * m44 - m43 * m14) + m42 * (m13 * m34 - m33 * m14));
        sfs[2]   = d * (m12 * (m23 * m44 - m43 * m24) - m22 * (m13 * m44 - m43 * m14) + m42 * (m13 * m24 - m23 * m14));
        sfs[3]   = -d * (m12 * (m23 * m34 - m33 * m24) - m22 * (m13 * m34 - m33 * m14) + m32 * (m13 * m24 - m23 * m14));
        sfs[4]   = -d * (m21 * (m33 * m44 - m43 * m34) - m31 * (m23 * m44 - m43 * m24) + m41 * (m23 * m34 - m33 * m24));
        sfs[5]   = d * (m11 * (m33 * m44 - m43 * m34) - m31 * (m13 * m44 - m43 * m14) + m41 * (m13 * m34 - m33 * m14));
        sfs[6]   = -d * (m11 * (m23 * m44 - m43 * m24) - m21 * (m13 * m44 - m43 * m14) + m41 * (m13 * m24 - m23 * m14));
        sfs[7]   = d * (m11 * (m23 * m34 - m33 * m24) - m21 * (m13 * m34 - m33 * m14) + m31 * (m13 * m24 - m23 * m14));
        sfs[8]   = d * (m21 * (m32 * m44 - m42 * m34) - m31 * (m22 * m44 - m42 * m24) + m41 * (m22 * m34 - m32 * m24));
        sfs[9]   = -d * (m11 * (m32 * m44 - m42 * m34) - m31 * (m12 * m44 - m42 * m14) + m41 * (m12 * m34 - m32 * m14));
        sfs[10]  = d * (m11 * (m22 * m44 - m42 * m24) - m21 * (m12 * m44 - m42 * m14) + m41 * (m12 * m24 - m22 * m14));
        sfs[11]  = -d * (m11 * (m22 * m34 - m32 * m24) - m21 * (m12 * m34 - m32 * m14) + m31 * (m12 * m24 - m22 * m14));
        sfs[12]  = -d * (m21 * (m32 * m43 - m42 * m33) - m31 * (m22 * m43 - m42 * m23) + m41 * (m22 * m33 - m32 * m23));
        sfs[13]  = d * (m11 * (m32 * m43 - m42 * m33) - m31 * (m12 * m43 - m42 * m13) + m41 * (m12 * m33 - m32 * m13));
        sfs[14]  = -d * (m11 * (m22 * m43 - m42 * m23) - m21 * (m12 * m43 - m42 * m13) + m41 * (m12 * m23 - m22 * m13));
        sfs[15]  = d * (m11 * (m22 * m33 - m32 * m23) - m21 * (m12 * m33 - m32 * m13) + m31 * (m12 * m23 - m22 * m13));
    };
    return invertable;
}
template <typename NumberType>
Matrix4<NumberType>* Matrix4<NumberType>::invertThis()
{
    invert();
    return this;
}
template <typename NumberType>
Matrix4<NumberType> Matrix4<NumberType>::clone()
{
    NumberType fs[16];
    std::memcpy(fs, m_localFS, m_localFSBytesTotal);
    return Matrix4(fs);
}

template <typename NumberType>
void Matrix4<NumberType>::perspectiveRH(NumberType fovy, NumberType aspect, NumberType zNear, NumberType zFar)
{
    //assert(abs(aspect - std::numeric_limits<float>::epsilon()) > minFloatValue)
    NumberType tanHalfFovy = std::tan(fovy * 0.5f);
    identity();
    m_localFS[0]  = 1.0f / (aspect * tanHalfFovy);
    m_localFS[5]  = 1.0f / tanHalfFovy;
    m_localFS[10] = -(zFar + zNear) / (zFar - zNear);
    m_localFS[11] = -1.0f;
    m_localFS[14] = -(2.0f * zFar * zNear) / (zFar - zNear);
}
template <typename NumberType>
void Matrix4<NumberType>::perspectiveLH(NumberType fovy, NumberType aspect, NumberType zNear, NumberType zFar)
{
    //assert(abs(aspect - std::numeric_limits<float>::epsilon()) > minFloatValue)

    NumberType tanHalfFovy = std::tan(fovy * 0.5f);
    identity();
    m_localFS[0]  = 1.0f / (aspect * tanHalfFovy);
    m_localFS[5]  = 1.0f / tanHalfFovy;
    m_localFS[10] = (zFar + zNear) / (zFar - zNear);
    m_localFS[11] = 1.0f;
    m_localFS[14] = (2.0f * zFar * zNear) / (zFar - zNear);
}

template <typename NumberType>
void Matrix4<NumberType>::orthoRH(NumberType b, NumberType t, NumberType l, NumberType r, NumberType zNear, NumberType zFar)
{
    identity();
    m_localFS[0]  = 2.0f / (r - l);
    m_localFS[5]  = 2.0f / (t - b);
    m_localFS[10] = -2.0f / (zFar - zNear);
    m_localFS[12] = -(r + l) / (r - l);
    m_localFS[13] = -(t + b) / (t - b);
    m_localFS[14] = -(zFar + zNear) / (zFar - zNear);
    m_localFS[15] = 1.0f;
}


template <typename NumberType>
void Matrix4<NumberType>::orthoLH(NumberType b, NumberType t, NumberType l, NumberType r, NumberType zNear, NumberType zFar)
{
    identity();
    m_localFS[0]  = 2.0f / (r - l);
    m_localFS[5]  = 2.0f / (t - b);
    m_localFS[10] = 2.0f / (zFar - zNear);
    m_localFS[12] = -(r + l) / (r - l);
    m_localFS[13] = -(t + b) / (t - b);
    m_localFS[14] = -(zFar + zNear) / (zFar - zNear);
    m_localFS[15] = 1.0f;
}
template <typename NumberType>
void Matrix4<NumberType>::lookAtRH(Vec3<NumberType>& eye, Vec3<NumberType>& center, Vec3<NumberType>& up)
{
    identity();
    auto f = center.subtract(eye);
    f.normalize();
    auto s = f.crossProduct(up);
    s.normalize();
    auto u = s.crossProduct(f);
    s.w    = -s.dot(eye);
    u.w    = -u.dot(eye);
    f.w    = f.dot(eye);
    f.negate();
    copyRowFrom(0, s);
    copyRowFrom(1, u);
    copyRowFrom(2, f);
}
template <typename NumberType>
void Matrix4<NumberType>::lookAtLH(Vec3<NumberType>& eye, Vec3<NumberType>& center, Vec3<NumberType>& up)
{
    identity();
    auto f = center.subtract(eye);
    f.normalize();
    auto s = f.crossProduct(up);
    s.normalize();
    auto u = s.crossProduct(f);
    s.w    = -s.dot(eye);
    u.w    = -u.dot(eye);
    f.w    = -f.dot(eye);
    copyRowFrom(0, s);
    copyRowFrom(1, u);
    copyRowFrom(2, f);
}


template <typename NumberType>
std::string Matrix4<NumberType>::toString()
{
    std::string str = "\n";
    str += std::to_string(m_localFS[0]) + "," + std::to_string(m_localFS[1]) + "," + std::to_string(m_localFS[2]) + "," + std::to_string(m_localFS[3]) + "\n";
    str += std::to_string(m_localFS[4]) + "," + std::to_string(m_localFS[5]) + "," + std::to_string(m_localFS[6]) + "," + std::to_string(m_localFS[7]) + "\n";
    str += std::to_string(m_localFS[8]) + "," + std::to_string(m_localFS[9]) + "," + std::to_string(m_localFS[10]) + "," + std::to_string(m_localFS[11]) + "\n";
    str += std::to_string(m_localFS[12]) + "," + std::to_string(m_localFS[13]) + "," + std::to_string(m_localFS[14]) + "," + std::to_string(m_localFS[15]) + "\n";
    return str;
}



template <typename NumberType>
void __$templateConstructMatrix4(NumberType value)
{
    Matrix4<NumberType> ma{};
    Matrix4<NumberType> mb{};
    Vec3<NumberType>    va;
    ma.identity();
    ma.perspectiveRH(value, value, value, value);
    ma.copyFrom(mb);
    ma.copyRowFrom(0, va);
    ma.copyColumnTo(0, va);
    ma.transformVector3Self(va);
    ma.transformVector4Self(va);

    ma.determinant();
    ma.multiplyMatrices(&mb, &mb);
    ma.multiply(&mb, &mb);
    ma.premultiply(&mb);
    ma.append(mb);
    ma.append3x3(mb);

    ma.appendRotationPivot(value, va, &va);
    ma.appendRotation(value, va);
    ma.appendRotationX(value);
    ma.appendRotationY(value);
    ma.appendRotationZ(value);

    // 用欧拉角形式旋转(heading->pitch->bank) => (y->x->z)
    ma.appendRotationEulerAngle(value, value, value);
    ma.setScale(va);
    ma.setScaleXYZ(value, value, value);
    ma.getScale(va);

    ma.setRotationEulerAngle(value, value, value);
    ma.setRotationEulerAngleWithTriFunc(value, value, value, value, value, value);
    ma.getAxisRotation(value, value, value, value);
    ma.rotationX(value);
    ma.rotationY(value);
    ma.rotationZ(value);

    ma.extractRotation(mb);
    ma.copyTranslation(mb);

    ma.setTranslationXYZ(value, value, value);
    ma.setTranslation(va);
    ma.appendScaleXYZ(value, value, value);

    ma.appendScaleXY(value, value);
    ma.appendTranslationXYZ(value, value, value);
    ma.appendTranslation(va);

    ma.invert();
    ma.invertThis();

    auto cm = ma.clone();

    ma.perspectiveRH(value, value, value, value);
    ma.perspectiveLH(value, value, value, value);
    ma.orthoRH(value, value, value, value, value, value);
    ma.orthoLH(value, value, value, value, value, value);
    ma.lookAtRH(va, va, va);
    ma.lookAtLH(va, va, va);

    ma.toString();
}
void __$templateImplyMatrix4()
{
    __$templateConstructMatrix4(1.0);
    __$templateConstructMatrix4(1.0f);
}
} // namespace math
} // namespace voxengine