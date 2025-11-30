#ifndef VOXOL_SCENE_DESCRITION_NODE_H
#define VOXOL_SCENE_DESCRITION_NODE_H

#include "../../Base/BaseDefine.h"
#include "../EntityComponent.h"
#include "../../Intent/IntentComponent.h"
#include "../JsonValue.h"
#include "../../Math/Vec2.h"
#include "../../Math/MathDef.h"
#include <nlohmann/json.hpp>
#include <string>
#include <functional>

namespace Voxol::Scene::Describe
{
using JsonType = nlohmann::json;

enum class RefLayoutSrcWrapping : uint8_t
{
    Repeat,
    Clamp
};

struct DescreferenceLayoutNode
{
    static constexpr const char* TYPE_DEFAULT    = "default";
    static constexpr const char* TYPE_BASIC       = "basic";
    static constexpr const char* TYPE_ADVANCED   = "advanced";
    static constexpr const char* TYPE_PHYSICS    = "physics";
    static constexpr const char* TYPE_GENERATIVE = "generative";
    static constexpr const char* TYPE_REACTIVE   = "reactive";

    // basic, advanced, physics, generative, reactive
    std::string type = "default";
    std::string method = "grid";

    Math::Vec2  position;
    bool        isTypeDefault() const;
    bool        isTypeBasic() const;
    void        parse(const JsonType& jsonNode);
};

struct RefLayoutValue
{

    Scene::Data::JsonValue jsonV;
    DescreferenceLayoutNode layoutNode;

    void parse(const JsonType& jsonNode);

    void reset() {
        jsonV.reset();
    }
    float floatValue(const JsonType& jsonNode, const std::string& name)
    {
        return jsonV.parseFloatWithName(jsonNode, name);
    }
    float intValue(const JsonType& jsonNode, const std::string& name)
    {
        return jsonV.parseIntWithName(jsonNode, name);
    }

    Math::Vec2 position(const JsonType& jsonNode)
    {
        return jsonV.parseVec2WithName(jsonNode, "position");
    }
    Math::Vec2 spacing(const JsonType& jsonNode)
    {
        return jsonV.parseVec2WithName(jsonNode, "spacing");
    }
    Math::Vec2 staggered(const JsonType& jsonNode)
    {
        return jsonV.parseVec2WithName(jsonNode, "staggered");
    }

    int count(const JsonType& jsonNode)
    {
        return jsonV.parseIntWithName(jsonNode, "count");
    }
    int rings(const JsonType& jsonNode)
    {
        return jsonV.parseIntWithName(jsonNode, "rings");
    }
    int columns(const JsonType& jsonNode)
    {
        return jsonV.parseIntWithName(jsonNode, "columns");
    }
    float hexRadius(const JsonType& jsonNode)
    {
        return jsonV.parseIntWithName(jsonNode, "hex-radius");
    }
    float startRadius(const JsonType& jsonNode)
    {
        return jsonV.parseIntWithName(jsonNode, "start-radius");
    }
    float stepRadius(const JsonType& jsonNode)
    {
        return jsonV.parseFloatWithName(jsonNode, "step-radius");
    }

    float radius(const JsonType& jsonNode)
    {
        return jsonV.parseFloatWithName(jsonNode, "radius");
    }
    float radiusSpacing(const JsonType& jsonNode)
    {
        return jsonV.parseFloatWithName(jsonNode, "radius-spacing");
    }
    float scale(const JsonType& jsonNode)
    {
        return jsonV.parseFloatWithName(jsonNode, "scale");
    }
    float amplitude(const JsonType& jsonNode)
    {
        return jsonV.parseFloatWithName(jsonNode, "amplitude");
    }

    float startAngle(const JsonType& jsonNode, bool toRadian = true)
    {
        auto v = jsonV.parseFloatWithName(jsonNode, "start-angle");
        if (toRadian)
        {
            v = Math::degrees_to_radians(v);
        }
        return v;
    }
    float stepAngle(const JsonType& jsonNode, bool toRadian = true)
    {
        auto v = jsonV.parseFloatWithName(jsonNode, "step-angle");
        if (toRadian)
        {
            v = Math::degrees_to_radians(v);
        }
        return v;
    }
    float angle(const JsonType& jsonNode, bool toRadian = true)
    {
        auto v = jsonV.parseFloatWithName(jsonNode, "angle");
        if (toRadian)
        {
            v = Math::degrees_to_radians(v);
        }
        return v;
    }
    float rotation(const JsonType& jsonNode, bool toRadian = true)
    {
        auto v = jsonV.parseFloatWithName(jsonNode, "rotation");
        if (toRadian)
        {
            v = Math::degrees_to_radians(v);
        }
        return v;
    }
    float arcAngle(const JsonType& jsonNode, bool toRadian = true)
    {
        auto v = jsonV.parseFloatWithName(jsonNode, "arc-angle");
        if (toRadian)
        {
            v = Math::degrees_to_radians(v);
        }
        return v;
    }
};

struct RefLayoutValueRef
{
    RefLayoutValue  value;
    const JsonType& jsonNodeRef;
    RefLayoutValueRef(const JsonType& jn) :
        jsonNodeRef(jn) {}

    
    void parse() {
        value.parse(jsonNodeRef);
    }

    void reset()
    {
        value.reset();
    }
    float floatValue(const std::string& name)
    {
        return value.floatValue(jsonNodeRef, name);
    }
    float intValue(const std::string& name)
    {
        return value.intValue(jsonNodeRef, name);
    }
    Math::Vec2 position()
    {
        return value.position(jsonNodeRef);
    }
    Math::Vec2 spacing()
    {
        return value.spacing(jsonNodeRef);
    }
    Math::Vec2 staggered()
    {
        return value.staggered(jsonNodeRef);
    }

    int count()
    {
        return value.count(jsonNodeRef);
    }
    int rings()
    {
        return value.rings(jsonNodeRef);
    }
    int columns()
    {
        return value.columns(jsonNodeRef);
    }
    float hexRadius()
    {
        return value.hexRadius(jsonNodeRef);
    }
    float startRadius()
    {
        return value.startRadius(jsonNodeRef);
    }
    float stepRadius()
    {
        return value.stepRadius(jsonNodeRef);
    }

    float radius()
    {
        return value.radius(jsonNodeRef);
    }

    float radiusSpacing()
    {
        return value.radiusSpacing(jsonNodeRef);
    }
    float scale()
    {
        return value.scale(jsonNodeRef);
    }
    float amplitude()
    {
        return value.amplitude(jsonNodeRef);
    }

    float startAngle(bool toRadian = true)
    {
        return value.startAngle(jsonNodeRef, toRadian);
    }
    float stepAngle(bool toRadian = true)
    {
        return value.stepAngle(jsonNodeRef, toRadian);
    }
    float angle(bool toRadian = true)
    {
        return value.angle(jsonNodeRef, toRadian);
    }
    float rotation(bool toRadian = true)
    {
        return value.rotation(jsonNodeRef, toRadian);
    }
    float arcAngle(bool toRadian = true)
    {
        return value.arcAngle(jsonNodeRef, toRadian);
    }
};


struct DescNodeRferenceSrcItem
{
    std::string type;
    std::string src;
    std::string condition;
    // some values: composite, sequential, layered
    std::string mergePolicy = "composite";
    // some values: last-wins, first-wins, merge-deep
    std::string conflictResolution = "last-wins";
    bool        empty() const;
    void        parse(const JsonType& jsonNode);
};
struct DescNodeRference
{
    std::string                          type;
    std::vector<DescNodeRferenceSrcItem> srcList;
    RefLayoutSrcWrapping                 srcWrapping = RefLayoutSrcWrapping::Repeat;
    bool                                 isContainer() const;
    bool                                 empty() const;
    void                                 reset();
    void                                 parse(const JsonType& jsonNode);
};


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

} // namespace Voxol::Scene::Describe
#endif