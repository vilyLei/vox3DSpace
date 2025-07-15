#ifndef VOXOL_ENTITY_ID_FACTORY_H
#define VOXOL_ENTITY_ID_FACTORY_H

#include <unordered_set>
#include <unordered_map>
#include <string>
#include "BaseDefine.h"

namespace Voxol::Base {

class EntityIDFactory {

    VoxolEntity nextId = 1;
    std::unordered_set<VoxolEntity> activeEntities;
    std::unordered_map<VoxolEntity, std::string> entityNames;
    std::unordered_map<std::string, VoxolEntity> nameToEntity;

public:
    VoxolEntity create(const std::string& name = "");

    std::string getName(VoxolEntity e) const;

    VoxolEntity getByName(const std::string& name) const;

    void destroy(VoxolEntity id);

    bool isAlive(VoxolEntity id) const;

    void reset();
};

} // namespace Voxol::Base


#endif  // VOXOL_ENTITY_ID_FACTORY_H