#include "mmrsl/types.hpp"
#include <sstream>
#include <iomanip>

namespace mmrsl {

// ============================================================================
// TypeKind conversion
// ============================================================================

std::string typeKindToString(TypeKind kind) {
    switch (kind) {
        case TypeKind::Void: return "void";
        case TypeKind::Int: return "int";
        case TypeKind::Float: return "float";
        case TypeKind::Bool: return "bool";
        case TypeKind::Vec2: return "vec2";
        case TypeKind::Vec3: return "vec3";
        case TypeKind::Vec4: return "vec4";
        case TypeKind::Mat2: return "mat2";
        case TypeKind::Mat3: return "mat3";
        case TypeKind::Mat4: return "mat4";
    }
    return "unknown";
}

TypeKind stringToTypeKind(const std::string& str) {
    if (str == "void") return TypeKind::Void;
    if (str == "int") return TypeKind::Int;
    if (str == "float") return TypeKind::Float;
    if (str == "bool") return TypeKind::Bool;
    if (str == "vec2") return TypeKind::Vec2;
    if (str == "vec3") return TypeKind::Vec3;
    if (str == "vec4") return TypeKind::Vec4;
    if (str == "mat2") return TypeKind::Mat2;
    if (str == "mat3") return TypeKind::Mat3;
    if (str == "mat4") return TypeKind::Mat4;
    return TypeKind::Void;
}

// ============================================================================
// Value accessors
// ============================================================================

int Value::asInt() const {
    if (!isInt()) throw TypeError("Expected int, got " + typeKindToString(kind_));
    return std::get<int>(data_);
}

float Value::asFloat() const {
    if (!isFloat()) throw TypeError("Expected float, got " + typeKindToString(kind_));
    return std::get<float>(data_);
}

bool Value::asBool() const {
    if (!isBool()) throw TypeError("Expected bool, got " + typeKindToString(kind_));
    return std::get<bool>(data_);
}

Vec2 Value::asVec2() const {
    if (!isVec2()) throw TypeError("Expected vec2, got " + typeKindToString(kind_));
    return std::get<Vec2>(data_);
}

Vec3 Value::asVec3() const {
    if (!isVec3()) throw TypeError("Expected vec3, got " + typeKindToString(kind_));
    return std::get<Vec3>(data_);
}

Vec4 Value::asVec4() const {
    if (!isVec4()) throw TypeError("Expected vec4, got " + typeKindToString(kind_));
    return std::get<Vec4>(data_);
}

Mat2 Value::asMat2() const {
    if (!isMat2()) throw TypeError("Expected mat2, got " + typeKindToString(kind_));
    return std::get<Mat2>(data_);
}

Mat3 Value::asMat3() const {
    if (!isMat3()) throw TypeError("Expected mat3, got " + typeKindToString(kind_));
    return std::get<Mat3>(data_);
}

Mat4 Value::asMat4() const {
    if (!isMat4()) throw TypeError("Expected mat4, got " + typeKindToString(kind_));
    return std::get<Mat4>(data_);
}

// ============================================================================
// String representation
// ============================================================================

std::string Value::toString() const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(4);
    
    switch (kind_) {
        case TypeKind::Void:
            return "void";
        case TypeKind::Int:
            oss << asInt();
            return oss.str();
        case TypeKind::Float:
            oss << asFloat();
            return oss.str();
        case TypeKind::Bool:
            return asBool() ? "true" : "false";
        case TypeKind::Vec2: {
            auto v = asVec2();
            oss << "vec2(" << v.x << ", " << v.y << ")";
            return oss.str();
        }
        case TypeKind::Vec3: {
            auto v = asVec3();
            oss << "vec3(" << v.x << ", " << v.y << ", " << v.z << ")";
            return oss.str();
        }
        case TypeKind::Vec4: {
            auto v = asVec4();
            oss << "vec4(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
            return oss.str();
        }
        case TypeKind::Mat2:
            return "mat2(...)";
        case TypeKind::Mat3:
            return "mat3(...)";
        case TypeKind::Mat4:
            return "mat4(...)";
    }
    return "unknown";
}

} // namespace mmrsl
