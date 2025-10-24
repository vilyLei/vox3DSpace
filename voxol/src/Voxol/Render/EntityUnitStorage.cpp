#include "EntityUnitStorage.h"
#include <unordered_map>

namespace Voxol::Render
{

EntityUnitStorage::SP EntityUnitStorage::make()
{
    auto sp = std::make_shared<EntityUnitStorage>();
    return sp;
}

void EntityUnitStorage::parse() {


    // entities
    auto total = 128;
    total = total < 128 ? 128 : total;

    
    comp = EntityCompStorage::make();
    if (!drawing)
    {
        drawing = DrawingUnitStorage::make();
    }
    drawing->initalize(total);


    auto& entitiesPool          = comp->entitiesPool;
    auto& shaderingEntitiesPool = comp->shaderingEntitiesPool;
    auto& shaderingDescPool     = comp->shaderingDescPool;

    entitiesPool.initialize(total);
    shaderingEntitiesPool.initialize(total);
    auto shaderingDescTotal = total * 2;
    shaderingDescPool.initialize(shaderingDescTotal);

    
    entitiesPool.forEach([&](auto& e, int32_t index) {
        e.id = index;
    });
    shaderingEntitiesPool.forEach([&](auto& e, int32_t index) {
        e.id = index;
    });

}

void EntityUnitStorage::initalizeFromFile(const std::string& fileName)
{
    auto fileNameStr = fileName;
    if (fileNameStr.empty())
    {
        fileNameStr = "IR/scIR01.json";
    }

    //parse();
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
    auto& transformPool     = comp->transformPool;

    entitiesPool.initialize(total);
    shaderingEntitiesPool.initialize(total);
    auto shaderingDescTotal = total * 2;
    shaderingDescPool.initialize(shaderingDescTotal);
    transformPool.initialize(shaderingDescTotal);

    entitiesPool.forEach([&](auto& e, int32_t index) {
        e.id = index;
    });
    shaderingEntitiesPool.forEach([&](auto& e, int32_t index) {
        e.id = index;
    });

    shaderingDescPool[0].color    = 0xff880077;
    transformPool[0]     = {150, 50, 200, 200, 0};
    shaderingDescPool[1].color     = 0xff008855;
    transformPool[1]    = {150, 50, 200, 200, 0};

    shaderingDescPool[2].color    = 0xff554433;
    transformPool[2] = {510, 150, 100, 100, 0};

    shaderingDescPool[3].color    = 0xff660066;
    transformPool[3] = {250, 50, 150, 150, 0};

    shaderingDescPool[4].color     = 0xff00aa76;
    transformPool[4] = {600, 150, 200, 200, 0};

    shaderingDescPool[5].color     = 0xff00aa76;
    transformPool[5] = {509, 350, 200, 200, 0};

    shaderingDescPool[6].color     = 0xff00aabb;
    transformPool[6]     = {350, 350, 200, 200, 0};
    shaderingDescPool[7].color     = 0xff00aabb;
    transformPool[7]     = {350, 350, 200, 200, 0};
    shaderingDescPool[8].color     = 0xff00aabb;
    transformPool[8]     = {350, 350, 200, 200, 0};

    /// letter B glyph
    //auto&& tb                      = drawing->msdfText.getGlyphBounds('D', 70, {350, 350});
    //shaderingDescPool[6].transform = {tb.x(), tb.y(), tb.width(), tb.height()};

    auto&& vbs = drawing->msdfText.getStringBounds("BcD", 70, {350, 350});
    auto   tb                      = vbs[0];
    transformPool[6]     = {tb.x(), tb.y(), tb.width(), tb.height()};
    tb                             = vbs[1];
    transformPool[7]     = {tb.x(), tb.y(), tb.width(), tb.height()};
    tb                             = vbs[2];
    auto sx                        = tb.width();
    transformPool[8]     = {tb.x(), tb.y(), tb.width(), tb.height()};

    /// circle
    shaderingEntitiesPool[0].drawUnitId    = drawing->getIdWithType(DrawingUnitType::Circle);
    shaderingEntitiesPool[0].shadingDescId = 0;
    /// circle
    shaderingEntitiesPool[1].drawUnitId    = drawing->getIdWithType(DrawingUnitType::Circle);
    shaderingEntitiesPool[1].shadingDescId = 1;
    /// circle
    shaderingEntitiesPool[2].drawUnitId    = drawing->getIdWithType(DrawingUnitType::Circle);
    shaderingEntitiesPool[2].shadingDescId = 2;

    /// ring
    shaderingEntitiesPool[3].drawUnitId    = drawing->getIdWithType(DrawingUnitType::Ring);
    shaderingEntitiesPool[3].shadingDescId = 3;
    
    /// multi-circles
    //shaderingEntitiesPool[4].drawUnitId    = drawing->getIdWithType(DrawingUnitType::MultiCircle);
    shaderingEntitiesPool[4].drawUnitId    = drawing->getIdWithType(DrawingUnitType::strokeShape);
    shaderingEntitiesPool[4].shadingDescId = 4;
    auto drawUnitId_5                     = drawing->getIdWithName("ship01.glsl");
    shaderingEntitiesPool[5].drawUnitId    = drawUnitId_5;
    shaderingEntitiesPool[5].shadingDescId = 5;


    auto drawUnitId_6    = drawing->getGlyphIdWithChar('B');
    shaderingEntitiesPool[6].drawUnitId    = drawUnitId_6;
    shaderingEntitiesPool[6].shadingDescId = 6;

    shaderingEntitiesPool[7].drawUnitId    = drawing->getGlyphIdWithChar('c');
    shaderingEntitiesPool[7].shadingDescId = 7;

    shaderingEntitiesPool[8].drawUnitId    = drawing->getGlyphIdWithChar('D');
    shaderingEntitiesPool[8].shadingDescId = 8;

    ///// circle
    //entitiesPool[0].shadingId = 6;
    //return;

    /// circle
    entitiesPool[0].shadingId = 0;
    entitiesPool[0].transformId = 0;
    ///// circle
    entitiesPool[1].shadingId = 1;
    entitiesPool[1].transformId = 1;
    /// circle
    entitiesPool[2].shadingId = 2;
    entitiesPool[2].transformId = 2;
    /// ring
    entitiesPool[3].shadingId = 3;
    entitiesPool[3].transformId = 3;
    /// multi-circles
    entitiesPool[4].shadingId = 4;
    entitiesPool[4].transformId = 4;
    /// ship
    entitiesPool[5].shadingId = 5;
    entitiesPool[5].transformId = 5;
    /// B
    entitiesPool[6].shadingId = 6;
    entitiesPool[6].transformId = 6;
    /// c
    entitiesPool[7].shadingId = 7;
    entitiesPool[7].transformId = 7;
    /// D
    entitiesPool[8].shadingId = 8;
    entitiesPool[8].transformId = 8;
}


} // namespace Voxol::Render