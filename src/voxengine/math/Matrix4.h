#pragma once
#include <string>
#include "Vec3.h"
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
    void        copyColumnTo(unsigned int column_index, Vec3<NumberType>& outV3) const;
    void        transformVector3Self(Vec3<NumberType>& v3);
    void        transformVector4Self(Vec3<NumberType>& v3);

    NumberType  determinant();
    Matrix4*    multiplyMatrices(Matrix4* a, Matrix4* b);
    Matrix4*    multiply(Matrix4* ma, Matrix4* mb = nullptr);
    Matrix4*    premultiply(Matrix4* ma);
    void        append(const Matrix4& lhs);
    void        append3x3(const Matrix4& lhs);

    void appendRotationPivot(NumberType radian, const Vec3<NumberType>& axis, Vec3<NumberType>* pivotPoint);
    void appendRotation(NumberType radian, const Vec3<NumberType>& axis);
    void appendRotationX(NumberType radian);
    void appendRotationY(NumberType radian);
    void appendRotationZ(NumberType radian);

    // 用欧拉角形式旋转(heading->pitch->bank) => (y->x->z)
    void appendRotationEulerAngle(NumberType radianX, NumberType radianY, NumberType radianZ);
    Matrix4* setScale(const Vec3<NumberType>& v3);
    void setScaleXYZ(NumberType xScale, NumberType yScale, NumberType zScale);
    void getScale(Vec3<NumberType>& outV3);

    void setRotationEulerAngle(NumberType radianX, NumberType radianY, NumberType radianZ);
    void setRotationEulerAngleWithTriFunc(NumberType cosX, NumberType sinX, NumberType cosY, NumberType sinY, NumberType cosZ, NumberType sinZ);
    void getAxisRotation(NumberType x, NumberType y, NumberType z, NumberType radian);
    void rotationX(NumberType radian);
    void rotationY(NumberType radian);
    void rotationZ(NumberType radian);

	Matrix4* extractRotation(const Matrix4& m);
    Matrix4* copyTranslation(const Matrix4& m);

    void setTranslationXYZ(NumberType px, NumberType py, NumberType pz);
    void setTranslation(const Vec3<NumberType>& v3);
    void appendScaleXYZ(NumberType xScale, NumberType yScale, NumberType zScale);

    void appendScaleXY(NumberType xScale, NumberType yScale);
    void appendTranslationXYZ(NumberType px, NumberType py, NumberType pz);
    void appendTranslation(const Vec3<NumberType>& v3);
    bool invert();
    Matrix4* invertThis();

    Matrix4 clone();


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
    static Matrix4          s_mat;
    const static NumberType  s_minv;
};

} // namespace math
} // namespace voxengine