#include "EntityView.h"
#include "../Math/MathDef.h"
#include "../Math/Random.h"
#include "../Math/AngleInterpolator.h"

namespace Voxol::Scene
{
EntityView::SP EntityView::make()
{
    auto sp = std::make_shared<EntityView>();
    return sp;
}

void EntityView::initialize(uint32_t etId, EntityCompStorage::SP comp_storage)
{
    targetEtProtoId = etId;
    compStorage     = comp_storage;
}

bool EntityView::isValid() const
{
    return Base::ID::isValidID(targetEtProtoId);
}

bool EntityView::isInvalid() const
{
    return Base::ID::isInvalidID(targetEtProtoId);
}

void EntityView::color(const Colour::Component::Color& c)
{
    compStorage->setEntityColorAt(c, targetEtProtoId);
    compStorage->dirtyIdMap[targetEtProtoId] = true;
}
Colour::Component::Color EntityView::color() const
{
    return compStorage->getEntityColorAt(targetEtProtoId);
}

void EntityView::colorAlpha(float alpha)
{
    //compStorage->setEntityColorAt(c, targetEtProtoId);
    //compStorage->dirtyIdMap[targetEtProtoId] = true;
}
float EntityView::colorAlpha() const
{
    //return compStorage->getEntityColorAt(targetEtProtoId);
    return 1.0f;
}

void EntityView::globalPos(const Math::Vec2& pos)
{
    compStorage->setEntityGlobalXYAt(pos, targetEtProtoId);
    compStorage->dirtyIdMap[targetEtProtoId] = true;
}
Math::Vec2 EntityView::globalPos() const
{
    return compStorage->getEntityGlobalXYAt(targetEtProtoId);
}

void EntityView::localPos(const Math::Vec2& pos)
{
    compStorage->setEntityLocalXYAt(pos, targetEtProtoId);
    compStorage->dirtyIdMap[targetEtProtoId] = true;
}
Math::Vec2 EntityView::localPos() const
{
    return compStorage->getEntityLocalXYAt(targetEtProtoId);
}

void EntityView::rotation(float rad)
{
    compStorage->setEntityRotationAt(rad, targetEtProtoId);
    compStorage->dirtyIdMap[targetEtProtoId] = true;
}
float EntityView::rotation() const
{
    return compStorage->getEntityRotationAt(targetEtProtoId);
}

void EntityView::rotationDeegree(float degree)
{
    compStorage->setEntityRotationAt(degree * MATH_PI_OVER_180, targetEtProtoId);
    compStorage->dirtyIdMap[targetEtProtoId] = true;
}
float EntityView::rotationDeegree() const
{
    return compStorage->getEntityRotationAt(targetEtProtoId) * MATH_180_OVER_PI;
}

void EntityView::visible(bool b)
{
    compStorage->setEntityVisibleAt(targetEtProtoId, targetEtProtoId);
    compStorage->dirtyIdMap[targetEtProtoId] = true;
}
bool EntityView::visible() const
{
    return compStorage->getEntityVisibleAt(targetEtProtoId);
}

uint32_t EntityView::etProtoId() const
{
    return targetEtProtoId;
}

void EntityView::update()
{
}

void EntityView::destory()
{
    targetEtProtoId = Base::ID::INVALID_ID;
    compStorage     = nullptr;
}
} // namespace Voxol::Scene