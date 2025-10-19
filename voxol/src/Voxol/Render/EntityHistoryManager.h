#ifndef VOXOL_RENDER_ENTITY_HISTORY_MANAGER_H
#define VOXOL_RENDER_ENTITY_HISTORY_MANAGER_H

#include "../Base/BaseDefine.h"
#include "../Math/Vec2.h"
#include "EntityComponent.h"

namespace Voxol::Render
{

struct HistoryItemData
{
    Component::UnitTransform trans;
    int32_t                  id;
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
    void initalize();
    HistoryItemData popItem();
    void pushItem(const HistoryItemData& itemData);
private:
    std::vector<HistoryItemData> mList;
};
}
#endif