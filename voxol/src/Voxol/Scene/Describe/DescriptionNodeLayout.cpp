
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
    jsonV.reset();
    pos         = jsonV.position();

    positions.reserve(count);
    angles.reserve(count);
    angles.clear();

    auto method = layoutNode.method;
    if (method == "superformula")
    {

        auto scale = jsonV.scale();
        if (!jsonNode.contains("params") || !jsonNode["params"].is_object())
            return;

        auto&&                     paramsJson = jsonNode["params"];
        RefLayoutValueRef          jsonParamsV{paramsJson};
        Superformula::FormulaParam param;

        param.m = jsonParamsV.floatValue("m");
        jsonParamsV.reset();
        param.a = jsonParamsV.floatValue("a");
        jsonParamsV.reset();
        param.b    = jsonParamsV.floatValue("b");
        jsonParamsV.reset();
        param.n1   = jsonParamsV.floatValue("n1");
        jsonParamsV.reset();
        param.n1   = jsonParamsV.floatValue("n1");
        jsonParamsV.reset();
        param.n2   = jsonParamsV.floatValue("n2");

        positions = Superformula::generateSuperformula(pos, count, param, scale);
        return;
    }
    if (method == "diamond")
    {
        //positions   = Distribution::diamondFilledGrid(20, 5);

        auto radius = jsonV.radius();
        for (auto i = 0; i < count; i++)
        {
            auto&& pv = Distribution::diamond(i, count, radius);
            positions.emplace_back(pos + pv);
        }
        return;
    }
    if (method == "diamond-filled")
    {
        auto radius = jsonV.radius();
        jsonV.reset();
        auto spacing = jsonV.spacing();
        positions    = Distribution::diamondFilledGrid(pos, spacing, radius);
        return;
    }
    if (method == "circle-filled-random")
    {
        auto radius  = jsonV.radius();
        positions    = Distribution::circleFilledRandom(pos, count, radius);
        return;
    }
    if (method == "circle-filled-grid")
    {
        auto radius = jsonV.radius();
        jsonV.reset();
        auto radiusSpacing = jsonV.radiusSpacing();
        positions          = Distribution::circleFilledGrid(pos, radiusSpacing, radius);
        return;
    }
    if (method == "circle-filled-hex")
    {
        auto radius = jsonV.radius();
        jsonV.reset();
        auto radiusSpacing = jsonV.radiusSpacing();
        positions          = Distribution::circleFilledHex(pos, radiusSpacing, radius);
        return;
    }
    if (method == "poisson-disk-circle")
    {
        auto radius = jsonV.radius();
        jsonV.reset();
        auto mindDistance = jsonV.mindDistance();
        positions          = Distribution::poissonDiskCircle(pos, radius, mindDistance);
        return;
    }
    //poissonDiskCircle

    if (method == "snowflake")
    {
        auto radius = jsonV.radius();
        for (auto i = 0; i < count; i++)
        {
            auto&& pv = Distribution::snowflake(i, count, radius);
            positions.emplace_back(pos + pv);
        }
        return;
    }
    if (method == "rhombus-grid")
    {
        auto spacing = jsonV.spacing();
        for (auto i = 0; i < count; i++)
        {
            auto&& pv = Distribution::rhombusGrid(i, count, spacing);
            positions.emplace_back(pos + pv);
        }
        return;
    }
    if (method == "rose")
    {
        auto radius = jsonV.radius();
        jsonV.reset();
        auto amplitude = jsonV.amplitude();
        for (auto i = 0; i < count; i++)
        {
            auto&& pv = Distribution::rose(i, count, radius, amplitude);
            positions.emplace_back(pos + pv);
        }
        return;
    }
    //rhombus-grid
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
    center = jsonV.position();
    jsonV.reset();
    start_radius = jsonV.startRadius();
    jsonV.reset();
    step_radius  = jsonV.stepRadius();
    jsonV.reset();
    step_angle   = jsonV.stepAngle();
    jsonV.reset();
    start_angle  = jsonV.startAngle();
    jsonV.reset();
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
    pos = jsonV.position();
    jsonV.reset();
    hexRadius = jsonV.hexRadius();
    jsonV.reset();
    rings     = jsonV.rings();
    jsonV.reset();
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
    center = jsonV.position();
    jsonV.reset();
    radius = jsonV.radius();
    jsonV.reset();
    startAngle                   = jsonV.startAngle();
    jsonV.reset();
    arcAngle                     = jsonV.arcAngle();
    jsonV.reset();
    count                        = jsonV.count();

    angles.reserve(count);
    angles.clear();
    positions = PositionDistribution::arc(angles, count, center, radius, startAngle, arcAngle);
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
    jsonV.reset();
    radius = jsonV.radius();
    jsonV.reset();
    startAngle = jsonV.startAngle();
    jsonV.reset();
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
    jsonV.reset();
    columns = jsonV.columns();
    jsonV.reset();
    count   = jsonV.count();
    jsonV.reset();
    spacing = jsonV.spacing();
    jsonV.reset();
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