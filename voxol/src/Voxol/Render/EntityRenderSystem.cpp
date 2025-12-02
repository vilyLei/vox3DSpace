#include "EntityRenderSystem.h"
#include "../base/ScopeGuard.h"
#include <format>
namespace Voxol::Render
{

EntityRenderSystem::SP EntityRenderSystem::make()
{
    auto sp = std::make_shared<EntityRenderSystem>();
    return sp;
}

void EntityRenderSystem::initalize()
{
    //if (mFbo)
    //    return;
    //mFbo = Draw::OglFbo::make();
    //mFbo->init(GL_ZERO);

    Render::Gpu::buildTexDrawUnitWithTex(rttUnit, GL_ZERO, true);
    Render::Gpu::buildTexDrawUnitWithTexBlur(blurHUnit, GL_ZERO, true, 0);
    Render::Gpu::buildTexDrawUnitWithTexBlur(blurVUnit, GL_ZERO, true, 1);
}

int EntityRenderSystem::render(const Draw::DrawContext& rctx, const Math::Mat33& vpM, const Math::Bounds& wbounds, const std::vector<Base::ID::KeyUint64>& queriedEIds)
{
    if (!entityStorage)
        return 0;

    //printf("EntityRenderSystem::render() B %d\n", queriedEIds.size());

    if (queriedEIds.empty())
        return 0;

    auto& compStorage = entityStorage->comp;

    auto   total        = queriedEIds.size();
    auto&  entitiesPool = compStorage->entitiesPool;
    size_t drawTotal    = 0;

    //printf("EntityRenderSystem::render() total: %zu\n", total);

    for (auto i = 0; i < total; i++)
    {
        auto&& key = queriedEIds[i];

        //printf("i: %zu, render key: %s\n",i, key.toString().c_str());

        if (key.isIDInvalid())
            continue;

        auto proId = key.protoId();
        if (Base::ID::isInvalidID(proId))
        {
            continue;
        }
        if (entitiesPool.isInvalid(proId))
        {
            continue;
        }
        auto  iid = key.iid();
        auto& et  = entitiesPool[proId];
        if (Base::ID::isInvalidID(et.shadingId) || !et.globalVisible)
        {
            continue;
        }
        if (key.flags() > 0)
        {
            drawUnitEffect(rctx, key, vpM, wbounds);
            continue;
        }

        //printf("EntityRenderSystem::render() ...\n");
        if (key.isIIDValid())
        {
            auto&& wmat = compStorage->entityInsGlobalMat33Map[key];
            auto   flag = drawUnit(rctx, et, vpM, wbounds, wmat);
            drawTotal += flag ? 1 : 0;
            //printf("drawUnit with prototype child rendering process ...\n");
            continue;
        }

        auto flag = drawUnit(rctx, et, vpM, wbounds, compStorage->entityGlobalMat33Map[proId]);
        drawTotal += flag ? 1 : 0;
    }
    
    return drawTotal;

    //if (drawTotal < total)
    //{
    //    printf(">>> >>> >>> EntityRenderSystem::render() , drawTotal: %d, total: %d\n", drawTotal, total);
    //}
    //return;
    //auto& ets = compStorage->entitiesPool;
    //ets.forEach([&](auto& et) {
    //    if (ID::isValidID(et.prototypeId))
    //    {
    //        printf("drawUnit with prototype process ...\n");
    //        return;
    //    }
    //    if (et.shadingId < 0 || !et.visible)
    //        return;
    //    drawUnit(et, vpM, wbounds);
    //});
}
bool EntityRenderSystem::drawUnitEffect(const Draw::DrawContext& rctx, const Base::ID::KeyUint64 etKey, const Math::Mat33& vpM, const Math::Bounds& wbounds)
{
    auto&  compStorage = entityStorage->comp;
    auto&& srUnit      = compStorage->effectShadowEntityMap[etKey];

    if (!compStorage->effectShadowMap.contains(srUnit.effectId))
        return false;

    auto&& entityId          = srUnit.entityId;
    auto&& entity   = compStorage->entitiesPool[entityId.protoId()];

    if (Base::ID::isInvalidID(entity.shadingId))
        return false;

    auto   etId              = entity.id;
    auto&  shaderingEntities = compStorage->shaderingEntitiesPool;
    auto&  shaderingDescVec  = compStorage->shaderingDescPool;
    auto&  transformsPool    = compStorage->transformsPool;

    auto& shadingEt = shaderingEntities[entity.shadingId];
    auto& shdDesc   = shaderingDescVec[shadingEt.shadingDescId];

    if (shdDesc.flags == 0)
        return false;

    auto&& shdData = compStorage->effectShadowMap[srUnit.effectId];

    auto wM = compStorage->getEntityGlobalMat33At(entityId);

    // shadow offset in the global space
    wM.offsetXY(shdData.offset);

    auto&&       trans = transformsPool[entity.transformId];

    auto&& vb = compStorage->getEntityGlobalBoundsAt(etId);
    //Scene::Component::defaultRect.mat33MapTo(wm, vb);
    if (!wbounds.intersects(vb))
        return false;

    if (Base::ID::isInvalidID(entity.modelId))
    {
        return false;
    }
    auto&  modelsPool = compStorage->modelsPool;
    auto   drawingId  = modelsPool[entity.modelId].drawUnitId;

    if (Base::ID::isInvalidID(drawingId))
    {
        return false;
    }

    auto&  drs        = *entityStorage->drawing;
    auto&& drawUnit   = drs[drawingId];

    Math::Mat33 svM;
    svM.setScaleXY(trans.sx, trans.sy);
    svM.prepend(wM);

    drawUnit.blendMode = 1;
    drawUnit.setColor(shdData.color);
    drawUnit.objMat = wM;
    drawUnit.mvp    = vpM;
    drawUnit.draw();

    return true;
}

bool EntityRenderSystem::drawUnit(const Draw::DrawContext& rctx, const Scene::Component::UnitEntity& entity, const Math::Mat33& vpM, const Math::Bounds& wbounds, const Math::Mat33& wM)
{

    if (Base::ID::isInvalidID(entity.shadingId))
        return false;

    auto& compStorage       = entityStorage->comp;
    auto& shaderingDescPool = compStorage->shaderingDescPool;
    auto& transformsPool    = compStorage->transformsPool;
    auto& modelsPool        = compStorage->modelsPool;

    auto&& shadingEt = compStorage->get<Scene::Component::UnitShadingEntity>(entity.shadingId);

    if (Base::ID::isInvalidID(entity.modelId) || Base::ID::isInvalidID(shadingEt.shadingDescId))
    {
        return false;
    }

    auto&& model     = modelsPool[entity.modelId];
    auto&& shdDesc   = shaderingDescPool[shadingEt.shadingDescId];
    auto&& trans     = transformsPool[entity.transformId];

    auto&& vb = compStorage->getEntityGlobalBoundsAt(entity.id);
    if (!wbounds.intersects(vb))
        return false;


    if (model.type == Scene::Component::UnitModelType::Text)
    {
        auto&& strModel = compStorage->entityStringModelMap[entity.id];
        auto&  textDesc      = strModel.text;

        if (!textDesc.text.empty())
        {
            //auto pos        = wM.getXY();
            //auto ph  = strModel.bounds.height();
            //pos += strModel.posOffset;
            //auto&& glyphUnits = entityStorage->getDrawUnitsFromText(textDesc.text, textDesc.fontSize, pos);

            auto&& glyphUnits = entityStorage->getDrawUnitsFromText(textDesc.text, textDesc.fontSize, {});
            for (auto& unit : glyphUnits)
            {
                auto objM = unit.objMat;
                objM.offsetXY(strModel.posOffset);
                objM.prepend(wM);

                unit.objMat    = objM;
                unit.blendMode = 1;
                unit.setColor(shdDesc.color);

                unit.mvp    = vpM;
                unit.draw();
            }
        }
        return true;
    }

    if (Base::ID::isInvalidID(model.drawUnitId))
    {
        return false;
    }
    auto   drawingId = model.drawUnitId;
    auto&  drs       = *entityStorage->drawing;
    auto&& drawUnit  = drs[drawingId];

    Math::Mat33 svM;
    svM.setScaleXY(trans.sx, trans.sy);
    svM.prepend(wM);

    //if (shdDesc.flags > 0 && compStorage->shadingShadowIdMap.contains(shadingEt.shadingDescId))
    //{
    //    auto&& effects = compStorage->shadingShadowIdMap[shadingEt.shadingDescId];
    //    auto   tot     = effects.size();
    //    // draw shadows
    //    for (auto i = 0; i < tot; i++)
    //    {
    //        auto&& shd         = compStorage->effectShadowMap[effects[i]];
    //        auto mat           = wM;
    //        mat.offsetXY(shd.offset);
    //        drawUnit.blendMode = 1;
    //        drawUnit.setColor(shd.color);
    //        drawUnit.objMat = mat;
    //        drawUnit.mvp    = vpM;
    //        drawUnit.draw();
    //    }
    //}

    //printf(c"xxx xxx trans(x=%f, y=%f)\n", trans.x, trans.y);

    auto tempColor     = shdDesc.color;
    drawUnit.blendMode = 1;
    if (drawingId == 8 && compStorage->textureMap.contains(drawingId))
    {
        auto tex = compStorage->textureMap.contains(drawingId);
        Render::Gpu::buildTexDrawUnitWithTex(drawUnit, tex, true);

    }
    // fbo test
    /*
    if (entity.id == 2)
    {
        tempColor = 0xffaaaa00;

        //auto pw   = vb.width();
        //auto ph   = vb.height();
        //vb.outset(30, 30);
        //vb.floatToRound();
        //auto        pw2       = vb.width();
        //auto        ph2       = vb.height();
        //auto        pos      = vb.min;

        uint32_t    gridSize = 256;
        Math::Mat33 vpMRtt;
        Math::Mat33 projMRtt;

        projMRtt.ortho(gridSize, gridSize);
        vpMRtt = projMRtt;

        auto& graph = rctx.fboGraph;

        auto scale = 1.0f;

        auto hasNode = graph.hasNode();

        Math::Mat33  projM = rctx.drawParam.projMat;
        Math::Mat33  pvwM = wM;

        if (hasNode)
        {
            auto&& node = graph.topNode();
            auto vm = node.viewMat;
            projM   = node.projMat;

            // model space to rtt view space
            pvwM = node.viewMat;
            pvwM.append(wM);
            Scene::Component::defaultRect.mat33MapTo(pvwM, vb);

            vb.outset(30, 30);
            vb.floatToRound();

            auto&& viewM = Math::Mat33::makeTranslate(-vb.min.x, -vb.min.y);
            vpMRtt.append(viewM);
        }
        else
        {
            vb.outset(30, 30);
            vb.floatToRound();
            Math::Mat33 viewM;
            viewM.setScaleXY(scale, scale);
            viewM.setXY(-vb.min.x * scale, -vb.min.y * scale);
            vpMRtt.append(viewM);
        }

        clearParam.clearColor = {0.0f, 0, 0.0f, 0.0f};
        clearParam.viewport   = {0, 0, gridSize, gridSize};

        // build fbo rendering process
        Render::Draw::FBOCtxNode fboCtx;
        fboCtx.clearParam          = clearParam;
        fboCtx.texUnits            = {{0, gridSize, gridSize, rttUnit.getTextureAt(0)}};
        std::string debugEnterInfo = "Render Sys RTT Begin ...";
        std::string debugExitInfo  = "Render Sys RTT End ...";
        auto&& nodeGuard = rctx.makeFBOGraphNodeGuard(fboCtx, debugEnterInfo, debugExitInfo);

        drawUnit.blendMode         = 1;
        drawUnit.setColor(tempColor);
        drawUnit.objMat = pvwM;
        drawUnit.mvp    = vpMRtt;
        drawUnit.draw();
        auto rttTex = nodeGuard.getRTTextureAt(0);
        printf("Render >>> nodeGuard.getRTTextureAt(0): %d\n", rttTex);
        Gpu::buildTexDrawUnitWithTex(rttUnit, rttTex, true);
        nodeGuard.execExitFunc();

        // rendering other content
        printf("render curr 2 ...\n");

        rttUnit.blendMode = 1;
        rttUnit.objMat.setTranslateAndScale(vb.min.x, vb.min.y, gridSize, gridSize);
        rttUnit.mvp = projM;
        rttUnit.draw();

        //drawUnit.blendMode = 1;
        //drawUnit.setColor(0x9000aa00);
        //drawUnit.objMat = wM;
        //drawUnit.mvp    = vpM;
        //drawUnit.draw();

        
        auto&& drawRUnit    = drs[0];
        drawRUnit.blendMode = 1;
        drawRUnit.vertex.lineWidth = 2.0f;
        drawRUnit.setColor(0xff000000);
        drawRUnit.vertex.toLine();
        drawRUnit.objMat.identity();
        drawRUnit.objMat.setXY(vb.min);
        drawRUnit.objMat.setScaleXY(vb.width(), vb.height());
        drawRUnit.mvp = projM;
        drawRUnit.draw();
        drawRUnit.vertex.toShape();
        return true;
    }
    //*/
    //printf("render curr ...\n");


    drawUnit.setColor(tempColor);
    //drawUnit.objMat = wM;
    drawUnit.objMat = svM;
    drawUnit.mvp    = vpM;
    drawUnit.draw();
    return true;
}

void EntityRenderSystem::clear()
{
}

} // namespace Voxol::Render