#ifndef VOXOL_RENDER_SCENE_IR_PARSER_H
#define VOXOL_RENDER_SCENE_IR_PARSER_H

#include "../Base/BaseDefine.h"
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
struct Model
{
    int         id;
    std::string type;
    std::string method;

    void parse(const JsonType& node)
    {
        id     = node["id"];
        type   = node["type"];
        method = node["method"];
    }
    bool isCircle() const
    {
        return type == "Circle";
    }
    bool isRectangle() const
    {
        return type == "Rectangle";
    }
};

struct Description
{
    int         id;
    std::string type;
    uint32_t    color;
    void        parse(const JsonType& node);
};

struct UnitModel
{
    int                             id;
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

struct Unit
{
    int       id;
    int       description;
    UnitModel model;
    void      parse(const JsonType& node)
    {
        id          = node["id"];
        description = node["description"];

        if (node.contains("model"))
        {
            model.parse(node["model"]);
        }
    }
};
struct Module
{
    std::unordered_map<int, Model>       modelsMap;
    std::unordered_map<int, Description> descriptionsMap;
    std::unordered_map<int, Unit>        unitsMap;

    void parse(const JsonType& node)
    {
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
        if (node.contains("descriptions"))
        {
            auto&& descriptions = node["descriptions"];
            for (auto& node : descriptions)
            {
                Description m;
                m.parse(node);
                descriptionsMap[m.id] = m;
            }
        }

        if (node.contains("units"))
        {
            auto& units = node["units"];
            for (auto& node : units)
            {
                Unit m;
                m.parse(node);
                unitsMap[m.id] = m;
            }
        }
    }
};

} // namespace Shadering
namespace Scene
{

struct Transform
{
    int        id;
    Math::Vec2 position;

    void parse(const JsonType& node);
};

struct Entity
{
    int  id;
    int  shadering;
    int  transform;
    bool visible;

    void parse(const JsonType& node)
    {
        id        = node["id"];
        shadering = node["shadering"];
        transform = node["transform"];
        visible   = static_cast<int>(node["visible"]) != 0 ? true : false;
    }
};

struct Module
{
    std::unordered_map<int, Transform> transformsMap;
    std::unordered_map<int, Entity>    entitiesMap;

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
    }
    void parse(const JsonType& node)
    {
        if (node.contains("component"))
        {
            parseComponent(node["component"]);
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