#ifndef VOXOL_MATH_MAT33_H
#define VOXOL_MATH_MAT33_H

#include <cmath>
#include <numbers>
#include <algorithm>

namespace Voxol::Math
{
class AngleInterpolator
{
public:

    static constexpr float PI         = std::numbers::pi_v<float>;
    static constexpr float TWO_PI     = 2.0f * PI;
    static constexpr float RAD_TO_DEG = 180.0f / PI;
    static constexpr float DEG_TO_RAD = PI / 180.0f;

    // 1. 计算最短角度差（带符号）
    // 返回值在 [-π, π] 或 [-180°, 180°] 范围内
    static float shortestAngleDifference(float from, float to, bool inRadians = true)
    {
        float maxAngle = inRadians ? TWO_PI : 360.0f;
        float halfMax  = maxAngle * 0.5f;

        float diff = to - from;
        diff       = std::fmod(diff, maxAngle);

        // 归一化到 [-halfMax, halfMax]
        if (diff > halfMax)
        {
            diff -= maxAngle;
        }
        else if (diff < -halfMax)
        {
            diff += maxAngle;
        }

        return diff;
    }

    // 2. 角度线性插值（考虑最短路径）
    static float lerpAngle(float from, float to, float t, bool inRadians = true)
    {
        t = std::clamp(t, 0.0f, 1.0f);

        // 计算最短路径差值
        float diff = shortestAngleDifference(from, to, inRadians);

        // 插值
        float result = from + diff * t;

        // 归一化到 [0, maxAngle)
        return normalizeAngle(result, inRadians);
    }

    // 3. 球形线性插值（更平滑）
    static float slerpAngle(float from, float to, float t, bool inRadians = true)
    {
        t = std::clamp(t, 0.0f, 1.0f);

        float diff    = shortestAngleDifference(from, to, inRadians);
        float fromRad = inRadians ? from : from * DEG_TO_RAD;
        float diffRad = inRadians ? diff : diff * DEG_TO_RAD;

        // 使用 sin 插值（球形线性插值）
        float sinDiff = std::sin(diffRad);
        if (std::abs(sinDiff) < 1e-6f)
        {
            return lerpAngle(from, to, t, inRadians);
        }

        float resultRad = fromRad + std::atan2(std::sin(diffRad * t), std::cos(diffRad * t) - std::tan(fromRad) * std::sin(diffRad * t));

        float result = inRadians ? resultRad : resultRad * RAD_TO_DEG;
        return normalizeAngle(result, inRadians);
    }

    // 4. 带角速度限制的转向
    static float rotateTowards(float current, float target, float maxDelta, bool inRadians = true)
    {
        float diff = shortestAngleDifference(current, target, inRadians);

        // 限制转向速度
        float delta = std::clamp(diff, -maxDelta, maxDelta);

        float result = current + delta;
        return normalizeAngle(result, inRadians);
    }

    // 5. 缓动函数插值
    enum class Easing
    {
        LINEAR,
        EASE_IN,
        EASE_OUT,
        EASE_IN_OUT,
        SMOOTHSTEP
    };

    static float lerpAngleWithEasing(float from, float to, float t, Easing easing = Easing::SMOOTHSTEP, bool inRadians = true)
    {
        float easedT = applyEasing(t, easing);
        return lerpAngle(from, to, easedT, inRadians);
    }

    // 6. 判断是否需要转向
    static bool shouldRotate(float current, float target, float tolerance, bool inRadians = true)
    {
        float diff = std::abs(shortestAngleDifference(current, target, inRadians));
        return diff > tolerance;
    }

    // 7. 获取转向方向
    // -1: 顺时针/向右转, 1: 逆时针/向左转, 0: 不需要转
    static int getTurnDirection(float current, float target, float tolerance = 0.001f, bool inRadians = true)
    {
        float diff = shortestAngleDifference(current, target, inRadians);

        if (std::abs(diff) <= tolerance)
        {
            return 0;
        }

        return diff > 0 ? 1 : -1; // 正数：逆时针，负数：顺时针
    }

    // 8. 角度归一化
    static float normalizeAngle(float angle, bool inRadians = true)
    {
        float maxAngle = inRadians ? TWO_PI : 360.0f;

        angle = std::fmod(angle, maxAngle);
        if (angle < 0)
        {
            angle += maxAngle;
        }
        return angle;
    }

private:
    static float applyEasing(float t, Easing easing)
    {
        switch (easing)
        {
            case Easing::LINEAR:
                return t;

            case Easing::EASE_IN:
                return t * t;

            case Easing::EASE_OUT:
                return 1.0f - (1.0f - t) * (1.0f - t);

            case Easing::EASE_IN_OUT:
                return t < 0.5f ? 2.0f * t * t : 1.0f - std::pow(-2.0f * t + 2.0f, 2.0f) * 0.5f;

            case Easing::SMOOTHSTEP:
                return t * t * (3.0f - 2.0f * t);

            default:
                return t;
        }
    }
};
}
#endif