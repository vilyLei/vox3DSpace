#include "EntityBVH.h"

namespace Voxol::Render
{
namespace BVH
{
int buildBVH(std::vector<BVHNode>& nodes,
             std::vector<int>&     indices,
             int                   begin,
             int                   end)
{
    BVHNode node{};
    node.begin = begin;
    node.end   = end;

    return 0;
    /*
    // 计算当前节点包围盒
    Math::VxRect bbox = boundsOf(indices, begin, end);
    node.bounds       = bbox;

    if (end - begin <= 4)
    { // 叶子条件
        int idx = nodes.size();
        nodes.push_back(node);
        return idx;
    }

    // 按最长轴分割
    int   axis = bbox.longestAxis();
    float mid  = bbox.centerAlong(axis);

    // 根据中心点划分索引
    auto midIter = std::partition(indices.begin() + begin, indices.begin() + end,
                                  [&](int i) { return boundsVec[i].bounds.center(axis) < mid; });

    int midIdx    = int(midIter - indices.begin());
    int nodeIndex = nodes.size();
    nodes.push_back(node);

    int left  = buildBVH(nodes, indices, begin, midIdx);
    int right = buildBVH(nodes, indices, midIdx, end);

    nodes[nodeIndex].left  = left;
    nodes[nodeIndex].right = right;

    return nodeIndex;
    //*/
}
void queryBVH(const std::vector<BVHNode>& nodes,
              int                         nodeIndex,
              const Math::VxRect&         queryBox,
              std::vector<int>&           outIndices)
{
    const auto& node = nodes[nodeIndex];
    if (!queryBox.intersects(node.bounds))
        return;

    if (node.isLeaf())
    {
        for (int i = node.begin; i < node.end; ++i)
            outIndices.push_back(i);
        return;
    }

    queryBVH(nodes, node.left, queryBox, outIndices);
    queryBVH(nodes, node.right, queryBox, outIndices);
}
} // namespace BVH

} // namespace Voxol::Render