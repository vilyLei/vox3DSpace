#ifndef VOXOL_ENTITY_H
#define VOXOL_ENTITY_H

#include <vector>
#include <unordered_map>
#include <cstdint>

using Entity = uint32_t;

class EntityManager {
    Entity nextId = 1;
public:
    Entity create() {
        return nextId++;
    }
};
#endif  // VOXOL_ENTITY_H