#pragma once
#include <string>
#include "Vec3.h"
#include "IMatrix4.h"
#include "IEuler.h"
namespace voxengine
{
namespace math
{
template <typename T>
class Matrix4 : public IMatrix4<T>
{
public:
    Matrix4(T* pfs, unsigned int index = 0) noexcept;
    Matrix4() noexcept;
    virtual ~Matrix4();

    void        identity();
    T* getLocalFS() const;
    void        copyFrom(const Matrix4<T>& mat);
    void        copyRowFrom(unsigned int row_index, const Vec3<T>& v3);
    void        copyColumnTo(unsigned int column_index, Vec3<T>& outV3) const;
    void        transformVector3Self(Vec3<T>& v3);
    void        transformVector4Self(Vec3<T>& v3);

    T           determinant();
    void        multiplyMatEle(T* ae, T* be, T* te);
    Matrix4&    multiplyMatrices(Matrix4& a, Matrix4& b);
    Matrix4&    multiplyMatrices(Matrix4&& a, Matrix4&& b);
    Matrix4&    multiply(Matrix4&& ma, Matrix4&& mb);
    Matrix4&    multiply(Matrix4&& ma);
    Matrix4&    premultiply(Matrix4&& ma);
    void        append(const Matrix4& lhs);
    void        append3x3(const Matrix4& lhs);

    void appendRotationPivot(T radian, const Vec3<T>& axis, Vec3<T>* pivotPoint);
    void appendRotation(T radian, const Vec3<T>& axis);
    void appendRotationX(T radian);
    void appendRotationY(T radian);
    void appendRotationZ(T radian);

    // 用欧拉角形式旋转(heading->pitch->bank) => (y->x->z)
    void appendRotationEulerAngle(T radianX, T radianY, T radianZ);
    Matrix4& setScale(const Vec3<T>& v3);
    void setScaleXYZ(T xScale, T yScale, T zScale);
    void getScale(Vec3<T>& outV3);

    void setRotationEulerAngle(T radianX, T radianY, T radianZ);
    void setRotationEulerAngleWithTriFunc(T cosX, T sinX, T cosY, T sinY, T cosZ, T sinZ);
    void getAxisRotation(T x, T y, T z, T radian);
    void rotationX(T radian);
    void rotationY(T radian);
    void rotationZ(T radian);
    Matrix4& setRotationFromEuler(const IEuler<T>& euler);

	Matrix4& extractRotation(const Matrix4& m);
    Matrix4& copyTranslation(const Matrix4& m);

    void setTranslationXYZ(T px, T py, T pz);
    void setTranslation(const Vec3<T>& v3);
    void appendScaleXYZ(T xScale, T yScale, T zScale);

    void appendScaleXY(T xScale, T yScale);
    void appendTranslationXYZ(T px, T py, T pz);
    void appendTranslation(const Vec3<T>& v3);
    bool invert();
    Matrix4& invertThis();

    Matrix4 clone();


    void        perspectiveRH(T fovy, T aspect, T zNear, T zFar);
    void        perspectiveLH(T fovy, T aspect, T zNear, T zFar);
    void        orthoRH(T b, T t, T l, T r, T zNear, T zFar);
    void        orthoLH(T b, T t, T l, T r, T zNear, T zFar);
    void        lookAtRH(Vec3<T>& eye, Vec3<T>& center, Vec3<T>& up);
    void        lookAtLH(Vec3<T>& eye, Vec3<T>& center, Vec3<T>& up);

    std::string toString() const;

private:
    T*  m_fs;
    T*  m_localFS;
    unsigned int m_index;
    unsigned int m_type;

    unsigned int m_localFSBytesTotal;

    const static T s_initData[16];
    static Vec3<T> s_v3;
    static Matrix4          s_mat;
    const static T  s_minv;
};

} // namespace math
} // namespace voxengine