#include "OglTestScene.h"
#include "OglText.h"

namespace Voxol::Test
{


void OglTestScene::initScene()
{
    if (entityModeFlag)
    {
        tileSys->initalize();
        etRenderSys->initalize();
        auto drawCall = [this](const Math::VxRect& bounds, const Math::Mat33& vpMat) {
            etRenderSys->render(drawCtx, vpMat, bounds);
        };
        auto queryCall = [this](const Math::VxRect& bounds, int phase) -> int {
            return etRenderSys->drawQuery(bounds, phase);
        };

        drawCtx.drawCall  = drawCall;
        drawCtx.drawQuery = queryCall;

        boundsUnit.color = {0.0f, 0.3, 0.3f, 1.f};
        boundsUnit.vertex.toLine();
        boundsUnit.vertex.lineWidth = 5;
        Render::Gpu::buildBaseDrawUnit(boundsUnit);

        shortcutMana.registerShortcut(
            {GLFW_KEY_LEFT_CONTROL, GLFW_KEY_Z}, [this] {
                //std::cout << "[Undo] Ctrl + Z pressed\n";
                undo();
            },
            System::ShortcutManager::TriggerType::Press);
        shortcutMana.registerShortcut({GLFW_KEY_LEFT_CONTROL, GLFW_KEY_LEFT_SHIFT, GLFW_KEY_Y}, [] {
            std::cout << "Ctrl + Shift + Y pressed\n";
        });
        return;
    }
    if (voassModeFlag)
    {
        initVoassScene();
        return;
    }
    baseDrawUnit.color = {0.1f, 0.6, 0.3f, 1.0f};
    baseDrawUnit.objMat.setTo(100, 100, 200, 80);
    Render::Gpu::buildBaseDrawUnit(baseDrawUnit);

    texDrawUnit.color = {0.9f, 0.9, 0.9f, 1.0f};
    texDrawUnit.objMat.setTo(150, 70, 100, 220);
    Render::Gpu::buildTexDrawUnit(texDrawUnit);


    static float time = 0.0f;

    time += 0.1f;

    redFormatexDrawUnit.color = {0.2f + std::abs(std::cos(time)) * 0.5f, 0.0, 0.3f, 1.0f};

    redFormatexDrawUnit.objMat.setTo(220, 220, 70, 70);
    Render::Gpu::buildRedFormatTexDrawUnit(redFormatexDrawUnit);

    Render::OglTextGlyphBuilder textBuilder{};
    textBuilder.initFont();
    //auto&& glyData = textBuilder.testBuildGlyph();

    char32_t ch         = U'中';
    auto&&   glyData    = textBuilder.createGlyph(ch, 32);
    auto&    imgData    = glyData.image;
    glyphDrawUnit.color = {0.0f, 0.3, 0.3f, 1.0f};
    glyphDrawUnit.objMat.setTo(360, 320, imgData.width, imgData.height);

    //Gpu::buildRedFormatTexDrawUnit(glyphDrawUnit, imgData);
    Render::Gpu::buildGlyphTexDrawUnit(glyphDrawUnit, glyData);

    Render::OglImage imgObj{};
    auto&&           pngData = imgObj.loadPNGFromAssets("letterA.png");
    pngUnit.objMat.setTo(360, 150, pngData.width, pngData.height);
    Render::Gpu::buildTexDrawUnit(pngUnit, pngData);

    msdfText.initialize("msdf/arial_atlas.png", "msdf/arial_atlas.json");

    std::string text = "Hello, Cute Boy!";
    msdfText.buildText(text, msdfTextDrawUnits, {300, 100}, 50);
}
void OglTestScene::render(const Voxol::Math::Mat33& vpMat)
{
    drawCtx.clearParam.apply();
    if (entityModeFlag)
    {
        auto& ctx      = drawCtx;
        auto& params   = ctx.drawParam;
        auto& viewport = ctx.clearParam.viewport;

        tileSys->run(ctx);

        // show mouse picked entity bounds
        boundsUnit.vertex.lineWidth = 1.0f;

        //auto& queriedEIds = mouseEvtMana.queryEIds;
        auto& queriedEIds = mouseCtrl.qeIds;
        for (auto id : queriedEIds)
        {
            //auto& vb = etRenderSys.bvhItems[id].bounds;
            auto& vb = etRenderSys->bvh->getBoundsAt(id);
            boundsUnit.objMat.setTo(vb.x(), vb.y(), vb.width(), vb.height());
            boundsUnit.mvp = vpMat;
            boundsUnit.draw();
        }
        boundsUnit.vertex.lineWidth = 5;
        auto vb                     = params.viewWBounds;
        boundsUnit.objMat.setTo(vb.x(), vb.y(), vb.width(), vb.height());
        boundsUnit.mvp = vpMat;
        boundsUnit.draw();
        return;
    }

    if (voassModeFlag)
    {
        renderVoass(vpMat);
        return;
    }
    baseDrawUnit.mvp = vpMat;
    baseDrawUnit.draw();

    texDrawUnit.mvp = vpMat;
    texDrawUnit.draw();

    redFormatexDrawUnit.mvp = vpMat;
    redFormatexDrawUnit.draw();

    glyphDrawUnit.mvp = vpMat;
    glyphDrawUnit.draw();

    pngUnit.mvp = vpMat;
    pngUnit.draw();

    for (auto& unit : msdfTextDrawUnits)
    {
        unit.mvp = vpMat;
        unit.draw();
    }
}

void OglTestScene::undo()
{
    ///*
    auto storage     = etRenderSys->entityStorage;
    auto compStorage = storage->comp;
    auto itemData    = compStorage->historyManager->popItem();
    printf("OglTestScene::setKeyParams(), press key z， itemData.id: %d\n", itemData.id);
    if (itemData.id < 0)
    {
        return;
    }

    printf("OglTestScene::setKeyParams(), ready to ctrl-z.\n");
    auto etrans = compStorage->getEntityTransformAt(itemData.id);
    //sys.storage->historyManager->pushItem({itemTrans, id});

    Math::Vec2 pv{itemData.trans.x, itemData.trans.y};

    compStorage->setEntityXYAt(pv, itemData.id);

    auto bvh = etRenderSys->bvh;
    auto b0  = bvh->getBoundsAt(itemData.id);
    auto b1  = b0;
    if (tileSys)
    {
        // 移出
        tileSys->addDirtyBounds(b0, 0);
        b1.moveTo(pv.x, pv.y);
        // 移入
        tileSys->addDirtyBounds(b1, 1);
    }
    if (bvh)
    {
        bvh->updateItemBoundsByObjectId(itemData.id, b1);
        bvh->updateDirty();
    }
}
void OglTestScene::setMouseParams(const System::Mouse::MouseInputParam& param)
{
    auto& ctx    = drawCtx;
    mouseCtrl.targetSys = etRenderSys;
    mouseCtrl.upateMouseParam(drawCtx, param);
    //auto& drawParam = ctx.drawParam;
    //Math::Vec2 wpv = drawParam.invViewMat.mapPoint({param.x, param.y});
    //mouseEvtMana.upateMouseParam(drawCtx, param, {tileSys, etRenderSys->bvh, etRenderSys->entityStorage->comp});
    //mouseEvtHandler(drawCtx, param, {tileSys, etRenderSys->bvh, etRenderSys->entityStorage->comp});
    /*
    Render::Component::UnitTransform unitParam{};
    Math::Vec2           originEtPos{};
    int32_t              etId = -1;
    //std::vector<int32_t> qeIds{};
    mouseEvtHandler.upateMouseParam(ctx, param, [&, this](const System::Mouse::MouseEvent& evt, const Math::Vec2& offset) {

        auto sys = etRenderSys;
        if (evt.type == System::UIMouseType::MOUSE_MOVE || evt.type == System::UIMouseType::MOUSE_DOWN)
        {
            qeIds.clear();
            if (drawParam.viewVBounds.contains(evt.globalPos))
                sys->bvh->queryPoint(evt.globalPos, qeIds);
        }

        int32_t topId = qeIds.empty() ? -1 : qeIds.back();
        if (topId < 0)
        {
            return;
        }
        auto    etStorage = sys->entityStorage->comp;
        switch (evt.type)
        {
            /// mouse down
            case System::UIMouseType::MOUSE_DOWN:
            {
                if (topId >= 0)
                {
                    etId        = topId;

                    unitParam   = etStorage->getEntityTransformAt(topId);
                    originEtPos             = etStorage->getEntityXYAt(topId);
                }
            }
            break;
            /// mouse up
            case System::UIMouseType::MOUSE_UP:
            {
                if (etId >= 0 && evt.isDragging())
                {

                    if (etStorage)
                    {
                        etStorage->historyManager->pushItem({unitParam, etId});
                    }
                }
            }
            break;
            /// mouse move
            case System::UIMouseType::MOUSE_MOVE:
            {
                if (etId >= 0 && evt.isDragging())
                {
                    //auto id = etId;
                    //auto pv = originEtPos + offset;
                    //entityScene.setEntityXYAt(pv, id);
                    //auto b0 = entityScene.bvh.getBoundsAt(id);
                    //auto b  = b0;
                    //b.setXY(pv.x, pv.y);
                    //entityScene.bvh.updateItemBoundsByObjectId(id, b);
                    auto id = etId;
                    auto pv = originEtPos;

                    //auto dv = wpv - dragEvt.mouseOriginPos;
                    pv += offset;
                    if (etStorage)
                    {
                        //auto itemTrans = sys.storage->getEntityTransformAt(id);
                        //sys.storage->historyManager->pushItem({itemTrans, id});

                        etStorage->setEntityXYAt(pv, id);
                        auto b0 = sys->bvh->getBoundsAt(id);
                        auto b1 = b0;
                        if (tileSys)
                        {
                            // 移出
                            tileSys->addDirtyBounds(b0, 0);
                            b1.moveTo(pv.x, pv.y);
                            // 移入
                            tileSys->addDirtyBounds(b1, 1);
                        }
                        if (sys->bvh)
                        {
                            sys->bvh->updateItemBoundsByObjectId(id, b1);
                            sys->bvh->updateDirty();
                        }
                    }
                }
            }
            break;
            default:
                etId = -1;
                break;
        }
    });
    //*/
}
void OglTestScene::initVoassScene()
{


    using namespace Voass::Render;


    auto& sdfCircleUnit       = sdfDrawUnits[0];
    auto& sdfMultiCirclesUnit = sdfDrawUnits[1];
    auto& sdfRingUnit         = sdfDrawUnits[2];
    auto& sdfSectorUnit       = sdfDrawUnits[3];
    auto& sdfRoundedRectUnit  = sdfDrawUnits[4];
    auto& sdfTriangleUnit     = sdfDrawUnits[5];
    auto& sdfRectUnit         = sdfDrawUnits[6];
    auto& strokeShapeUnit     = sdfDrawUnits[7];

    //Gpu::DrawingUnit sdfCircleUnit{};
    bool colorClip      = false;
    sdfCircleUnit.color = {0.9f, 0.0, 0.3f, 1.0f};
    sdfCircleUnit.objMat.setTo(100, 100, 200, 200);
    Render::Gpu::buildSDFDrawUnit(sdfCircleUnit, Shader::SDFShapeType::Circle, colorClip);
    //Gpu::buildSDFDrawUnit(sdfCircleUnit, Shader::SDFShapeType::MultiCircles);

    sdfMultiCirclesUnit.color = {0.6f, 0.0, 0.3f, 1.0f};
    sdfMultiCirclesUnit.objMat.setTo(330, 100, 200, 200);
    //Gpu::buildSDFDrawUnit(sdfCircleUnit, Shader::SDFShapeType::Circle);
    Render::Gpu::buildSDFDrawUnit(sdfMultiCirclesUnit, Shader::SDFShapeType::MultiCircles, colorClip);


    sdfSectorUnit.color = {0.6f, 0.2, 0.5f, 1.f};
    sdfSectorUnit.objMat.setTo(100, 100, 200, 200);
    Render::Gpu::buildSDFDrawUnit(sdfSectorUnit, Shader::SDFShapeType::Sector, colorClip);

    sdfRingUnit.color = {0.6f, 0.2f, 0.6f, 0.5f};
    sdfRingUnit.objMat.setTo(100, 100, 200, 200);
    Render::Gpu::buildSDFDrawUnit(sdfRingUnit, Shader::SDFShapeType::Ring, colorClip);

    sdfRoundedRectUnit.color = {0.5f, 0.6, 0.3f, 1.f};
    sdfRoundedRectUnit.objMat.setTo(300, 260, 200, 200);
    Render::Gpu::buildSDFDrawUnit(sdfRoundedRectUnit, Shader::SDFShapeType::RoundedRect, colorClip);

    sdfTriangleUnit.color = {0.7f, 0.2, 0.2f, 1.f};
    sdfTriangleUnit.objMat.setTo(300, 350, 200, 200);
    Render::Gpu::buildSDFDrawUnit(sdfTriangleUnit, Shader::SDFShapeType::Triangle, colorClip);


    sdfRectUnit.color = {0.1f, 0.6, 0.3f, 1.0f};
    sdfRectUnit.objMat.setTo(100, 100, 200, 80);
    Render::Gpu::buildSDFDrawUnit(sdfRectUnit, Shader::SDFShapeType::Rect, colorClip);

    Render::Gpu::buildSDFDrawUnit(strokeShapeUnit, Shader::SDFShapeType::DefaultShape, colorClip);


    boundsUnit.color = {0.0f, 0.3, 0.3f, 1.f};
    boundsUnit.objMat.setTo(0, 0, 150, 150);
    boundsUnit.vertex.toLine();
    boundsUnit.vertex.lineWidth = 5;
    Render::Gpu::buildBaseDrawUnit(boundsUnit);
}


void OglTestScene::renderVoass(const Math::Mat33& vpMat)
{
    auto& ctx      = drawCtx;
    auto& params   = ctx.drawParam;
    auto& viewport = ctx.clearParam.viewport;
    /*
    auto drawCall = [this](const Math::VxRect& bounds, const Math::Mat33& vpMat) {
        etRenderSys.render(drawCtx, vpMat, bounds);
    };
    auto queryCall = [this](const Math::VxRect& bounds, int phase) -> int {
        return etRenderSys.drawQuery(bounds, phase);
    };

    ctx.drawCall  = drawCall;
    ctx.drawQuery = queryCall;
    tileSys.run(ctx);

    // show mouse picked entity bounds
    boundsUnit.vertex.lineWidth = 1.0f;

    auto& queriedEIds = mouseEvtMana.queryEIds;

    for (auto id : queriedEIds)
    {
        //auto& vb = etRenderSys.bvhItems[id].bounds;
        auto& vb = etRenderSys.bvh.getBoundsAt(id);
        boundsUnit.objMat.setTo(vb.x(), vb.y(), vb.width(), vb.height());
        boundsUnit.mvp = vpMat;
        boundsUnit.draw();

    }
    boundsUnit.vertex.lineWidth = 5;
    auto vb = params.viewWBounds;
    boundsUnit.objMat.setTo(vb.x(), vb.y(), vb.width(), vb.height());
    boundsUnit.mvp = vpMat;
    boundsUnit.draw();
    return;
    //*/
    mFbo.init(GL_ZERO);

    auto useTexSampleDrawig = true;
    if (useTexSampleDrawig)
    {
        Render::Draw::ClearParams clearParam{};

        int  fboSize     = 256 * 2;
        auto fboW        = fboSize;
        auto fboH        = fboSize;
        auto fboTexIndex = 0;

        auto pos     = params.viewMat.getXY();
        auto scaleXY = params.viewMat.getScaleXY();

        Voxol::Math::Mat33 fboVPM{};
        fboVPM.ortho(fboW, fboH);

        Voxol::Math::Mat33 vMat;
        vMat.setXY(pos);
        vMat.setScaleXY(scaleXY);

        fboVPM.append(vMat);

        clearParam.viewport   = {0, 0, fboW, fboH};
        clearParam.clearColor = {0.1, 0.3, 0.1, 1};

        mFbo.bindFBO();
        mFbo.bindTextureAt(tile0Unit.getTextureAt(0), fboTexIndex, fboW, fboH);
        //mFbo.renderBegin({0, 0, fboW, fboH}, {0.1, 0.3, 0.1, 1});
        mFbo.renderBegin(clearParam);

        renderSdfUnits(fboVPM);

        mFbo.unbindFBO(ctx.clearParam);

        Render::Gpu::buildTexDrawUnitWithTex(tile0Unit, mFbo.getTextureAt(fboTexIndex), true);

        fboW = fboH = 256;
        tile0Unit.objMat.setTo(0, 0, fboW, fboH);
        //tile0Unit.mvp = vpMat;
        tile0Unit.mvp = params.projMat;
        tile0Unit.draw();
    }
    else
    {
        renderSdfUnits(vpMat);
    }
}

void OglTestScene::renderSdfUnits(const Voxol::Math::Mat33& vpMat)
{
    auto& sdfCircleUnit       = sdfDrawUnits[0];
    auto& sdfMultiCirclesUnit = sdfDrawUnits[1];
    auto& sdfRingUnit         = sdfDrawUnits[2];
    auto& sdfSectorUnit       = sdfDrawUnits[3];
    auto& sdfRoundedRectUnit  = sdfDrawUnits[4];
    auto& sdfTriangleUnit     = sdfDrawUnits[5];
    auto& sdfRectUnit         = sdfDrawUnits[6];
    auto& strokeShapeUnit     = sdfDrawUnits[7];

    sdfCircleUnit.color = {0.9f, 0.0, 0.7f, 1.0f};
    sdfCircleUnit.mvp   = vpMat;
    sdfCircleUnit.draw();

    sdfCircleUnit.color = {0.7f, 0.7, 0.7f, 1.0f};
    sdfCircleUnit.mvp   = vpMat;
    sdfCircleUnit.draw();

    sdfMultiCirclesUnit.color = {0.6f, 0.0, 0.3f, 1.0f};
    sdfMultiCirclesUnit.objMat.setTo(330, 100, 200, 200);
    sdfMultiCirclesUnit.mvp = vpMat;
    sdfMultiCirclesUnit.draw();

    sdfMultiCirclesUnit.objMat.setTo(360, 130, 200, 200);
    sdfMultiCirclesUnit.color = {0.0, 0.7, 0.7f, 1.0f};
    sdfMultiCirclesUnit.mvp   = vpMat;
    sdfMultiCirclesUnit.draw();

    sdfSectorUnit.mvp = vpMat;
    sdfSectorUnit.draw();

    sdfRingUnit.mvp = vpMat;
    sdfRingUnit.draw();

    sdfRoundedRectUnit.color = {0.5f, 0.6, 0.3f, 1.f};
    sdfRoundedRectUnit.objMat.setTo(300, 260, 200, 200);
    sdfRoundedRectUnit.mvp = vpMat;
    sdfRoundedRectUnit.draw();

    sdfRoundedRectUnit.color = {0.5f, 0.6, 0.3f, 1.f};
    sdfRoundedRectUnit.objMat.setTo(500, 260, 200, 200);
    sdfRoundedRectUnit.mvp = vpMat;
    sdfRoundedRectUnit.draw();


    sdfRectUnit.color = {0.1f, 0.6, 0.3f, 1.0f};
    sdfRectUnit.objMat.setTo(100, 300, 300, 30, 3.1415926f / 3);
    sdfRectUnit.mvp = vpMat;
    sdfRectUnit.draw();

    sdfTriangleUnit.setColor(0x8000aa00);
    sdfTriangleUnit.mvp = vpMat;
    sdfTriangleUnit.draw();

    strokeShapeUnit.color = {0.5f, 0.3, 0.3f, 1.0f};
    strokeShapeUnit.objMat.setTo(700, 300, 100, 100);
    strokeShapeUnit.mvp = vpMat;
    strokeShapeUnit.draw();
}
} // namespace Voxol::Test