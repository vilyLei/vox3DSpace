#include "RenderCmdWorld.h"
#include <cstdio>
#include <cmath>
namespace Voxol::Motion
{

bool Point2DDesc::isEqual(const Point2DDesc& pos) const
{
    return std::abs(pos.x - x) < 1e-5f && std::abs(pos.y - y) < 1e-5f;
}
bool SizeDesc::isEqual(const SizeDesc& desc) const
{
    return std::abs(desc.width - width) < 1e-5f && std::abs(desc.height - height) < 1e-5f;
}


bool ViewComponent::viewZoomWithFixPos(const Point2DDesc& fixPos, float dstScale)
{

    printf("ViewComponent::viewZoomWithFixPos(), fixPos(%f, %f)\n", fixPos.x, fixPos.y);
    // 基本约定: 在scale为1.0的时候，窗口坐标和世界坐标一致(假定没有相对平移)
    auto scale = desc.zoom;
    auto pos   = desc.position;

    // 可能有某种标准化计算操作
    desc.zoom = dstScale;
    dstScale  = desc.zoom;

    auto px  = -pos.x / scale;
    auto py  = -pos.y / scale;
    auto wx0 = (fixPos.x / scale) + px;
    auto wy0 = (fixPos.y / scale) + py;
    // 将当前的鼠标坐标转换到world坐标
    // 已知 position( wx0,  wy0 ) 数据
    // 计算出在窗口坐标空间下的对应的坐标, 假定world space和window space坐标原点对齐
    auto vx0      = wx0 * dstScale;
    auto vy0      = wy0 * dstScale;
    auto disX     = vx0 - fixPos.x;
    auto disY     = vy0 - fixPos.y;
    desc.position = {-disX, -disY};
    viewScale     = static_cast<double>(dstScale);

    printf("ViewComponent::viewZoomWithFixPos(), desc.zoom: %f\n", desc.zoom);
    printf("ViewComponent::viewZoomWithFixPos(), desc.position(x=%f, y=%f)\n", desc.position.x, desc.position.y);
    return true;
}

bool ViewComponent::updateViewZoom(const Point2DDesc& fixPos, float delta, float speed)
{
    viewScale *= delta > 0.1f ? speed : 1 / speed;
    return viewZoomWithFixPos(fixPos, static_cast<float>(viewScale));
}

void ViewComponent::moveBegin(const Point2DDesc& pos)
{
    fixOriginPos = pos;
    fixPos       = pos;
    fixViewPos   = desc.position;
    moving       = true;
}
bool ViewComponent::move(const Point2DDesc& pos)
{
    if (!moving)
        return false;

    auto disX = fixPos.x - pos.x;
    auto disY = fixPos.y - pos.y;
    auto dis  = std::sqrt(disX * disX + disY * disY);
    if (dis < 1e-5f)
        return false;

    fixPos = pos;

    disX    = pos.x - fixOriginPos.x;
    disY    = pos.y - fixOriginPos.y;
    auto px = fixViewPos.x + disX;
    auto py = fixViewPos.y + disY;

    desc.position = {std::roundf(px), std::roundf(py)};

    return true;
}
void ViewComponent::moveEnd(const Point2DDesc& pos)
{
    moving = false;
}


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

    auto index = 0;
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

void RenderCmdWorld::setMouseParams(float x, float y, int type, int flag)
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
            viewDirty = view.updateViewZoom(mousePos, flag / -100.0f, 1.2f);
        }
        break;

        default:
            break;
    }
    if (viewDirty)
    {
        dirty          = true;
        auto& viewDesc = view.desc;
        auto& pos      = viewDesc.position;
        viewMat.setTo(pos.x, pos.y, viewDesc.zoom, viewDesc.zoom);
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

    uint32_t default_cmd = 0x33;
    uint32_t end_cmd     = 0x0;
    /// 8bytes head
    /// 4bytes version
    /// 4bytes cmds total
    auto bufBytesLength = buffer.size() - 8;
    auto bufIndex       = 0;
    auto descSize       = sizeof(mHeadData);
    auto bufPtr         = (uint8_t*)buffer.data();
    std::memcpy(bufPtr + bufIndex, mHeadData, descSize);
    bufIndex += descSize;
    uint32_t version = 1;

    // printf("version: %d\n", version);

    descSize = sizeof(version);
    std::memcpy(bufPtr + bufIndex, &version, descSize);
    bufIndex += descSize;
    descSize = sizeof(cmdsTotal);
    std::memcpy(bufPtr + bufIndex, &cmdsTotal, descSize);
    bufIndex += descSize;


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
    //     node.x = r.pos.x;
    //     node.y = r.pos.y;
    //     //printf("node (x=%f, y=%f)\n", node.x, node.y);
    // }

    RectDrawCmdDesc rectDesc{};
    for (auto i = 0; i < cmdsTotal; i++)
    {

        auto descSize = sizeof(RectDrawCmdDesc);
        if ((bufIndex + descSize) > bufBytesLength)
            break;
        auto& node = cmdNodes[i];

        rectDesc.rcmd  = node.rcmd;
        rectDesc.color = node.color;
        node.updateToMat33(rectDesc.transform);

        // rectDesc.transform.print();
        // printf(">    >     >\n");
        rectDesc.transform.prepend(viewMat);
        rectDesc.transform.prepend(projMat);
        // rectDesc.transform.print();
        // printf(">    >     >\n");

        rectDesc.updateToBuffer(bufPtr + bufIndex);

        bufIndex += descSize;
    }

    auto bytesTotal = sizeof(mTailData);
    std::memcpy(bufPtr + bufIndex, mTailData, bytesTotal);
    // printf("RenderCmdWorld::run() B cmdsTotal: %zu\n", cmdsTotal);
}


const uint8_t* RenderCmdWorld::cmdBuffer() const
{
    return buffer.data();
}

} // namespace Voxol::Motion