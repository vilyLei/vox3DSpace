#include "EntityObjectBuilder.h"


namespace Voxol::Base
{


EntityObjectBuilder::SP EntityObjectBuilder::make()
{
    auto sp = std::make_shared<EntityObjectBuilder>();
    return sp;
}

EntityObjectBuilder::EntityObjectBuilder() noexcept
    :
    compFactory(arena)
{
    printf("EntityObjectBuilder::EntityObjectBuilder() ...\n");
}

EntityObjectBuilder::~EntityObjectBuilder()
{
    printf("EntityObjectBuilder::~EntityObjectBuilder() ...\n");
    compFactory.clearAllComponents();
}
VoxolEntity EntityObjectBuilder::createEntity(const std::string& name, VoxolEntity parent)
{
    auto e = eidFactory.create(name);
    compFactory.addComponent<EntityHierarchyComp>(e, {});
    if (parent != VoxolEntity_None && compFactory.hasComponent<EntityHierarchyComp>(parent))
    {
        compFactory.getComponent<EntityHierarchyComp>(parent)->children.push_back(e);
    }
    return e;
}

VoxolEntity EntityObjectBuilder::createRectFillEntity(const std::string& name, const TransformComp& trans, const RectShapeComp& rectShape, const SolidColorComp& color, VoxolEntity parent)
{
    VoxolEntity e = createEntity(name, parent);
    compFactory.addComponent<TransformComp>(e, trans);
    compFactory.addComponent<RectShapeComp>(e, rectShape);
    compFactory.addComponent<SolidColorComp>(e, color);
    return e;
}
VoxolEntity EntityObjectBuilder::createRectFillGradientBlurEntity(const std::string& name, const TransformComp& trans, const RectShapeComp& rectShape, const GradientColorComp& gradientColor, const BlurComp& blurComp, VoxolEntity parent)
{
    VoxolEntity e = createEntity(name, parent);
    compFactory.addComponent<TransformComp>(e, trans);
    compFactory.addComponent<RectShapeComp>(e, rectShape);
    compFactory.addComponent<GradientColorComp>(e, gradientColor);
    compFactory.addComponent<BlurComp>(e, blurComp);
    return e;
}


void EntityObjectBuilder::removeEntity(VoxolEntity entity) {
    eidFactory.remove(entity);
    compFactory.removeAllComponents(entity);
}

void EntityObjectBuilder::reset() {
    eidFactory.reset();
    compFactory.clearAllComponents();
    arena.reset();
}

} // namespace Voxol::Base