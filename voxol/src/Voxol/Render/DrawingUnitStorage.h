#ifndef VOXOL_RENDER_DRAWING_STORAGE_H
#define VOXOL_RENDER_DRAWING_STORAGE_H

#include "../Base/BaseDefine.h"
#include "../Math/Mat33.h"
#include "../Math/VxRect.h"
#include "../Render/OglGpuResUtils.h"
#include "OglImage.h"
#include "OglText.h"

#include <iostream>
#include <cmath>
#include <vector>

namespace Voxol::Render
{
/*
auto& rectUnit            = drawingUnits[0];
auto& sdfCircleUnit       = drawingUnits[1];
auto& sdfMultiCirclesUnit = drawingUnits[2];
auto& sdfRingUnit         = drawingUnits[3];
auto& sdfSectorUnit       = drawingUnits[4];
auto& sdfRoundedRectUnit  = drawingUnits[5];
auto& sdfTriangleUnit     = drawingUnits[6];
auto& strokeShapeUnit     = drawingUnits[7];    
*/
enum class DrawingUnitType : short
{
    Rect = 0,
    Circle = 1,
    MultiCircle = 2,
    Ring = 3,
    Sector      = 4,
    RoundedRect = 5,
    Triangle    = 6,
    strokeShape = 7
};
class DrawingUnitStorage
{
public:
    using SP = std::shared_ptr<DrawingUnitStorage>;
    using WP = std::weak_ptr<DrawingUnitStorage>;
    using UP = std::unique_ptr<DrawingUnitStorage>;
public:
    static DrawingUnitStorage::SP make();

public:
    DrawingUnitStorage() = default;
    ~DrawingUnitStorage() = default;

public:

    void initalize(int total);

    int32_t getIdWithType(DrawingUnitType type) const;

    bool                    hasType(DrawingUnitType type) const;

    Gpu::DrawingUnit&       getWithType(DrawingUnitType type);
    const Gpu::DrawingUnit& getWithType(DrawingUnitType type) const;

    bool hasAt(int32_t id) const;
    Gpu::DrawingUnit& getAt(int32_t id);
    Gpu::DrawingUnit& operator[](int32_t id);
    const Gpu::DrawingUnit& getAt(int32_t id) const;
    const Gpu::DrawingUnit& operator[](int32_t id) const;

private:
    void initVoassScene();

    bool voassModeFlag = true;

    Gpu::DrawingUnit              baseDrawUnit{};
    Gpu::DrawingUnit              texDrawUnit{};
    Gpu::DrawingUnit              redFormatexDrawUnit{};
    Gpu::DrawingUnit              glyphDrawUnit{};
    Gpu::DrawingUnit              pngUnit{};
    Render::MSDFText                      msdfText{};
    std::vector<Gpu::DrawingUnit>   msdfTextDrawUnits{};

    std::vector<Gpu::DrawingUnit> drawingUnits;

};


} // namespace Voxol::Test
#endif // VOXOL_OGL_RENDERER_H
