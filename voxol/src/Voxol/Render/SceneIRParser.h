#ifndef VOXOL_RENDER_SCENE_IR_PARSER_H
#define VOXOL_RENDER_SCENE_IR_PARSER_H

#include "../Base/BaseDefine.h"
#include "EntityComponent.h"
#include "../Math/Vec2.h"
#include <nlohmann/json.hpp>
#include <string>
#include <algorithm>
#include <variant>

namespace Voxol::Render
{
using JsonType = nlohmann::json;
namespace SceneIR
{
namespace Shadering
{

struct EffectShadow
{
    uint32_t    id = ID::INVALID_ID;
    std::string type;
    uint32_t    color      = 0xff000000;
    float       blurRadius = 0;
    Math::Vec2  offset;
    void        parse(const JsonType& node);
};

struct Description
{
    uint32_t    id = ID::INVALID_ID;
    std::string type;
    uint32_t    color = 0xff000000;
    void        parse(const JsonType& node);
};

struct Unit
{
    uint32_t id = ID::INVALID_ID;
    uint32_t description;
    uint32_t blendmode;
    void     parse(const JsonType& node)
    {
        id          = node["id"];
        description = node["description"];

        std::string blendmodeStr = node["blendmode"];

        static std::unordered_map<std::string, uint32_t> blendmodeMap{};

        if (blendmodeMap.empty())
        {
            blendmodeMap["None"]        = 0;
            blendmodeMap["Transparent"] = 1;
            blendmodeMap["Add"]         = 2;
        }

        blendmode = 0;
        if (blendmodeMap.contains(blendmodeStr))
        {
            blendmode = blendmodeMap[blendmodeStr];
        }
    }
};
struct Module
{
    std::unordered_map<uint32_t, EffectShadow> shadowsMap;
    std::unordered_map<uint32_t, Description> descriptionsMap;
    std::unordered_map<uint32_t, Unit>        unitsMap;
    void                                      parse(const JsonType& node);
};

} // namespace Shadering
namespace Scene
{
struct ModelMethod
{
    uint32_t id = ID::INVALID_ID;
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
    uint32_t                        id = ID::INVALID_ID;
    std::string                     type;
    ModelMethod                     method;
    std::variant<float, Math::Vec2> value;
    void                            parse(const JsonType& node);
    template <typename T>
    T getValue()
    {
        return std::get<T>(value);
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
        if (hasRadius())
        {
            return hasRadius() ? getValue<float>() : 0;
        }
        return 0;
    }
    Math::Vec2 getSize()
    {
        return hasSize() ? getValue<Math::Vec2>() : Math::Vec2{};
    }
};

struct Transform
{
    uint32_t   id = ID::INVALID_ID;
    Math::Vec2 position;
    void       parse(const JsonType& node);
};

struct Hierarchy
{
    uint32_t id         = ID::INVALID_ID;
    uint32_t parent     = ID::INVALID_ID;
    uint32_t next       = ID::INVALID_ID;
    uint32_t firstChild = ID::INVALID_ID;

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
    uint32_t id        = ID::INVALID_ID;
    uint32_t shadering = ID::INVALID_ID;
    uint32_t transform = ID::INVALID_ID;
    uint32_t model     = ID::INVALID_ID;
    uint32_t hierarchy = ID::INVALID_ID;
    uint32_t prototype = ID::INVALID_ID;
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


class SceneIRParser
{
public:
    SceneIRParser()  = default;
    ~SceneIRParser() = default;

public:
    SceneIR::Shadering::Module shaderingModule;
    SceneIR::Scene::Module     sceneModule;

public:
    void parseFromFile(const std::string& fileName);
    void parse(const nlohmann::json& json);
};

} // namespace Voxol::Render
#endif