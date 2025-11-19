#include "DescriptionParser.h"
#include <fstream>
#include <filesystem>

namespace Voxol::Scene
{
namespace Desc
{
void SceneNode::parse(const JsonType& jsonNode)
{
    if (jsonNode.contains("type"))
    {
        type = jsonNode["type"];
    }
    if (jsonNode.contains("name"))
    {
        name = jsonNode["name"];
    }
    if (jsonNode.contains("children") && jsonNode["children"].is_array())
    {
        auto&& elements = jsonNode["children"];
        hasChild        = !elements.empty();
    }
}
void SceneNode::print()
{

    std::string hasChildStr = ", hasChild=" + (hasChild ? std::string("true") : std::string("false"));
    std::string info        = "Node(name=" + name + ",type=" + type + ", childrenTotal=" + std::to_string(childrenTotal) + hasChildStr + ")";
    printf("%s\n", info.c_str());
}

void FileParser::initFromFile(const std::string& fileName)
{
    auto           pngPath = std::filesystem::path(SRC_DIR) / "assets/";
    auto           pathStr = pngPath.string() + fileName;
    std::ifstream  fs(pathStr);
    nlohmann::json jsonObj;
    fs >> jsonObj;

    parseHeriNodes(jsonObj["scene"]);
}
void FileParser::parseHeriNodes(const JsonType& jsonNode)
{
    if (jsonNode.contains("nodes") && jsonNode["nodes"].is_array())
    {
        auto&& elements = jsonNode["nodes"];

        auto sizeValue = elements.size();

        if (elements.empty())
            return;

        rootNode.hasParent = false;
        for (auto& item : elements)
        {
            parseSceneNode(rootNode, item);
            rootNode.print();
        }
    }
}
void FileParser::parseSceneNode(SceneNode& parentNode, const JsonType& jsonNode)
{

    parentNode.parse(jsonNode);

    if (!parentNode.hasChild)
        return;

    if (jsonNode.contains("children") && jsonNode["children"].is_array())
    {
        auto&& elements = jsonNode["children"];
        if (elements.empty())
            return;
        for (auto& item : elements)
        {
            SceneNode node;
            parseSceneNode(node, item);
            node.print();
            parentNode.children.emplace_back(std::move(node));
        }
        parentNode.childrenTotal = static_cast<int>(parentNode.children.size());
    }
}
} // namespace Desc

void DescriptionParser::initialize()
{
    std::string filePath = "scdesc/scdesc01.json";
    fileParser.initFromFile(filePath);
}

} // namespace Voxol::Scene