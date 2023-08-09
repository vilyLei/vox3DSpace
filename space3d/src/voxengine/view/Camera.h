#pragma once
#include "../math/vec3.h"
#include "../math/Matrix4.h"

namespace voxengine
{
namespace view
{
    using namespace voxengine::math;
template <typename T>
class Camera
{
public:
    Camera();
    virtual ~Camera();

    void lookAtRH(const Vec3<T>& camPos, const Vec3<T>& lookAtPos, const Vec3<T>& up);
    void lookAtLH(const Vec3<T>& camPos, const Vec3<T>& lookAtPos, const Vec3<T>& up);

    void perspectiveRH(T fovRadian, T aspect, T zNear, T zFar);
    void perspectiveLH(T fovRadian, T aspect, T zNear, T zFar);

    void orthoRH(T zNear, T zFar, T b, T t, T l, T r);
    void orthoLH(T zNear, T zFar, T b, T t, T l, T r);

    void setViewXY(T px, T py);
    void setViewSize(T pw, T ph);

    T getViewX() const;
    T getViewY() const;
    T getViewWidth() const;
    T getViewHeight() const;

    void calcScreenNormalizeXYByWorldPos(const Vec3<T>& pv3, Vec3<T>& scPV3);
    void worldPosToScreen(Vec3<T>& pv);
    void update();

private:
    bool       m_unlock;
    bool       m_lookRHEnabled;
    bool       m_rightHandEnabled;
    bool       m_perspectiveEnabled;
    bool       m_changed;

    unsigned int version;

    T m_viewFieldZoom;
    T m_aspect;
    T m_fovRadian;
    T m_zNear;
    T m_zFar;

    T m_b;
    T m_t;
    T m_l;
    T m_r;

    T m_viewX;
    T m_viewY;
    T m_viewW;
    T m_viewH;
    T m_viewHalfW;
    T m_viewHalfH;

    T m_nearPlaneWidth;
    T m_nearPlaneHeight;

    Vec3<T> m_initRV;
    Vec3<T> m_initUP;
    Vec3<T> m_camPos;
    Vec3<T> m_lookAtPos;
    Vec3<T> m_up;
    Vec3<T> m_lookAtDirec;
    Vec3<T> m_lookDirectNV;

    Matrix4<T> m_viewMat;
    Matrix4<T> m_projMat;
    Matrix4<T> m_vpMat;
};

} // namespace view
} // namespace voxengine