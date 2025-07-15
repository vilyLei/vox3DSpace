#ifndef VOXOL_RENDER_SYSTEM_H
#define VOXOL_RENDER_SYSTEM_H

#include "../Base/EntityObjectFactory.h"

namespace Voxol::System
{

using namespace Voxol::Base;
class RenderSystem
{

public:
    void operator()(EntityObjectFactory& factory) const;

private:
    void renderEntity(const EntityObjectFactory& factory, VoxolEntity e, int indent = 0) const;
};

} // namespace Voxol::System
#endif
