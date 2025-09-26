#include "RenderCmdComp.h"
#include <cstdio>
#include <cmath>
namespace Voxol::Motion
{

bool SizeDesc::isEqual(const SizeDesc& desc) const
{
    return std::abs(desc.width - width) < 1e-5f && std::abs(desc.height - height) < 1e-5f;
}


bool ViewComponent::viewZoomWithFixPos(const Point2DDesc& fixPos, float dstScale)
{
    if (dstScale < 0.0001f)
        return false;
    if (dstScale > 10000.0f)
        return false;

    // printf("ViewComponent::viewZoomWithFixPos(), fixPos(%f, %f)\n", fixPos.x, fixPos.y);
    // 基本约定: 在scale为1.0的时候，窗口坐标和世界坐标一致(假定没有相对平移)
    auto scale = desc.zoom;
    auto pos   = desc.position;

    // 可能有某种标准化计算操作
    desc.zoom = dstScale;
    dstScale  = desc.zoom;

    // preserve precision
    auto s = std::roundf(scale * 1000) / 1000;

    auto px  = -pos.x / s;
    auto py  = -pos.y / s;
    auto wx0 = (fixPos.x / s) + px;
    auto wy0 = (fixPos.y / s) + py;

    // preserve precision
    s = std::roundf(dstScale * 1000) / 1000;

    // 将当前的鼠标坐标转换到 world 坐标
    // 已知 position( wx0,  wy0 ) 数据
    // 计算出在窗口坐标空间下的对应的坐标, 假定world space和window space坐标原点对齐
    auto vx0      = wx0 * s;
    auto vy0      = wy0 * s;
    auto disX     = vx0 - fixPos.x;
    auto disY     = vy0 - fixPos.y;
    desc.position = {-disX, -disY};
    viewScale     = static_cast<double>(dstScale);

    // printf("ViewComponent::viewZoomWithFixPos(), desc.zoom: %f\n", desc.zoom);
    // printf("ViewComponent::viewZoomWithFixPos(), desc.position(x=%f, y=%f)\n", desc.position.x, desc.position.y);
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

void ViewComponent::update()
{
    auto pos = desc.position;

    // preserve precision
    pos.x = std::roundf(pos.x * 1000) / 1000;
    pos.y = std::roundf(pos.y * 1000) / 1000;

    // preserve precision
    auto zoom = std::roundf(desc.zoom * 1000) / 1000;
    printf("ViewComponent::update() zoom: %f\n", zoom);

    viewMat.setTo(std::roundf(pos.x), std::roundf(pos.y), zoom, zoom);
}

} // namespace Voxol::Motion