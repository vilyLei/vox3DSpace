#ifndef VOXOL_RENDER_CMD_WORLD_H
#define VOXOL_RENDER_CMD_WORLD_H


// #include <vector>
// #include "../Base/BaseDefine.h"
#include "RenderCmdComp.h"
#include "../Math/Mat33.h"
#include "RenderCmdNode.h"

namespace Voxol::Motion
{
/*
struct Point2DDesc
{
    float x{0};
    float y{0};

    bool isEqual(const Point2DDesc& pos) const;
};
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
    ViewDesc desc{};
    Point2DDesc mousePos{};
    float    viewScale{1};

    bool     viewZoomWithFixPos(const Point2DDesc& zoomPos, float dstScale);
    bool     updateViewZoom(const Point2DDesc& fixPos, float delta, float speed = 1.2f);

    void moveBegin(const Point2DDesc& pos);
    bool move(const Point2DDesc& pos);
    void moveEnd(const Point2DDesc& pos);
private:
    Point2DDesc fixOriginPos{};
    Point2DDesc fixPos{};
    Point2DDesc fixViewPos{};
    bool moving{false};
};

struct CanvasDesc
{
    SizeDesc      size{};
    ViewComponent view{};
    int           index{0};
};
//*/
class RenderCmdWorld
{
public:
    std::vector<uint32_t>        commands{};
    std::vector<DrawCmdTestNode> cmdNodes{};
    Voxol::Math::Mat33           viewMat{};
    Voxol::Math::Mat33           projMat{};

    Point2DDesc mousePos{};
    CanvasDesc  canvas{};
    ViewDesc    view{};

    std::vector<uint8_t> buffer{};

    bool dirty = true;

public:
    RenderCmdWorld();
    ~RenderCmdWorld();

public:
    void           initialize();
    void           run();
    void           setGPUCtxSize(int w, int h);
    void           setMouseXY(float x, float y);
    void           setMouseParams(float x, float y, int type, float value);
    const uint8_t* cmdBuffer() const;

private:
    bool     mInit = true;
    uint32_t mHeadData[2]{0xffffffff, 0xffffffff};
    uint32_t mTailData[2]{0x0, 0x0};
};

} // namespace Voxol::Motion
#endif