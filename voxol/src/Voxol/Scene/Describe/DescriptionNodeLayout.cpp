
#include "DescriptionNodeLayout.h"
#include "../Scene/Layout/PositionDistributor.h"

namespace Voxol::Scene::Describe
{

    
std::vector<float> DescriptionNodeLauout::angles{};
std::vector<Math::Vec2> DescriptionNodeLauout::positions{};

void DescriptionNodeLauout::referenceLayoutSceneNodeWithHexagonalGrid(const JsonType& jsonNode, const Math::Vec2& nodeSize, const DescNodeLayoutCallbackType& callback)
{
    using namespace Voxol::Scene::Layout;

    auto       count = 10;
    Math::Vec2 pos{300, 300};
    int        rings     = 5;
    float      hexRadius = 180;

    RefLayoutValueRef jsonV{jsonNode};
    pos = jsonV.position();
    hexRadius = jsonV.hexRadius();
    rings     = jsonV.rings();
    count     = jsonV.count();

    auto&& pvs = PositionDistribution::hexagonalGrid(count, pos, rings, hexRadius);
    auto   tot = static_cast<int>(pvs.size());
    tot              = tot < count ? tot : count;
    positions.reserve(tot);
    positions.resize(tot);
    for (auto i = 0; i < tot; i++)
    {
        //callback(i, pvs[i], {1, 1}, 0);
        positions[i] = pvs[i];
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

    RefLayoutValueRef jsonV{jsonNode};
    center       = jsonV.position();
    start_radius = jsonV.startRadius();
    step_radius  = jsonV.stepRadius();
    step_angle   = jsonV.stepAngle();
    start_angle  = jsonV.startAngle();
    count        = jsonV.count();

    angles.reserve(count);
    angles.clear();

    
    //positions.reserve(tot);
    positions = PositionDistribution::spiral(angles, count, center, start_radius, step_radius, start_angle, step_angle);
    //for (auto i = 0; i < positions.size(); i++)
    //{
    //    callback(i, positions[i], {1, 1}, angles[i]);
    //}
}
void DescriptionNodeLauout::referenceLayoutSceneNodeWithArc(const JsonType& jsonNode, const Math::Vec2& nodeSize, const DescNodeLayoutCallbackType& callback)
{
    using namespace Voxol::Scene::Layout;


    auto       count = 10;
    Math::Vec2 center{300, 300};
    float      radius     = 100.0f;
    float      startAngle = 0;
    float      arcAngle = 180;

    RefLayoutValueRef jsonV{jsonNode};
    center                       = jsonV.position();
    radius                       = jsonV.radius();
    startAngle                   = jsonV.startAngle();
    arcAngle                     = jsonV.arcAngle();
    count                        = jsonV.count();

    angles.reserve(count);
    angles.clear();
    positions = PositionDistribution::arc(angles, count, center, radius, startAngle, arcAngle);
    //for (auto i = 0; i < positions.size(); i++)
    //{
    //    callback(i, positions[i], {1, 1}, angles[i]);
    //}
}
void DescriptionNodeLauout::referenceLayoutSceneNodeWithCircle(const JsonType& jsonNode, const Math::Vec2& nodeSize, const DescNodeLayoutCallbackType& callback)
{
    using namespace Voxol::Scene::Layout;


    auto       count = 10;
    Math::Vec2 center{300, 300};
    float      radius   = 100.0f;
    float      startAngle = 0;

    RefLayoutValueRef jsonV{jsonNode};
    center = jsonV.position();
    radius = jsonV.radius();
    startAngle = jsonV.startAngle();
    count      = jsonV.count();

    angles.reserve(count);
    angles.clear();
    positions = PositionDistribution::circle(angles, count, center, radius, startAngle);
    //for (auto i = 0; i < positions.size(); i++)
    //{
    //    callback(i, positions[i], {1, 1}, angles[i]);
    //}
}
void DescriptionNodeLauout::referenceLayoutSceneNodeWithGrid(const JsonType& jsonNode, const Math::Vec2& nodeSize, const DescNodeLayoutCallbackType& callback)
{
    using namespace Voxol::Scene::Layout;

    int        count   = 10;
    int        columns = 3;
    Math::Vec2 pos{30, 30};
    Math::Vec2 spacing{25, 25};
    Math::Vec2 staggered;

    RefLayoutValueRef jsonV{jsonNode};
    pos = jsonV.position();
    columns = jsonV.columns();
    count     = jsonV.count();
    spacing = jsonV.spacing();
    staggered = jsonV.staggered();

    positions = PositionDistribution::grid(count, pos, spacing, columns, nodeSize, staggered);
    //for (auto i = 0; i < positions.size(); i++)
    //{
    //    callback(i, positions[i], {1, 1}, 0);
    //}
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
        // test tombinators

        auto tot = positions.size();
        for (auto i = 0; i < positions.size(); i++)
        {
            callback(i, positions[i], {1, 1}, angles.size() >= tot ? angles[i] : 0);
        }
    }
}
void DescriptionNodeLauout::referenceLayoutSceneNode(const JsonType& jsonNode, const Math::Vec2& nodeSize, const DescNodeLayoutCallbackType& callback)
{
    std::string refLayoutKey = "reference-layout";

    if (jsonNode.contains(refLayoutKey) && jsonNode[refLayoutKey].is_object())
    {
        auto&& refLayoutNode = jsonNode[refLayoutKey];
        referenceLayoutSceneNodeOnce(refLayoutNode, nodeSize, callback);
        auto tot = positions.size();
        for (auto i = 0; i < positions.size(); i++)
        {
            callback(i, positions[i], {1, 1}, angles.size() >= tot ? angles[i] : 0);
        }
    }

    referenceLayoutSceneNodeMany(jsonNode, nodeSize, callback);
}

} // namespace Voxol::Scene::Describe