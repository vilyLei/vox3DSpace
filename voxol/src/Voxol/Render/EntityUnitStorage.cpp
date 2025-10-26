#include "EntityUnitStorage.h"
#include "SceneIRParser.h"

namespace Voxol::Render
{

EntityUnitStorage::SP EntityUnitStorage::make()
{
    auto sp = std::make_shared<EntityUnitStorage>();
    return sp;
}


void EntityUnitStorage::initalizeFromFile(const std::string& fileName)
{
    if (comp)
    {
        return;
    }
    auto fileNameStr = fileName;
    if (fileNameStr.empty())
    {
        fileNameStr = "scene/IR/scIR01.json";
    }
    SceneIRParser parser;
    parser.parseFromFile(fileNameStr);
    auto& shaderingModule = parser.shaderingModule;
    auto& sceneModule     = parser.sceneModule;

    
    auto total = 128;
    total      = total < 128 ? 128 : total;


    comp = EntityCompStorage::make();
    if (!drawing)
    {
        drawing = DrawingUnitStorage::make();
    }
    drawing->initalize(total);


    auto& entitiesPool          = comp->entitiesPool;
    auto& shaderingEntitiesPool = comp->shaderingEntitiesPool;
    auto& shaderingDescPool     = comp->shaderingDescPool;
    auto& transformsPool        = comp->transformsPool;
    auto& modelsPool            = comp->modelsPool;

    entitiesPool.initialize(total);
    shaderingEntitiesPool.initialize(total);
    modelsPool.initialize(total);
    auto shaderingDescTotal = total * 2;
    shaderingDescPool.initialize(shaderingDescTotal);
    transformsPool.initialize(shaderingDescTotal);


    entitiesPool.forEach([&](auto& e, int32_t index) {
        e.id = index;
    });
    shaderingEntitiesPool.forEach([&](auto& e, int32_t index) {
        e.id = index;
    });
    modelsPool.forEach([&](auto& e, int32_t index) {
        e.id = index;
    });

    
    auto updateUnitData = [&, this](int i) {

        auto& descMap = shaderingModule.descriptionsMap;
        auto& unitMap = shaderingModule.unitsMap;

        auto&& unit     = unitMap[i];
        auto&  descData = descMap[unit.description];

        //auto& model = unit.model;
        auto& desc  = shaderingDescPool[i];
        desc.color  = descData.color;
        printf("        desc.color: %x\n", desc.color);
        auto&& descEt = shaderingEntitiesPool[i];
        //descEt.drawUnitId = model.id;
        descEt.shadingDescId = unit.description;
    };

    auto updateEntityData = [&, this](int i) {

        auto& dataEt = sceneModule.entitiesMap[i];

        auto&& et       = entitiesPool[dataEt.id];
        et.shadingId      = dataEt.shadering;
        et.transformId  = dataEt.transform;
        et.modelId  = dataEt.model;
        et.visible        = dataEt.visible;

        auto&& dataTrans = sceneModule.transformsMap[et.transformId];
        auto&& dataModel = sceneModule.modelsMap[et.modelId];
        auto&& model = modelsPool[et.modelId];
        model.drawUnitId = dataModel.method.id;

        auto& unitMap = shaderingModule.unitsMap;
        auto&& unit    = unitMap[i];
        auto&& trans   = transformsPool[et.transformId];

        trans.x        = dataTrans.position.x;
        trans.y        = dataTrans.position.y;
        if (dataModel.hasRadius())
        {
            auto r   = dataModel.getRadius() * 2;
            trans.sx = r;
            trans.sy = r;
        }
        else if (dataModel.hasSize())
        {
            auto&& sv = dataModel.getSize();
            trans.sx  = sv.x;
            trans.sy  = sv.y;
        }
        printf("entity(%d), modelId: %d, drawUnitId: %d\n", i, et.modelId, model.drawUnitId);
        printf("        pos(%f,%f), size(%f, %f)\n", trans.x, trans.y, trans.sx, trans.sy);
    };

    for (auto& ut : shaderingModule.unitsMap)
    {
        updateUnitData(ut.second.id);
    }
    for (auto& et : sceneModule.entitiesMap)
    {
        updateEntityData(et.second.id);
    }
}
void EntityUnitStorage::initalize(int total)
{
    if (comp)
    {
        return;
    }

    total = total < 512 ? 512 : total;

    comp = EntityCompStorage::make();
    if (!drawing) {
        drawing = DrawingUnitStorage::make();
    }
    drawing->initalize(total);

    auto& entitiesPool          = comp->entitiesPool;
    auto& shaderingEntitiesPool = comp->shaderingEntitiesPool;
    auto& shaderingDescPool     = comp->shaderingDescPool;
    auto& transformsPool     = comp->transformsPool;
    auto& modelsPool            = comp->modelsPool;

    entitiesPool.initialize(total);
    shaderingEntitiesPool.initialize(total);
    modelsPool.initialize(total);
    auto shaderingDescTotal = total * 2;
    shaderingDescPool.initialize(shaderingDescTotal);
    transformsPool.initialize(shaderingDescTotal);

    entitiesPool.forEach([&](auto& e, int32_t index) {
        e.id = index;
    });
    shaderingEntitiesPool.forEach([&](auto& e, int32_t index) {
        e.id = index;
    });
    modelsPool.forEach([&](auto& e, int32_t index) {
        e.id = index;
    });

    shaderingDescPool[0].color    = 0xff880077;
    transformsPool[0]     = {150, 50, 200, 200, 0};
    shaderingDescPool[1].color     = 0xff008855;
    transformsPool[1]    = {150, 50, 200, 200, 0};

    shaderingDescPool[2].color    = 0xff554433;
    transformsPool[2] = {510, 150, 100, 100, 0};

    shaderingDescPool[3].color    = 0xff660066;
    transformsPool[3] = {250, 50, 150, 150, 0};

    shaderingDescPool[4].color     = 0xff00aa76;
    transformsPool[4] = {600, 150, 200, 200, 0};

    shaderingDescPool[5].color     = 0xff00aa76;
    transformsPool[5] = {509, 350, 200, 200, 0};

    shaderingDescPool[6].color     = 0xff00aabb;
    transformsPool[6]     = {350, 350, 200, 200, 0};
    shaderingDescPool[7].color     = 0xff00aabb;
    transformsPool[7]     = {350, 350, 200, 200, 0};
    shaderingDescPool[8].color     = 0xff00aabb;
    transformsPool[8]     = {350, 350, 200, 200, 0};

    /// letter B glyph
    //auto&& tb                      = drawing->msdfText.getGlyphBounds('D', 70, {350, 350});
    //shaderingDescPool[6].transform = {tb.x(), tb.y(), tb.width(), tb.height()};

    auto&& vbs = drawing->msdfText.getStringBounds("BcD", 70, {350, 350});
    auto   tb                      = vbs[0];
    transformsPool[6]     = {tb.x(), tb.y(), tb.width(), tb.height()};
    tb                             = vbs[1];
    transformsPool[7]     = {tb.x(), tb.y(), tb.width(), tb.height()};
    tb                             = vbs[2];
    auto sx                        = tb.width();
    transformsPool[8]     = {tb.x(), tb.y(), tb.width(), tb.height()};

    /// circle
    modelsPool[0].drawUnitId    = drawing->getIdWithType(DrawingUnitType::Circle);
    shaderingEntitiesPool[0].shadingDescId = 0;
    /// circle
    modelsPool[1].drawUnitId               = drawing->getIdWithType(DrawingUnitType::Circle);
    shaderingEntitiesPool[1].shadingDescId = 1;
    /// circle
    modelsPool[2].drawUnitId               = drawing->getIdWithType(DrawingUnitType::Circle);
    shaderingEntitiesPool[2].shadingDescId = 2;

    /// ring
    modelsPool[3].drawUnitId               = drawing->getIdWithType(DrawingUnitType::Ring);
    shaderingEntitiesPool[3].shadingDescId = 3;
    
    /// multi-circles
    //shaderingEntitiesPool[4].drawUnitId    = drawing->getIdWithType(DrawingUnitType::MultiCircle);
    modelsPool[4].drawUnitId               = drawing->getIdWithType(DrawingUnitType::strokeShape);
    shaderingEntitiesPool[4].shadingDescId = 4;
    auto drawUnitId_5                     = drawing->getIdWithName("ship01.glsl");
    modelsPool[5].drawUnitId               = drawUnitId_5;
    shaderingEntitiesPool[5].shadingDescId = 5;


    auto drawUnitId_6    = drawing->getGlyphIdWithChar('B');
    modelsPool[6].drawUnitId               = drawUnitId_6;
    shaderingEntitiesPool[6].shadingDescId = 6;

    modelsPool[7].drawUnitId               = drawing->getGlyphIdWithChar('c');
    shaderingEntitiesPool[7].shadingDescId = 7;

    modelsPool[8].drawUnitId               = drawing->getGlyphIdWithChar('D');
    shaderingEntitiesPool[8].shadingDescId = 8;

    ///// circle
    //entitiesPool[0].shadingId = 6;
    //return;

    /// circle
    entitiesPool[0].shadingId = 0;
    entitiesPool[0].transformId = 0;
    entitiesPool[0].modelId = 0;
    ///// circle
    entitiesPool[1].shadingId = 1;
    entitiesPool[1].transformId = 1;
    entitiesPool[1].modelId     = 1;
    /// circle
    entitiesPool[2].shadingId = 2;
    entitiesPool[2].transformId = 2;
    entitiesPool[2].modelId     = 2;
    /// ring
    entitiesPool[3].shadingId = 3;
    entitiesPool[3].transformId = 3;
    entitiesPool[3].modelId     = 3;
    /// multi-circles
    entitiesPool[4].shadingId = 4;
    entitiesPool[4].transformId = 4;
    entitiesPool[4].modelId     = 4;
    /// ship
    entitiesPool[5].shadingId = 5;
    entitiesPool[5].transformId = 5;
    entitiesPool[5].modelId     = 5;
    /// B
    entitiesPool[6].shadingId = 6;
    entitiesPool[6].transformId = 6;
    entitiesPool[6].modelId     = 6;
    /// c
    entitiesPool[7].shadingId = 7;
    entitiesPool[7].transformId = 7;
    entitiesPool[7].modelId     = 7;
    /// D
    entitiesPool[8].shadingId = 8;
    entitiesPool[8].transformId = 8;
    entitiesPool[8].modelId     = 8;
}


} // namespace Voxol::Render