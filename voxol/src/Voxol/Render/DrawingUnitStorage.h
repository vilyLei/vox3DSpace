#ifndef VOXOL_RENDER_DRAWING_UNIT_STORAGE_H
#define VOXOL_RENDER_DRAWING_UNIT_STORAGE_H

#include "../Base/BaseDefine.h"
#include "../Math/Mat33.h"
#include "../Math/VxRect.h"
#include "../Render/OglGpuResUtils.h"
#include "OglImage.h"
#include "OglText.h"

#include <iostream>
#include <unordered_map>
#include <cmath>
#include <vector>
#include <string>

namespace Voxol::Render
{

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

    int32_t getGlyphIdWithChar(int32_t glyphChar);

    int32_t getIdWithType(DrawingUnitType type) const;
    int32_t           getIdWithName(const std::string& name);
    Gpu::DrawingUnit& getWithName(const std::string& name);

    bool                    hasType(DrawingUnitType type) const;

    Gpu::DrawingUnit&       getWithType(DrawingUnitType type);
    const Gpu::DrawingUnit& getWithType(DrawingUnitType type) const;

    bool hasAt(int32_t id) const;
    Gpu::DrawingUnit& getAt(int32_t id);
    Gpu::DrawingUnit& operator[](int32_t id);
    const Gpu::DrawingUnit& getAt(int32_t id) const;
    const Gpu::DrawingUnit& operator[](int32_t id) const;

    Render::MSDFText::SP        msdfText{};

private:
    void initVoassScene();

    bool voassModeFlag = true;

    Gpu::DrawingUnit              baseDrawUnit{};
    Gpu::DrawingUnit              texDrawUnit{};
    Gpu::DrawingUnit              redFormatexDrawUnit{};
    Gpu::DrawingUnit              glyphDrawUnit{};
    Gpu::DrawingUnit              pngUnit{};
    std::vector<Gpu::DrawingUnit>   msdfTextDrawUnits{};

    std::unordered_map<int32_t, int32_t> glyphDrawingUnitMap{};

    std::vector<Gpu::DrawingUnit> drawingUnits;
    std::unordered_map<std::string, int32_t> nameMap{};
    int32_t                                  drawingIDIndex = 0;

};


} // namespace Voxol::Test
#endif // VOXOL_OGL_RENDERER_H
