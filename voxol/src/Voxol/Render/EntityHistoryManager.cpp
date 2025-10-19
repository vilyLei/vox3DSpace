#include "EntityHistoryManager.h"

namespace Voxol::Render
{
    
EntityHistoryManager::SP EntityHistoryManager::make() {
    auto sp = std::make_shared<EntityHistoryManager>();
    return sp;
}
void EntityHistoryManager::initalize()
{
}

HistoryItemData EntityHistoryManager::popItem() {

    if(mList.empty()) {
        HistoryItemData item{};
        item.id = -1;
        return item;
    }
    auto itemData = std::move(mList.back());
    mList.pop_back();
    return itemData;
}

void EntityHistoryManager::pushItem(const HistoryItemData& itemData) {
    mList.emplace_back(itemData);
}

} // namespace Voxol::Render