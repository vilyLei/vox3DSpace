
#include "DescriptionNode.h"

namespace Voxol::Scene::Describe
{

void DisplayShape::parse(const JsonType& jsonNode)
{

    if (jsonNode.contains("type"))
    {
        type = jsonNode["type"];
    }
    else if (jsonNode.contains("shape"))
    {
        type = jsonNode["shape"];
    }
    else if (jsonNode.contains("shapeType"))
    {
        type = jsonNode["shapeType"];
    }
    if (!type.empty())
    {
        std::transform(type.begin(), type.end(), type.begin(),
                       [](unsigned char c) { return std::tolower(c); });
    }

    if (type == "rectangle" || type == "round-rectangle")
    {
        jsonValue.parseWithName(jsonNode, "size");
        if (jsonValue.is<Math::Vec2>())
        {
            size = jsonValue.get<Math::Vec2>();
        }
    }
    else if (type == "circle")
    {
        jsonValue.parseWithName(jsonNode, "radius");
        float radius = 100.0f;
        if (jsonValue.is<float>())
        {
            radius = jsonValue.get<float>() * 2;
            size   = {2 * radius, 2 * radius};
        }
        else if (jsonValue.is<int>())
        {
            radius = jsonValue.get<int>() * 2;
            size   = {2 * radius, 2 * radius};
        }
    }
    else if (type == "text")
    {
        jsonValue.parseTextWithName(jsonNode, "content");
        if (jsonValue.is<Component::UnitTextDesc>())
        {
            auto&& textDesc = jsonValue.get<Component::UnitTextDesc>();
            size            = {textDesc.fontSize, textDesc.fontSize};
        }
    }
}

} // namespace Voxol::Scene::Describe