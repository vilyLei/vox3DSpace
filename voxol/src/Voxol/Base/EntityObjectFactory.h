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
    VoxolEntity      createLayer(const std::string& name, VoxolEntity parent = VoxolEntity_None);
    VoxolEntity      createRectFillEntity(const std::string& name, const TransformComp& transComp, const RectFillComp& rectFillComp, VoxolEntity parent = VoxolEntity_None);
    VoxolEntity createRectFillGradientBlurEntity(const std::string& name, const TransformComp& transComp, const RectFillComp& rectFillComp, const GradientComp& gradientComp, const BlurComp& blurComp, VoxolEntity parent = VoxolEntity_None);

private:
    EntityIDFactory eidFactory{};
    SlabArena       arena{};
};

} // namespace Voxol::Base

#endif // VOXOL_ENTITY_OBJECT_FACTORY_H