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

struct SceneActionDesc
{
    std::string target;
    std::string type;
    std::string cmd;
    uint32_t    color = 0xff000000;
};


} // namespace Voxol::Scene::Layout
#endif