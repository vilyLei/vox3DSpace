#include "EntityUnitStorage.h"

namespace Voxol::Render
{
EntityUnitStorage::SP EntityUnitStorage::make()
{
    auto sp = std::make_shared<EntityUnitStorage>();
    return sp;
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

    //auto storage = comp;
    drawing->initalize(total);

    auto& entitiesPool          = comp->entitiesPool;
    auto& shaderingEntitiesPool = comp->shaderingEntitiesPool;
    auto& shaderingDescPool     = comp->shaderingDescPool;

    entitiesPool.initialize(total);
    shaderingEntitiesPool.initialize(total);

    auto shaderingDescTotal = total * 2;
    shaderingDescPool.initialize(shaderingDescTotal);

    //for (auto i = 0; i < entities.size(); ++i)
    //{
    //    entities[i].id = i;
    //}

    entitiesPool.forEach([&](auto& e, int32_t index) {
        e.id = index;
    });
    shaderingEntitiesPool.forEach([&](auto& e, int32_t index) {
        e.id = index;
    });

    shaderingDescPool[0].color    = 0xff880077;
    shaderingDescPool[0].transform = {150, 50, 200, 200, 0};
    shaderingDescPool[1].color     = 0xff008855;
    shaderingDescPool[1].transform = {150, 50, 200, 200, 0};

    shaderingDescPool[2].color    = 0xff554433;
    shaderingDescPool[2].transform = {510, 150, 100, 100, 0};

    shaderingDescPool[3].color    = 0xff660066;
    shaderingDescPool[3].transform = {250, 50, 150, 150, 0};

    shaderingDescPool[4].color     = 0xff00aa76;
    shaderingDescPool[4].transform = {600, 150, 200, 200, 0};

    shaderingDescPool[5].color     = 0xff00aa76;
    shaderingDescPool[5].transform = {509, 350, 200, 200, 0};

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
    auto drawUnitId_4                      = drawing->getIdWithName("ship01.glsl");
    shaderingEntitiesPool[5].drawUnitId    = drawUnitId_4;
    shaderingEntitiesPool[5].shadingDescId = 5;

    /// circle
    //entities[0].shadingId = 4;

    /// circle
    entitiesPool[0].shadingId = 0;
    ///// circle
    entitiesPool[1].shadingId = 1;
    /// circle
    entitiesPool[2].shadingId = 2;
    /// ring
    entitiesPool[3].shadingId = 3;
    /// multi-circles
    entitiesPool[4].shadingId = 4;
    /// ship
    entitiesPool[5].shadingId = 5;
}


} // namespace Voxol::Render