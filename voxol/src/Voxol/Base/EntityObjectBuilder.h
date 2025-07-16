#ifndef VOXOL_ENTITY_OBJECT_BUILDER_H
#define VOXOL_ENTITY_OBJECT_BUILDER_H

#include "EntityIDFactory.h"
#include "ComponentWorld.h"
#include "Component.h"

namespace Voxol::Base
{
class EntityObjectBuilder
{
public:
    using SP = std::shared_ptr<EntityObjectBuilder>;
    using WP = std::weak_ptr<EntityObjectBuilder>;
    static EntityObjectBuilder::SP make();

public:
    explicit EntityObjectBuilder() noexcept;
    ~EntityObjectBuilder();

public:
    ComponentWorld compFactory;
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