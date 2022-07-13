#pragma once
#include <string>
#include "vec3.h"
namespace voxengine
{
namespace math
{
template <typename NumberType>
class Matrix4
{
public:
    Matrix4(NumberType* pfs, unsigned int index = 0) noexcept;
    Matrix4();
    virtual ~Matrix4();

    void        identity();
    NumberType* getLocalFS() const;
    void        copyFrom(const Matrix4<NumberType>& mat);
    void        copyRowFrom(unsigned int row_index, const Vec3<NumberType>& v3);
    void        transformVector3Self(Vec3<NumberType>& v3);
    void        transformVectorSelf(Vec3<NumberType>& v3);

    NumberType  determinant();
    Matrix4*    multiplyMatrices(Matrix4* a, Matrix4* b);
    Matrix4*    multiply(Matrix4* ma, Matrix4* mb = nullptr);
    Matrix4*    premultiply(Matrix4* ma);
    void        append(Matrix4& lhs);

    void        perspectiveRH(NumberType fovy, NumberType aspect, NumberType zNear, NumberType zFar);
    void        perspectiveLH(NumberType fovy, NumberType aspect, NumberType zNear, NumberType zFar);
    void        orthoRH(NumberType b, NumberType t, NumberType l, NumberType r, NumberType zNear, NumberType zFar);
    void        orthoLH(NumberType b, NumberType t, NumberType l, NumberType r, NumberType zNear, NumberType zFar);
    void        lookAtRH(Vec3<NumberType>& eye, Vec3<NumberType>& center, Vec3<NumberType>& up);
    void        lookAtLH(Vec3<NumberType>& eye, Vec3<NumberType>& center, Vec3<NumberType>& up);

    std::string toString();

private:
    NumberType*  m_fs;
    NumberType*  m_localFS;
    unsigned int m_index;
    unsigned int m_type;

    unsigned int m_localFSBytesTotal;

    const static NumberType s_initData[16];
    static Vec3<NumberType> s_v3;
};

} // namespace math
} // namespace voxengine