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
    if (entityView)
        return;

    entityView = EntityView::make();
    entityView->initialize(etId, comp_storage);
}

bool EntityMotionObject::isValid() const
{
    return entityView->isValid();
}

bool EntityMotionObject::isInvalid() const
{
    return entityView->isInvalid();
}

uint32_t EntityMotionObject::etProtoId() const
{
    return entityView->etProtoId();
}

void EntityMotionObject::update()
{
    auto pos    = entityView->globalPos();
    auto direcV = targetPos - pos;
    if (direcV.length() <= 2.0f)
    {
        targetPos = {Math::SimpleRandom::get_float() * 600 + 100, Math::SimpleRandom::get_float() * 300 + 100};
        direcV    = targetPos - pos;
    }

    auto currRad   = Math::AngleInterpolator::normalizeAngle(entityView->rotation());
    auto targetRad = Math::AngleInterpolator::normalizeAngle(direcV.radian());

    auto dRad = Math::AngleInterpolator::shortestAngleDifference(currRad, targetRad, true) * 0.05f;
    dRad      = std::abs(dRad);
    dRad      = dRad > 0.1 ? 0.1f : dRad;
    dRad      = dRad > 0.02f ? dRad : 0.02f;
    //auto rad       = Math::AngleInterpolator::rotateTowards(currRad, targetRad, 0.02f);
    auto rad = Math::AngleInterpolator::rotateTowards(currRad, targetRad, dRad);
    entityView->rotation(rad);
    auto       spd = 2.0f;
    Math::Vec2 spdV{spd * cos(rad), spd * sin(rad)};
    pos += spdV;
    entityView->globalPos(pos);

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
    if (entityView)
        entityView->destory();
    entityView = nullptr;
}
} // namespace Voxol::Scene