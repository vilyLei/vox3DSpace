#ifndef VOXOL_SCENE_SCENE_IR_COMPONENT_H
#define VOXOL_SCENE_SCENE_IR_COMPONENT_H

#include "../Base/BaseDefine.h"
#include "EntityComponent.h"
#include "JsonValue.h"
#include <string>
#include <algorithm>
#include <variant>

namespace Voxol::Scene
{
using JsonType = nlohmann::json;
namespace SceneIR
{
namespace Shadering
{

struct EffectShadow
{
    uint32_t    id = Base::ID::INVALID_ID;
    std::string type;
    uint32_t    color      = 0xff000000;
    float       blurRadius = 0;
    Math::Vec2  offset;
    void        parse(const JsonType& node);
};

struct Description
{
    uint32_t              id = Base::ID::INVALID_ID;
    std::string           type;
    uint32_t              color = 0xff000000;
    std::vector<uint32_t> effects;
    void                  parseColor(const JsonType& node);
    void                  parse(const JsonType& node);
};

struct Unit
{
    uint32_t id = Base::ID::INVALID_ID;
    uint32_t description;
    uint32_t blendmode;
    void     parse(const JsonType& node);
};
struct Module
{
    std::unordered_map<uint32_t, EffectShadow> shadowsMap;
    std::unordered_map<uint32_t, Description>  descriptionsMap;
    std::unordered_map<uint32_t, Unit>         unitsMap;
    void                                       parse(const JsonType& node);
};

} // namespace Shadering
namespace Scene
{
struct ModelMethod
{
    uint32_t id = Base::ID::INVALID_ID;
    // Mesh / SDF / Procedural / Volume / Voxel
    std::string type;
    void        parse(const JsonType& node)
    {
        id   = node["id"];
        type = node["type"];
    }
};
struct Model
{
    uint32_t                        id = Base::ID::INVALID_ID;
    std::string                     type;
    std::string                     content;
    ModelMethod                     method;
    std::variant<float, Math::Vec2> value;
    void                            parse(const JsonType& node);
    template <typename T>
    T getValue()
    {
        return std::get<T>(value);
    }
    bool isText() const
    {
        return type == "Text";
    }
    bool hasFontSize() const
    {
        return std::holds_alternative<float>(value);
    }
    bool hasRadius() const
    {
        return std::holds_alternative<float>(value);
    }
    bool hasSize() const
    {
        return std::holds_alternative<Math::Vec2>(value);
    }
    float getRadius()
    {
        return hasRadius() ? getValue<float>() : 0;
    }
    float getFontSize()
    {
        return hasFontSize() ? getValue<float>() : 0;
    }
    Math::Vec2 getSize()
    {
        return hasSize() ? getValue<Math::Vec2>() : Math::Vec2{};
    }
};

struct Transform
{
    uint32_t   id = Base::ID::INVALID_ID;
    Math::Vec2 position;
    void       parse(const JsonType& node);
};

struct Hierarchy
{
    uint32_t id         = Base::ID::INVALID_ID;
    uint32_t parent     = Base::ID::INVALID_ID;
    uint32_t next       = Base::ID::INVALID_ID;
    uint32_t firstChild = Base::ID::INVALID_ID;

    void parse(const JsonType& node)
    {
        if (node.contains("id"))
            id = node["id"];
        if (node.contains("parent"))
            parent = node["parent"];
        if (node.contains("next"))
            next = node["next"];
        if (node.contains("firstChild"))
            firstChild = node["firstChild"];
    }
};
struct Entity
{
    uint32_t id        = Base::ID::INVALID_ID;
    uint32_t shadering = Base::ID::INVALID_ID;
    uint32_t transform = Base::ID::INVALID_ID;
    uint32_t model     = Base::ID::INVALID_ID;
    uint32_t hierarchy = Base::ID::INVALID_ID;
    uint32_t prototype = Base::ID::INVALID_ID;
    bool     visible   = true;

    void parse(const JsonType& node)
    {
        if (node.contains("id"))
            id = node["id"];

        if (node.contains("shadering"))
            shadering = node["shadering"];

        if (node.contains("transform"))
            transform = node["transform"];

        if (node.contains("model"))
            model = node["model"];

        if (node.contains("hierarchy"))
            hierarchy = node["hierarchy"];

        if (node.contains("prototype"))
        {
            if (node["prototype"].is_number())
            {
                prototype = node["prototype"];
            }
            else
            {
                // todo
            }
        }

        if (node.contains("visible"))
            visible = static_cast<uint32_t>(node["visible"]) != 0 ? true : false;
    }
};

struct Module
{
    std::unordered_map<uint32_t, Model>     modelsMap;
    std::unordered_map<uint32_t, Hierarchy> hierarchiesMap;
    std::unordered_map<uint32_t, Transform> transformsMap;
    std::unordered_map<uint32_t, Entity>    entitiesMap;

    void parseComponent(const JsonType& node)
    {
        if (node.contains("transforms"))
        {
            auto& transforms = node["transforms"];
            for (auto& node : transforms)
            {
                Transform m;
                m.parse(node);
                transformsMap[m.id] = m;
            }
        }
        if (node.contains("models"))
        {
            auto&& models = node["models"];
            for (auto& node : models)
            {
                Model m;
                m.parse(node);
                modelsMap[m.id] = m;
            }
        }
    }
    void parse(const JsonType& node)
    {
        if (node.contains("component"))
        {
            parseComponent(node["component"]);
        }
        if (node.contains("hierarchies"))
        {
            auto&& hierarchies = node["hierarchies"];
            for (auto& node : hierarchies)
            {
                Hierarchy m;
                m.parse(node);
                hierarchiesMap[m.id] = m;
            }
        }
        if (node.contains("entities"))
        {
            auto& entities = node["entities"];
            for (auto& node : entities)
            {
                Entity m;
                m.parse(node);
                entitiesMap[m.id] = m;
            }
        }
    }
};
} // namespace Scene
} // namespace SceneIR
} // namespace Voxol::Scene
#endif