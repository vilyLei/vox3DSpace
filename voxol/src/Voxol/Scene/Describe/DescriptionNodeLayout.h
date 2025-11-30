#ifndef VOXOL_SCENE_DESCRITION_NODE_LAYOUT_H
#define VOXOL_SCENE_DESCRITION_NODE_LAYOUT_H

#include "DescriptionNode.h"

namespace Voxol::Scene::Describe
{


using DescNodeLayoutCallbackType = std::function<void(int index, const Math::Vec2& pos, const Math::Vec2& scale, float rotation)>;
class DescriptionNodeLauout
{
public:
    static void referenceLayoutSceneNodeWithHexagonalGrid(const JsonType& jsonNode, const Math::Vec2& nodeSize, const DescNodeLayoutCallbackType& callback);
    static void referenceLayoutSceneNodeWithSpiral(const JsonType& jsonNode, const Math::Vec2& nodeSize, const DescNodeLayoutCallbackType& callback);
    static void referenceLayoutSceneNodeWithArc(const JsonType& jsonNode, const Math::Vec2& nodeSize, const DescNodeLayoutCallbackType& callback);
    static void referenceLayoutSceneNodeWithCircle(const JsonType& jsonNode, const Math::Vec2& nodeSize, const DescNodeLayoutCallbackType& callback);
    static void referenceLayoutSceneNodeWithGrid(const JsonType& jsonNode, const Math::Vec2& nodeSize, const DescNodeLayoutCallbackType& callback);
    static void referenceLayoutSceneNodeOnce(const JsonType& refLayoutNode, const Math::Vec2& nodeSize, const DescNodeLayoutCallbackType& callback);
    static void referenceLayoutSceneNodeMany(const JsonType& jsonNode, const Math::Vec2& nodeSize, const DescNodeLayoutCallbackType& callback);
    static void referenceLayoutSceneNode(const JsonType& jsonNode, const Math::Vec2& nodeSize, const DescNodeLayoutCallbackType& callback);

private:
    static std::vector<float> angles;
    static std::vector<Math::Vec2> positions;
};

} // namespace Voxol::Scene::Layout
#endif