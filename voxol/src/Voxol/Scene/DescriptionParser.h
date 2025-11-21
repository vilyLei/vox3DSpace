#ifndef VOXOL_SCENE_DESCRIPTION_PARSER_H
#define VOXOL_SCENE_DESCRIPTION_PARSER_H

#include "../Base/BaseDefine.h"
#include "EntityComponent.h"
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
    //Component::UnitStringModel   textModel;

    int childrenTotal = 0;

    bool hasChild  = false;
    bool hasParent = true;

    //void parseTrans(const JsonType& jsonNode);
    //void parse(const JsonType& jsonNode);
    void print() const;
    void printTransform() const;
};

class FileParser
{
public:
    FileParser()  = default;
    ~FileParser() = default;

public:
    SceneNode rootNode;
    std::unordered_map<uint32_t, Component::UnitStringModel> textModelMap;

public:
    void initFromFile(const std::string& fileName);
    void parseHeriNodes(const JsonType& jsonNode);
    void parseSceneNode(SceneNode& parentNode, uint32_t& id, const JsonType& jsonNode);

    void parseNodeTransData(SceneNode& node, const JsonType& jsonNode);
    void parseNodeData(SceneNode& node, const JsonType& jsonNode);
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