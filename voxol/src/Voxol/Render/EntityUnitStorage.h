#ifndef VOXOL_RENDER_ENTITY_UNIT_STORAGE_H
#define VOXOL_RENDER_ENTITY_UNIT_STORAGE_H

#include "EntityCompStorage.h"
#include "DrawingUnitStorage.h"
#include <string>


namespace Voxol::Render
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
    void initalize(int total = 512);

    EntityCompStorage::SP  comp{};
    DrawingUnitStorage::SP drawing{};

private:
    void parse();
};


} // namespace Voxol::Render
#endif
