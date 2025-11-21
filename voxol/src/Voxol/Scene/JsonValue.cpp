#include "JsonValue.h"

namespace Voxol::Scene
{
namespace Desc
{

bool JsonValue::parseWithName(const JsonType& node, const std::string& valueName)
{
    if (!node.contains(valueName))
        return false;

    parse(node[valueName]);
    return true;
}
void JsonValue::parse(const JsonType& j)
{
    if (j.is_null())
    {
        type  = Type::Null;
        value = std::monostate{};
    }
    else if (j.is_boolean())
    {
        type  = Type::Bool;
        value = static_cast<bool>(j);
    }
    else if (j.is_number_float())
    {
        type  = Type::Float;
        value = static_cast<float>(j);
    }
    else if (j.is_number_integer())
    {
        type  = Type::Int;
        value = static_cast<int>(j);
    }
    else if (j.is_number_unsigned())
    {
        type  = Type::Uint32;
        value = static_cast<uint32_t>(j);
    }
    else if (j.is_string())
    {
        type  = Type::String;
        value = j.get<std::string>();
    }
    else if (j.is_array())
    {
        // vec2/vec3/vec4 auto-detect
        if (j.size() == 2 && j[0].is_number() && j[1].is_number())
        {
            type  = Type::Vec2;
            value = Math::Vec2{(float)j[0], (float)j[1]};
            return;
        }
        if (j.size() == 3)
        {
            bool allNum = true;
            for (auto& e : j)
                if (!e.is_number()) allNum = false;
            if (allNum)
            {
                type  = Type::Vec3;
                value = Math::Vec3{(float)j[0], (float)j[1], (float)j[2]};
                return;
            }
        }
        if (j.size() == 4)
        {
            bool allNum = true;
            for (auto& e : j)
                if (!e.is_number()) allNum = false;
            if (allNum)
            {
                type  = Type::Vec4;
                value = Math::Vec4{
                    (float)j[0], (float)j[1],
                    (float)j[2], (float)j[3]};
                return;
            }
        }

        // general array
        type = Type::Array;
        {
            ArrayType arr;
            arr.reserve(j.size());
            for (auto& item : j)
            {
                JsonValue val;
                val.parse(item);
                arr.push_back(std::move(val));
            }
            value = std::move(arr);
        }
    }
    else if (j.is_object())
    {
        type = Type::Object;
        ObjectType obj;
        for (auto it = j.begin(); it != j.end(); ++it)
        {
            JsonValue val;
            val.parse(it.value());
            obj[it.key()] = std::move(val);
        }
        value = std::move(obj);
    }
}
}
} // namespace Voxol::Scene