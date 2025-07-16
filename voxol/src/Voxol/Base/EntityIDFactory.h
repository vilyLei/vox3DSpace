#ifndef VOXOL_ENTITY_ID_FACTORY_H
#define VOXOL_ENTITY_ID_FACTORY_H

#include <unordered_set>
#include <unordered_map>
#include <string>
#include "BaseDefine.h"

namespace Voxol::Base {

// class EntityIDFactory {
//     VoxolEntity nextId = 1;
//     std::unordered_set<VoxolEntity> activeEntities;
//     std::unordered_map<VoxolEntity, std::string> entityNames;
//     std::unordered_map<std::string, VoxolEntity> nameToEntity;
// public:
//     VoxolEntity create(const std::string& name = "");
//     std::string getName(VoxolEntity e) const;
//     VoxolEntity getByName(const std::string& name) const;
//     void remove(VoxolEntity id);
//     bool isAlive(VoxolEntity id) const;
//     void reset();
// };

class EntityIDFactory {
public:
    EntityIDFactory();

    VoxolEntity create(const std::string& name = "");

    std::string getName(VoxolEntity e) const;
    VoxolEntity getByName(const std::string& name) const;

    void remove(VoxolEntity entity);
    bool isAlive(VoxolEntity entity) const;
    uint8_t version(VoxolEntity entity) const;
    void reset();

private:
    std::unordered_map<VoxolEntity, std::string> entityNames;
    std::unordered_map<std::string, VoxolEntity> nameToEntity;

    static constexpr VoxolEntity indexBits   = 24;
    static constexpr VoxolEntity versionBits = 8;
    static constexpr VoxolEntity indexMask   = (1u << indexBits) - 1;
    static constexpr VoxolEntity versionMask = (1u << versionBits) - 1;

    static VoxolEntity makeEntity(VoxolEntity index, uint8_t version);
    static VoxolEntity extractIndex(VoxolEntity entity);
    static uint8_t extractVersion(VoxolEntity entity);

    class Impl;
    Impl* impl;
};


} // namespace Voxol::Base


#endif  // VOXOL_ENTITY_ID_FACTORY_H