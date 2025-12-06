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
    targetEtProtoId = etId;
    compStorage     = comp_storage;
}

bool EntityMotionObject::isValid() const
{
    return Base::ID::isValidID(targetEtProtoId);
}

bool EntityMotionObject::isInvalid() const
{
    return Base::ID::isInvalidID(targetEtProtoId);
}

void EntityMotionObject::color(const Colour::Component::Color& c)
{
    compStorage->setEntityColorAt(c, targetEtProtoId);
    compStorage->dirtyIdMap[targetEtProtoId] = true;
}
Colour::Component::Color EntityMotionObject::color() const
{
    return compStorage->getEntityColorAt(targetEtProtoId);
}
void EntityMotionObject::globalPos(const Math::Vec2& pos)
{
    compStorage->setEntityGlobalXYAt(pos, targetEtProtoId);
    compStorage->dirtyIdMap[targetEtProtoId] = true;
}
Math::Vec2 EntityMotionObject::globalPos() const
{
    return compStorage->getEntityGlobalXYAt(targetEtProtoId);
}

void EntityMotionObject::localPos(const Math::Vec2& pos)
{
    compStorage->setEntityLocalXYAt(pos, targetEtProtoId);
    compStorage->dirtyIdMap[targetEtProtoId] = true;
}
Math::Vec2 EntityMotionObject::localPos() const
{
    return compStorage->getEntityLocalXYAt(targetEtProtoId);
}

void EntityMotionObject::rotation(float rad)
{
    compStorage->setEntityRotationAt(rad, targetEtProtoId);
    compStorage->dirtyIdMap[targetEtProtoId] = true;
}
float EntityMotionObject::rotation() const
{
    return compStorage->getEntityRotationAt(targetEtProtoId);
}

void EntityMotionObject::rotationDeegree(float degree)
{
    compStorage->setEntityRotationAt(degree * MATH_PI_OVER_180, targetEtProtoId);
    compStorage->dirtyIdMap[targetEtProtoId] = true;
}
float EntityMotionObject::rotationDeegree() const
{
    return compStorage->getEntityRotationAt(targetEtProtoId) * MATH_180_OVER_PI;
}

uint32_t EntityMotionObject::etProtoId() const
{
    return targetEtProtoId;
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
    if (ptsTotal > 330)
    {
        pts.pop_front();
    }
}

void EntityMotionObject::applyPoints(PtApplyCallbackType callback, int stride)
{
    auto ptsTotal = pts.size();
    if (ptsTotal < 1 || stride < 1)
        return;

    auto index = 1;
    for (auto i = 0; i < ptsTotal;)
    {
        auto t = index * stride;
        if (t >= ptsTotal)
            break;

        auto ptIndex = ptsTotal - index * stride;
        auto flag    = callback(index - 1, pts[ptIndex - 1], pts[ptIndex]);
        if (!flag)
            break;

        index++;
        i = t;
    }
    auto ti = index;
}
void EntityMotionObject::destory()
{
    targetEtProtoId = Base::ID::INVALID_ID;
}
} // namespace Voxol::Scene