#pragma once
#include "../math/vec3.h"
#include "../math/Matrix4.h"

namespace voxengine
{
namespace math
{
template <typename NumberType>
class Camera
{
public:
    Camera();
    virtual ~Camera();

    void lookAtRH(Vec3<NumberType>& camPos, Vec3<NumberType>& lookAtPos, Vec3<NumberType>& up);
    void lookAtLH(Vec3<NumberType>& camPos, Vec3<NumberType>& lookAtPos, Vec3<NumberType>& up);

    void perspectiveRH(NumberType fovRadian, NumberType aspect, NumberType zNear, NumberType zFar);
    void perspectiveLH(NumberType fovRadian, NumberType aspect, NumberType zNear, NumberType zFar);

    void orthoRH(NumberType zNear, NumberType zFar, NumberType b, NumberType t, NumberType l, NumberType r);
    void orthoLH(NumberType zNear, NumberType zFar, NumberType b, NumberType t, NumberType l, NumberType r);

    void setViewXY(NumberType px, NumberType py);
    void setViewSize(NumberType pw, NumberType ph);

    NumberType getViewX() const;
    NumberType getViewY() const;
    NumberType getViewWidth() const;
    NumberType getViewHeight() const;

    void calcScreenNormalizeXYByWorldPos(Vec3<NumberType>& pv3, Vec3<NumberType>& scPV3);
    void worldPosToScreen(Vec3<NumberType>& pv);
    void update();

private:
    bool       m_unlock;
    bool       m_lookRHEnabled;
    bool       m_rightHandEnabled;
    bool       m_perspectiveEnabled;
    bool       m_changed;

    unsigned int version;

    NumberType m_viewFieldZoom;
    NumberType m_aspect;
    NumberType m_fovRadian;
    NumberType m_zNear;
    NumberType m_zFar;

    NumberType m_b;
    NumberType m_t;
    NumberType m_l;
    NumberType m_r;

    NumberType m_viewX;
    NumberType m_viewY;
    NumberType m_viewW;
    NumberType m_viewH;
    NumberType m_viewHalfW;
    NumberType m_viewHalfH;

    NumberType m_nearPlaneWidth;
    NumberType m_nearPlaneHeight;

    Vec3<NumberType> m_initRV;
    Vec3<NumberType> m_initUP;
    Vec3<NumberType> m_camPos;
    Vec3<NumberType> m_lookAtPos;
    Vec3<NumberType> m_up;
    Vec3<NumberType> m_lookAtDirec;
    Vec3<NumberType> m_lookDirectNV;

    Matrix4<NumberType> m_viewMat;
    Matrix4<NumberType> m_projMat;
    Matrix4<NumberType> m_vpMat;
};

} // namespace math
} // namespace voxengine