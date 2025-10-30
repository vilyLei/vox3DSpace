#ifndef VOXOL_SYSTEM_ENTITY_SYSTEM_LAYER_H
#define VOXOL_SYSTEM_ENTITY_SYSTEM_LAYER_H


#include "../Render/EntitySceneSystem.h"
#include "../Render/EntityRenderSystem.h"
#include "../Tile/TileSystem.h"
#include "../System/UIOperationLayer.h"

namespace Voxol::System
{

class EntitySystemLayer
{
public:
    using SP = std::shared_ptr<EntitySystemLayer>;
    using WP = std::weak_ptr<EntitySystemLayer>;
    using UP = std::unique_ptr<EntitySystemLayer>;
public:
    static EntitySystemLayer::SP make();


public:
    EntitySystemLayer()  = default;
    ~EntitySystemLayer() = default;

public:
    void initalize(const std::string& configFileName = {});
    void updateCtx(const Render::Draw::DrawContext& ctx);
    void render(const Math::Mat33& vpMat);
    void undo();
    void updateKeyboardParams(int key, int scancode, int action, int mods);
    void updateMouseParams(const System::Mouse::MouseInputParam& param);

    void updateTileWithEntityId(uint32_t eId);
    void updateBVHAndTileWithEntityId(uint32_t eId);

    public:
    Render::Draw::DrawContext                 drawCtx;

    std::shared_ptr<System::UIOperationLayer> uiOpLayer;
    Render::EntitySceneSystem::SP             etSceneSys  = Render::EntitySceneSystem::make();
    Render::EntityRenderSystem::SP            etRenderSys = Render::EntityRenderSystem::make();
    Tile::TileSystem::SP                      tileSys     = Tile::TileSystem::make();

private:
};
} // namespace Voxol::Render
#endif
