
#include "DescriptionNodeLayout.h"
#include "../Scene/Layout/PositionDistributor.h"

namespace Voxol::Scene::Describe
{

void DescriptionNodeLauout::referenceLayoutSceneNodeWithHexagonalGrid(const JsonType& jsonNode, const Math::Vec2& nodeSize, const DescNodeLayoutCallbackType& callback)
{
    using namespace Voxol::Scene::Layout;

    auto       count = 10;
    Math::Vec2 pos{300, 300};
    int        rings     = 5;
    float      hexRadius = 180;

    Data::JsonValue positionV;
    positionV.parseWithName(jsonNode, "position");
    if (positionV.is<Math::Vec2>())
    {
        pos = positionV.get<Math::Vec2>();
    }
    Data::JsonValue hexRadiusV;
    hexRadiusV.parseWithName(jsonNode, "hex-radius");
    if (hexRadiusV.is<int>())
    {
        hexRadius = hexRadiusV.get<int>();
    }
    else if (positionV.is<float>())
    {
        hexRadius = hexRadiusV.get<float>();
    }

    Data::JsonValue columnsV;
    columnsV.parseWithName(jsonNode, "rings");
    if (columnsV.is<int>())
    {
        rings = columnsV.get<int>();
    }

    Data::JsonValue countV;
    countV.parseWithName(jsonNode, "count");
    if (countV.is<int>())
    {
        count = countV.get<int>();
    }

    auto&& positions = PositionDistribution::hexagonalGrid(count, pos, rings, hexRadius);
    auto   tot       = static_cast<int>(positions.size());
    tot              = tot < count ? tot : count;
    for (auto i = 0; i < tot; i++)
    {
        callback(i, positions[i], {1, 1}, 0);
    }
}
void DescriptionNodeLauout::referenceLayoutSceneNodeWithSpiral(const JsonType& jsonNode, const Math::Vec2& nodeSize, const DescNodeLayoutCallbackType& callback)
{
    using namespace Voxol::Scene::Layout;


    auto       count = 10;
    Math::Vec2 center{300, 300};
    float      start_radius     = 0;
    float      step_radius     = 30;
    float      start_angle = 0;
    float      step_angle = 30;

    Data::JsonValue jsonV;
    center       = jsonV.parseVec2WithName(jsonNode, "position");
    start_radius = jsonV.parseFloatWithName(jsonNode, "start-radius");
    step_radius  = jsonV.parseFloatWithName(jsonNode, "step-radius");
    step_angle   = jsonV.parseFloatWithName(jsonNode, "step-angle");
    start_angle  = jsonV.parseFloatWithName(jsonNode, "start-angle");
    count        = jsonV.parseIntWithName(jsonNode, "count");

    step_angle = Math::degrees_to_radians(step_angle);
    start_angle = Math::degrees_to_radians(start_angle);

    std::vector<float> angles;
    angles.reserve(count);
    auto&& positions = PositionDistribution::spiral(angles, count, center, start_radius, step_radius, start_angle, step_angle);
    for (auto i = 0; i < positions.size(); i++)
    {
        callback(i, positions[i], {1, 1}, angles[i]);
    }
}
void DescriptionNodeLauout::referenceLayoutSceneNodeWithArc(const JsonType& jsonNode, const Math::Vec2& nodeSize, const DescNodeLayoutCallbackType& callback)
{
    using namespace Voxol::Scene::Layout;


    auto       count = 10;
    Math::Vec2 center{300, 300};
    float      radius     = 100.0f;
    float      startAngle = 0;
    float      arcAngle = 180;

    Data::JsonValue positionV;
    center = positionV.parseVec2WithName(jsonNode, "position");
    //if (positionV.is<Math::Vec2>())
    //{
    //    center = positionV.get<Math::Vec2>();
    //}
    //Data::JsonValue radiusV;
    radius = positionV.parseFloatWithName(jsonNode, "radius");
    //radiusV.parseWithName(jsonNode, "radius");
    //if (radiusV.is<int>())
    //{
    //    radius = radiusV.get<int>();
    //}
    //else if (positionV.is<float>())
    //{
    //    radius = radiusV.get<float>();
    //}
    startAngle = positionV.parseFloatWithName(jsonNode, "start-angle");
    //Data::JsonValue startAngleV;
    //startAngleV.parseWithName(jsonNode, "start-angle");
    //if (startAngleV.is<int>())
    //{
    //    startAngle = startAngleV.get<int>();
    //}
    //else if (startAngleV.is<float>())
    //{
    //    startAngle = startAngleV.get<float>();
    //}
    arcAngle = positionV.parseFloatWithName(jsonNode, "arc-angle");
    //Data::JsonValue arcAngleV;
    //arcAngleV.parseWithName(jsonNode, "arc-angle");
    //if (arcAngleV.is<int>())
    //{
    //    arcAngle = arcAngleV.get<int>();
    //}
    //else if (arcAngleV.is<float>())
    //{
    //    arcAngle = arcAngleV.get<float>();
    //}

    count = positionV.parseIntWithName(jsonNode, "count");
    //Data::JsonValue countV;
    //countV.parseWithName(jsonNode, "count");
    //if (countV.is<int>())
    //{
    //    count = countV.get<int>();
    //}
    auto   startRad  = Math::degrees_to_radians(startAngle);
    auto   arctRad   = Math::degrees_to_radians(arcAngle);
    std::vector<float> angles;
    angles.reserve(count);
    auto&&             positions = PositionDistribution::arc(angles, count, center, radius, startRad, arctRad);
    for (auto i = 0; i < positions.size(); i++)
    {
        callback(i, positions[i], {1, 1}, angles[i]);
    }
}
void DescriptionNodeLauout::referenceLayoutSceneNodeWithCircle(const JsonType& jsonNode, const Math::Vec2& nodeSize, const DescNodeLayoutCallbackType& callback)
{
    using namespace Voxol::Scene::Layout;


    auto       count = 10;
    Math::Vec2 center{300, 300};
    float      radius   = 100.0f;
    float      startAngle = 0;

    Data::JsonValue positionV;
    positionV.parseWithName(jsonNode, "position");
    if (positionV.is<Math::Vec2>())
    {
        center = positionV.get<Math::Vec2>();
    }
    Data::JsonValue radiusV;
    radiusV.parseWithName(jsonNode, "radius");
    if (radiusV.is<int>())
    {
        radius = radiusV.get<int>();
    }
    else if (positionV.is<float>())
    {
        radius = radiusV.get<float>();
    }

    Data::JsonValue startAngleV;
    startAngleV.parseWithName(jsonNode, "start-angle");
    if (startAngleV.is<int>())
    {
        startAngle = startAngleV.get<int>();
    }
    else if (startAngleV.is<float>())
    {
        startAngle = startAngleV.get<float>();
    }

    Data::JsonValue countV;
    countV.parseWithName(jsonNode, "count");
    if (countV.is<int>())
    {
        count = countV.get<int>();
    }
    auto               startRad = Math::degrees_to_radians(startAngle);
    std::vector<float> angles;
    angles.reserve(count);
    auto&& positions = PositionDistribution::circle(angles, count, center, radius, startRad);
    for (auto i = 0; i < positions.size(); i++)
    {
        callback(i, positions[i], {1, 1}, angles[i]);
    }
}
void DescriptionNodeLauout::referenceLayoutSceneNodeWithGrid(const JsonType& jsonNode, const Math::Vec2& nodeSize, const DescNodeLayoutCallbackType& callback)
{
    using namespace Voxol::Scene::Layout;

    int        count   = 10;
    int        columns = 3;
    Math::Vec2 beginPos{30, 30};
    Math::Vec2 spacing{25, 25};
    Math::Vec2 staggered;

    Data::JsonValue positionV;
    positionV.parseWithName(jsonNode, "position");
    if (positionV.is<Math::Vec2>())
    {
        beginPos = positionV.get<Math::Vec2>();
    }
    Data::JsonValue columnsV;
    columnsV.parseWithName(jsonNode, "columns");
    if (columnsV.is<int>())
    {
        columns = columnsV.get<int>();
    }
    Data::JsonValue countV;
    countV.parseWithName(jsonNode, "count");
    if (countV.is<int>())
    {
        count = countV.get<int>();
    }
    Data::JsonValue spacingV;
    spacingV.parseWithName(jsonNode, "spacing");
    if (spacingV.is<Math::Vec2>())
    {
        spacing = spacingV.get<Math::Vec2>();
    }

    Data::JsonValue staggeredV;
    staggeredV.parseWithName(jsonNode, "staggered");
    if (staggeredV.is<Math::Vec2>())
    {
        staggered = staggeredV.get<Math::Vec2>();
    }

    auto&& positions = PositionDistribution::grid(count, beginPos, spacing, columns, nodeSize, staggered);
    for (auto i = 0; i < positions.size(); i++)
    {
        callback(i, positions[i], {1, 1}, 0);
    }
}

void DescriptionNodeLauout::referenceLayoutSceneNodeOnce(const JsonType& refLayoutNode, const Math::Vec2& nodeSize, const DescNodeLayoutCallbackType& callback)
{
    DescreferenceLayoutNode layoutNode;
    layoutNode.parse(refLayoutNode);

    if (layoutNode.method == "grid")
    {
        referenceLayoutSceneNodeWithGrid(refLayoutNode, nodeSize, callback);
    }
    else if (layoutNode.method == "circle")
    {
        referenceLayoutSceneNodeWithCircle(refLayoutNode, nodeSize, callback);
    }
    else if (layoutNode.method == "arc")
    {
        referenceLayoutSceneNodeWithArc(refLayoutNode, nodeSize, callback);
    }
    else if (layoutNode.method == "hexagonal-grid")
    {
        referenceLayoutSceneNodeWithHexagonalGrid(refLayoutNode, nodeSize, callback);
    }
    else if (layoutNode.method == "spiral")
    {
        referenceLayoutSceneNodeWithSpiral(refLayoutNode, nodeSize, callback);
    }
}
void DescriptionNodeLauout::referenceLayoutSceneNodeMany(const JsonType& jsonNode, const Math::Vec2& nodeSize, const DescNodeLayoutCallbackType& callback) {

    std::string refLayoutKey = "reference-layouts";

    if (!jsonNode.contains(refLayoutKey) || !jsonNode[refLayoutKey].is_array())
        return;

    auto&& elements = jsonNode[refLayoutKey];
    if (elements.empty())
        return;

    for (auto& item : elements)
    {
        if (!item.is_object())
            continue;

        referenceLayoutSceneNodeOnce(item, nodeSize, callback);
    }
}
void DescriptionNodeLauout::referenceLayoutSceneNode(const JsonType& jsonNode, const Math::Vec2& nodeSize, const DescNodeLayoutCallbackType& callback)
{
    std::string refLayoutKey = "reference-layout";

    if (jsonNode.contains(refLayoutKey) && jsonNode[refLayoutKey].is_object())
    {
        auto&& refLayoutNode = jsonNode[refLayoutKey];
        referenceLayoutSceneNodeOnce(refLayoutNode, nodeSize, callback);
    }

    referenceLayoutSceneNodeMany(jsonNode, nodeSize, callback);
}

} // namespace Voxol::Scene::Describe