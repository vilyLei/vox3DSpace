#ifndef VOXOL_SCENE_DESCRIPTION_PARSER_H
#define VOXOL_SCENE_DESCRIPTION_PARSER_H

#include "../Base/BaseDefine.h"
#include "Describe/DescriptionNode.h"
#include "../Math/Vec2.h"
#include <nlohmann/json.hpp>
#include <string>
#include <functional>

namespace Voxol::Scene
{
namespace Describe
{
using JsonType = nlohmann::json;


class FileParser
{
public:
    FileParser()  = default;
    ~FileParser() = default;

public:
    SceneNode                                                                 rootNode;
    nlohmann::json                                                            jsonObj;
    std::unordered_map<uint32_t, Component::UnitTextModel>                    textModelMap;
    std::unordered_map<std::string, Intent::Interaction::InteractionNodeName> nodeNameMap;
    std::unordered_map<uint32_t, Intent::Interaction::InteractionNodeName>    interactionIDMap;
    Base::ID::keyUint64Unordered_map<Intent::Interaction::InteractionSource>  interactionSrcMap;
    std::unordered_map<uint32_t, Intent::Interaction::InteractionNodeName>    actionIDMap;

public:

    void initFromFile(const std::string& fileName);

    bool hasSceneNodeWithNameFromRoot(const std::string& nodeName);
    bool hasSceneNodeWithNameRecursive(const std::string& nodeName, const JsonType& jsonNode, const std::string& nodesName);

    void parseSceneNodeWithNameFromRoot(SceneNode& currNode, uint32_t& id, const std::string& nodeName);
    void parseSceneNodeWithNameRecursive(SceneNode& currNode, uint32_t& id, const std::string& nodeName, const JsonType& jsonNode, const std::string& nodesName);
    void parseHeriNodes(const JsonType& jsonNode);
    void parseSceneNode(SceneNode& parentNode, uint32_t& id, const JsonType& jsonNode, const std::string& nodesName);

    void parseNodeDisplayStyle(SceneNode& node, const JsonType& jsonNode);
    void parseNodeDisplayShape(SceneNode& node, const JsonType& jsonNode);
    void parseNodeTransData(SceneNode& node, const JsonType& jsonNode);
    void parseNodeData(SceneNode& node, const JsonType& jsonNode);

    void buildInteraction(Intent::Interaction::InteractionSource& srcNode, const std::string& srcActType, uint8_t actFlag, const JsonType& jsonNode);

    void parseSceneActions(const JsonType& jsonNode);
    void parseNodeAction(SceneNode& parentNode, const JsonType& jsonNode);
    void parseNodeInteractionMouseData(SceneNode& currNode, const JsonType& jsonNode);
    void parseNodeInteractionData(SceneNode& currNode, const JsonType& jsonNode);
    void parseNodeActionData(SceneNode& currNode, const JsonType& jsonNode);
};

using SceneNodeForeachCallbackType = std::function<void(Describe::SceneNode& node)>;
class HierarchyParser
{
public:
    HierarchyParser()  = default;
    ~HierarchyParser() = default;

public:
    void foreachNode(Describe::SceneNode& parentNode, SceneNodeForeachCallbackType callback);
    void parse(Describe::SceneNode& parentNode, Describe::HierarchyNode& parentHierNode);
};
} // namespace Desc

class DescriptionParser
{
public:
    DescriptionParser()  = default;
    ~DescriptionParser() = default;

public:
    Describe::FileParser  fileParser;
    Describe::HierarchyParser hierParser;

public:
    void initialize();
};

} // namespace Voxol::Scene
#endif