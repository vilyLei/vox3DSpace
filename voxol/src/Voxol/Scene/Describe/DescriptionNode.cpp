
#include "DescriptionNode.h"

namespace Voxol::Scene::Describe
{

    
//"reference": {
//    "srcTT": "rect-01-label",
//    "src-list": [
//        "rect-01-label",
//        "circle-01-label",
//        "circle-02-label"
//    ],
//    "src-wrapping": "repeat",
//    "type": "container"
//},

bool DescNodeRferenceSrcItem::empty() const {

    return src.empty();
}

void DescNodeRferenceSrcItem::parse(const JsonType& jsonNode)
{
    if (!jsonNode.is_object())
        return;

    Data::JsonValue jsonV;
    type = jsonV.parseStringWithName(jsonNode, "type");
    src  = jsonV.parseStringWithName(jsonNode, "src");
    condition = jsonV.parseStringWithName(jsonNode, "condition");
    mergePolicy = jsonV.parseStringWithName(jsonNode, "merge-policy", "composite");
    conflictResolution = jsonV.parseStringWithName(jsonNode, "conflict-resolution", "last-wins");

}
bool DescNodeRference::empty() const {
    return srcList.empty();
}
void DescNodeRference::reset() {

    srcWrapping = RefLayoutSrcWrapping::Repeat;
    srcList.clear();
}
void DescNodeRference::parse(const JsonType& jsonNode)
{
    Data::JsonValue jsonV;

    type = jsonV.parseStringWithName(jsonNode, "type");
    auto srcWrappingStr = jsonV.parseStringWithName(jsonNode, "src-wrapping");
    if (srcWrappingStr == "clamp")
    {
        srcWrapping = RefLayoutSrcWrapping::Clamp;
    }

    std::string srcNodeName = (jsonNode.contains("src") && jsonNode["src"].is_string()) ? jsonNode["src"] : "";
    if (!srcNodeName.empty())
    {
        srcList.emplace_back("", srcNodeName);
    }
    std::string srcListKey = "src-list";
    if (jsonNode.contains(srcListKey) && jsonNode[srcListKey].is_array())
    {
        auto&& elements = jsonNode[srcListKey];
        if (elements.empty())
            return;

        for (auto& item : elements)
        {

            if (item.is_string())
            {
                std::string srcNodeName = item;
                if (srcNodeName.empty())
                    continue;
                srcList.emplace_back("", srcNodeName);
                continue;
            }

            DescNodeRferenceSrcItem srcitem;
            srcitem.parse(item);
            if (srcitem.empty())
                continue;
            srcList.emplace_back(srcitem);
        }
    }
}
void DisplayShape::parse(const JsonType& jsonNode)
{

    if (jsonNode.contains("type"))
    {
        type = jsonNode["type"];
    }
    else if (jsonNode.contains("shape"))
    {
        type = jsonNode["shape"];
    }
    else if (jsonNode.contains("shapeType"))
    {
        type = jsonNode["shapeType"];
    }
    if (!type.empty())
    {
        std::transform(type.begin(), type.end(), type.begin(),
                       [](unsigned char c) { return std::tolower(c); });
    }

    if (type == "rectangle" || type == "round-rectangle")
    {
        jsonValue.parseWithName(jsonNode, "size");
        if (jsonValue.is<Math::Vec2>())
        {
            size = jsonValue.get<Math::Vec2>();
        }
    }
    else if (type == "circle")
    {
        jsonValue.parseWithName(jsonNode, "radius");
        float radius = 100.0f;
        if (jsonValue.is<float>())
        {
            radius = jsonValue.get<float>() * 2;
            size   = {2 * radius, 2 * radius};
        }
        else if (jsonValue.is<int>())
        {
            radius = jsonValue.get<int>() * 2;
            size   = {2 * radius, 2 * radius};
        }
    }
    else if (type == "text")
    {
        jsonValue.parseTextWithName(jsonNode, "content");
        if (jsonValue.is<Component::UnitTextDesc>())
        {
            auto&& textDesc = jsonValue.get<Component::UnitTextDesc>();
            size            = {textDesc.fontSize, textDesc.fontSize};
        }
    }
}

void DescreferenceLayoutNode::parse(const JsonType& jsonNode)
{
    if (jsonNode.contains("type") && jsonNode["type"].is_string())
    {
        type = jsonNode["type"];
    }
    if(jsonNode.contains("method") && jsonNode["method"].is_string())
    {
        method = jsonNode["method"];
    }
}

void SceneActionTargetNode::reset()
{
    srcActType = "";
    actions.clear();
}

void SceneActionTargetNode::parse(const JsonType& jsonNode, const std::string& srcActType_)
{
    if (!jsonNode.contains(srcActType_))
    {
        return;
    }
    srcActType      = srcActType_;
    auto&& elements = jsonNode[srcActType_];
    if (elements.empty())
    {
        return;
    }

    for (auto& item : elements)
    {
        auto& jNode = item;

        SceneActionDesc actDesc;
        actDesc.target = jNode["target"];

        auto&& actJNode = jNode["action"];
        if (actJNode.contains("type"))
        {
            actDesc.type = actJNode["type"];
        }

        Data::ColorValue cv;
        if (cv.parse(actJNode))
        {
            actDesc.color = cv.color.argb();
        }
        if (actJNode.contains("cmd"))
        {
            actDesc.cmd = actJNode["cmd"];
        }
        else
        {
            actDesc.cmd = "None";
        }
        printf("actDesc.target: %s, srcActType: %s\n", actDesc.target.c_str(), srcActType.c_str());
        actions.emplace_back(actDesc);
    }
}


void SceneNode::print() const
{
    std::string info = ", hasChild=" + (hasChild ? std::string("true") : std::string("false"));
    info += ", childrenTotal=" + std::to_string(childrenTotal) + ", id=" + std::to_string(id);
    info = "SceneNode(name=" + name + ",type=" + type + info + ")";
    printf("%s\n", info.c_str());
}
void SceneNode::printTransform() const
{
    std::string info = ", tran(x=" + std::to_string(transform.x) + ",y=" + std::to_string(transform.y);
    info += ",sx=" + std::to_string(transform.sx) + ",sy=" + std::to_string(transform.sy) + ")";
    info = "NodeTrans(name=" + name + info + ")";
    printf("%s\n", info.c_str());
}


} // namespace Voxol::Scene::Describe