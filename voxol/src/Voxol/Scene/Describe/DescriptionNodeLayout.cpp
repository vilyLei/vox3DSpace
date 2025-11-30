
#include "DescriptionNodeLayout.h"
#include "../../Scene/Layout/PositionDistributor.h"
#include "../../Math/Mat33.h"

namespace Voxol::Scene::Describe
{

    
std::vector<float> DescriptionNodeLauout::angles{};
std::vector<Math::Vec2> DescriptionNodeLauout::positions{};

void DescriptionNodeLauout::referenceLayoutSceneNodeWithBasic(const JsonType& jsonNode, const Math::Vec2& nodeSize)
{
    using namespace Voxol::Scene::Layout;
    positions.clear();

    auto       count = 10;
    Math::Vec2 pos{300, 300};

    float      start_radius = 0;
    float      step_radius  = 30;
    float      start_angle  = 0;
    float      step_angle   = 30;

    RefLayoutValueRef jsonV{jsonNode};
    jsonV.parse();

    auto& layoutNode = jsonV.value.layoutNode;
    if (!layoutNode.isTypeBasic())
    {
        return;
    }
    count       = jsonV.count();
    if (count < 1)
    {
        return;
    }
    pos         = jsonV.position();

    positions.reserve(count);
    angles.reserve(count);
    angles.clear();

    auto method = layoutNode.method;
    if (method == "diamond")
    {
        auto radius = jsonV.radius();
        for (auto i = 0; i < count; i++)
        {
            auto&& pos = Distribution::diamond(i, count, radius);
            positions.emplace_back(pos);
        }
    }
}
void DescriptionNodeLauout::referenceLayoutSceneNodeWithSpiral(const JsonType& jsonNode, const Math::Vec2& nodeSize)
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

    
    positions = PositionDistribution::spiral(angles, count, center, start_radius, step_radius, start_angle, step_angle);
}

void DescriptionNodeLauout::referenceLayoutSceneNodeWithHexagonalGrid(const JsonType& jsonNode, const Math::Vec2& nodeSize)
{
    using namespace Voxol::Scene::Layout;

    auto       count = 10;
    Math::Vec2 pos{300, 300};
    int        rings     = 5;
    float      hexRadius = 180;

    RefLayoutValueRef jsonV{jsonNode};
    pos       = jsonV.position();
    hexRadius = jsonV.hexRadius();
    rings     = jsonV.rings();
    count     = jsonV.count();

    auto&& pvs = PositionDistribution::hexagonalGrid(count, pos, rings, hexRadius);
    auto   tot = static_cast<int>(pvs.size());
    tot        = tot < count ? tot : count;
    positions.reserve(tot);
    positions.resize(tot);
    for (auto i = 0; i < tot; i++)
    {
        positions[i] = pvs[i];
    }
}

void DescriptionNodeLauout::referenceLayoutSceneNodeWithArc(const JsonType& jsonNode, const Math::Vec2& nodeSize)
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
    //auto tot = positions.size();
    //for (auto i = 0; i < tot; i++)
    //{
    //}
}
void DescriptionNodeLauout::referenceLayoutSceneNodeWithCircle(const JsonType& jsonNode, const Math::Vec2& nodeSize)
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
}
void DescriptionNodeLauout::referenceLayoutSceneNodeWithGrid(const JsonType& jsonNode, const Math::Vec2& nodeSize)
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
}

void DescriptionNodeLauout::referenceLayoutSceneNodeOnce(const JsonType& refLayoutNode, const Math::Vec2& nodeSize)
{
    DescreferenceLayoutNode layoutNode;
    layoutNode.parse(refLayoutNode);

    auto& method = layoutNode.method;

    if (layoutNode.isTypeDefault())
    {
        if (method == "grid")
        {
            referenceLayoutSceneNodeWithGrid(refLayoutNode, nodeSize);
        }
        else if (method == "circle")
        {
            referenceLayoutSceneNodeWithCircle(refLayoutNode, nodeSize);
        }
        else if (method == "arc")
        {
            referenceLayoutSceneNodeWithArc(refLayoutNode, nodeSize);
        }
        else if (method == "hexagonal-grid")
        {
            referenceLayoutSceneNodeWithHexagonalGrid(refLayoutNode, nodeSize);
        }
        else if (method == "spiral")
        {
            referenceLayoutSceneNodeWithSpiral(refLayoutNode, nodeSize);
        }
        return;
    }
    if (layoutNode.isTypeBasic())
    {
        referenceLayoutSceneNodeWithBasic(refLayoutNode, nodeSize);
        return;
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

        referenceLayoutSceneNodeOnce(item, nodeSize);
        // test combinators
        //referenceLayoutSceneNodeCombinate();

        referenceLayoutCallback(callback);
    }
}
void DescriptionNodeLauout::referenceLayoutSceneNode(const JsonType& jsonNode, const Math::Vec2& nodeSize, const DescNodeLayoutCallbackType& callback)
{
    std::string refLayoutKey = "reference-layout";

    if (jsonNode.contains(refLayoutKey) && jsonNode[refLayoutKey].is_object())
    {
        auto&& refLayoutNode = jsonNode[refLayoutKey];
        referenceLayoutSceneNodeOnce(refLayoutNode, nodeSize);
        referenceLayoutCallback( callback );
    }
    referenceLayoutSceneNodeMany(jsonNode, nodeSize, callback);
}

void DescriptionNodeLauout::referenceLayoutSceneNodeCombinate(const JsonType& jsonNode, const Math::Vec2& nodeSize, const DescNodeLayoutCallbackType& callback)
{

}
void DescriptionNodeLauout::referenceLayoutCallback(const DescNodeLayoutCallbackType& callback)
{
    auto tot = positions.size();
    for (auto i = 0; i < positions.size(); i++)
    {
        callback(i, positions[i], {1, 1}, angles.size() >= tot ? angles[i] : 0);
    }

}
} // namespace Voxol::Scene::Describe