#ifndef VOXOL_COMPONENT_H
#define VOXOL_COMPONENT_H

#include <vector>
#include "BaseDefine.h"

namespace Voxol::Base
{

struct TransformComp
{
    float x = 0, y = 0;
};

struct RectShapeComp
{
    float x = 0, y = 0, width = 100, height = 100;
};
struct SolidColorComp
{
    uint32_t color    = 0xFFFFFFFF;
};
struct GradientColorComp
{
    uint32_t startColor = 0xFFFFFFFF;
    uint32_t endColor   = 0xFF0000FF;
};

struct BlurComp
{
    float radius = 5.0f;
};

/// Entity Tree Relationship
struct EntityHierarchyComp
{
    VoxolEntity              parent = VoxolEntity_None;
    std::vector<VoxolEntity> children;
};
} // namespace Voxol::Base

#endif