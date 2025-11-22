#include "DescriptionParser.h"
#include "SceneIRComponent.h"
#include <fstream>
#include <filesystem>

namespace Voxol::Scene
{
namespace Desc
{

void SceneNode::print() const
{
    std::string info = ", hasChild=" + (hasChild ? std::string("true") : std::string("false"));
    info += ", childrenTotal=" + std::to_string(childrenTotal) + ", id=" + std::to_string(id);
    info = "SceneNode(name=" + name + ",type=" + type + info + ")";
    printf("%s\n", info.c_str());
}
void SceneNode::printTransform() const
{
    std::string info = ", tran(x=" + std::to_string(transform.x) + ",y=" + std::to_string(transform.y);
    info += ",sx=" + std::to_string(transform.sx) + ",sy=" + std::to_string(transform.sy) + ")";
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

        uint32_t id     = 0;
        rootNode.id        = id;
        rootNode.hasParent = false;
        for (auto& item : elements)
        {
            id++;
            parseSceneNode(rootNode, id, item);
            rootNode.print();
        }
    }
}
void FileParser::parseSceneNode(SceneNode& parentNode, uint32_t& id, const JsonType& jsonNode)
{

    parseNodeData(parentNode, jsonNode);

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
            node.id = id++;
            parseSceneNode(node, id, item);
            node.print();
            parentNode.children.emplace_back(std::move(node));
        }
        parentNode.childrenTotal = static_cast<int>(parentNode.children.size());
    }
}



void FileParser::parseNodeTransData(SceneNode& node, const JsonType& jsonNode)
{
    auto id = node.id;
    if (jsonNode.contains("transform"))
    {
        SceneIR::Scene::Transform jTrans;
        jTrans.parse(jsonNode["transform"]);
        auto pv         = jTrans.position;
        node.transform.pos() = jTrans.position;
    }
    if (jsonNode.contains("display"))
    {
        auto&& displayNode          = jsonNode["display"];
        node.shadingEntity.id       = id;
        node.shadingEntity.shadingDescId = id;

        node.unitModel.id = id;

        std::string shapeType = "";
        if (displayNode.contains("shape") && displayNode["shape"].is_string())
        {
            shapeType = displayNode["shape"];
        }
        if (shapeType == "rectangle" || shapeType == "round-rectangle")
        {
            node.unitModel.drawUnitId = 0;
            node.unitModel.type       = Component::UnitModelType::Mesh;
        }
        else if (shapeType == "text" || shapeType == "Text")
        {
            node.unitModel.drawUnitId = 0;
            node.unitModel.type       = Component::UnitModelType::Text;
        }

        SceneIR::Shadering::Description jDesc;
        jDesc.parse(displayNode);

        node.shaingDesc.color = jDesc.color;

        SceneIR::Scene::Model jModel;
        jModel.parse(displayNode);
        if (jModel.hasRadius())
        {
            auto pw           = jModel.getRadius() * 2;
            node.transform.scale() = {pw, pw};
        }
        else if (jModel.hasSize())
        {
            auto&& size       = jModel.getSize();
            node.transform.scale() = size;
        }

        //if (jModel.isText() && !jModel.content.empty())
        //{
        //    Component::UnitStringModel textModel = {node.id,
        //                                            jModel.getFontSize(),
        //                                            jModel.content};
        //    textModelMap[textModel.id] = textModel;
        //    node.transform.scale() = {textModel.fontSize, textModel.fontSize};
        //}
        if (jModel.hasText())
        {
            Component::UnitStringModel textModel;
            textModel.id = node.id;
            textModel.text = jModel.getText();
            //= {node.id,
            //                                        jModel.getFontSize(),
            //                                        jModel.content};
            textModelMap[textModel.id]           = textModel;
            node.transform.scale()     = {textModel.text.fontSize, textModel.text.fontSize};
        }
    }
}

void FileParser::parseNodeData(SceneNode& node, const JsonType& jsonNode)
{
    if (jsonNode.contains("type"))
    {
        node.type = jsonNode["type"];
    }
    if (jsonNode.contains("name"))
    {
        node.name = jsonNode["name"];
    }
    auto id = node.id;
    parseNodeTransData(node, jsonNode);
    auto& entity       = node.entity;
    entity.id          = id;
    entity.shadingId   = id;
    entity.transformId = id;
    entity.hierarchyId = id;
    entity.modelId     = id;

    if (jsonNode.contains("children") && jsonNode["children"].is_array())
    {
        auto&& elements = jsonNode["children"];
        node.hasChild    = !elements.empty();
    }
}


void HierarchyParser::foreachNode(Desc::SceneNode& parentNode, SceneNodeForeachCallbackType callback)
{

    callback(parentNode);

    auto& children = parentNode.children;
    if (children.empty())
        return;

    for (auto i = 0; i < children.size(); ++i)
    {
        foreachNode(children[i], callback);
    }
}
void HierarchyParser::parse(Desc::SceneNode& parentNode, Desc::HierarchyNode& parentHierNode)
{
    parentHierNode.id   = parentNode.id;
    parentHierNode.name = parentNode.name;

    auto& children = parentNode.children;
    if (children.empty())
    {
        parentNode.hieraychy = parentHierNode.hieraychy;
        parentHierNode.print();
        return;
    }

    parentHierNode.hieraychy.firstChild = children[0].id;
    for (auto i = 0; i < children.size(); ++i)
    {
        auto&&              child = children[i];
        Desc::HierarchyNode hierNode;
        hierNode.hieraychy.parent = parentNode.id;

        if ((i + 1) < children.size())
            hierNode.hieraychy.next = children[i + 1].id;

        parse(child, hierNode);
    }
    parentNode.hieraychy = parentHierNode.hieraychy;
    parentHierNode.print();
}

} // namespace Desc

void DescriptionParser::initialize()
{
    std::string filePath = "scene/scdesc/scdesc01.json";
    fileParser.initFromFile(filePath);

    Desc::HierarchyNode rootHierNode;
    hierParser.parse(fileParser.rootNode, rootHierNode);
}

} // namespace Voxol::Scene