#ifndef VOXOL_RENDER_SYSTEM_H
#define VOXOL_RENDER_SYSTEM_H

#include "../Base/EntityObjectBuilder.h"

namespace Voxol::System
{

using namespace Voxol::Base;
class RenderSystem
{

public:
    void operator()(EntityObjectBuilder& factory) const;

private:
    void renderEntity(const EntityObjectBuilder& factory, VoxolEntity e, int indent = 0) const;
};

} // namespace Voxol::System
#endif
