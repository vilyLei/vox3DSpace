#include "RenderCmdWorld.h"
#include <cstdio>
#include <cmath>
namespace Voxol::Motion
{

RenderCmdWorld::RenderCmdWorld()
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


    // printf("RenderCmdWorld::initialize() viewMat:\n");
    // viewMat.print();
    // viewMat = Mat33::translate(100, 200);
    // printf("RenderCmdWorld::initialize() translate viewMat:\n");
    // viewMat.print();

    auto rn    = 3 * 1;
    auto cn    = 4 * 1;
    auto total = rn * cn;
    auto rsize = 70.0f;

    printf("Voxol::Motion::RenderCmdWorld::initialize() total: %d\n", total);

    buffer.resize(total * 2 * sizeof(Mat33) + 32);
    // build head data
    auto bytesTotal = sizeof(mHeadData);
    std::memcpy(buffer.data(), mHeadData, bytesTotal);

    commands.resize(total);
    cmdNodes.resize(total);

    auto  index = 0;
    float dis   = 2;
    for (auto i = 0; i < rn; ++i)
    {
        auto py = 50 + (i * (rsize + dis));
        for (auto j = 0; j < cn; ++j)
        {
            auto px         = 50 + (j * (rsize + dis));
            commands[index] = 1;
            auto& node      = cmdNodes[index];
            node.color      = 0xff0000aa | ((index % 256) << 16);
            node.color      = node.color | (((i * j * 2) % 256) << 16);
            // printf("node.color: %X\n", node.color);
            node.x      = px;
            node.y      = py;
            node.scaleX = rsize;
            node.scaleY = rsize;

            /*
            auto pindex = i * cn + j;
            if(pindex == 11) {
                node.scaleY = 1;
                node.x = px - (rsize + dis);
                // node.y = py - 0.25f;
                node.color = 0xffaa0000;
            }
            if(pindex == 10) {
                node.scaleX = rsize * 2;
                node.scaleY = 1;
                // node.x = px - (rsize + dis);
                node.y += (rsize + dis) * 2;
                node.rotation = 0.5f;
                // node.y = py - 0.25f;
                node.color = 0xffaa0000;
            }            
            if(pindex == 9) {
                // node.scaleX = rsize * 2;
                node.scaleY = 0.2f;
                // node.x = px - (rsize + dis);
                node.y += (rsize + dis);
                // node.rotation = 0.5f;
                // node.y = py - 0.25f;
                node.color = 0xffaa0000;
            }
            //*/

            node.moveingNode.rect = {node.x, node.y, rsize * 1.0f, rsize * 1.0f};
            node.init();

            index++;
        }
    }

    mInit = false;
}

void RenderCmdWorld::setGPUCtxSize(int w, int h)
{
    SizeDesc desc{static_cast<float>(w), static_cast<float>(h)};
    if (!canvas.size.isEqual(desc))
    {
        canvas.size = desc;
        projMat.ortho(w, h);
        dirty = true;
        // printf("RenderCmdWorld::setGPUCtxSize() ...\n");
    }
}

void RenderCmdWorld::setMouseXY(float x, float y)
{
    // using namespace Voxol::Math;

    mousePos             = {x, y};
    canvas.view.mousePos = mousePos;

    // auto& node = cmdNodes[0];
    // node.x     = x;
    // node.y     = y;
    // dirty      = true;
    // printf("RenderCmdWorld::setMouseXY(%f, %f)\n", x, y);
}

void RenderCmdWorld::setMouseParams(float x, float y, int type, float value)
{
    setMouseXY(x, y);

    auto  viewDirty = false;
    auto& view      = canvas.view;
    // printf("RenderCmdWorld::setMouseParams(), type: %d\n", type);
    switch (type)
    {
        case 11:
        {
            view.moveBegin(mousePos);
        }
        break;
        case 12:
        {
            view.moveEnd(mousePos);
        }
        break;
        case 3:
        {

            viewDirty = view.move(mousePos);
        }
        break;
        case 4:
        {
            auto dv   = std::abs(value) > 1 ? value * 0.01f : value;
            viewDirty = view.updateViewZoom(mousePos, -dv, 1.2f);
        }
        break;
        case 5:
        {

            printf("RenderCmdWorld::setMouseParams(), click, type: %d, value: %f\n", type, value);
            printf("RenderCmdWorld::setMouseParams(), click, mousePos(%f, %f)\n", mousePos.x, mousePos.y);
            auto  i    = 0;
            auto  pos  = mousePos;
            auto& node = cmdNodes[i];
            printf("RenderCmdWorld::setMouseParams(), click, node(x=%f, y=%f), scale(sx=%f, sy=%f)\n", node.x, node.y, node.scaleX, node.scaleY);
            Mat33 mat{};
            node.updateToMat33(mat);
            printf("mat:\n");
            mat.print();
            Mat33 invMat{};
            mat.inverseTo(invMat);
            printf("invMat:\n");
            invMat.print();

            auto&& v0 = mat.mapPoint({0.0f, 0.0f});
            printf("RenderCmdWorld::setMouseParams(), click, v0(%f, %f)\n", v0.x, v0.y);
            auto&& v1 = invMat.mapPoint({mousePos.x, mousePos.y});
            auto hit = node.contains(v1.x, v1.y);
            printf("RenderCmdWorld::setMouseParams(), click, node.contains() hit: %d\n", hit);
            v1.x *= node.scaleX;
            v1.y *= node.scaleY;
            printf("RenderCmdWorld::setMouseParams(), click, v1(%f, %f)\n", v1.x, v1.y);
        }

        default:
            break;
    }
    if (viewDirty)
    {
        dirty          = true;
        auto& viewDesc = view.desc;

        auto pos      = viewDesc.position;

        // preserve precision
        pos.x = std::roundf(pos.x * 1000) / 1000;
        pos.y = std::roundf(pos.y * 1000) / 1000;
        
        // preserve precision
        auto zoom = std::roundf(viewDesc.zoom * 1000) / 1000;
        printf("RenderCmdWorld::run() zoom: %f\n", zoom);

        viewMat.setTo(std::roundf(pos.x), std::roundf(pos.y), zoom, zoom);
        // viewMat.setTo(pos.x, pos.y, zoom, zoom);
    }
    // if (type == 4)
    // {
    //     dirty = true;
    //     canvas.view.updateViewZoom(mousePos, flag / -100.0f, 1.2f);
    //     auto& viewDesc = canvas.view.desc;
    //     auto& pos      = viewDesc.position;
    //     viewMat.setTo(pos.x, pos.y, viewDesc.zoom, viewDesc.zoom);
    // }
}
void RenderCmdWorld::run()
{
    if (!dirty)
        return;
    dirty = false;


    initialize();

    auto cmdsTotal = static_cast<uint32_t>(cmdNodes.size());

    // printf("RenderCmdWorld::run() sizeof(projMat): %zu\n", sizeof(projMat));
    // printf("RenderCmdWorld::run() cmdsTotal: %zu\n", cmdsTotal);

    uint32_t default_cmd = 0x32;
    uint32_t end_cmd     = 0x0;
    /// 8bytes head
    /// 4bytes version
    /// 4bytes cmds total
    auto bufBytesLength = buffer.size() - 8;
    auto bufBytesIndex       = 0;
    auto descBytesSize       = sizeof(mHeadData);
    auto bufPtr         = (uint8_t*)buffer.data();
    std::memcpy(bufPtr + bufBytesIndex, mHeadData, descBytesSize);
    bufBytesIndex += descBytesSize;
    uint32_t version = 3;

    // printf("version: %d\n", version);

    descBytesSize = sizeof(version);
    std::memcpy(bufPtr + bufBytesIndex, &version, descBytesSize);
    bufBytesIndex += descBytesSize;
        
    uint32_t trunkCmd = 20;
    descBytesSize = sizeof(trunkCmd);
    std::memcpy(bufPtr + bufBytesIndex, &trunkCmd, descBytesSize);
    bufBytesIndex += descBytesSize;

    // printf("projMat:\n");
    // projMat.print();

    CameraCmdDesc camDesc{};
    camDesc.projMat = projMat;
    camDesc.viewMat = viewMat;
    camDesc.updateToBuffer(bufPtr + bufBytesIndex, bufBytesIndex, bufBytesLength);
    bufBytesIndex += camDesc.descSize * 4;
    // batch rounit rendering cmd
    trunkCmd = 22;
    descBytesSize = sizeof(trunkCmd);
    std::memcpy(bufPtr + bufBytesIndex, &trunkCmd, descBytesSize);
    bufBytesIndex += descBytesSize;

    BatchElementCmdDesc batchDesc{};
    batchDesc.descSize = cmdsTotal;
    batchDesc.updateToBuffer(bufPtr + bufBytesIndex, bufBytesIndex, bufBytesLength);
    bufBytesIndex += 2 * 4;

    // RectTarget::Rect boundary = {0, 0, canvas.size.width * 1.0f, canvas.size.height * 1.0f};

    // for (auto i = 0; i < cmdsTotal; i++)
    // {
    //     auto& node = cmdNodes[i];
    //     node.moveingNode.update(2, boundary);
    // }
    // for (size_t i = 0; i < cmdsTotal; ++i)
    // {
    //     auto& node0 = cmdNodes[i];
    //     for (size_t j = i + 1; j < cmdsTotal; ++j)
    //     {
    //         auto& node1 = cmdNodes[j];
    //         RectTarget::handleCollision(node0.moveingNode, node1.moveingNode);
    //     }
    // }
    // for (auto i = 0; i < cmdsTotal; i++)
    // {
    //     auto& node = cmdNodes[i];
    //     auto& r = node.moveingNode.rect;
    //     node.dirty = true;
    //     node.x = r.pos.x;
    //     node.y = r.pos.y;
    //     //printf("node (x=%f, y=%f)\n", node.x, node.y);
    // }

    RectDrawCmdDesc rectDesc{};
    for (auto i = 0; i < cmdsTotal; i++)
    {

        auto& node = cmdNodes[i];

        rectDesc.rcmd  = node.rcmd;
        rectDesc.color = node.color;
        node.update();
        auto& bounds = rectDesc.bounds;
        bounds.pos.x = node.x;
        bounds.pos.y = node.y;
        bounds.width = node.scaleX;
        bounds.height = node.scaleY;

        rectDesc.transform = node.transform;

        // rectDesc.transform = projMat;
        // // view mat and proj mat maybe become to a camera function 
        // rectDesc.transform.append(viewMat);
        // rectDesc.transform.append(node.transform);
        
        // node.updateToMat33(rectDesc.transform);
        // rectDesc.transform.print();
        // printf(">    >     >\n");
        // rectDesc.transform.prepend(viewMat);
        // rectDesc.transform.prepend(projMat);
        // rectDesc.transform.print();
        // printf(">    >     >\n");

        rectDesc.updateToBuffer(bufPtr + bufBytesIndex, bufBytesIndex, bufBytesLength);

        bufBytesIndex += rectDesc.descSize * 4;
    }

    auto bytesTotal = sizeof(mTailData);
    std::memcpy(bufPtr + bufBytesIndex, mTailData, bytesTotal);
    // printf("RenderCmdWorld::run() B cmdsTotal: %zu\n", cmdsTotal);
}


const uint8_t* RenderCmdWorld::cmdBuffer() const
{
    return buffer.data();
}

} // namespace Voxol::Motion