
#include "BVH2DV2.h"
#include <stack>
#include <algorithm>

namespace Voxol::Render
{
namespace V2 {

BVH2D_LazyGC::SP BVH2D_LazyGC::make()
{
    return std::make_shared<BVH2D_LazyGC>();
}
}
}