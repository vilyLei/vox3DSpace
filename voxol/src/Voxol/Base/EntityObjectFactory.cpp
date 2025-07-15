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

EntityObjectFactory::~EntityObjectFactory()
{
    printf("EntityObjectFactory::~EntityObjectFactory() ...\n");
    compFactory.clearAllComponents();
}
VoxolEntity EntityObjectFactory::createEntity(const std::string& name, VoxolEntity parent)
{
    auto e = eidFactory.create(name);
    compFactory.addComponent<EntityHierarchyComp>(e, {});
    if (parent && compFactory.hasComponent<EntityHierarchyComp>(parent))
    {
        compFactory.getComponent<EntityHierarchyComp>(parent)->children.push_back(e);
    }
    return e;
}

VoxolEntity EntityObjectFactory::createRectFillEntity(const std::string& name, const TransformComp& transComp, const RectFillComp& rectFillComp, VoxolEntity parent)
{
    VoxolEntity e = createEntity(name, parent);
    compFactory.addComponent<TransformComp>(e, transComp);
    compFactory.addComponent<RectFillComp>(e, rectFillComp);
    return e;
}
VoxolEntity EntityObjectFactory::createRectFillGradientBlurEntity(const std::string& name, const TransformComp& transComp, const RectFillComp& rectFillComp, const GradientComp& gradientComp, const BlurComp& blurComp, VoxolEntity parent)
{
    VoxolEntity e = createEntity(name, parent);
    compFactory.addComponent<TransformComp>(e, transComp);
    compFactory.addComponent<RectFillComp>(e, rectFillComp);
    compFactory.addComponent<GradientComp>(e, gradientComp);
    compFactory.addComponent<BlurComp>(e, blurComp);
    return e;
}


void EntityObjectFactory::removeEntity(VoxolEntity entity) {
    eidFactory.remove(entity);
    compFactory.removeAllComponents(entity);
}

void EntityObjectFactory::reset() {
    eidFactory.reset();
    compFactory.clearAllComponents();
    arena.reset();
}

} // namespace Voxol::Base