#ifndef VOXOL_ENTITY_RENDER_SYSTEM_H
#define VOXOL_ENTITY_RENDER_SYSTEM_H

#include "../Tile/GridDef.h"
#include "BVH2D.h"
#include "DrawCtx.h"
#include "CompPool.h"
#include "EntityComponent.h"

namespace Voxol::Render
{

using namespace Voxol::Test;
using namespace Voxol::Tile;

class EntityRenderSystem
{
public:
    EntityRenderSystem()  = default;
    ~EntityRenderSystem() = default;

public:
    void initalize();
    int  drawQuery(const Math::VxRect& bounds, const Math::Mat33& vpM);
    void render(const Draw::DrawContext& rctx, const Math::Mat33& vpM, std::vector<Gpu::DrawingUnit> drawingUnits);
    void clear();

    std::vector<BVHItem2D> bvhItems;
    BVH2D                  bvh{};

    template <typename T>
    CompPool<T>& getPool()
    {
        if constexpr (std::is_same_v<T, Component::UnitShadingEntity>)
            return shaderingEntitiesPool;
        else if constexpr (std::is_same_v<T, Component::UnitShadingBaseDesc>)
            return shaderingDescPool;
        else
            static_assert(!sizeof(T), "Unsupported pool type");
    }
    template <typename T>
    const CompPool<T>& getPool() const
    {
        if constexpr (std::is_same_v<T, Component::UnitShadingEntity>)
            return shaderingEntitiesPool;
        else if constexpr (std::is_same_v<T, Component::UnitShadingBaseDesc>)
            return shaderingDescPool;
        else
            static_assert(!sizeof(T), "Unsupported pool type");
    }

    template <typename T>
    const T& getCompAt(int32_t index) const
    {
        const auto& pool = getPool<T>();
        return pool.get(index);
    }
    template <typename T>
    const T& operator[](int32_t index) const
    {
        const auto& pool = getPool<T>();
        return pool.get(index);
    }

private:
    void                                      drawUnit(const Component::UnitEntity& entity, const Draw::DrawContext& rctx, const Math::Mat33& vpM, std::vector<Gpu::DrawingUnit> drawingUnits);
    std::vector<Component::UnitEntity>        entities{};
    CompPool<Component::UnitShadingEntity>     shaderingEntitiesPool{};
    CompPool<Component::UnitShadingBaseDesc>  shaderingDescPool{};
    std::vector<int32_t>                     queriedEIds{};
};
} // namespace Voxol::Render
#endif
