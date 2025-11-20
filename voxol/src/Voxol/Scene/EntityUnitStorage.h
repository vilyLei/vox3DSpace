#ifndef VOXOL_SCENE_ENTITY_UNIT_STORAGE_H
#define VOXOL_SCENE_ENTITY_UNIT_STORAGE_H

#include "EntityCompStorage.h"
#include "../Render/DrawingUnitStorage.h"


namespace Voxol::Scene
{

class EntityUnitStorage
{
public:
    using SP = std::shared_ptr<EntityUnitStorage>;
    using WP = std::weak_ptr<EntityUnitStorage>;
    using UP = std::unique_ptr<EntityUnitStorage>;

public:
    static EntityUnitStorage::SP make();

public:
    EntityUnitStorage()  = default;
    ~EntityUnitStorage() = default;

public:
    void initalizeFromFile(const std::string& fileName);
    void initalizeFromDescFile(const std::string& fileName);
    void initalizeFromIRFile(const std::string& fileName);
    void initalize(int total = 512);

    std::vector<Render::Gpu::DrawingUnit> getDrawUnitsFromText(const std::string& text, float fontSize = 36, const Voxol::Math::Vec2& pos = {});
    EntityCompStorage::SP  comp{};
    Render::DrawingUnitStorage::SP drawing{};

private:
};


} // namespace Voxol::Scene
#endif
