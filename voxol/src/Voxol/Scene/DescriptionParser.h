#ifndef VOXOL_SCENE_DESCRIPTION_PARSER_H
#define VOXOL_SCENE_DESCRIPTION_PARSER_H

#include "../Base/BaseDefine.h"
#include "EntityComponent.h"
#include "Intent/IntentComponent.h"
#include "JsonValue.h"
#include "../Math/Vec2.h"
#include <nlohmann/json.hpp>
#include <string>
#include <functional>

namespace Voxol::Scene
{
namespace Desc
{
using JsonType = nlohmann::json;


struct HierarchyNode
{
    uint32_t                 id = Base::ID::INVALID_ID;
    std::string              name;
    Component::UnitHierarchy hieraychy;
    void                     print()
    {
        std::string info = ", parent=" + std::to_string(hieraychy.parent);
        info += ", firstChild=" + std::to_string(hieraychy.firstChild);
        info += ", next=" + std::to_string(hieraychy.next);
        info += ", id=" + std::to_string(id);
        info = "HierarchyNode(name=" + name + info + ")";
        printf("%s\n", info.c_str());
    }
};
struct DisplayShape
{
    std::string     type;
    Math::Vec2      size;
    Data::JsonValue jsonValue;
    void            parse(const JsonType& jsonNode);
};

struct SceneActionDesc
{
    std::string target;
    std::string type;
    std::string cmd;
    uint32_t    color = 0xff000000;
};

struct SceneActionTargetNode
{
    std::string                  srcActType;
    std::vector<SceneActionDesc> actions;
    void                         reset();
    void                         parse(const JsonType& jsonNode, const std::string& srcActType_);
};

struct SceneNode
{
    std::string            type;
    std::string            name;
    uint32_t               id = Base::ID::INVALID_ID;
    std::vector<SceneNode> children;

    Component::UnitHierarchy     hieraychy;
    Component::UnitTransform     transform;
    Component::UnitShadingDesc   shaingDesc;
    Component::UnitShadingEntity shadingEntity;
    Component::UnitModel         unitModel;
    Component::UnitEntity        entity;

    int childrenTotal = 0;

    bool hasChild  = false;
    bool hasParent = true;

    void print() const;
    void printTransform() const;
};

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

using SceneNodeForeachCallbackType = std::function<void(Desc::SceneNode& node)>;
class HierarchyParser
{
public:
    HierarchyParser()  = default;
    ~HierarchyParser() = default;

public:
    void foreachNode(Desc::SceneNode& parentNode, SceneNodeForeachCallbackType callback);
    void parse(Desc::SceneNode& parentNode, Desc::HierarchyNode& parentHierNode);
};
} // namespace Desc

class DescriptionParser
{
public:
    DescriptionParser()  = default;
    ~DescriptionParser() = default;

public:
    Desc::FileParser      fileParser;
    Desc::HierarchyParser hierParser;

public:
    void initialize();
};

} // namespace Voxol::Scene
#endif