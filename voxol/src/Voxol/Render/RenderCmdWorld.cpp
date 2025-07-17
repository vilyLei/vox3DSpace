#include "RenderCmdWorld.h"
#include <cstdio>
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

    commands[0] = 3;
    commands[1] = 5;

    objTransforms[0] = Mat33(500, 200, 150, 150);
    objTransforms[1] = Mat33(600, 300, 200, 270);
    mInit = false;
}

void RenderCmdWorld::setGPUCtxSize(int w, int h) {
    projMat.ortho(w, h);
}

void RenderCmdWorld::setMouseXY(float x, float y)
{
    using namespace Voxol::Math;
    objTransforms[0] = Mat33(x, y, 150, 150);
    dirty = true;
}
void RenderCmdWorld::run()
{
    if(!dirty)
        return;
    dirty = false;


    initialize();

    printf("RenderCmdWorld::run() sizeof(projMat): %zu\n", sizeof(projMat));
    printf("RenderCmdWorld::run() objTransforms.size(): %zu\n", objTransforms.size());
    
    auto total = objTransforms.size();

    for(auto i = 0; i < total; i++) {
        transforms[i] = objTransforms[i];
        transforms[i].prepend(projMat);
    }
}

} // namespace Voxol::Render