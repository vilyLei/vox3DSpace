#ifndef VOXOL_SCENE_DESCRIPTION_PARSER_H
#define VOXOL_SCENE_DESCRIPTION_PARSER_H

#include "../Base/BaseDefine.h"
#include "EntityComponent.h"
#include "../Math/Vec2.h"
#include <nlohmann/json.hpp>
#include <string>

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
    uint32_t               index = 0;
    std::vector<SceneNode> children;

    Component::UnitHierarchy       hieraychy;
    Component::UnitTransform       trans;
    Component::UnitShadingDesc     shaingDesc;
    Component::UnitShadingEntity   shadingEntity;
    Component::UnitModel           unitModel;
    Component::UnitEntity          entity;
    Component::UnitStringModel     textModel;

    int childrenTotal = 0;

    bool hasChild  = false;
    bool hasParent = true;

    void parseTrans(const JsonType& jsonNode);
    void parse(const JsonType& jsonNode);
        /*
    void parseTrans(const JsonType& jsonNode)
    {
        if (jsonNode.contains("transform"))
        {
            SceneIR::Scene::Transform jTrans;
            jTrans.parse(jsonNode["transform"]);
            auto pv     = jTrans.position;
            trans.pos() = jTrans.position;
        }
        if (jsonNode.contains("display"))
        {
            auto&& displayNode    = jsonNode["display"];
            shadingEntity.id      = index;
            unitModel.id          = index;
            std::string shapeType = "";
            if (displayNode.contains("shape") && displayNode["shape"].is_string())
            {
                shapeType = displayNode["shape"];
            }
            if (shapeType == "rectangle" || shapeType == "round-rectangle")
            {
                shadingEntity.shadingDescId = index;
                unitModel.drawUnitId        = 0;
                unitModel.type              = Component::UnitModelType::Mesh;
            }
            else if (shapeType == "text" || shapeType == "Text")
            {
                unitModel.drawUnitId = 0;
                unitModel.type       = Component::UnitModelType::Text;
            }

            SceneIR::Shadering::Description jDesc;
            jDesc.parse(displayNode);

            shaingDesc.color = jDesc.color;

            SceneIR::Scene::Model jModel;
            jModel.parse(displayNode);
            if (jModel.hasRadius())
            {
                auto pw       = jModel.getRadius() * 2;
                trans.scale() = {pw, pw};
            }
            else if (jModel.hasSize())
            {
                auto&& size   = jModel.getSize();
                trans.scale() = size;
            }

            if ((jModel.type == "text" || jModel.type == "Text") && !jModel.content.empty())
            {
                textModel = {index,
                             jModel.getFontSize(),
                             jModel.content};

                trans.scale() = {textModel.fontSize, textModel.fontSize};
            }
        }
    }
    void parse(const JsonType& jsonNode)
    {
        if (jsonNode.contains("type"))
        {
            type = jsonNode["type"];
        }
        if (jsonNode.contains("name"))
        {
            name = jsonNode["name"];
        }
        parseTrans(jsonNode);
        entity.id        = index;
        entity.shadingId = index;
        entity.transformId = index;
        entity.hierarchyId = index;
        entity.modelId = index;

        if (jsonNode.contains("children") && jsonNode["children"].is_array())
        {
            auto&& elements = jsonNode["children"];
            hasChild        = !elements.empty();
        }
    }
    //*/
    void print() const;
    //{
    //    std::string info = ", hasChild=" + (hasChild ? std::string("true") : std::string("false"));
    //    info += ", childrenTotal=" + std::to_string(childrenTotal) + ", index=" + std::to_string(index);
    //    std::string info = "Node(name=" + name + ",type=" + type + info + ")";
    //    printf("%s\n", info.c_str());
    //}
    void printTrans() const;
    //{
    //    std::string info = ", tran(x=" + std::to_string(trans.x) + ",y=" + std::to_string(trans.y);
    //    info += ",sx=" + std::to_string(trans.sx) + ",sy=" + std::to_string(trans.sy) + ")";
    //    std::string info = "Node(name=" + name + info + ")";
    //    printf("%s\n", info.c_str());
    //}
};

class FileParser
{
public:
    FileParser()  = default;
    ~FileParser() = default;

public:
    SceneNode rootNode;

public:
    void initFromFile(const std::string& fileName);
    void parseHeriNodes(const JsonType& jsonNode);
    void parseSceneNode(SceneNode& parentNode, const JsonType& jsonNode);
};
} // namespace Desc

class DescriptionParser
{
public:
    DescriptionParser()  = default;
    ~DescriptionParser() = default;

public:
    Desc::FileParser fileParser;

public:
    void initialize();
};

} // namespace Voxol::Scene
#endif