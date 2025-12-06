#include "DescriptionParser.h"
#include "../SceneIRComponent.h"
#include "../../Math/MathDef.h"
#include "../Layout/PositionDistributor.h"
#include "../../Render/OglImage.h"
#include "../../Render/OglGpuResUtils.h"
#include "DescriptionNodeLayout.h"

#include <fstream>
#include <filesystem>

namespace Voxol::Scene::Describe
{


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
        if (!entityNameMap.contains(tarAct.target))
            continue;

        auto&& ni = entityNameMap[tarAct.target];

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



bool FileParser::hasSceneNodeWithNameFromRoot(const std::string& nodeName)
{

    auto&& sceneNode = jsonObj["scene"];
    return hasSceneNodeWithNameRecursive(nodeName, sceneNode, "nodes");
}

bool FileParser::hasSceneNodeWithNameRecursive(const std::string& nodeName, const JsonType& jsonNode, const std::string& nodesName)
{

    if (!jsonNode.contains(nodesName) || !jsonNode[nodesName].is_array())
        return false;

    auto&& elements = jsonNode[nodesName];
    if (elements.empty())
        return false;

    for (auto& item : elements)
    {
        std::string keyName = "name";
        if (item.contains(keyName) && item[keyName].is_string())
        {
            std::string nameStr = item[keyName];
            if (!nameStr.empty() && nameStr == nodeName)
            {
                return true;
            }
        }
        auto flag = hasSceneNodeWithNameRecursive(nodeName, item, "children");
        if (flag)
        {
            return true;
        }
    }
    return false;
}

void FileParser::parseSceneNodeWithNameFromRoot(SceneNode& currNode, uint32_t& id, const std::string& nodeName)
{

    auto&& sceneNode = jsonObj["scene"];
    parseSceneNodeWithNameRecursive(currNode, id, nodeName, sceneNode, "nodes");
}
void FileParser::parseSceneNodeWithNameRecursive(SceneNode& currNode, uint32_t& id, const std::string& nodeName, const JsonType& jsonNode, const std::string& nodesName)
{

    if (!jsonNode.contains(nodesName) || !jsonNode[nodesName].is_array())
        return;

    auto&& elements = jsonNode[nodesName];
    if (elements.empty())
        return;
    for (auto& item : elements)
    {
        std::string keyName = "name";
        if (item.contains(keyName) && item[keyName].is_string())
        {
            std::string nameStr = item[keyName];
            if (!nameStr.empty() && nameStr == nodeName)
            {
                parseSceneNode(currNode, id, item, "children");
                return;
            }
        }
        parseSceneNodeWithNameRecursive(currNode, id, nodeName, item, "children");
    }
}
void FileParser::parseHeriNodes(const JsonType& jsonNode)
{
    uint32_t id        = 0;
    rootNode.id        = id;
    rootNode.hasParent = false;
    rootNode.type      = "root";
    rootNode.name      = "root-node";
    id++;

    parseSceneNode(rootNode, id, jsonNode, "nodes");
    rootNode.entity.modelId   = Base::ID::INVALID_ID;
    rootNode.entity.shadingId = Base::ID::INVALID_ID;
    rootNode.print();

}

bool FileParser::parseSceneNodeReference(SceneNode& parentNode, uint32_t& id, const JsonType& jsonNode, const std::string& nodesName)
{
    std::string refKey = "reference";
    if (!jsonNode.contains(refKey) || !jsonNode[refKey].is_object())
        return false;


    auto&& refNode = jsonNode[refKey];
    auto& preTrans = parentNode.transform;
    auto  preName  = parentNode.name;
    auto  visible  = parentNode.entity.visible;

    DescNodeRference ref;
    ref.parse(refNode);
    if (refNode.empty())
        return true;

    auto& srcList = ref.srcList;
    if (ref.isContainer())
    {
        auto srcCount = static_cast<int>(srcList.size());

        SceneNode tempNode;
        parseSceneNodeWithNameFromRoot(tempNode, tempNode.id, srcList[0].src);
        DescriptionNodeLauout::referenceLayoutSceneNode(
            jsonNode,
            tempNode.transform.scale(),
            [&, this](int index, const Math::Vec2& pos, const Math::Vec2& scale, float rotation) {
                SceneNode node;
                node.id = id++;
                auto k  = index;
                switch (ref.srcWrapping)
                {
                    case Voxol::Scene::Describe::RefLayoutSrcWrapping::Clamp:
                        k = index > (srcCount - 1) ? (srcCount - 1) : index;
                        break;
                    case Voxol::Scene::Describe::RefLayoutSrcWrapping::Repeat:
                        k = index % srcCount;
                        break;
                    default:
                        k = index;
                        break;
                }

                const auto& ns = srcList[k].src;
                parseSceneNodeWithNameFromRoot(node, id, ns);
                node.transform.pos()    = pos;
                node.transform.rotation = rotation;
                node.entity.visible     = visible;
                parentNode.children.emplace_back(std::move(node));
            });

        parentNode.childrenTotal = static_cast<int>(parentNode.children.size());
    }
    else
    {
        auto& srcNodeName = srcList[0].src;
        if (!hasSceneNodeWithNameFromRoot(srcNodeName))
            return true;

        parseSceneNodeWithNameFromRoot(parentNode, id, srcNodeName);
        parentNode.transform.pos() = preTrans.pos();
        parentNode.name            = preName;
        parentNode.entity.visible  = visible;
    }
    return true;
}
void FileParser::parseSceneNode(SceneNode& parentNode, uint32_t& id, const JsonType& jsonNode, const std::string& nodesName)
{

    parseNodeData(parentNode, jsonNode);
    auto flag = parseSceneNodeReference(parentNode, id, jsonNode, nodesName);
    if (flag)
    {
        return;
    }

    if (!jsonNode.contains(nodesName) || !jsonNode[nodesName].is_array())
        return;

    auto&& elements = jsonNode[nodesName];
    if (elements.empty())
        return;

    auto parentId = parentNode.id;

    for (auto& item : elements)
    {
        SceneNode node;
        node.id = id++;
        parseSceneNode(node, id, item, "children");
        node.print();
        parentNode.children.emplace_back(std::move(node));
    }
    parentNode.childrenTotal = static_cast<int>(parentNode.children.size());
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
                if (jNode.contains("blend-mode") && jNode["blend-mode"].is_string())
                {
                    auto&& blendModeStr = jNode["blend-mode"];
                    auto&& blendMode    = Render::Gpu::DrawingBlendMode::fromString(blendModeStr);
                    node.shaingDesc.drawState.blendMode(blendMode);
                }
            }
            return;
        }
    }

    Data::ColorValue cv;
    cv.parse(jNode);
    node.shaingDesc.color = cv.color.argb();
    if (jNode.contains("blend-mode") && jNode["blend-mode"].is_string())
    {
        auto&& blendModeStr = jNode["blend-mode"];
        auto&& blendMode = Render::Gpu::DrawingBlendMode::fromString(blendModeStr);
        node.shaingDesc.drawState.blendMode(blendMode);
    }

    if (jNode.contains("image-url") && jNode["image-url"].is_string())
    {
        node.imageUrl = jNode["image-url"];
    }

    if (!node.imageUrl.empty())
    {
        if (!textureUrlMap.contains(node.imageUrl))
        {
            Render::OglImage imgObj{};
            auto&&           imgData = imgObj.loadPNGFromAssets(node.imageUrl);
            if (imgData.width > 0 && imgData.height > 0)
            {
                auto tex                     = Render::ResUtils::createTextureFromImageBytes(imgData.width, imgData.height, imgData.buffer);
                textureUrlMap[node.imageUrl] = {tex, node.imageUrl};
            }
        }
        if (textureUrlMap.contains(node.imageUrl))
        {
            textureMap[node.id] = textureUrlMap[node.imageUrl];
        }
    }
}
void FileParser::parseNodeDisplayShape(SceneNode& node, const JsonType& jsonNode)
{
    if (!jsonNode.contains("shape"))
        return;

    auto&& jNode = jsonNode["shape"];

    DisplayShape shape;
    shape.parse(jNode);
    node.transform.scale() = shape.size;
    node.unitModel.id      = node.id;

    Component::updateUnitModel(node.unitModel, shape.type, !node.imageUrl.empty());

    if (node.unitModel.isTextType())
    {
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
        node.transform.pos()    = jTrans.position;
        node.transform.rotation = jTrans.rotation;
        if (jTrans.pivotNormalized.length() > 0)
        {
            transformPivotMap[node.id] = jTrans.pivotNormalized;
        }
    }
    if (jsonNode.contains("display"))
    {
        auto&& displayNode               = jsonNode["display"];
        node.shadingEntity.id            = id;
        node.shadingEntity.shadingDescId = id;
        node.unitModel.id                = id;
        parseNodeDisplayStyle(node, displayNode);
        parseNodeDisplayShape(node, displayNode);
    }
    else
    {
        node.transform.scale() = {0, 0};
    }
}

void FileParser::parseNodeData(SceneNode& node, const JsonType& jsonNode)
{

    if (jsonNode.contains("type") && jsonNode["type"].is_string())
    {
        node.type = jsonNode["type"];
    }

    if (jsonNode.contains("name") && jsonNode["name"].is_string())
    {
        node.name = jsonNode["name"];
    }

    auto id                = node.id;
    entityNameMap[node.name] = {id, node.name};

    parseNodeTransData(node, jsonNode);
    auto& entity       = node.entity;
    entity.id          = id;
    entity.shadingId   = id;
    entity.transformId = id;
    entity.hierarchyId = id;
    entity.modelId     = id;
    if (jsonNode.contains("visible") && jsonNode["visible"].is_boolean())
    {
        entity.visible = jsonNode["visible"];
    }

    std::string childrenName = "children";
    if (jsonNode.contains(childrenName) && jsonNode[childrenName].is_array())
    {
        auto&& elements = jsonNode[childrenName];
        node.hasChild   = !elements.empty();
    }
}


void HierarchyParser::foreachNode(SceneNode& parentNode, SceneNodeForeachCallbackType callback)
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
void HierarchyParser::parse(SceneNode& parentNode, HierarchyNode& parentHierNode)
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
        auto&&        child = children[i];
        HierarchyNode hierNode;
        hierNode.hieraychy.parent = parentNode.id;

        if ((i + 1) < children.size())
            hierNode.hieraychy.next = children[i + 1].id;

        parse(child, hierNode);
    }
    parentNode.hieraychy = parentHierNode.hieraychy;
    parentHierNode.print();
}

void DescriptionParser::initialize()
{
    std::string filePath = "scene/scdesc/scdesc01.json";
    filePath             = "scene/scdesc/scdesc_word_snake.json";
    filePath             = "scene/scdesc/scdesc_pos_distribution.json";
    filePath             = "scene/scdesc/scdesc_motion.json";
    fileParser.initFromFile(filePath);

    HierarchyNode rootHierNode;
    hierParser.parse(fileParser.rootNode, rootHierNode);

    hierParser.foreachNode(fileParser.rootNode, [](SceneNode& node) {
        node.printTransform();
    });
}

} // namespace Voxol::Scene::Describe