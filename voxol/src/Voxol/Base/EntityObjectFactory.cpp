#include "EntityObjectFactory.h"


namespace Voxol::Base
{


EntityObjectFactory::SP EntityObjectFactory::make()
{
    auto sp = std::make_shared<EntityObjectFactory>();
    return sp;
}

EntityObjectFactory::EntityObjectFactory() noexcept
    :
    compFactory(arena)
{
    printf("EntityObjectFactory::EntityObjectFactory() ...\n");
}

EntityObjectFactory::~EntityObjectFactory() {
    printf("EntityObjectFactory::~EntityObjectFactory() ...\n");
    compFactory.clearAllComponents();
}
VoxolEntity EntityObjectFactory::createLayer(const std::string& name, VoxolEntity parent)
{
    auto e = eidFactory.create(name);
    compFactory.addComponent<EntityHierarchyComp>(e, {});
    // hierarchies.add(e, {parent, {}});
    // if (parent && hierarchies.has(parent)) {
    if (parent && compFactory.hasComponent<EntityHierarchyComp>(parent))
    {
        // hierarchies.get(parent)->children.push_back(e);
        compFactory.getComponent<EntityHierarchyComp>(parent)->children.push_back(e);
    }
    return e;
}

} // namespace Voxol::Base