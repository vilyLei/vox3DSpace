//#include <iostream>
#include <cmath>
#include "Camera.h"
namespace voxengine
{
namespace view
{
template <typename T>
Camera<T>::Camera() :
    m_unlock(true),
    m_lookRHEnabled(false),
    m_rightHandEnabled(false),
    m_perspectiveEnabled(false),
    m_changed(false),
    version(0),
    m_viewFieldZoom(1.0f),
    m_aspect(1.0f),
    m_fovRadian(1.0f),
    m_zNear(1.0f),
    m_zFar(1.0f),
    m_b(-100.0f),
    m_t(100.0f),
    m_l(-100.0f),
    m_r(100.0f),
    m_viewX(0.0f),
    m_viewY(0.0f),
    m_viewW(100.0f),
    m_viewH(100.0f),
    m_viewHalfW(50.0f),
    m_viewHalfH(50.0f),
    m_nearPlaneWidth(0.2f),
    m_nearPlaneHeight(0.2f)
{
}
template <typename T>
Camera<T>::~Camera()
{
}

template <typename T>
void Camera<T>::lookAtRH(const Vec3<T>& camPos, const Vec3<T>& lookAtPos, const Vec3<T>& up)
{
    if (m_unlock)
    {
        m_camPos.copyFrom(camPos);
        m_lookAtPos.copyFrom(lookAtPos);
        m_lookAtDirec.x = m_lookAtPos.x - m_camPos.x;
        m_lookAtDirec.y = m_lookAtPos.y - m_camPos.y;
        m_lookAtDirec.z = m_lookAtPos.z - m_camPos.z;
        m_lookRHEnabled = true;
        m_lookDirectNV.copyFrom(m_lookAtDirec);
        m_lookDirectNV.normalize();
        Vec3<T>::cross(m_lookAtDirec, up, m_initRV);
        Vec3<T>::cross(m_initRV, m_lookAtDirec, m_initUP);
        m_initUP.normalize();
        m_initRV.normalize();
        m_up.copyFrom(m_initUP);
        m_changed = true;
    }
}
template <typename T>
void Camera<T>::lookAtLH(const Vec3<T>& camPos, const Vec3<T>& lookAtPos, const Vec3<T>& up)
{
    if (m_unlock)
    {
        m_camPos.copyFrom(camPos);
        m_lookAtPos.copyFrom(lookAtPos);
        m_up.copyFrom(up);

        m_lookAtDirec.x = m_lookAtPos.x - m_camPos.x;
        m_lookAtDirec.y = m_lookAtPos.y - m_camPos.y;
        m_lookAtDirec.z = m_lookAtPos.z - m_camPos.z;
        m_lookRHEnabled = false;
        m_lookDirectNV.copyFrom(m_lookAtDirec);
        m_lookDirectNV.normalize();
        m_initUP.copyFrom(up);
        m_initUP.normalize();

        Vec3<T>::cross(m_lookAtDirec, m_up, m_initRV);
        m_initRV.normalize();
        m_changed = true;
    }
}

/**
 * right-hand axis perspective projection
 * @param fovRadian radian value
 * @param aspect the value is the view port width / height
 * @param zNear the camera near plane distance
 * @param zFar the camera far plane distance
 */

template <typename T>
void Camera<T>::perspectiveRH(T fovRadian, T aspect, T zNear, T zFar)
{
    if (m_unlock)
    {
        m_aspect    = aspect;
        m_fovRadian = fovRadian;
        m_zNear     = zNear;
        m_zFar      = zFar;
        m_projMat.perspectiveRH(fovRadian, aspect, zNear, zFar);
        m_viewFieldZoom      = std::tan(fovRadian * 0.5f);
        m_perspectiveEnabled = true;
        m_rightHandEnabled   = true;
        m_changed            = true;
    }
}

/**
 * left-hand axis perspective projection
 * @param fovRadian radian value
 * @param aspect the value is the view port width / height
 * @param zNear the camera near plane distance
 * @param zFar the camera far plane distance
 */
template <typename T>
void Camera<T>::perspectiveLH(T fovRadian, T aspect, T zNear, T zFar)
{
    if (m_unlock)
    {
        m_aspect    = aspect;
        m_fovRadian = fovRadian;
        m_zNear     = zNear;
        m_zFar      = zFar;
        m_projMat.perspectiveLH(fovRadian, aspect, zNear, zFar);
        m_viewFieldZoom      = std::tan(fovRadian * 0.5f);
        m_perspectiveEnabled = true;
        m_rightHandEnabled   = false;
        m_changed            = true;
    }
}

template <typename T>
void Camera<T>::orthoRH(T zNear, T zFar, T b, T t, T l, T r)
{
    if (m_unlock)
    {
        m_zNear = zNear;
        m_zFar  = zFar;
        m_b     = b;
        m_t     = t;
        m_l     = l;
        m_r     = r;
        m_projMat.orthoRH(b, t, l, r, zNear, zFar);
        m_perspectiveEnabled = false;
        m_rightHandEnabled   = true;
        m_changed            = true;
    }
}
template <typename T>
void Camera<T>::orthoLH(T zNear, T zFar, T b, T t, T l, T r)
{
    if (m_unlock)
    {
        m_zNear = zNear;
        m_zFar  = zFar;
        m_b     = b;
        m_t     = t;
        m_l     = l;
        m_r     = r;
        m_projMat.orthoLH(b, t, l, r, zNear, zFar);
        m_perspectiveEnabled = false;
        m_rightHandEnabled   = false;
        m_changed            = true;
    }
}


template <typename T>
void Camera<T>::setViewXY(T px, T py)
{
    if (m_unlock)
    {
        m_viewX = px;
        m_viewY = py;
    }
}
template <typename T>
void Camera<T>::setViewSize(T pw, T ph)
{
    if (m_unlock)
    {
        if (pw != m_viewW || ph != m_viewH)
        {
            m_viewW     = pw;
            m_viewH     = ph;
            m_viewHalfW = pw * 0.5f;
            m_viewHalfH = ph * 0.5f;

            if (m_perspectiveEnabled)
            {
                if (m_rightHandEnabled) perspectiveRH(m_fovRadian, pw / ph, m_zNear, m_zFar);
                else
                    perspectiveLH(m_fovRadian, pw / ph, m_zNear, m_zFar);
            }
            else
            {
                orthoRH(m_zNear, m_zFar, -0.5f * ph, 0.5f * ph, -0.5f * pw, 0.5f * pw);
            }
        }
    }
}
template <typename T>
T Camera<T>::getViewX() const
{
    return m_viewX;
}
template <typename T>
T Camera<T>::getViewY() const
{
    return m_viewY;
}
template <typename T>
T Camera<T>::getViewWidth() const
{
    return m_viewW;
}
template <typename T>
T Camera<T>::getViewHeight() const
{
    return m_viewH;
}

/**
 * @param pv3 the 3d world space 3d position
 * @param scPV3 the normalization(-1.0 -> 1.0) screen space 2d position, it will store the 2d position
 */
template <typename T>
void Camera<T>::calcScreenNormalizeXYByWorldPos(const Vec3<T>& pv3, Vec3<T>& scPV3)
{
    scPV3.copyFrom(pv3);
    m_vpMat.transformVector4Self(scPV3);
    scPV3.x /= scPV3.w;
    scPV3.y /= scPV3.w;
}
/**
 * @param pv it is the 3d world space 3d position, its x and y will store the screen space 2d position
 */
template <typename T>
void Camera<T>::worldPosToScreen(Vec3<T>& pv)
{
    m_viewMat.transformVector3Self(pv);
    m_projMat.transformVector4Self(pv);
    pv.x /= pv.w;
    pv.y /= pv.w;
    pv.x *= m_viewHalfW;
    pv.y *= m_viewHalfH;
    pv.x += m_viewX;
    pv.y += m_viewY;
}

template <typename T>
void Camera<T>::update()
{
    if (m_changed)
    {
        version++;
        m_changed = false;
        if (m_lookRHEnabled)
        {
            m_viewMat.lookAtRH(m_camPos, m_lookAtPos, m_up);
        }
        else
        {
            m_viewMat.lookAtLH(m_camPos, m_lookAtPos, m_up);
        }
        m_nearPlaneHeight = m_zNear * std::tan(m_fovRadian * 0.5f) * 2.0f;
        m_nearPlaneWidth  = m_aspect * m_nearPlaneHeight;
        m_vpMat.identity();
        m_vpMat.copyFrom(m_viewMat);
        m_vpMat.append(m_projMat);
    }
}


template <typename T>
void __$templateConstructCamera(T value)
{
    Camera<T> cam{};
    Vec3<T>       v0{};
    cam.lookAtRH(v0, v0, v0);
    cam.lookAtLH(v0, v0, v0);
    cam.perspectiveRH(value, value, value, value);
    cam.perspectiveLH(value, value, value, value);
    cam.orthoRH(value, value, value, value, value, value);
    cam.orthoLH(value, value, value, value, value, value);
    cam.setViewXY(value, value);
    cam.setViewSize(value, value);

    cam.getViewX();
    cam.getViewY();
    cam.getViewWidth();
    cam.getViewHeight();

    cam.calcScreenNormalizeXYByWorldPos(v0, v0);
    cam.worldPosToScreen(v0);
    cam.update();
    
}
void __$templateImplyCamera()
{
    __$templateConstructCamera(1.0);
    __$templateConstructCamera(1.0f);
}

} // namespace math
} // namespace voxengine