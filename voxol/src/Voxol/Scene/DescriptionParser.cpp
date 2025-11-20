#include "DescriptionParser.h"
#include "SceneIRComponent.h"
#include <fstream>
#include <filesystem>

namespace Voxol::Scene
{
namespace Desc
{

void SceneNode::parseTrans(const JsonType& jsonNode)
{
    if (jsonNode.contains("transform"))
    {
        SceneIR::Scene::Transform jTrans;
        jTrans.parse(jsonNode["transform"]);
        auto pv     = jTrans.position;
        trans.pos() = jTrans.position;
    }
    if (jsonNode.contains("display"))
    {
        auto&& displayNode    = jsonNode["display"];
        shadingEntity.id      = index;
        unitModel.id          = index;
        std::string shapeType = "";
        if (displayNode.contains("shape") && displayNode["shape"].is_string())
        {
            shapeType = displayNode["shape"];
        }
        if (shapeType == "rectangle" || shapeType == "round-rectangle")
        {
            shadingEntity.shadingDescId = index;
            unitModel.drawUnitId        = 0;
            unitModel.type              = Component::UnitModelType::Mesh;
        }
        else if (shapeType == "text" || shapeType == "Text")
        {
            unitModel.drawUnitId = 0;
            unitModel.type       = Component::UnitModelType::Text;
        }

        SceneIR::Shadering::Description jDesc;
        jDesc.parse(displayNode);

        shaingDesc.color = jDesc.color;

        SceneIR::Scene::Model jModel;
        jModel.parse(displayNode);
        if (jModel.hasRadius())
        {
            auto pw       = jModel.getRadius() * 2;
            trans.scale() = {pw, pw};
        }
        else if (jModel.hasSize())
        {
            auto&& size   = jModel.getSize();
            trans.scale() = size;
        }

        if ((jModel.type == "text" || jModel.type == "Text") && !jModel.content.empty())
        {
            textModel = {index,
                         jModel.getFontSize(),
                         jModel.content};

            trans.scale() = {textModel.fontSize, textModel.fontSize};
        }
    }
}
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
    parseTrans(jsonNode);
    entity.id          = index;
    entity.shadingId   = index;
    entity.transformId = index;
    entity.hierarchyId = index;
    entity.modelId     = index;

    if (jsonNode.contains("children") && jsonNode["children"].is_array())
    {
        auto&& elements = jsonNode["children"];
        hasChild        = !elements.empty();
    }
}

void SceneNode::print() const
{

    std::string info = ", hasChild=" + (hasChild ? std::string("true") : std::string("false"));
    info += ", childrenTotal=" + std::to_string(childrenTotal) + ", index=" + std::to_string(index);
    info = "SceneNode(name=" + name + ",type=" + type + info + ")";
    printf("%s\n", info.c_str());
}
void SceneNode::printTrans() const
{
    std::string info = ", tran(x=" + std::to_string(trans.x) + ",y=" + std::to_string(trans.y);
    info += ",sx=" + std::to_string(trans.sx) + ",sy=" + std::to_string(trans.sy) + ")";
    info = "SceneNode(name=" + name + info + ")";
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
    std::string filePath = "scene/scdesc/scdesc01.json";
    fileParser.initFromFile(filePath);
}

} // namespace Voxol::Scene