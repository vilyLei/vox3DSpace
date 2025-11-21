#ifndef VOXOL_SCENE_DESC_JSON_VALUE_H
#define VOXOL_SCENE_DESC_JSON_VALUE_H

#include <variant>
#include <nlohmann/json.hpp>
#include "../Math/Vec2.h"
#include "../Math/Vec3.h"
#include "../Math/Vec4.h"

namespace Voxol::Scene
{
namespace Data
{
using JsonType = nlohmann::json;

struct ColorValue
{
    uint32_t color = 0x0;
    bool     parseStringColor(const JsonType& node);
    bool     parseStringColorWithName(const JsonType& node, const std::string& valueName);

};

struct JsonValue
{
    enum class Type : uint8_t
    {
        Null,
        Bool,
        Int,
        Uint32,
        Float,
        String,
        Vec2,
        Vec3,
        Vec4,
        Array,
        Object
    };

    using ArrayType  = std::vector<JsonValue>;
    using ObjectType = std::unordered_map<std::string, JsonValue>;

    using Variant = std::variant<
        std::monostate, // Null
        bool,
        int,
        uint32_t,
        float,
        std::string,
        Math::Vec2,
        Math::Vec3,
        Math::Vec4,
        ArrayType,
        ObjectType>;

    Type    type = Type::Null;
    Variant value;

    JsonValue() = default;

    JsonValue(float v) :
        type(Type::Float), value(v) {}
    JsonValue(int v) :
        type(Type::Int), value(v) {}
    JsonValue(uint32_t v) :
        type(Type::Uint32), value(v) {}
    JsonValue(bool v) :
        type(Type::Bool), value(v) {}
    JsonValue(const std::string& s) :
        type(Type::String), value(s) {}
    JsonValue(const char* s) :
        type(Type::String), value(std::string(s)) {}

    JsonValue(const Math::Vec2& v2) :
        type(Type::Vec2), value(v2) {}
    JsonValue(const Math::Vec3& v3) :
        type(Type::Vec3), value(v3) {}
    JsonValue(const Math::Vec4& v4) :
        type(Type::Vec4), value(v4) {}

    JsonValue(const ArrayType& arr) :
        type(Type::Array), value(arr) {}
    JsonValue(const ObjectType& obj) :
        type(Type::Object), value(obj) {}

    bool parseStringColorWithName(const JsonType& node, const std::string& valueName);
    bool parseWithName(const JsonType& node, const std::string& valueName);

    void parse(const JsonType& j);

    template <typename T>
    bool is() const { return std::holds_alternative<T>(value); }

    template <typename T>
    T& get() { return std::get<T>(value); }

    template <typename T>
    const T& get() const { return std::get<T>(value); }
};
} // namespace Desc
} // namespace Voxol::Scene
#endif