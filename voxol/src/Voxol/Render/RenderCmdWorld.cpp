#include "RenderCmdWorld.h"
namespace Voxol::Render
{
RenderCmdWorld::RenderCmdWorld(/* args */)
{
}

RenderCmdWorld::~RenderCmdWorld()
{
}
void RenderCmdWorld::initialize()
{
    if(!mInit)
        return;
    using namespace Voxol::Math;
    
    auto total = 0;
    commands.resize(2);
    transforms.resize(2);
    objTransforms.resize(2);

    objTransforms[0] = Mat33(500, 200, 150, 150);
    objTransforms[1] = Mat33(600, 300, 200, 50);
    mInit = false;
}
void RenderCmdWorld::run()
{
    if(!dirty)
        return;
    dirty = false;

    initialize();

    auto total = objTransforms.size();

    for(auto i = 0; i < total; i++) {
        transforms[i] = objTransforms[i];
        transforms[i].prepend(projMat);
    }
}

} // namespace Voxol::Render