#include <cmath>
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
void Matrix4<NumberType>::transformVectorSelf(Vec3<NumberType>& v3)
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

    auto &a11 = ae[0], a12 = ae[4], a13 = ae[8], a14 = ae[12];
    auto &a21 = ae[1], a22 = ae[5], a23 = ae[9], a24 = ae[13];
    auto &a31 = ae[2], a32 = ae[6], a33 = ae[10], a34 = ae[14];
    auto &a41 = ae[3], a42 = ae[7], a43 = ae[11], a44 = ae[15];

    auto &b11 = be[0], b12 = be[4], b13 = be[8], b14 = be[12];
    auto &b21 = be[1], b22 = be[5], b23 = be[9], b24 = be[13];
    auto &b31 = be[2], b32 = be[6], b33 = be[10], b34 = be[14];
    auto &b41 = be[3], b42 = be[7], b43 = be[11], b44 = be[15];

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
void Matrix4<NumberType>::append(Matrix4& lhs)
{
    auto lfs = lhs.getLocalFS();
    auto sfs = m_localFS;

    auto& m111 = sfs[0];
    auto& m121 = sfs[4];
    auto& m131 = sfs[8];
    auto& m141 = sfs[12];
    auto& m112 = sfs[1];
    auto& m122 = sfs[5];
    auto& m132 = sfs[9];
    auto& m142 = sfs[13];
    auto& m113 = sfs[2];
    auto& m123 = sfs[6];
    auto& m133 = sfs[10];
    auto& m143 = sfs[14];
    auto& m114 = sfs[3];
    auto& m124 = sfs[7];
    auto& m134 = sfs[11];
    auto& m144 = sfs[15];
    auto& m211 = lfs[0];
    auto& m221 = lfs[4];
    auto& m231 = lfs[8];
    auto& m241 = lfs[12];
    auto& m212 = lfs[1];
    auto& m222 = lfs[5];
    auto& m232 = lfs[9];
    auto& m242 = lfs[13];
    auto& m213 = lfs[2];
    auto& m223 = lfs[6];
    auto& m233 = lfs[10];
    auto& m243 = lfs[14];
    auto& m214 = lfs[3];
    auto& m224 = lfs[7];
    auto& m234 = lfs[11];
    auto& m244 = lfs[15];

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
    ma.transformVector3Self(va);
    ma.transformVectorSelf(va);

    ma.determinant();
    ma.multiplyMatrices(&mb, &mb);
    ma.multiply(&mb, &mb);
    ma.premultiply(&mb);
    ma.append(mb);

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