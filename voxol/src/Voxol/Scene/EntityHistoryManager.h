#ifndef VOXOL_SCENE_ENTITY_HISTORY_MANAGER_H
#define VOXOL_SCENE_ENTITY_HISTORY_MANAGER_H

#include "../Base/BaseDefine.h"
#include "../Math/Vec2.h"
#include "EntityComponent.h"

namespace Voxol::Scene
{

struct HistoryItemData
{
    Component::UnitTransform trans;
    Base::ID::KeyUint64  id;
};

class EntityHistoryManager
{
public:
    using SP = std::shared_ptr<EntityHistoryManager>;
    using WP = std::weak_ptr<EntityHistoryManager>;
    using UP = std::unique_ptr<EntityHistoryManager>;

public:
    static EntityHistoryManager::SP make();

public:
    EntityHistoryManager()  = default;
    ~EntityHistoryManager() = default;

public:
    void            initalize();
    HistoryItemData popItem();
    void            pushItem(const HistoryItemData& itemData);

private:
    std::vector<HistoryItemData> mList;
};
} // namespace Voxol::Scene
#endif