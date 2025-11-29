
#include "DescriptionNodeLayout.h"
#include "../Scene/Layout/PositionDistributor.h"

namespace Voxol::Scene::Describe
{

void DescriptionNodeLauout::referenceLayoutSceneNodeWithCircle(const JsonType& jsonNode, const Math::Vec2& nodeSize, const DescNodeLayoutCallbackType& callback)
{
    using namespace Voxol::Scene::Layout;


    auto       count = 10;
    Math::Vec2 center{300, 300};
    float      radius   = 100.0f;
    float      rotation = 0;

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


    Data::JsonValue rotationV;
    rotationV.parseWithName(jsonNode, "rotation");
    if (rotationV.is<int>())
    {
        rotation = rotationV.get<int>();
    }
    else if (rotationV.is<float>())
    {
        rotation = rotationV.get<float>();
    }

    Data::JsonValue countV;
    countV.parseWithName(jsonNode, "count");
    if (countV.is<int>())
    {
        count = countV.get<int>();
    }
    auto   startRad  = Math::degrees_to_radians(rotation);
    auto&& positions = PositionDistribution::circle(count, center, radius, startRad);
    for (auto i = 0; i < positions.size(); i++)
    {
        float rotation = startRad + MATH_2PI * float(i) / count;
        callback(i, positions[i], {1, 1}, rotation);
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

    std::string method        = "grid";

    if (refLayoutNode.contains("method") && refLayoutNode["method"].is_string())
    {
        method = refLayoutNode["method"];
    }
    if (method == "grid")
    {
        referenceLayoutSceneNodeWithGrid(refLayoutNode, nodeSize, callback);
    }
    else if (method == "circle")
    {
        referenceLayoutSceneNodeWithCircle(refLayoutNode, nodeSize, callback);
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
    /*
    std::string method        = "grid";
    auto&&      refLayoutNode = jsonNode[refLayoutKey];
    if (refLayoutNode.contains("method") && refLayoutNode["method"].is_string())
    {
        method = refLayoutNode["method"];
    }

    if (method == "grid")
    {
        referenceLayoutSceneNodeWithGrid(refLayoutNode, nodeSize, callback);
    }
    else if (method == "circle")
    {
        referenceLayoutSceneNodeWithCircle(refLayoutNode, nodeSize, callback);
    }
    //*/
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

    /*
    std::string refLayoutKey = "reference-layout";

    if (!jsonNode.contains(refLayoutKey) || !jsonNode[refLayoutKey].is_object())
        return;

    std::string method        = "grid";
    auto&&      refLayoutNode = jsonNode[refLayoutKey];
    if (refLayoutNode.contains("method") && refLayoutNode["method"].is_string())
    {
        method = refLayoutNode["method"];
    }

    if (method == "grid")
    {
        referenceLayoutSceneNodeWithGrid(refLayoutNode, nodeSize, callback);
    }
    else if (method == "circle")
    {
        referenceLayoutSceneNodeWithCircle(refLayoutNode, nodeSize, callback);
    }
    //*/
}

} // namespace Voxol::Scene::Describe