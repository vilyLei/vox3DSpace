#ifndef VOXOL_SCENE_DESCRITION_NODE_H
#define VOXOL_SCENE_DESCRITION_NODE_H

#include "../../Base/BaseDefine.h"
#include "../EntityComponent.h"
#include "../../Intent/IntentComponent.h"
#include "../JsonValue.h"
#include "../../Math/Vec2.h"
#include <nlohmann/json.hpp>
#include <string>
#include <functional>

namespace Voxol::Scene::Describe
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

struct DescreferenceLayoutNode
{
    static constexpr const char* TYPE_DEFAULT    = "default";
    static constexpr const char* TYPE_BASE       = "basic";
    static constexpr const char* TYPE_ADVANCED   = "advanced";
    static constexpr const char* TYPE_PHYSICS    = "physics";
    static constexpr const char* TYPE_GENERATIVE = "generative";
    static constexpr const char* TYPE_REACTIVE   = "reactive";

    std::string     type = "default";
    // basic, advanced, physics, generative, reactive
    std::string     method = "grid";
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

} // namespace Voxol::Scene::Layout
#endif