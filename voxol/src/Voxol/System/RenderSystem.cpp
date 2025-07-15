
#include "RenderSystem.h"

namespace Voxol::System
{
void RenderSystem::renderEntity(const EntityObjectFactory& factory, VoxolEntity e, int indent) const
{
    ///*

    if (!factory.eidFactory.isAlive(e)) return;

    auto& compFactory = factory.compFactory;
    
    const std::string& name  = factory.eidFactory.getName(e);
    const auto   tf    = compFactory.getComponent<TransformComp>(e);
    const auto   shape = compFactory.getComponent<RectShapeComp>(e);
    // const auto   shape = compFactory.getComponent<RectShapeComp>(e);

    for (int i = 0; i < indent; ++i) printf("  ");
    printf("VoxolEntity %u (%s)\n", e, name.c_str());

    if (shape)
    {
        for (int i = 0; i < indent + 1; ++i) printf("  ");
        printf("Rect at (%.1f, %.1f) size(%.1f, %.1f)\n",
               tf ? tf->x : 0.0f,
               tf ? tf->y : 0.0f,
               shape->width,
               shape->height);

        if (compFactory.hasComponent<GradientColorComp>(e))
        {
            const auto ptr = compFactory.getComponent<GradientColorComp>(e);
            for (int i = 0; i < indent + 2; ++i) printf("  ");
            printf("Gradient: 0x%08X -> 0x%08X\n", ptr->startColor, ptr->endColor);
        }
        if (compFactory.hasComponent<SolidColorComp>(e))
        {
            const auto ptr = compFactory.getComponent<SolidColorComp>(e);
            for (int i = 0; i < indent + 2; ++i) printf("  ");
            printf("Solid Color: 0x%08X\n", ptr->color);
        }

        if (compFactory.hasComponent<BlurComp>(e))
        {
            const auto ptr = compFactory.getComponent<BlurComp>(e);
            for (int i = 0; i < indent + 2; ++i) printf("  ");
            printf("Blur Radius: %.1f\n", ptr->radius);
        }
    }

    if (compFactory.getComponent<EntityHierarchyComp>(e))
    {
        const auto ptr = compFactory.getComponent<EntityHierarchyComp>(e);
        for (VoxolEntity child : ptr->children)
        {
            renderEntity(factory, child, indent + 1);
        }
    }
    //*/
}

void RenderSystem::operator()(EntityObjectFactory& factory) const
{
    printf("[RenderSystem] Rendering Layer Tree:\n");
    factory.compFactory.each<EntityHierarchyComp>([&, this](VoxolEntity entity, EntityHierarchyComp& h) {
        if (h.parent != 0)
            return;
        renderEntity(factory, entity);
    });
    // for (const auto& [entity, h] : const_cast<EntityObjectFactory&>(world).hierarchies.all()) {
    //     if (h->parent == 0) {
    //         renderEntity(world, entity);
    //     }
    // }
}
} // namespace Voxol::System