#ifndef VOXOL_OGL_TEST_SCENE_H
#define VOXOL_OGL_TEST_SCENE_H

#include "../Tile/TileSystem.h"
#include "../Render/EntityRenderSystem.h"
#include "../Base/BaseDefine.h"
#include "../Math/Mat33.h"
#include "../Math/VxRect.h"
#include "../Render/OglFbo.h"
#include "../System/MouseEventSystem.h"
#include "../System/ShortcutManager.h"
#include "../Render/OglGpuResUtils.h"
#include "OglImage.h"
#include "OglText.h"

#include <iostream>
#include <cmath>
#include <vector>
#include <functional>

#include <GL/glew.h>
#include <glfw3.h>

namespace Voxol::Test
{
enum class SelectType
{
    Single,
    Multiple,
    Bounds
};
struct MouseonCtroller
{
    Math::Vec2           originEtPos{};
    int32_t              etId = -1;
    std::vector<int32_t> qeIds{};

    Render::Component::UnitTransform unitTransform{};

    Tile::TileSystem::SP           tileSys;
    Render::EntityRenderSystem::SP targetSys;
    Math::Bounds                   selectBounds{};
    SelectType                     selectType = SelectType::Single;
    System::Mouse::MouseEvtHandler handler{};

    void selectSingle(const System::Mouse::MouseEvent& evt, const Math::Vec2& offset)
    {
        auto bvh = targetSys->bvh;
        if (evt.isMoving() || evt.isBegin())
        {
            qeIds.clear();
            bvh->queryPoint(evt.globalPos, qeIds);
        }
        auto    etStorage = targetSys->entityStorage->comp;

        int32_t topId = qeIds.empty() ? -1 : qeIds.back();

        if (topId >= 0 && evt.isBegin())
        {
            etId        = topId;
            originEtPos   = etStorage->getEntityXYAt(etId);
            unitTransform = etStorage->getEntityTransformAt(etId);
            printf("selectSingle(), A01.\n");
            return;
        }

        if (etId >= 0 && evt.isDragging())
        {
            /*
            auto id = etId;
            auto pv = originEtPos + offset;
            etStorage->setEntityXYAt(pv, id);

            auto b0 = targetSys->bvh->getBoundsAt(id);
            auto b  = b0;
            b.moveTo(pv.x, pv.y);
            targetSys->bvh->updateItemBoundsByObjectId(id, b);
            //*/
            auto id = etId;
            auto pv = originEtPos;

            //auto dv = wpv - dragEvt.mouseOriginPos;
            pv += offset;
            etStorage->setEntityXYAt(pv, id);
            auto b0 = bvh->getBoundsAt(id);
            auto b1 = b0;

            // ÒÆ³ö
            tileSys->addDirtyBounds(b0, 0);
            b1.moveTo(pv.x, pv.y);
            // ÒÆÈë
            tileSys->addDirtyBounds(b1, 1);

            bvh->updateItemBoundsByObjectId(id, b1);
            bvh->updateDirty();
            printf("selectSingle(), A02.\n");
            return;
        }
        if (evt.isEnd())
        {
            if (etId >= 0 && evt.isDragging())
            {
                etStorage->historyManager->pushItem({unitTransform, etId});
                printf("selectSingle(), A03.\n");
            }
            etId = -1;
        }
    }
    void upateLeftMouseParam(const Render::Draw::DrawContext& rctx, const System::Mouse::MouseInputParam& param)
    {
        handler.upateMouseLeftBtnParam(rctx, param, [&, this](const System::Mouse::MouseEvent& evt, const Math::Vec2& offset) {
            selectSingle(evt, offset);
        });
    }
};

class OglTestScene
{

public:
    OglTestScene()          = default;
    virtual ~OglTestScene() = default;

public:
    void                      initScene();
    void                      render(const Voxol::Math::Mat33& vpMat);
    void                      undo();
    void                      setMouseParams(const System::Mouse::MouseInputParam& param);

    Render::Draw::DrawContext drawCtx{};

    MouseonCtroller         mouseCtrl;
    System::ShortcutManager     shortcutMana{};

private:
    void initVoassScene();
    void renderVoass(const Math::Mat33& vpMat);
    void renderSdfUnits(const Voxol::Math::Mat33& vpMat);

    bool voassModeFlag = true;
    bool entityModeFlag = true;

    Render::OglFbo             mFbo{};
    Tile::TileSystem::SP       tileSys = Tile::TileSystem::make();
    Render::EntityRenderSystem::SP etRenderSys = Render::EntityRenderSystem::make();

    Render::Gpu::DrawingUnit              baseDrawUnit{};
    Render::Gpu::DrawingUnit              texDrawUnit{};
    Render::Gpu::DrawingUnit              redFormatexDrawUnit{};
    Render::Gpu::DrawingUnit              glyphDrawUnit{};
    Render::Gpu::DrawingUnit              pngUnit{};
    Render::MSDFText                      msdfText{};
    std::vector<Render::Gpu::DrawingUnit>   msdfTextDrawUnits{};

    std::vector<Render::Gpu::DrawingUnit> sdfDrawUnits{8};

    Render::Gpu::DrawingUnit     tile0Unit{};
    Render::Gpu::DrawingUnit boundsUnit{};
};


} // namespace Voxol::Test
#endif // VOXOL_OGL_RENDERER_H
