#ifndef VOXOL_RENDER_CMD_COMP_H
#define VOXOL_RENDER_CMD_COMP_H

#include <vector>
#include "../Base/BaseDefine.h"
#include "../Math/Mat33.h"

namespace Voxol::Motion
{
//struct Point2DDesc
//{
//    float x{0};
//    float y{0};
//
//    bool isEqual(const Point2DDesc& pos) const;
//};
 
using Point2DDesc = Voxol::Math::Vec2;

struct SizeDesc
{
    float width{0};
    float height{0};
    bool  isEqual(const SizeDesc& desc) const;
};
struct ViewDesc
{
    Point2DDesc position{};
    float       zoom{1};
};

struct ViewComponent
{
    ViewDesc           desc{};
    Point2DDesc        mousePos{};
    float              viewScale{1};
    Voxol::Math::Mat33 viewMat{};
    Voxol::Math::Mat33 projMat{};

    bool viewZoomWithFixPos(const Point2DDesc& zoomPos, float dstScale);
    bool updateViewZoom(const Point2DDesc& fixPos, float delta, float speed = 1.2f);

    void moveBegin(const Point2DDesc& pos);
    bool move(const Point2DDesc& pos);
    void moveEnd(const Point2DDesc& pos);
    void update();

private:
    Point2DDesc fixOriginPos{};
    Point2DDesc fixPos{};
    Point2DDesc fixViewPos{};
    bool        moving{false};
};

struct CanvasDesc
{
    SizeDesc      size{};
    ViewComponent view{};
    int           index{0};
};

} // namespace Voxol::Motion
#endif