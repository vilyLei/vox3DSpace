#ifndef VOXOL_RENDER_ENTITY_RENDER_SYSTEM_H
#define VOXOL_RENDER_ENTITY_RENDER_SYSTEM_H

#include "BVH2DV1.h"
#include "DrawCtx.h"
#include "CompPool.h"

#include "../Scene/EntityUnitStorage.h"
#include "OglFbo.h"

namespace Voxol::Render
{

class EntityRenderSystem
{
public:
    using SP = std::shared_ptr<EntityRenderSystem>;
    using WP = std::weak_ptr<EntityRenderSystem>;
    using UP = std::unique_ptr<EntityRenderSystem>;

public:
    static EntityRenderSystem::SP make();

public:
    EntityRenderSystem()  = default;
    ~EntityRenderSystem() = default;

public:
    void initalize();
    void render(const Draw::DrawContext& rctx, const Math::Mat33& vpM, const Math::Bounds& wbounds, const std::vector<Base::ID::KeyUint64>& queriedEIds);
    void clear();

    Scene::EntityUnitStorage::SP entityStorage{};

private:
    bool drawUnitEffect(const Draw::DrawContext& rctx, const Base::ID::KeyUint64 etKey, const Math::Mat33& vpM, const Math::Bounds& wbounds);
    bool drawUnit(const Draw::DrawContext& rctx, const Scene::Component::UnitEntity& entity, const Math::Mat33& vpM, const Math::Bounds& wbounds, const Math::Mat33& wM);

private:
    Draw::ClearParams clearParam{};
    //Draw::OglFbo::SP  mFbo{};
    Gpu::DrawingUnit  rttUnit{};
    Gpu::DrawingUnit  blurHUnit{};
    Gpu::DrawingUnit  blurVUnit{};
};
} // namespace Voxol::Render
#endif
