#pragma once

#include <variant>
#include <string>
#include <stdexcept>
#include <array>

namespace mmrsl {

// Forward declarations
class Value;

// ============================================================================
// GLSL Type Enumeration
// ============================================================================

enum class TypeKind {
    Void,
    Int,
    Float,
    Bool,
    Vec2,
    Vec3,
    Vec4,
    Mat2,
    Mat3,
    Mat4
};

std::string typeKindToString(TypeKind kind);
TypeKind stringToTypeKind(const std::string& str);

// ============================================================================
// Public Vector Types (POD, GLSL-compatible)
// ============================================================================

struct Vec2 {
    float x, y;
    
    Vec2() noexcept : x(0.0f), y(0.0f) {}
    explicit Vec2(float v) noexcept : x(v), y(v) {}
    Vec2(float x_, float y_) noexcept : x(x_), y(y_) {}
    
    float& operator[](size_t i) noexcept { return i == 0 ? x : y; }
    const float& operator[](size_t i) const noexcept { return i == 0 ? x : y; }
    
    bool operator==(const Vec2& other) const noexcept { return x == other.x && y == other.y; }
    bool operator!=(const Vec2& other) const noexcept { return !(*this == other); }
    
    // Arithmetic operators
    Vec2 operator+(const Vec2& other) const noexcept { return Vec2(x + other.x, y + other.y); }
    Vec2 operator-(const Vec2& other) const noexcept { return Vec2(x - other.x, y - other.y); }
    Vec2 operator*(const Vec2& other) const noexcept { return Vec2(x * other.x, y * other.y); }
    Vec2 operator/(const Vec2& other) const noexcept { return Vec2(x / other.x, y / other.y); }
    Vec2 operator*(float s) const noexcept { return Vec2(x * s, y * s); }
    Vec2 operator/(float s) const noexcept { return Vec2(x / s, y / s); }
    Vec2 operator-() const noexcept { return Vec2(-x, -y); }
};

// Non-member scalar multiplication (scalar * vector)
inline Vec2 operator*(float s, const Vec2& v) noexcept { return v * s; }

struct Vec3 {
    float x, y, z;
    
    Vec3() noexcept : x(0.0f), y(0.0f), z(0.0f) {}
    explicit Vec3(float v) noexcept : x(v), y(v), z(v) {}
    Vec3(float x_, float y_, float z_) noexcept : x(x_), y(y_), z(z_) {}
    Vec3(const Vec2& v, float z_) noexcept : x(v.x), y(v.y), z(z_) {}
    
    float& operator[](size_t i) noexcept { 
        return i == 0 ? x : (i == 1 ? y : z); 
    }
    const float& operator[](size_t i) const noexcept { 
        return i == 0 ? x : (i == 1 ? y : z); 
    }
    
    bool operator==(const Vec3& other) const noexcept { 
        return x == other.x && y == other.y && z == other.z; 
    }
    bool operator!=(const Vec3& other) const noexcept { return !(*this == other); }
    
    // Arithmetic operators
    Vec3 operator+(const Vec3& other) const noexcept { return Vec3(x + other.x, y + other.y, z + other.z); }
    Vec3 operator-(const Vec3& other) const noexcept { return Vec3(x - other.x, y - other.y, z - other.z); }
    Vec3 operator*(const Vec3& other) const noexcept { return Vec3(x * other.x, y * other.y, z * other.z); }
    Vec3 operator/(const Vec3& other) const noexcept { return Vec3(x / other.x, y / other.y, z / other.z); }
    Vec3 operator*(float s) const noexcept { return Vec3(x * s, y * s, z * s); }
    Vec3 operator/(float s) const noexcept { return Vec3(x / s, y / s, z / s); }
    Vec3 operator-() const noexcept { return Vec3(-x, -y, -z); }
};

// Non-member scalar multiplication (scalar * vector)
inline Vec3 operator*(float s, const Vec3& v) noexcept { return v * s; }

struct Vec4 {
    float x, y, z, w;
    
    Vec4() noexcept : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
    explicit Vec4(float v) noexcept : x(v), y(v), z(v), w(v) {}
    Vec4(float x_, float y_, float z_, float w_) noexcept : x(x_), y(y_), z(z_), w(w_) {}
    Vec4(const Vec2& v, float z_, float w_) noexcept : x(v.x), y(v.y), z(z_), w(w_) {}
    Vec4(const Vec3& v, float w_) noexcept : x(v.x), y(v.y), z(v.z), w(w_) {}
    
    float& operator[](size_t i) noexcept { 
        return i == 0 ? x : (i == 1 ? y : (i == 2 ? z : w)); 
    }
    const float& operator[](size_t i) const noexcept { 
        return i == 0 ? x : (i == 1 ? y : (i == 2 ? z : w)); 
    }
    
    bool operator==(const Vec4& other) const noexcept { 
        return x == other.x && y == other.y && z == other.z && w == other.w; 
    }
    bool operator!=(const Vec4& other) const noexcept { return !(*this == other); }
    
    // Arithmetic operators
    Vec4 operator+(const Vec4& other) const noexcept { return Vec4(x + other.x, y + other.y, z + other.z, w + other.w); }
    Vec4 operator-(const Vec4& other) const noexcept { return Vec4(x - other.x, y - other.y, z - other.z, w - other.w); }
    Vec4 operator*(const Vec4& other) const noexcept { return Vec4(x * other.x, y * other.y, z * other.z, w * other.w); }
    Vec4 operator/(const Vec4& other) const noexcept { return Vec4(x / other.x, y / other.y, z / other.z, w / other.w); }
    Vec4 operator*(float s) const noexcept { return Vec4(x * s, y * s, z * s, w * s); }
    Vec4 operator/(float s) const noexcept { return Vec4(x / s, y / s, z / s, w / s); }
    Vec4 operator-() const noexcept { return Vec4(-x, -y, -z, -w); }
};

// Non-member scalar multiplication (scalar * vector)
inline Vec4 operator*(float s, const Vec4& v) noexcept { return v * s; }

// ============================================================================
// Public Matrix Types (Column-major, GLSL-compatible)
// ============================================================================

struct Mat2 {
    // Column-major: columns[col][row]
    std::array<Vec2, 2> columns;
    
    Mat2() noexcept : columns{Vec2(1,0), Vec2(0,1)} {}
    Mat2(const Vec2& col0, const Vec2& col1) noexcept : columns{col0, col1} {}
    Mat2(float m00, float m01, float m10, float m11) noexcept
        : columns{Vec2(m00, m01), Vec2(m10, m11)} {}
    
    Vec2& operator[](size_t col) noexcept { return columns[col]; }
    const Vec2& operator[](size_t col) const noexcept { return columns[col]; }
    
    float& operator()(size_t col, size_t row) noexcept { return columns[col][row]; }
    const float& operator()(size_t col, size_t row) const noexcept { return columns[col][row]; }
    
    bool operator==(const Mat2& other) const noexcept { 
        return columns[0] == other.columns[0] && columns[1] == other.columns[1]; 
    }
    bool operator!=(const Mat2& other) const noexcept { return !(*this == other); }
    
    // Matrix addition
    Mat2 operator+(const Mat2& other) const noexcept {
        return Mat2(columns[0] + other.columns[0], columns[1] + other.columns[1]);
    }
    Mat2 operator-(const Mat2& other) const noexcept {
        return Mat2(columns[0] - other.columns[0], columns[1] - other.columns[1]);
    }
    
    // Scalar multiplication
    Mat2 operator*(float s) const noexcept {
        return Mat2(columns[0] * s, columns[1] * s);
    }
    Mat2 operator/(float s) const noexcept {
        return Mat2(columns[0] / s, columns[1] / s);
    }
    
    // Matrix multiplication
    Mat2 operator*(const Mat2& other) const noexcept {
        Mat2 result;
        for (int col = 0; col < 2; ++col) {
            for (int row = 0; row < 2; ++row) {
                result(col, row) = (*this)(0, row) * other(col, 0) + (*this)(1, row) * other(col, 1);
            }
        }
        return result;
    }
    
    // Matrix-vector multiplication
    Vec2 operator*(const Vec2& v) const noexcept {
        return Vec2(
            columns[0][0] * v.x + columns[1][0] * v.y,
            columns[0][1] * v.x + columns[1][1] * v.y
        );
    }
};

inline Mat2 operator*(float s, const Mat2& m) noexcept { return m * s; }

struct Mat3 {
    // Column-major: columns[col][row]
    std::array<Vec3, 3> columns;
    
    Mat3() noexcept : columns{Vec3(1,0,0), Vec3(0,1,0), Vec3(0,0,1)} {}
    Mat3(const Vec3& col0, const Vec3& col1, const Vec3& col2) noexcept
        : columns{col0, col1, col2} {}
    Mat3(float m00, float m01, float m02,
         float m10, float m11, float m12,
         float m20, float m21, float m22) noexcept
        : columns{Vec3(m00, m01, m02), Vec3(m10, m11, m12), Vec3(m20, m21, m22)} {}
    
    Vec3& operator[](size_t col) noexcept { return columns[col]; }
    const Vec3& operator[](size_t col) const noexcept { return columns[col]; }
    
    float& operator()(size_t col, size_t row) noexcept { return columns[col][row]; }
    const float& operator()(size_t col, size_t row) const noexcept { return columns[col][row]; }
    
    bool operator==(const Mat3& other) const noexcept { 
        return columns[0] == other.columns[0] && 
               columns[1] == other.columns[1] && 
               columns[2] == other.columns[2]; 
    }
    bool operator!=(const Mat3& other) const noexcept { return !(*this == other); }
    
    // Matrix addition
    Mat3 operator+(const Mat3& other) const noexcept {
        return Mat3(columns[0] + other.columns[0], columns[1] + other.columns[1], columns[2] + other.columns[2]);
    }
    Mat3 operator-(const Mat3& other) const noexcept {
        return Mat3(columns[0] - other.columns[0], columns[1] - other.columns[1], columns[2] - other.columns[2]);
    }
    
    // Scalar multiplication
    Mat3 operator*(float s) const noexcept {
        return Mat3(columns[0] * s, columns[1] * s, columns[2] * s);
    }
    Mat3 operator/(float s) const noexcept {
        return Mat3(columns[0] / s, columns[1] / s, columns[2] / s);
    }
    
    // Matrix multiplication
    Mat3 operator*(const Mat3& other) const noexcept {
        Mat3 result;
        for (int col = 0; col < 3; ++col) {
            for (int row = 0; row < 3; ++row) {
                result(col, row) = (*this)(0, row) * other(col, 0) + 
                                   (*this)(1, row) * other(col, 1) + 
                                   (*this)(2, row) * other(col, 2);
            }
        }
        return result;
    }
    
    // Matrix-vector multiplication
    Vec3 operator*(const Vec3& v) const noexcept {
        return Vec3(
            columns[0][0] * v.x + columns[1][0] * v.y + columns[2][0] * v.z,
            columns[0][1] * v.x + columns[1][1] * v.y + columns[2][1] * v.z,
            columns[0][2] * v.x + columns[1][2] * v.y + columns[2][2] * v.z
        );
    }
};

inline Mat3 operator*(float s, const Mat3& m) noexcept { return m * s; }

struct Mat4 {
    // Column-major: columns[col][row]
    std::array<Vec4, 4> columns;
    
    Mat4() noexcept : columns{Vec4(1,0,0,0), Vec4(0,1,0,0), Vec4(0,0,1,0), Vec4(0,0,0,1)} {}
    Mat4(const Vec4& col0, const Vec4& col1, const Vec4& col2, const Vec4& col3) noexcept
        : columns{col0, col1, col2, col3} {}
    Mat4(float m00, float m01, float m02, float m03,
         float m10, float m11, float m12, float m13,
         float m20, float m21, float m22, float m23,
         float m30, float m31, float m32, float m33) noexcept
        : columns{Vec4(m00, m01, m02, m03), Vec4(m10, m11, m12, m13), 
                  Vec4(m20, m21, m22, m23), Vec4(m30, m31, m32, m33)} {}
    
    Vec4& operator[](size_t col) noexcept { return columns[col]; }
    const Vec4& operator[](size_t col) const noexcept { return columns[col]; }
    
    float& operator()(size_t col, size_t row) noexcept { return columns[col][row]; }
    const float& operator()(size_t col, size_t row) const noexcept { return columns[col][row]; }
    
    bool operator==(const Mat4& other) const noexcept { 
        return columns[0] == other.columns[0] && 
               columns[1] == other.columns[1] && 
               columns[2] == other.columns[2] &&
               columns[3] == other.columns[3]; 
    }
    bool operator!=(const Mat4& other) const noexcept { return !(*this == other); }
    
    // Matrix addition
    Mat4 operator+(const Mat4& other) const noexcept {
        return Mat4(columns[0] + other.columns[0], columns[1] + other.columns[1], 
                    columns[2] + other.columns[2], columns[3] + other.columns[3]);
    }
    Mat4 operator-(const Mat4& other) const noexcept {
        return Mat4(columns[0] - other.columns[0], columns[1] - other.columns[1], 
                    columns[2] - other.columns[2], columns[3] - other.columns[3]);
    }
    
    // Scalar multiplication
    Mat4 operator*(float s) const noexcept {
        return Mat4(columns[0] * s, columns[1] * s, columns[2] * s, columns[3] * s);
    }
    Mat4 operator/(float s) const noexcept {
        return Mat4(columns[0] / s, columns[1] / s, columns[2] / s, columns[3] / s);
    }
    
    // Matrix multiplication
    Mat4 operator*(const Mat4& other) const noexcept {
        Mat4 result;
        for (int col = 0; col < 4; ++col) {
            for (int row = 0; row < 4; ++row) {
                result(col, row) = (*this)(0, row) * other(col, 0) + 
                                   (*this)(1, row) * other(col, 1) + 
                                   (*this)(2, row) * other(col, 2) +
                                   (*this)(3, row) * other(col, 3);
            }
        }
        return result;
    }
    
    // Matrix-vector multiplication
    Vec4 operator*(const Vec4& v) const noexcept {
        return Vec4(
            columns[0][0] * v.x + columns[1][0] * v.y + columns[2][0] * v.z + columns[3][0] * v.w,
            columns[0][1] * v.x + columns[1][1] * v.y + columns[2][1] * v.z + columns[3][1] * v.w,
            columns[0][2] * v.x + columns[1][2] * v.y + columns[2][2] * v.z + columns[3][2] * v.w,
            columns[0][3] * v.x + columns[1][3] * v.y + columns[2][3] * v.z + columns[3][3] * v.w
        );
    }
};

inline Mat4 operator*(float s, const Mat4& m) noexcept { return m * s; }

// ============================================================================
// Value variant holding any GLSL value
// ============================================================================

class Value {
public:
    using Data = std::variant<
        std::monostate,  // void/uninitialized
        int,
        float,
        bool,
        Vec2,
        Vec3,
        Vec4,
        Mat2,
        Mat3,
        Mat4
    >;

    Value() = default;
    explicit Value(int v) noexcept : data_(v), kind_(TypeKind::Int) {}
    explicit Value(float v) noexcept : data_(v), kind_(TypeKind::Float) {}
    explicit Value(bool v) noexcept : data_(v), kind_(TypeKind::Bool) {}
    explicit Value(const Vec2& v) noexcept : data_(v), kind_(TypeKind::Vec2) {}
    explicit Value(const Vec3& v) noexcept : data_(v), kind_(TypeKind::Vec3) {}
    explicit Value(const Vec4& v) noexcept : data_(v), kind_(TypeKind::Vec4) {}
    explicit Value(const Mat2& v) noexcept : data_(v), kind_(TypeKind::Mat2) {}
    explicit Value(const Mat3& v) noexcept : data_(v), kind_(TypeKind::Mat3) {}
    explicit Value(const Mat4& v) noexcept : data_(v), kind_(TypeKind::Mat4) {}

    // Type queries
    TypeKind kind() const noexcept { return kind_; }
    bool isVoid() const noexcept { return kind_ == TypeKind::Void; }
    bool isInt() const noexcept { return kind_ == TypeKind::Int; }
    bool isFloat() const noexcept { return kind_ == TypeKind::Float; }
    bool isBool() const noexcept { return kind_ == TypeKind::Bool; }
    bool isVec2() const noexcept { return kind_ == TypeKind::Vec2; }
    bool isVec3() const noexcept { return kind_ == TypeKind::Vec3; }
    bool isVec4() const noexcept { return kind_ == TypeKind::Vec4; }
    bool isMat2() const noexcept { return kind_ == TypeKind::Mat2; }
    bool isMat3() const noexcept { return kind_ == TypeKind::Mat3; }
    bool isMat4() const noexcept { return kind_ == TypeKind::Mat4; }

    // Scalar accessors
    int asInt() const;
    float asFloat() const;
    bool asBool() const;

    // Vector accessors
    Vec2 asVec2() const;
    Vec3 asVec3() const;
    Vec4 asVec4() const;

    // Matrix accessors
    Mat2 asMat2() const;
    Mat3 asMat3() const;
    Mat4 asMat4() const;

    // Raw data access
    const Data& data() const noexcept { return data_; }
    Data& data() noexcept { return data_; }

    // String representation for debugging
    std::string toString() const;

private:
    Data data_;
    TypeKind kind_ = TypeKind::Void;
};

// ============================================================================
// Exception Types
// ============================================================================

class TypeError : public std::runtime_error {
public:
    explicit TypeError(const std::string& msg) : std::runtime_error(msg) {}
};

// ============================================================================
// Math Constants
// ============================================================================

inline constexpr float PI = 3.14159265359f;
inline constexpr float DEG_TO_RAD = PI / 180.0f;
inline constexpr float RAD_TO_DEG = 180.0f / PI;

// ============================================================================
// Epsilon Constants
// ============================================================================

inline constexpr float EPSILON = 1e-12f;        // General division by zero check
inline constexpr float MATRIX_EPSILON = 1e-6f;  // Matrix singularity check

} // namespace mmrsl
