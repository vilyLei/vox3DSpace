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

    if (mList.empty())
    {
        return HistoryItemData{.id = Base::INVALID_ID};
    }
    // 下面这句auto会导致debug模式下ide看到的itemData值是错的
    //auto itemData = mList.back();
    // ，但是下面这样指明类型就是对的了
    //HistoryItemData itemData = std::move(mList.back());

    auto&&          itemData = mList.back();
    mList.pop_back();
    return itemData;
}

void EntityHistoryManager::pushItem(const HistoryItemData& itemData) {
    mList.emplace_back(itemData);
}

} // namespace Voxol::Render