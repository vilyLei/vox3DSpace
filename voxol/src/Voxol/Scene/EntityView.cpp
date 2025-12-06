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

void EntityView::initializeWithName(const std::string& name, EntityCompStorage::SP comp_storage)
{
    if (!comp_storage || !comp_storage->entityNameMap.contains(name))
        return;

    compStorage = comp_storage;    
    auto id = compStorage->entityNameMap[name].id;
    targetKey   = Base::ID::KeyUint64::make(id);
}
void EntityView::initialize(uint32_t etId, EntityCompStorage::SP comp_storage)
{
    if (Base::ID::isInvalidID(etId) || !comp_storage)
        return;

    targetKey   = Base::ID::KeyUint64::make(etId);
    compStorage     = comp_storage;
}

void EntityView::initialize(const Base::ID::KeyUint64& etId, EntityCompStorage::SP comp_storage)
{
    if (etId.isIDInvalid() || !comp_storage)
        return;

    targetKey   = etId;
    compStorage = comp_storage;
}

bool EntityView::isValid() const
{
    return targetKey.isIDValid();
}

bool EntityView::isInvalid() const
{
    return targetKey.isIDInvalid();
}

void EntityView::color(const Colour::Component::Color& c)
{
    compStorage->setEntityColorAt(c, targetKey.protoId());
    auto&& desc = compStorage->dirtyEntityMap[targetKey.protoId()];
    desc.key    = targetKey;
    desc.shadingDirty();
}
Colour::Component::Color EntityView::color() const
{
    return compStorage->getEntityColorAt(targetKey.protoId());
}

void EntityView::colorAlpha(float alpha)
{
    auto c = compStorage->getEntityColorAt(targetKey.protoId());
    c.normaplizedAlpha(alpha);
    compStorage->setEntityColorAt(c, targetKey.protoId());
    auto&& desc = compStorage->dirtyEntityMap[targetKey.protoId()];
    desc.key    = targetKey;
    desc.shadingDirty();
}
float EntityView::colorAlpha() const
{
    auto c = compStorage->getEntityColorAt(targetKey.protoId());
    return c.normaplizedAlpha();
}

void EntityView::globalPos(const Math::Vec2& pos)
{
    compStorage->setEntityGlobalXYAt(pos, targetKey.protoId());
    auto&& desc = compStorage->dirtyEntityMap[targetKey.protoId()];
    desc.key    = targetKey;
    desc.boundsDirty();
}
Math::Vec2 EntityView::globalPos() const
{
    return compStorage->getEntityGlobalXYAt(targetKey.protoId());
}

void EntityView::localPos(const Math::Vec2& pos)
{
    compStorage->setEntityLocalXYAt(pos, targetKey.protoId());
    auto&& desc = compStorage->dirtyEntityMap[targetKey.protoId()];
    desc.key    = targetKey;
    desc.boundsDirty();
}
Math::Vec2 EntityView::localPos() const
{
    return compStorage->getEntityLocalXYAt(targetKey.protoId());
}

void EntityView::rotation(float rad)
{
    compStorage->setEntityRotationAt(rad, targetKey.protoId());
    auto&& desc = compStorage->dirtyEntityMap[targetKey.protoId()];
    desc.key    = targetKey;
    desc.boundsDirty();
}
float EntityView::rotation() const
{
    return compStorage->getEntityRotationAt(targetKey.protoId());
}

void EntityView::rotationDeegree(float degree)
{
    compStorage->setEntityRotationAt(degree * MATH_PI_OVER_180, targetKey.protoId());
    auto&& desc = compStorage->dirtyEntityMap[targetKey.protoId()];
    desc.key    = targetKey;
    desc.boundsDirty();
}
float EntityView::rotationDeegree() const
{
    return compStorage->getEntityRotationAt(targetKey.protoId()) * MATH_180_OVER_PI;
}

void EntityView::visible(bool b)
{
    compStorage->setEntityVisibleAt(targetKey.protoId(), targetKey.protoId());
    auto&& desc = compStorage->dirtyEntityMap[targetKey.protoId()];
    desc.key    = targetKey;
    desc.boundsDirty();
}
bool EntityView::visible() const
{
    return compStorage->getEntityVisibleAt(targetKey.protoId());
}

uint32_t EntityView::etProtoId() const
{
    return targetKey.protoId();
}

Base::ID::KeyUint64 EntityView::etKeyId() const
{
    return targetKey;
}

void EntityView::update()
{
}

void EntityView::destory()
{
    targetKey   = Base::ID::INVALID_KEY;
    compStorage     = nullptr;
}
} // namespace Voxol::Scene