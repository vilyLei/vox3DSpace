#include "SceneIRParser.h"
#include <fstream>
#include <filesystem>
#include <cctype>
namespace Voxol::Render
{
namespace SceneIR
{
namespace Shadering
{

void Description::parse(const JsonType& node)
{
    id    = node["id"];
    type  = node["type"];
    color = 0x0;
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
}
void Module::parse(const JsonType& node)
{
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
    id     = node["id"];
    type   = node["type"];
    method.parse( node["method"] );

    if (node.contains("radius") && node["radius"].is_number())
    {
        auto v = static_cast<float>(node["radius"]);
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

void SceneIRParser::parseFromFile(const std::string& fileName)
{

    auto           pngPath = std::filesystem::path(SRC_DIR) / "assets/";
    auto           pathStr = pngPath.string() + fileName;
    std::ifstream  fs(pathStr);
    nlohmann::json jo;
    fs >> jo;
    parse(jo);
}
void SceneIRParser::parse(const nlohmann::json& json)
{
    if (json.contains("shadering"))
    {
        shaderingModule.parse(json["shadering"]);
    }
    if (json.contains("scene"))
    {
        sceneModule.parse(json["scene"]);
    }
}
} // namespace Voxol::Render