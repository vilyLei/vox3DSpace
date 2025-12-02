#ifndef VOXOL_SCENE_ENTITY_COMPONENT_H
#define VOXOL_SCENE_ENTITY_COMPONENT_H

#include "../Math/VxRect.h"
#include "../Base/IDDef.h"
#include "../Render/OglGpuResUtils.h"


namespace Voxol::Scene
{
namespace Component
{
struct UnitTransform
{
    float x;
    float y;
    /// x-axis scale
    float sx;
    /// y-axis scale
    float sy;
    ///  rotation radian
    float rotation;

    inline Math::Vec2&       pos() { return *reinterpret_cast<Math::Vec2*>(&x); }
    inline Math::Vec2&       scale() { return *reinterpret_cast<Math::Vec2*>(&sx); }
    inline const Math::Vec2& pos() const { return *reinterpret_cast<const Math::Vec2*>(&x); }
    inline const Math::Vec2& scale() const { return *reinterpret_cast<const Math::Vec2*>(&sx); }

    constexpr UnitTransform() noexcept :
        x(0), y(0), sx(1), sy(1), rotation{0} {}
    constexpr UnitTransform(float x_, float y_, float sx_, float sy_, float rotation_ = 0) :
        x(x_), y(y_), sx(sx_), sy(sy_), rotation(rotation_) {}
};
constexpr UnitTransform defaultTrans{0, 0, 1, 1, 0};
constexpr Math::VxRect  defaultRect{0, 0, 1, 1};


struct UnitEffectShadowDesc
{
    uint32_t   color = 0xff000000;
    Math::Vec2 offset;
    float      blurRadius = 0;
};

struct UnitShadowEntity
{
    Base::ID::KeyUint64 shadowId = Base::ID::INVALID_KEY;
    Base::ID::KeyUint64 entityId = Base::ID::INVALID_KEY;
    uint32_t            effectId = Base::ID::INVALID_ID;
};



struct UnitTexture
{
    GLuint texture = GL_ZERO;
    std::string name;
};

struct UnitIdName
{
    uint32_t    id = Base::ID::INVALID_ID;
    std::string name;
};
struct UnitLocation
{
    Math::Vec2 local;
    Math::Vec2 global;
};

struct UnitShadingDesc
{
    uint32_t color = 0xff000000;
    uint32_t flags = 0x0;
};

struct UnitShadingEntity
{
    uint32_t id            = Base::ID::INVALID_ID;
    uint32_t shadingDescId = Base::ID::INVALID_ID;
};

enum class UnitModelType : uint32_t
{
    Default = 0,
    Mesh    = 0,
    Text    = 1 << 1
};


struct UnitTextDesc
{
    float       fontSize = 12;
    uint32_t    color    = 0xff000000;
    std::string text;
    bool        getColorAt(int i)
    {
        return color;
    }
};
struct UnitTextModel
{
    uint32_t id = Base::ID::INVALID_ID;
    UnitTextDesc text;
    Math::Vec2   posOffset;
    Math::Bounds bounds;
};

struct UnitModel
{
    uint32_t      id         = Base::ID::INVALID_ID;
    uint32_t      drawUnitId = Base::ID::INVALID_ID;
    UnitModelType type       = UnitModelType::Mesh;

    constexpr bool isText(){
        return type == UnitModelType::Text;
    };
    constexpr bool isMesh()
    {
        return type == UnitModelType::Mesh;
    };
};

struct UnitHierarchy
{
    uint32_t parent     = Base::ID::INVALID_ID;
    uint32_t next       = Base::ID::INVALID_ID;
    uint32_t firstChild = Base::ID::INVALID_ID;
};
struct UnitEntity
{
    uint32_t id = Base::ID::INVALID_ID;
    /// UnitShadingEntity vector index value
    uint32_t shadingId   = Base::ID::INVALID_ID;
    uint32_t transformId = Base::ID::INVALID_ID;
    uint32_t modelId     = Base::ID::INVALID_ID;
    uint32_t hierarchyId = Base::ID::INVALID_ID;
    uint32_t prototypeId = Base::ID::INVALID_ID;

    bool visible       = true;
    bool globalVisible = true; // derived visibility
    bool dirty   = true;
};



struct InsNodeSlot
{
    uint32_t    id;
    Math::Mat33 mat;
    bool        active;
};

struct FlatInsStorage
{
    std::vector<InsNodeSlot>               nodes;
    std::unordered_map<uint32_t, uint32_t> idToIndex;
    std::vector<uint32_t>                  freeList;

    InsNodeSlot& emplace(uint32_t id, const Math::Mat33& mat)
    {
        if (!freeList.empty())
        {
            auto idx = freeList.back();
            freeList.pop_back();
            nodes[idx]    = {id, mat, true};
            idToIndex[id] = idx;
            return nodes[idx];
        }
        uint32_t idx = nodes.size();
        nodes.push_back({id, mat, true});
        idToIndex[id] = idx;
        return nodes.back();
    }

    void erase(uint32_t id)
    {
        auto it = idToIndex.find(id);
        if (it != idToIndex.end())
        {
            uint32_t idx      = it->second;
            nodes[idx].active = false;
            freeList.push_back(idx);
            idToIndex.erase(it);
        }
    }

    InsNodeSlot* find(uint32_t id)
    {
        auto it = idToIndex.find(id);
        return it != idToIndex.end() ? &nodes[it->second] : nullptr;
    }
};


struct UnitInstanceMap
{
    uint32_t                                              iid     = Base::ID::INVALID_ID;
    uint32_t                                              protoId = Base::ID::INVALID_ID;
    Base::ID::keyUint64Unordered_map<Base::ID::KeyUint64> map;
    bool                                                  dirty = true;
};
void updateUunitModel(UnitModel& model, const std::string typeName);

} // namespace Component

} // namespace Voxol::Scene
#endif