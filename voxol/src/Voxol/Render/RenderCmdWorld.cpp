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
    if (!mInit)
        return;
    using namespace Voxol::Math;

    auto rn    = 20;
    auto cn    = 20;
    auto total = rn * cn;
    auto rsize = 16;

    commands.resize(total);
    transforms.resize(total);
    objTransforms.resize(total);

    auto index = 0;
    for (auto i = 0; i < rn; ++i)
    {
        auto py = 5 + (i * (rsize + 1));
        for (auto j = 0; j < cn; ++j)
        {
            auto px = 5 + (j * (rsize + 1));
            commands[index] = 1;
            objTransforms[index] = Mat33(px, py, rsize, rsize);

            index++;
        }
    }

    // commands[0] = 3;
    // commands[1] = 5;
    // objTransforms[0] = Mat33(500, 200, 150, 150);
    // objTransforms[1] = Mat33(600, 300, 200, 270);

    mInit            = false;
}

void RenderCmdWorld::setGPUCtxSize(int w, int h)
{
    projMat.ortho(w, h);
}

void RenderCmdWorld::setMouseXY(float x, float y)
{
    using namespace Voxol::Math;
    objTransforms[0] = Mat33(x, y, 150, 150);
    dirty            = true;
}
void RenderCmdWorld::run()
{
    if (!dirty)
        return;
    dirty = false;


    initialize();

    printf("RenderCmdWorld::run() sizeof(projMat): %zu\n", sizeof(projMat));
    printf("RenderCmdWorld::run() objTransforms.size(): %zu\n", objTransforms.size());

    auto total = objTransforms.size();

    for (auto i = 0; i < total; i++)
    {
        transforms[i] = objTransforms[i];
        transforms[i].prepend(projMat);
    }
}

} // namespace Voxol::Render