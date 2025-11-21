#include "SceneIRComponent.h"
#include <cctype>

namespace Voxol::Scene
{
namespace SceneIR
{
namespace Shadering
{
void EffectShadow::parse(const JsonType& node)
{
    if (node.contains("id"))
        id = node["id"];
    if (node.contains("type"))
        type = node["type"];

    if (node.contains("color"))
    {
        auto&& vo = node["color"];
        if (vo.is_number())
        {
            color = static_cast<uint32_t>(vo);
        }
        else if (vo.is_string())
        {
            std::string&& hex_str = vo;
            std::transform(hex_str.begin(), hex_str.end(), hex_str.begin(),
                           [](unsigned char c) { return std::tolower(c); });

            if (hex_str.find('#') == 0)
            {
                color = std::stoul(hex_str.substr(1), nullptr, 16);
            }
            else if (hex_str.find('x') == 1)
            {
                if (hex_str.size() >= 3)
                {
                    color = std::stoul(hex_str.substr(2), nullptr, 16);
                }
            }
            else
            {
                color = std::stoul(hex_str.substr(2), nullptr, 16);
            }
            printf("color: %x\n", color);
        }
    }

    if (node.contains("offset") && node["offset"].is_array())
    {
        auto&& elements = node["offset"];
        if (elements.size() != 2)
            return;

        std::vector<float> vs;
        for (const auto& element : elements)
        {
            vs.push_back(element);
        }
        if (std::isnan(vs[0]) || std::isnan(vs[1]))
            return;
        offset = {vs[0], vs[1]};
    }
};

void Description::parseColor(const JsonType& vo)
{
    if (vo.is_number())
    {
        color = static_cast<uint32_t>(vo);
    }
    else if (vo.is_string())
    {
        std::string&& hex_str = vo;
        std::transform(hex_str.begin(), hex_str.end(), hex_str.begin(),
                       [](unsigned char c) { return std::tolower(c); });

        if (hex_str.find('#') == 0)
        {
            color = std::stoul(hex_str.substr(1), nullptr, 16);
        }
        else if (hex_str.find('x') == 1)
        {
            if (hex_str.size() >= 3)
            {
                color = std::stoul(hex_str.substr(2), nullptr, 16);
            }
        }
        else
        {
            color = std::stoul(hex_str.substr(2), nullptr, 16);
        }
        printf("color: %x\n", color);
    }
}
void Description::parse(const JsonType& node)
{

    if (node.contains("id"))
        id = node["id"];
    if (node.contains("type"))
        type = node["type"];

    color = 0x0;
    if (node.contains("color"))
    {
        parseColor(node["color"]);
    }
    else if (node.contains("fillColor"))
    {
        parseColor(node["fillColor"]);
    }
    else if (node.contains("strokeColor"))
    {
        parseColor(node["strokeColor"]);
    }

    if (node.contains("effects") && node["effects"].is_array())
    {
        auto elements = node["effects"];
        if (elements.empty())
            return;

        effects.clear();
        for (const auto& element : elements)
        {
            if (!element.is_number())
                continue;
            effects.push_back(static_cast<uint32_t>(element));
        }
    }
}

void Unit::parse(const JsonType& node)
{
    if (node.contains("id"))
        id = node["id"];
    if (node.contains("description"))
        description = node["description"];

    std::string blendmodeStr = node["blendmode"];

    static std::unordered_map<std::string, uint32_t> blendmodeMap{};

    if (blendmodeMap.empty())
    {
        blendmodeMap["None"]        = 0;
        blendmodeMap["Transparent"] = 1;
        blendmodeMap["Add"]         = 2;
    }

    blendmode = 0;
    if (blendmodeMap.contains(blendmodeStr))
    {
        blendmode = blendmodeMap[blendmodeStr];
    }
}
void Module::parse(const JsonType& node)
{
    if (node.contains("effects"))
    {
        auto&& effects = node["effects"];
        for (auto& node : effects)
        {
            std::string type = node["type"];
            if (type == "shadow")
            {
                EffectShadow m;
                m.parse(node);
                shadowsMap[m.id] = m;
            }
        }
    }
    if (node.contains("descriptions"))
    {
        auto&& descriptions = node["descriptions"];
        for (auto& node : descriptions)
        {
            Description m;
            m.parse(node);
            descriptionsMap[m.id] = m;
        }
    }

    if (node.contains("units"))
    {
        auto& units = node["units"];
        for (auto& node : units)
        {
            Unit m;
            m.parse(node);
            unitsMap[m.id] = m;
        }
    }
}

} // namespace Shadering
namespace Scene
{

void Model::parse(const JsonType& node)
{
    if (node.contains("id"))
        id = node["id"];

    if (node.contains("content"))
        content = node["content"];

    if (node.contains("type"))
        type = node["type"];
    if (node.contains("shape"))
        type = node["shape"];

    if (node.contains("method"))
        method.parse(node["method"]);

    if (node.contains("radius") && node["radius"].is_number())
    {
        auto v = static_cast<float>(node["radius"]);
        value  = std::isnan(v) ? 0.0f : v;
        return;
    }
    if (node.contains("fontSize") && node["fontSize"].is_number())
    {
        auto v = static_cast<float>(node["fontSize"]);
        value  = std::isnan(v) ? 0.0f : v;
        return;
    }

    if (node.contains("size") && node["size"].is_array())
    {
        auto&& elements = node["size"];
        if (elements.size() != 2)
            return;

        std::vector<float> vs;
        for (const auto& element : elements)
        {
            vs.push_back(element);
        }
        if (std::isnan(vs[0]) || std::isnan(vs[1]))
            return;
        Math::Vec2 v2{vs[0], vs[1]};
        value = v2;
    }
}

void Transform::parse(const JsonType& node)
{
    if (node.contains("id"))
        id = node["id"];

    if (node.contains("position") && node["position"].is_array())
    {
        auto elements = node["position"];
        if (elements.size() != 2)
            return;

        std::vector<float> vs;
        for (const auto& element : elements)
        {
            vs.push_back(element);
        }
        if (std::isnan(vs[0]) || std::isnan(vs[1]))
            return;
        position = {vs[0], vs[1]};
    }
}
} // namespace Scene
} // namespace SceneIR
} // namespace Voxol::Scene