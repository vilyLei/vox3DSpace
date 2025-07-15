#ifndef VOXOL_ENTITY_OBJECT_FACTORY_H
#define VOXOL_ENTITY_OBJECT_FACTORY_H

#include "EntityIDFactory.h"
#include "ComponentFactory.h"
#include "Component.h"

namespace Voxol::Base
{
class EntityObjectFactory
{
public:
    using SP = std::shared_ptr<EntityObjectFactory>;
    using WP = std::weak_ptr<EntityObjectFactory>;
    static EntityObjectFactory::SP make();

public:
    explicit EntityObjectFactory() noexcept;
    ~EntityObjectFactory();

public:
    ComponentFactory compFactory;
    EntityIDFactory eidFactory{};
    
public:

    VoxolEntity createEntity(const std::string& name, VoxolEntity parent = VoxolEntity_None);
    VoxolEntity createRectFillEntity(const std::string& name, const TransformComp& trans, const RectShapeComp& rectShape, const SolidColorComp& color = {}, VoxolEntity parent = VoxolEntity_None);
    VoxolEntity createRectFillGradientBlurEntity(const std::string& name, const TransformComp& trans, const RectShapeComp& rectShape, const GradientColorComp& gradientColor, const BlurComp& blurComp, VoxolEntity parent = VoxolEntity_None);

    void removeEntity(VoxolEntity entity);
    void reset();

private:
    SlabArena       arena{};
};

} // namespace Voxol::Base

#endif // VOXOL_ENTITY_OBJECT_FACTORY_H