#include "EntityMotionObject.h"
#include "../Math/MathDef.h"
#include "../Math/Random.h"
#include "../Math/AngleInterpolator.h"

namespace Voxol::Scene
{
EntityMotionObject::SP EntityMotionObject::make()
{
    auto sp = std::make_shared<EntityMotionObject>();
    return sp;
}

void EntityMotionObject::initialize(uint32_t etId, EntityCompStorage::SP comp_storage)
{
    targetEtId  = etId;
    compStorage = comp_storage;
}

bool EntityMotionObject::isValid() const
{
    return Base::ID::isValidID(targetEtId);
}

bool EntityMotionObject::isInvalid() const
{
    return Base::ID::isInvalidID(targetEtId);
}

void EntityMotionObject::color(uint32_t c)
{
    compStorage->setEntityColorAt(c, targetEtId);
    compStorage->dirtyIdMap[targetEtId] = true;
}
uint32_t EntityMotionObject::color() const
{
    return compStorage->getEntityColorAt(targetEtId);
}
void EntityMotionObject::globalPos(const Math::Vec2& pos)
{
    compStorage->setEntityGlobalXYAt(pos, targetEtId);
    compStorage->dirtyIdMap[targetEtId] = true;
}
Math::Vec2 EntityMotionObject::globalPos() const
{
    return compStorage->getEntityGlobalXYAt(targetEtId);
}

void EntityMotionObject::localPos(const Math::Vec2& pos)
{
    compStorage->setEntityLocalXYAt(pos, targetEtId);
    compStorage->dirtyIdMap[targetEtId] = true;
}
Math::Vec2 EntityMotionObject::localPos() const
{
    return compStorage->getEntityLocalXYAt(targetEtId);
}

void EntityMotionObject::rotation(float rad)
{
    compStorage->setEntityRotationAt(rad, targetEtId);
    compStorage->dirtyIdMap[targetEtId] = true;
}
float EntityMotionObject::rotation() const
{
    return compStorage->getEntityRotationAt(targetEtId);
}

void EntityMotionObject::rotationDeegree(float degree)
{
    compStorage->setEntityRotationAt(degree * MATH_PI_OVER_180, targetEtId);
    compStorage->dirtyIdMap[targetEtId] = true;
}
float EntityMotionObject::rotationDeegree() const
{
    return compStorage->getEntityRotationAt(targetEtId) * MATH_180_OVER_PI;
}

uint32_t EntityMotionObject::etId() const
{
    return targetEtId;
}

void EntityMotionObject::update()
{
    auto pos    = globalPos();
    auto direcV = targetPos - pos;
    if (direcV.length() <= 2.0f)
    {
        targetPos = {Math::SimpleRandom::get_float() * 600 + 100, Math::SimpleRandom::get_float() * 300 + 100};
        direcV    = targetPos - pos;
    }

    auto currRad   = Math::AngleInterpolator::normalizeAngle(rotation());
    auto targetRad = Math::AngleInterpolator::normalizeAngle(direcV.radian());
    auto rad       = Math::AngleInterpolator::rotateTowards(currRad, targetRad, 0.02f);
    rotation(rad);
    auto       spd = 2.0f;
    Math::Vec2 spdV{spd * cos(rad), spd * sin(rad)};
    pos += spdV;
    globalPos(pos);

    pts.emplace_back(pos);
    auto ptsTotal = pts.size();
    if (ptsTotal > 130)
    {
        pts.pop_front();
    }
}

void EntityMotionObject::applyPoints(PtApplyCallbackType callback)
{
    auto ptsTotal = pts.size();
    auto stride   = 20;
    auto index    = 1;

    for (auto i = 0; i < ptsTotal;)
    {
        auto t = index * stride;
        if (t < ptsTotal)
            break;
        //if (t < ptsTotal)
        //{
        auto ptIndex = ptsTotal - index * stride;
        auto flag    = callback(index - 1, pts[ptIndex - 1], pts[ptIndex]);
        if (!flag)
            break;

        //t += i;
        index++;
        //}
        i += t;
    }
}
void EntityMotionObject::destory()
{
    targetEtId = Base::ID::INVALID_ID;
}
} // namespace Voxol::Scene