#ifndef VOXOL_RENDER_ENTITY_BVH_H
#define VOXOL_RENDER_ENTITY_BVH_H

#include "../Base/BaseDefine.h"
#include "../Math/VxRect.h"
#include <vector>
#include <algorithm>

namespace Voxol::Render
{
namespace BVH
{
struct BVHNode
{
    Math::VxRect bounds;
    int          left  = -1;
    int          right = -1;
    int          begin = 0;
    int          end   = 0;
    bool         isLeaf() const { return left < 0 && right < 0; }
};
int buildBVH(std::vector<BVHNode>& nodes,
             std::vector<int>&     indices,
             int                   begin,
             int                   end);
void queryBVH(const std::vector<BVHNode>& nodes,
              int                         nodeIndex,
              const Math::VxRect&         queryBox,
              std::vector<int>&           outIndices);

} // namespace BVH
} // namespace Voxol::Render
#endif