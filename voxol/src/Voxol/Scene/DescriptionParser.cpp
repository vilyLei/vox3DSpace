#include "DescriptionParser.h"
#include "SceneIRComponent.h"
#include <fstream>
#include <filesystem>

namespace Voxol::Scene
{
namespace Desc
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



void SceneActionTargetNode::reset()
{

    srcActType = "";
    actions.clear();
}
void SceneActionTargetNode::parse(const JsonType& jsonNode, const std::string& srcActType_)
{
    if (!jsonNode.contains(srcActType_))
    {
        return;
    }
    srcActType      = srcActType_;
    auto&& elements = jsonNode[srcActType_];
    if (elements.empty())
    {
        return;
    }

    for (auto& item : elements)
    {
        auto& jNode = item;

        SceneActionDesc actDesc;
        actDesc.target = jNode["target"];

        auto&& actJNode = jNode["action"];
        if (actJNode.contains("type"))
        {
            actDesc.type = actJNode["type"];
        }

        Data::ColorValue cv;
        if (cv.parse(actJNode))
        {
            actDesc.color = cv.color.argb();
        }
        if (actJNode.contains("cmd"))
        {
            actDesc.cmd = actJNode["cmd"];
        }
        else
        {
            actDesc.cmd = "None";
        }
        printf("actDesc.target: %s, srcActType: %s\n", actDesc.target.c_str(), srcActType.c_str());
        actions.emplace_back(actDesc);
    }
}



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
    auto          pngPath = std::filesystem::path(SRC_DIR) / "assets/";
    auto          pathStr = pngPath.string() + fileName;
    std::ifstream fs(pathStr);
    fs >> jsonObj;

    auto&& sceneNode = jsonObj["scene"];
    parseHeriNodes(sceneNode);
    parseSceneActions(sceneNode);
}

void FileParser::parseSceneActions(const JsonType& jsonNode)
{
    if (jsonNode.contains("nodes") && jsonNode["nodes"].is_array())
    {
        auto&& elements = jsonNode["nodes"];

        if (elements.empty())
            return;

        //for (auto& item : elements)
        auto& children = rootNode.children;
        for (auto i = 0; i < elements.size(); i++)
        {
            parseNodeAction(children[i], elements[i]);
        }
    }
}

void FileParser::parseNodeActionData(SceneNode& currNode, const JsonType& jsonNode)
{
    actionIDMap[currNode.id] = {currNode.id, currNode.name};
}

void FileParser::parseNodeAction(SceneNode& parentNode, const JsonType& jsonNode)
{

    if (jsonNode.contains("interactions"))
    {
        auto&& jNode = jsonNode["interactions"];
        parseNodeInteractionData(parentNode, jNode);
    }

    if (jsonNode.contains("actions"))
    {
        auto&& jNode = jsonNode["actions"];
        parseNodeActionData(parentNode, jNode);
    }

    if (!parentNode.hasChild)
        return;

    auto flag = !jsonNode.contains("children") || !jsonNode["children"].is_array();
    if (flag)
        return;

    auto&& elements = jsonNode["children"];
    if (elements.empty())
        return;

    for (auto i = 0; i < elements.size(); i++)
    {
        parseNodeAction(parentNode.children[i], elements[i]);
    }
}

void FileParser::buildInteraction(Intent::Interaction::InteractionSource& srcNode, const std::string& srcActType, uint8_t actFlag, const JsonType& jsonNode)
{
    if (!jsonNode.contains(srcActType))
    {
        return;
    }
    SceneActionTargetNode tarNode;
    tarNode.parse(jsonNode, srcActType);
    Intent::Interaction::InteractionTargetSet tarSet;
    tarSet.flag  = actFlag;
    auto visible = true;
    for (auto& tarAct : tarNode.actions)
    {
        if (!nodeNameMap.contains(tarAct.target))
            continue;

        auto&& ni = nodeNameMap[tarAct.target];

        printf("FileParser::buildInteraction(), srcActType: %s, ni.id: %d\n", srcActType.c_str(), ni.id);
        auto tarKeyId = Base::ID::KeyUint64::make(ni.id);
        tarSet.targets.push_back({0, tarKeyId, tarAct.type, tarAct.cmd, tarAct.color, visible});
    }
    srcNode.addTargetSet(tarSet);
}
void FileParser::parseNodeInteractionMouseData(SceneNode& currNode, const JsonType& jsonNode)
{
    printf("FileParser::parseNodeInteractionMouseData(), name: %s\n", currNode.name.c_str());

    using namespace Intent::Interaction;

    InteractionSource srcNode;
    srcNode.id = Base::ID::KeyUint64::make(currNode.id);

    interactionIDMap[currNode.id] = {currNode.id, currNode.name};

    bool visible = true;

    auto actFlag = static_cast<uint8_t>(MouseStatus::Over);
    buildInteraction(srcNode, "over", actFlag, jsonNode);

    actFlag = static_cast<uint8_t>(MouseStatus::Out);
    buildInteraction(srcNode, "out", actFlag, jsonNode);

    actFlag = static_cast<uint8_t>(MouseStatus::Down);
    buildInteraction(srcNode, "down", actFlag, jsonNode);

    actFlag = static_cast<uint8_t>(MouseStatus::Up);
    buildInteraction(srcNode, "up", actFlag, jsonNode);

    interactionSrcMap[srcNode.id] = srcNode;
}
void FileParser::parseNodeInteractionData(SceneNode& currNode, const JsonType& jsonNode)
{
    printf("FileParser::parseNodeInteractionData(), name: %s\n", currNode.name.c_str());
    if (jsonNode.contains("mouse"))
    {
        parseNodeInteractionMouseData(currNode, jsonNode["mouse"]);
    }
}

void FileParser::parseHeriNodes(const JsonType& jsonNode)
{
    //auto flag = jsonNode.contains("nodes") && jsonNode["nodes"].is_array();
    //if (!flag)
    //    return;

    //auto&& elements = jsonNode["nodes"];
    //if (elements.empty())
    //    return;

    uint32_t id        = 0;
    rootNode.id        = id;
    rootNode.hasParent = false;
    rootNode.type      = "root";
    rootNode.name      = "root-node";
    id++;
    //for (auto& item : elements)
    //{
    //    SceneNode node;
    //    node.id = id++;
    //    parseSceneNode(node, id, item, "children");
    //    node.print();
    //    rootNode.children.emplace_back(std::move(node));
    //}
    //rootNode.childrenTotal = static_cast<int>(rootNode.children.size());
    //rootNode.print();
    parseSceneNode(rootNode, id, jsonNode, "nodes");
}
void FileParser::parseSceneNode(SceneNode& parentNode, uint32_t& id, const JsonType& jsonNode, const std::string& nodesName)
{

    parseNodeData(parentNode, jsonNode);
    std::string refKey = "reference";
    if (jsonNode.contains(refKey) && jsonNode[refKey].is_object())
    {
        auto&& refNode = jsonNode[refKey];
    }

    //if (!parentNode.hasChild)
    //    return;
    if (!jsonNode.contains(nodesName) || !jsonNode[nodesName].is_array())
        return;

    //if (jsonNode.contains(nodesName) && jsonNode[nodesName].is_array())
    //{
    auto&& elements = jsonNode[nodesName];
    if (elements.empty())
        return;
    for (auto& item : elements)
    {
        SceneNode node;
        node.id = id++;
        parseSceneNode(node, id, item, "children");
        node.print();
        parentNode.children.emplace_back(std::move(node));
    }
    parentNode.childrenTotal = static_cast<int>(parentNode.children.size());
    //}
}


void FileParser::parseNodeDisplayStyle(SceneNode& node, const JsonType& jsonNode)
{
    if (!jsonNode.contains("style"))
        return;

    auto&& jNode = jsonNode["style"];
    if (jNode.contains("fills"))
    {
        auto&& elements = jNode["fills"];
        if (!elements.empty())
        {
            for (auto& item : elements)
            {
                Data::ColorValue cv;
                cv.parseWithName(item, "value");
                node.shaingDesc.color = cv.color.argb();
            }
            return;
        }
    }
    Data::ColorValue cv;
    cv.parse(jNode);
    node.shaingDesc.color = cv.color.argb();
}
void FileParser::parseNodeDisplayShape(SceneNode& node, const JsonType& jsonNode)
{
    if (!jsonNode.contains("shape"))
        return;

    auto&& jNode = jsonNode["shape"];

    DisplayShape shape;
    shape.parse(jNode);
    node.transform.scale() = shape.size;

    if (shape.type == "rectangle" || shape.type == "round-rectangle")
    {
        node.unitModel.drawUnitId = 0;
        node.unitModel.type       = Component::UnitModelType::Mesh;
    }
    else if (shape.type == "circle")
    {
        node.unitModel.drawUnitId = 1;
        node.unitModel.type       = Component::UnitModelType::Mesh;
    }
    else if (shape.type == "text")
    {
        node.unitModel.drawUnitId = 0;
        node.unitModel.type       = Component::UnitModelType::Text;

        auto&                    textDesc = shape.jsonValue.get<Component::UnitTextDesc>();
        Component::UnitTextModel textModel;
        textModel.id               = node.id;
        textModel.text             = textDesc;
        textModelMap[textModel.id] = textModel;
    }
}
void FileParser::parseNodeTransData(SceneNode& node, const JsonType& jsonNode)
{
    auto id = node.id;
    if (jsonNode.contains("transform"))
    {
        SceneIR::Scene::Transform jTrans;
        jTrans.parse(jsonNode["transform"]);
        auto pv              = jTrans.position;
        node.transform.pos() = jTrans.position;
    }
    if (jsonNode.contains("display"))
    {
        auto&& displayNode               = jsonNode["display"];
        node.shadingEntity.id            = id;
        node.shadingEntity.shadingDescId = id;

        node.unitModel.id = id;

        parseNodeDisplayShape(node, displayNode);
        parseNodeDisplayStyle(node, displayNode);
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

    auto id                = node.id;
    nodeNameMap[node.name] = {id, node.name};

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
        node.hasChild   = !elements.empty();
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