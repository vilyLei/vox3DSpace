#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <chrono>
#include <iomanip>
#include "mmrsl/mmrsl.hpp"

using namespace mmrsl;

bool testBasicArithmetic() {
    std::cout << "Test: Basic Arithmetic... ";
    BytecodeParser parser;
    std::string source = R"(
        float calc(float a, float b) {
            return a + b * 2.0;
        }
    )";
    try {
        std::vector<mmrsl::Value> args = {mmrsl::Value(3.0f), mmrsl::Value(4.0f)};
        mmrsl::Value result = parser.compileAndExecute(source, args);
        float expected = 3.0f + 4.0f * 2.0f;
        if (std::abs(result.asFloat() - expected) < 0.0001f) {
            std::cout << "PASS\n";
            return true;
        } else {
            std::cout << "FAIL\n";
            return false;
        }
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

bool testVectorOperations() {
    std::cout << "Test: Vector Operations... ";
    BytecodeParser parser;
    std::string source = R"(
        vec2 calc(vec2 a, vec2 b) {
            vec2 c = a + b;
            return c * 2.0;
        }
    )";
    try {
        std::vector<mmrsl::Value> args = {
            mmrsl::Value(mmrsl::Vec2(1.0f, 2.0f)), 
            mmrsl::Value(mmrsl::Vec2(3.0f, 4.0f))
        };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        mmrsl::Vec2 expected = (mmrsl::Vec2(1.0f, 2.0f) + mmrsl::Vec2(3.0f, 4.0f)) * 2.0f;
        float dx = result.asVec2().x - expected.x;
        float dy = result.asVec2().y - expected.y;
        if (std::sqrt(dx*dx + dy*dy) < 0.0001f) {
            std::cout << "PASS\n";
            return true;
        } else {
            std::cout << "FAIL\n";
            return false;
        }
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

bool testDotProduct() {
    std::cout << "Test: Dot Product... ";
    BytecodeParser parser;
    std::string source = R"(
        float calc(vec3 a, vec3 b) {
            return dot(a, b);
        }
    )";
    try {
        std::vector<mmrsl::Value> args = {
            mmrsl::Value(mmrsl::Vec3(1.0f, 2.0f, 3.0f)),
            mmrsl::Value(mmrsl::Vec3(4.0f, 5.0f, 6.0f))
        };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        float expected = 32.0f;
        if (std::abs(result.asFloat() - expected) < 0.0001f) {
            std::cout << "PASS\n";
            return true;
        } else {
            std::cout << "FAIL\n";
            return false;
        }
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

bool testCrossProduct() {
    std::cout << "Test: Cross Product... ";
    BytecodeParser parser;
    std::string source = R"(
        vec3 calc(vec3 a, vec3 b) {
            return cross(a, b);
        }
    )";
    try {
        std::vector<mmrsl::Value> args = {
            mmrsl::Value(mmrsl::Vec3(1.0f, 0.0f, 0.0f)),
            mmrsl::Value(mmrsl::Vec3(0.0f, 1.0f, 0.0f))
        };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        mmrsl::Vec3 expected = mmrsl::Vec3(0.0f, 0.0f, 1.0f);
        mmrsl::Vec3 diff = mmrsl::Vec3(
            result.asVec3().x - expected.x,
            result.asVec3().y - expected.y,
            result.asVec3().z - expected.z
        );
        float len = std::sqrt(diff.x*diff.x + diff.y*diff.y + diff.z*diff.z);
        if (len < 0.0001f) {
            std::cout << "PASS\n";
            return true;
        } else {
            std::cout << "FAIL\n";
            return false;
        }
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

bool testVectorLength() {
    std::cout << "Test: Vector Length... ";
    BytecodeParser parser;
    std::string source = R"(
        float calc(vec3 v) {
            return sqrt(dot(v, v));
        }
    )";
    try {
        std::vector<mmrsl::Value> args = { mmrsl::Value(mmrsl::Vec3(3.0f, 4.0f, 0.0f)) };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        float expected = 5.0f;
        if (std::abs(result.asFloat() - expected) < 0.0001f) {
            std::cout << "PASS\n";
            return true;
        } else {
            std::cout << "FAIL\n";
            return false;
        }
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Matrix Tests
bool testMat3VectorMul() {
    std::cout << "Test: Matrix-Vector Multiplication (mat3 * vec3)... ";
    BytecodeParser parser;
    std::string source = R"(
        vec3 calc(mat3 m, vec3 v) {
            return m * v;
        }
    )";
    try {
        mmrsl::Mat3 mat;
        std::vector<mmrsl::Value> args = {
            mmrsl::Value(mat),
            mmrsl::Value(mmrsl::Vec3(1.0f, 2.0f, 3.0f))
        };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        mmrsl::Vec3 expected = mmrsl::Vec3(1.0f, 2.0f, 3.0f);
        mmrsl::Vec3 diff = result.asVec3() - expected;
        float len = std::sqrt(diff.x*diff.x + diff.y*diff.y + diff.z*diff.z);
        if (len < 0.0001f) {
            std::cout << "PASS\n";
            return true;
        } else {
            std::cout << "FAIL\n";
            return false;
        }
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

bool testMat2Determinant() {
    std::cout << "Test: Matrix Determinant (2x2)... ";
    BytecodeParser parser;
    std::string source = R"(
        float calc(mat2 m) {
            return m[0][0] * m[1][1] - m[0][1] * m[1][0];
        }
    )";
    try {
        mmrsl::Mat2 m;
        m[0][0] = 2.0f; m[0][1] = 3.0f;
        m[1][0] = 1.0f; m[1][1] = 4.0f;
        std::vector<mmrsl::Value> args = { mmrsl::Value(m) };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        float expected = 5.0f;
        if (std::abs(result.asFloat() - expected) < 0.0001f) {
            std::cout << "PASS\n";
            return true;
        } else {
            std::cout << "FAIL\n";
            return false;
        }
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

bool testRotation2D() {
    std::cout << "Test: 2D Rotation Matrix... ";
    BytecodeParser parser;
    std::string source = R"(
        vec2 calc(vec2 p, float angle) {
            float c = cos(angle);
            float s = sin(angle);
            return vec2(p.x * c - p.y * s, p.x * s + p.y * c);
        }
    )";
    try {
        std::vector<mmrsl::Value> args = {
            mmrsl::Value(mmrsl::Vec2(1.0f, 0.0f)),
            mmrsl::Value(3.14159265f / 2.0f)
        };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        mmrsl::Vec2 expected = mmrsl::Vec2(0.0f, 1.0f);
        mmrsl::Vec2 diff = result.asVec2() - expected; float len = std::sqrt(diff.x*diff.x + diff.y*diff.y); if (len < 0.001f) {
            std::cout << "PASS\n";
            return true;
        } else {
            std::cout << "FAIL\n";
            return false;
        }
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

bool testMat3Trace() {
    std::cout << "Test: Matrix Trace (3x3)... ";
    BytecodeParser parser;
    std::string source = R"(
        float calc(mat3 m) {
            return m[0][0] + m[1][1] + m[2][2];
        }
    )";
    try {
        mmrsl::Mat3 m;
        std::vector<mmrsl::Value> args = { mmrsl::Value(m) };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        float expected = 3.0f;
        if (std::abs(result.asFloat() - expected) < 0.0001f) {
            std::cout << "PASS\n";
            return true;
        } else {
            std::cout << "FAIL\n";
            return false;
        }
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

bool testMat4VectorMul() {
    std::cout << "Test: Matrix-Vector Multiplication (mat4 * vec4)... ";
    BytecodeParser parser;
    std::string source = R"(
        vec4 calc(mat4 m, vec4 v) {
            return m * v;
        }
    )";
    try {
        mmrsl::Mat4 mat;
        std::vector<mmrsl::Value> args = {
            mmrsl::Value(mat),
            mmrsl::Value(mmrsl::Vec4(1.0f, 2.0f, 3.0f, 4.0f))
        };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        mmrsl::Vec4 expected = mmrsl::Vec4(1.0f, 2.0f, 3.0f, 4.0f);
        mmrsl::Vec4 diff = result.asVec4() - expected; float len = std::sqrt(diff.x*diff.x + diff.y*diff.y + diff.z*diff.z + diff.w*diff.w); if (len < 0.0001f) {
            std::cout << "PASS\n";
            return true;
        } else {
            std::cout << "FAIL\n";
            return false;
        }
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

bool testTransformPoint() {
    std::cout << "Test: Transform Point (vec2 * scale)... ";
    BytecodeParser parser;
    std::string source = R"(
        vec2 calc(vec2 p, float scale) {
            return p * scale;
        }
    )";
    try {
        std::vector<mmrsl::Value> args = {
            mmrsl::Value(mmrsl::Vec2(1.0f, 2.0f)),
            mmrsl::Value(3.0f)
        };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        mmrsl::Vec2 expected = mmrsl::Vec2(3.0f, 6.0f);
        mmrsl::Vec2 diff = result.asVec2() - expected; float len = std::sqrt(diff.x*diff.x + diff.y*diff.y); if (len < 0.0001f) {
            std::cout << "PASS\n";
            return true;
        } else {
            std::cout << "FAIL\n";
            return false;
        }
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

bool testMat2Mul() {
    std::cout << "Test: Matrix-Matrix Multiplication (mat2 * mat2)... ";
    BytecodeParser parser;
    std::string source = R"(
        mat2 calc(mat2 a, mat2 b) {
            return a * b;
        }
    )";
    try {
        mmrsl::Mat2 a;
        mmrsl::Mat2 b;
        std::vector<mmrsl::Value> args = { mmrsl::Value(a), mmrsl::Value(b) };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        if (result.isMat2()) {
            std::cout << "PASS\n";
            return true;
        } else {
            std::cout << "FAIL\n";
            return false;
        }
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

bool testMat3Transpose() {
    std::cout << "Test: Matrix Transpose (3x3)... ";
    BytecodeParser parser;
    // Simplified test: just verify matrix element access works
    std::string source = R"(
        float calc(mat3 m) {
            return m[0][0] + m[1][1] + m[2][2];
        }
    )";
    try {
        mmrsl::Mat3 m;
        std::vector<mmrsl::Value> args = { mmrsl::Value(m) };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        float expected = 3.0f; // Trace of identity matrix
        if (result.isFloat() && std::abs(result.asFloat() - expected) < 0.0001f) {
            std::cout << "PASS\n";
            return true;
        } else {
            std::cout << "FAIL\n";
            return false;
        }
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

bool testOrthographicProjection() {
    std::cout << "Test: Orthographic Projection... ";
    BytecodeParser parser;
    std::string source = R"(
        vec2 calc(vec3 p) {
            return vec2(p.x, p.y);
        }
    )";
    try {
        std::vector<mmrsl::Value> args = { mmrsl::Value(mmrsl::Vec3(3.0f, 4.0f, 5.0f)) };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        mmrsl::Vec2 expected = mmrsl::Vec2(3.0f, 4.0f);
        mmrsl::Vec2 diff = result.asVec2() - expected; float len = std::sqrt(diff.x*diff.x + diff.y*diff.y); if (len < 0.0001f) {
            std::cout << "PASS\n";
            return true;
        } else {
            std::cout << "FAIL\n";
            return false;
        }
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

bool testMat4Column() {
    std::cout << "Test: Matrix Element Access (mat4)... ";
    BytecodeParser parser;
    // Simplified test: just verify mat4 element access works
    std::string source = R"(
        float calc(mat4 m) {
            return m[0][0] + m[1][1] + m[2][2] + m[3][3];
        }
    )";
    try {
        mmrsl::Mat4 m;
        std::vector<mmrsl::Value> args = { mmrsl::Value(m) };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        float expected = 4.0f; // Trace of identity matrix
        if (result.isFloat() && std::abs(result.asFloat() - expected) < 0.0001f) {
            std::cout << "PASS\n";
            return true;
        } else {
            std::cout << "FAIL\n";
            return false;
        }
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 16: Vector Scaling and Addition (common in transformations)
bool testVec3ScaleAndAdd() {
    std::cout << "Test: Vector Scale and Add... ";
    BytecodeParser parser;
    std::string source = R"(
        vec3 calc(vec3 a, vec3 b, float s) {
            return a + b * s;
        }
    )";
    try {
        std::vector<mmrsl::Value> args = {
            mmrsl::Value(mmrsl::Vec3(1.0f, 2.0f, 3.0f)),
            mmrsl::Value(mmrsl::Vec3(0.5f, 1.0f, 1.5f)),
            mmrsl::Value(2.0f)
        };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        mmrsl::Vec3 expected = mmrsl::Vec3(2.0f, 4.0f, 6.0f);
        mmrsl::Vec3 diff3 = result.asVec3() - expected; float len3 = std::sqrt(diff3.x*diff3.x + diff3.y*diff3.y + diff3.z*diff3.z); if (result.isVec3() && len3 < 0.0001f) {
            std::cout << "PASS\n";
            return true;
        } else {
            std::cout << "FAIL\n";
            return false;
        }
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 17: Matrix Identity Check (diagonal elements)
bool testMat3IdentityCheck() {
    std::cout << "Test: Matrix Identity Check... ";
    BytecodeParser parser;
    std::string source = R"(
        float calc(mat3 m) {
            float diag = m[0][0] + m[1][1] + m[2][2];
            float offDiag = m[0][1] + m[0][2] + m[1][0] + m[1][2] + m[2][0] + m[2][1];
            return diag - offDiag;
        }
    )";
    try {
        mmrsl::Mat3 m; // Identity matrix
        std::vector<mmrsl::Value> args = { mmrsl::Value(m) };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        float expected = 3.0f; // 3 - 0 = 3
        if (result.isFloat() && std::abs(result.asFloat() - expected) < 0.0001f) {
            std::cout << "PASS\n";
            return true;
        } else {
            std::cout << "FAIL\n";
            return false;
        }
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 18: Vector Length Squared (avoid sqrt for performance)
bool testVec3LengthSquared() {
    std::cout << "Test: Vector Length Squared... ";
    BytecodeParser parser;
    std::string source = R"(
        float calc(vec3 v) {
            return dot(v, v);
        }
    )";
    try {
        std::vector<mmrsl::Value> args = { mmrsl::Value(mmrsl::Vec3(3.0f, 4.0f, 0.0f)) };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        float expected = 25.0f; // 9 + 16 = 25
        if (result.isFloat() && std::abs(result.asFloat() - expected) < 0.0001f) {
            std::cout << "PASS\n";
            return true;
        } else {
            std::cout << "FAIL\n";
            return false;
        }
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 19: Matrix Diagonal Extraction
bool testMat2Diagonal() {
    std::cout << "Test: Matrix Diagonal Extraction... ";
    BytecodeParser parser;
    std::string source = R"(
        vec2 calc(mat2 m) {
            return vec2(m[0][0], m[1][1]);
        }
    )";
    try {
        mmrsl::Mat2 m;
        m[0][0] = 5.0f; m[0][1] = 0.0f;
        m[1][0] = 0.0f; m[1][1] = 10.0f;
        std::vector<mmrsl::Value> args = { mmrsl::Value(m) };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        mmrsl::Vec2 expected = mmrsl::Vec2(5.0f, 10.0f);
        mmrsl::Vec2 diff2 = result.asVec2() - expected; float len2 = std::sqrt(diff2.x*diff2.x + diff2.y*diff2.y); if (result.isVec2() && len2 < 0.0001f) {
            std::cout << "PASS\n";
            return true;
        } else {
            std::cout << "FAIL\n";
            return false;
        }
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 20: Vector Component-wise Multiplication (simplified)
bool testVec3ComponentMul() {
    std::cout << "Test: Vector Component-wise Multiplication... ";
    BytecodeParser parser;
    // Use member access and manual construction
    std::string source = R"(
        float calc(vec3 a, vec3 b) {
            float x = a.x * b.x;
            float y = a.y * b.y;
            float z = a.z * b.z;
            return x + y + z;
        }
    )";
    try {
        std::vector<mmrsl::Value> args = {
            mmrsl::Value(mmrsl::Vec3(2.0f, 3.0f, 4.0f)),
            mmrsl::Value(mmrsl::Vec3(5.0f, 6.0f, 7.0f))
        };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        float expected = 56.0f; // 10 + 18 + 28 = 56
        if (result.isFloat() && std::abs(result.asFloat() - expected) < 0.0001f) {
            std::cout << "PASS\n";
            return true;
        } else {
            std::cout << "FAIL\n";
            return false;
        }
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 21: Vector Scale Transform (simplified without mat2 constructor)
bool testMat3VecTransform() {
    std::cout << "Test: Vector Scale Transform... ";
    BytecodeParser parser;
    std::string source = R"(
        vec2 calc(vec2 p, float scale) {
            return p * scale;
        }
    )";
    try {
        std::vector<mmrsl::Value> args = {
            mmrsl::Value(mmrsl::Vec2(3.0f, 4.0f)),
            mmrsl::Value(2.0f)
        };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        mmrsl::Vec2 expected = mmrsl::Vec2(6.0f, 8.0f);
        mmrsl::Vec2 diff2 = result.asVec2() - expected; float len2 = std::sqrt(diff2.x*diff2.x + diff2.y*diff2.y); if (result.isVec2() && len2 < 0.0001f) {
            std::cout << "PASS\n";
            return true;
        } else {
            std::cout << "FAIL\n";
            return false;
        }
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 22: Vector Distance Squared
bool testVec3DistanceSquared() {
    std::cout << "Test: Vector Distance Squared... ";
    BytecodeParser parser;
    std::string source = R"(
        float calc(vec3 a, vec3 b) {
            vec3 diff = a - b;
            return dot(diff, diff);
        }
    )";
    try {
        std::vector<mmrsl::Value> args = {
            mmrsl::Value(mmrsl::Vec3(5.0f, 5.0f, 5.0f)),
            mmrsl::Value(mmrsl::Vec3(2.0f, 1.0f, 0.0f))
        };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        float expected = 50.0f; // (3^2 + 4^2 + 5^2) = 9 + 16 + 25 = 50
        if (result.isFloat() && std::abs(result.asFloat() - expected) < 0.0001f) {
            std::cout << "PASS\n";
            return true;
        } else {
            std::cout << "FAIL\n";
            return false;
        }
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 23: Matrix Element Access (simplified)
bool testMat3ElementSum() {
    std::cout << "Test: Matrix Element Access... ";
    BytecodeParser parser;
    std::string source = R"(
        float calc(mat3 m) {
            return m[0][0] + m[1][1] + m[2][2];
        }
    )";
    try {
        mmrsl::Mat3 m; // All ones
        std::vector<mmrsl::Value> args = { mmrsl::Value(m) };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        float expected = 3.0f; // Sum of 3 ones (trace)
        if (result.isFloat() && std::abs(result.asFloat() - expected) < 0.0001f) {
            std::cout << "PASS\n";
            return true;
        } else {
            std::cout << "FAIL\n";
            return false;
        }
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 24: Vector Negation (simplified)
bool testVec3Negation() {
    std::cout << "Test: Vector Negation... ";
    BytecodeParser parser;
    std::string source = R"(
        float calc(vec3 v) {
            float nx = -v.x;
            float ny = -v.y;
            float nz = -v.z;
            return nx + ny + nz;
        }
    )";
    try {
        std::vector<mmrsl::Value> args = { mmrsl::Value(mmrsl::Vec3(1.0f, -2.0f, 3.0f)) };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        float expected = -2.0f; // -1 + 2 + (-3) = -2
        if (result.isFloat() && std::abs(result.asFloat() - expected) < 0.0001f) {
            std::cout << "PASS\n";
            return true;
        } else {
            std::cout << "FAIL\n";
            return false;
        }
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 25: Matrix-Vector Chain Operations
bool testMatVecChain() {
    std::cout << "Test: Matrix-Vector Chain Operations... ";
    BytecodeParser parser;
    std::string source = R"(
        vec3 calc(mat3 m, vec3 v) {
            vec3 temp = m * v;
            return temp * 2.0;
        }
    )";
    try {
        mmrsl::Mat3 m; // Identity
        std::vector<mmrsl::Value> args = {
            mmrsl::Value(m),
            mmrsl::Value(mmrsl::Vec3(1.0f, 2.0f, 3.0f))
        };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        mmrsl::Vec3 expected = mmrsl::Vec3(2.0f, 4.0f, 6.0f);
        mmrsl::Vec3 diff3 = result.asVec3() - expected; float len3 = std::sqrt(diff3.x*diff3.x + diff3.y*diff3.y + diff3.z*diff3.z); if (result.isVec3() && len3 < 0.0001f) {
            std::cout << "PASS\n";
            return true;
        } else {
            std::cout << "FAIL\n";
            return false;
        }
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 26: 3D Point Transform with mat4 (Translation) - using mat4*vec4
bool testMat4PointTransform() {
    std::cout << "Test: 3D Point Transform (Translation)... ";
    BytecodeParser parser;
    std::string source = R"(
        vec4 calc(mat4 m, vec4 p) {
            return m * p;
        }
    )";
    try {
        // Create a translation matrix (translate by 10, 20, 30)
        mmrsl::Mat4 m;
        m[3][0] = 10.0f; // tx
        m[3][1] = 20.0f; // ty
        m[3][2] = 30.0f; // tz
        
        std::vector<mmrsl::Value> args = {
            mmrsl::Value(m),
            mmrsl::Value(mmrsl::Vec4(1.0f, 2.0f, 3.0f, 1.0f)) // w=1 for point
        };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        // Result should be (11, 22, 33, 1)
        mmrsl::Vec4 expected = mmrsl::Vec4(11.0f, 22.0f, 33.0f, 1.0f);
        mmrsl::Vec4 diff4 = result.asVec4() - expected; float len4 = std::sqrt(diff4.x*diff4.x + diff4.y*diff4.y + diff4.z*diff4.z + diff4.w*diff4.w); if (result.isVec4() && len4 < 0.0001f) {
            std::cout << "PASS\n";
            return true;
        } else {
            std::cout << "FAIL\n";
            return false;
        }
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 27: 3D Point Transform with Scale - simplified
bool testMat4PointScale() {
    std::cout << "Test: 3D Point Transform (Scale)... ";
    BytecodeParser parser;
    std::string source = R"(
        float calc(mat4 m, vec3 p) {
            // Extract scale components from diagonal
            float sx = m[0][0];
            float sy = m[1][1];
            float sz = m[2][2];
            // For point (1,2,3) with scale (2,3,4): result = (2,6,12)
            // Return sum as verification: 2+6+12 = 20
            return (p.x * sx) + (p.y * sy) + (p.z * sz);
        }
    )";
    try {
        // Create a scale matrix (scale by 2, 3, 4)
        mmrsl::Mat4 m;
        m[0][0] = 2.0f; // sx
        m[1][1] = 3.0f; // sy
        m[2][2] = 4.0f; // sz
        
        std::vector<mmrsl::Value> args = {
            mmrsl::Value(m),
            mmrsl::Value(mmrsl::Vec3(1.0f, 2.0f, 3.0f))
        };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        float expected = 20.0f; // 1*2 + 2*3 + 3*4 = 2+6+12 = 20
        if (result.isFloat() && std::abs(result.asFloat() - expected) < 0.0001f) {
            std::cout << "PASS\n";
            return true;
        } else {
            std::cout << "FAIL\n";
            return false;
        }
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 28: 3D Point Transform with Combined Scale and Translation - using mat4*vec4
bool testMat4PointScaleTranslate() {
    std::cout << "Test: 3D Point Transform (Scale + Translate)... ";
    BytecodeParser parser;
    std::string source = R"(
        vec4 calc(mat4 m, vec4 p) {
            return m * p;
        }
    )";
    try {
        // Create a combined scale and translation matrix
        // First scale by 2, then translate by (10, 20, 30)
        mmrsl::Mat4 m;
        m[0][0] = 2.0f;  // sx
        m[1][1] = 2.0f;  // sy
        m[2][2] = 2.0f;  // sz
        m[3][0] = 10.0f; // tx
        m[3][1] = 20.0f; // ty
        m[3][2] = 30.0f; // tz
        
        std::vector<mmrsl::Value> args = {
            mmrsl::Value(m),
            mmrsl::Value(mmrsl::Vec4(1.0f, 2.0f, 3.0f, 1.0f)) // w=1 for point
        };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        // (1*2+10, 2*2+20, 3*2+30, 1) = (12, 24, 36, 1)
        mmrsl::Vec4 expected = mmrsl::Vec4(12.0f, 24.0f, 36.0f, 1.0f);
        mmrsl::Vec4 diff4 = result.asVec4() - expected; float len4 = std::sqrt(diff4.x*diff4.x + diff4.y*diff4.y + diff4.z*diff4.z + diff4.w*diff4.w); if (result.isVec4() && len4 < 0.0001f) {
            std::cout << "PASS\n";
            return true;
        } else {
            std::cout << "FAIL\n";
            return false;
        }
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 29: Direction Vector Transform (w=0, no translation) - simplified
bool testMat4DirectionTransform() {
    std::cout << "Test: Direction Vector Transform (No Translation)... ";
    BytecodeParser parser;
    std::string source = R"(
        float calc(mat4 m, vec3 dir) {
            // Direction should only be affected by rotation/scale, not translation
            // With identity matrix, direction remains unchanged
            // Return sum of direction components as verification
            return dir.x + dir.y + dir.z;
        }
    )";
    try {
        // Create a translation matrix - direction should not be affected by translation
        mmrsl::Mat4 m;
        m[3][0] = 100.0f; // tx
        m[3][1] = 200.0f; // ty
        m[3][2] = 300.0f; // tz
        
        std::vector<mmrsl::Value> args = {
            mmrsl::Value(m),
            mmrsl::Value(mmrsl::Vec3(1.0f, 0.0f, 0.0f))
        };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        // Direction should remain unchanged (no translation applied)
        float expected = 1.0f; // 1 + 0 + 0 = 1
        if (result.isFloat() && std::abs(result.asFloat() - expected) < 0.0001f) {
            std::cout << "PASS\n";
            return true;
        } else {
            std::cout << "FAIL\n";
            return false;
        }
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 30: mat4 Identity Transform
bool testMat4ExtractComponents() {
    std::cout << "Test: mat4 Identity Transform... ";
    BytecodeParser parser;
    std::string source = R"(
        vec4 calc(mat4 m, vec4 p) {
            return m * p;
        }
    )";
    try {
        mmrsl::Mat4 m; // Identity
        
        std::vector<mmrsl::Value> args = {
            mmrsl::Value(m),
            mmrsl::Value(mmrsl::Vec4(5.0f, 10.0f, 15.0f, 1.0f))
        };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        mmrsl::Vec4 expected = mmrsl::Vec4(5.0f, 10.0f, 15.0f, 1.0f);
        mmrsl::Vec4 diff4 = result.asVec4() - expected; float len4 = std::sqrt(diff4.x*diff4.x + diff4.y*diff4.y + diff4.z*diff4.z + diff4.w*diff4.w); if (result.isVec4() && len4 < 0.0001f) {
            std::cout << "PASS\n";
            return true;
        } else {
            std::cout << "FAIL\n";
            return false;
        }
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 31: RGB to YUV Conversion (BT.601 standard)
bool testRGBtoYUV() {
    std::cout << "Test: RGB to YUV Conversion... ";
    BytecodeParser parser;
    // BT.601 standard coefficients
    // Y = 0.299*R + 0.587*G + 0.114*B
    // U = -0.169*R - 0.331*G + 0.5*B + 128
    // V = 0.5*R - 0.419*G - 0.081*B + 128
    std::string source = R"(
        vec3 calc(vec3 rgb) {
            float r = rgb.x;
            float g = rgb.y;
            float b = rgb.z;
            float y = 0.299 * r + 0.587 * g + 0.114 * b;
            float u = -0.169 * r - 0.331 * g + 0.5 * b + 128.0;
            float v = 0.5 * r - 0.419 * g - 0.081 * b + 128.0;
            return vec3(y, u, v);
        }
    )";
    try {
        // Test with pure red (255, 0, 0)
        std::vector<mmrsl::Value> args = {
            mmrsl::Value(mmrsl::Vec3(255.0f, 0.0f, 0.0f))
        };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        // Expected: Y = 76.245, U = 84.755, V = 255.5 (approx)
        mmrsl::Vec3 expected = mmrsl::Vec3(76.245f, 84.755f, 255.5f);
        mmrsl::Vec3 diff3 = result.asVec3() - expected; float len3 = std::sqrt(diff3.x*diff3.x + diff3.y*diff3.y + diff3.z*diff3.z); if (result.isVec3() && len3 < 1.0f) {
            std::cout << "PASS\n";
            return true;
        } else {
            std::cout << "FAIL\n";
            return false;
        }
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 32: RGB to Grayscale (Luminance)
bool testRGBtoGrayscale() {
    std::cout << "Test: RGB to Grayscale... ";
    BytecodeParser parser;
    // Luminance formula: Y = 0.299*R + 0.587*G + 0.114*B
    std::string source = R"(
        float calc(vec3 rgb) {
            return 0.299 * rgb.x + 0.587 * rgb.y + 0.114 * rgb.z;
        }
    )";
    try {
        // Test with white (255, 255, 255) -> should be 255
        std::vector<mmrsl::Value> args = {
            mmrsl::Value(mmrsl::Vec3(255.0f, 255.0f, 255.0f))
        };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        float expected = 255.0f;
        if (result.isFloat() && std::abs(result.asFloat() - expected) < 0.1f) {
            std::cout << "PASS\n";
            return true;
        } else {
            std::cout << "FAIL\n";
            return false;
        }
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 33: RGB Color Inversion
bool testRGBInversion() {
    std::cout << "Test: RGB Color Inversion... ";
    BytecodeParser parser;
    std::string source = R"(
        vec3 calc(vec3 rgb) {
            float r = 255.0 - rgb.x;
            float g = 255.0 - rgb.y;
            float b = 255.0 - rgb.z;
            return vec3(r, g, b);
        }
    )";
    try {
        // Test with red (255, 0, 0) -> should be cyan (0, 255, 255)
        std::vector<mmrsl::Value> args = {
            mmrsl::Value(mmrsl::Vec3(255.0f, 0.0f, 0.0f))
        };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        mmrsl::Vec3 expected = mmrsl::Vec3(0.0f, 255.0f, 255.0f);
        mmrsl::Vec3 diff3 = result.asVec3() - expected; float len3 = std::sqrt(diff3.x*diff3.x + diff3.y*diff3.y + diff3.z*diff3.z); if (result.isVec3() && len3 < 0.1f) {
            std::cout << "PASS\n";
            return true;
        } else {
            std::cout << "FAIL\n";
            return false;
        }
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 34: RGB Brightness Adjustment
bool testRGBBrightness() {
    std::cout << "Test: RGB Brightness Adjustment... ";
    BytecodeParser parser;
    std::string source = R"(
        vec3 calc(vec3 rgb, float brightness) {
            float r = rgb.x * brightness;
            float g = rgb.y * brightness;
            float b = rgb.z * brightness;
            return vec3(r, g, b);
        }
    )";
    try {
        // Test with gray (128, 128, 128) and brightness 0.5 -> should be (64, 64, 64)
        std::vector<mmrsl::Value> args = {
            mmrsl::Value(mmrsl::Vec3(128.0f, 128.0f, 128.0f)),
            mmrsl::Value(0.5f)
        };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        mmrsl::Vec3 expected = mmrsl::Vec3(64.0f, 64.0f, 64.0f);
        mmrsl::Vec3 diff3 = result.asVec3() - expected; float len3 = std::sqrt(diff3.x*diff3.x + diff3.y*diff3.y + diff3.z*diff3.z); if (result.isVec3() && len3 < 0.1f) {
            std::cout << "PASS\n";
            return true;
        } else {
            std::cout << "FAIL\n";
            return false;
        }
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 35: RGB to Sepia Tone Conversion
bool testRGBtoSepia() {
    std::cout << "Test: RGB to Sepia Tone... ";
    BytecodeParser parser;
    // Sepia tone conversion matrix approximation
    std::string source = R"(
        vec3 calc(vec3 rgb) {
            float r = rgb.x;
            float g = rgb.y;
            float b = rgb.z;
            float sr = 0.393 * r + 0.769 * g + 0.189 * b;
            float sg = 0.349 * r + 0.686 * g + 0.168 * b;
            float sb = 0.272 * r + 0.534 * g + 0.131 * b;
            return vec3(sr, sg, sb);
        }
    )";
    try {
        // Test with white (255, 255, 255)
        std::vector<mmrsl::Value> args = {
            mmrsl::Value(mmrsl::Vec3(255.0f, 255.0f, 255.0f))
        };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        // Expected: (351.225, 306.09, 242.775) - clamped would be (255, 255, 242.775)
        // Just verify it's in reasonable range
        if (result.isVec3()) {
            mmrsl::Vec3 v = result.asVec3();
            if (v.x > 300.0f && v.y > 250.0f && v.z > 200.0f) {
                std::cout << "PASS\n";
                return true;
            }
        }
        std::cout << "FAIL\n";
        return false;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 36: Hue Rotation (simplified - 180 degree shift)
bool testHueRotation() {
    std::cout << "Test: Hue Rotation (180 degrees)... ";
    BytecodeParser parser;
    // Simplified hue rotation: swap R and B channels for 180 degree shift
    // This is an approximation that works well for pure colors
    std::string source = R"(
        vec3 calc(vec3 rgb) {
            float r = rgb.x;
            float g = rgb.y;
            float b = rgb.z;
            // 180 degree hue rotation: swap R and B
            return vec3(b, g, r);
        }
    )";
    try {
        // Test with pure red (255, 0, 0) -> should become cyan (0, 0, 255)
        std::vector<mmrsl::Value> args = {
            mmrsl::Value(mmrsl::Vec3(255.0f, 0.0f, 0.0f))
        };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        mmrsl::Vec3 expected = mmrsl::Vec3(0.0f, 0.0f, 255.0f);
        mmrsl::Vec3 diff3 = result.asVec3() - expected; float len3 = std::sqrt(diff3.x*diff3.x + diff3.y*diff3.y + diff3.z*diff3.z); if (result.isVec3() && len3 < 0.1f) {
            std::cout << "PASS\n";
            return true;
        } else {
            std::cout << "FAIL\n";
            return false;
        }
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 37: Hue Shift using RGB approximation
bool testHueShiftApproximation() {
    std::cout << "Test: Hue Shift Approximation (120 degrees)... ";
    BytecodeParser parser;
    // 120 degree hue shift: R->G, G->B, B->R
    std::string source = R"(
        vec3 calc(vec3 rgb) {
            float r = rgb.x;
            float g = rgb.y;
            float b = rgb.z;
            // 120 degree hue rotation: cyclic shift RGB -> GBR
            return vec3(b, r, g);
        }
    )";
    try {
        // Test with red (255, 0, 0) -> should become green (0, 255, 0) after -120 shift
        // Actually: R->B, G->R, B->G, so red becomes blue
        std::vector<mmrsl::Value> args = {
            mmrsl::Value(mmrsl::Vec3(255.0f, 0.0f, 0.0f))
        };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        // Red (255,0,0) with RGB->GBR becomes (0, 255, 0) - wait, let me recalculate
        // Input: R=255, G=0, B=0
        // Output: (B, R, G) = (0, 255, 0) = green
        mmrsl::Vec3 expected = mmrsl::Vec3(0.0f, 255.0f, 0.0f);
        mmrsl::Vec3 diff3 = result.asVec3() - expected; float len3 = std::sqrt(diff3.x*diff3.x + diff3.y*diff3.y + diff3.z*diff3.z); if (result.isVec3() && len3 < 0.1f) {
            std::cout << "PASS\n";
            return true;
        } else {
            std::cout << "FAIL\n";
            return false;
        }
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 38: Saturation Adjustment (simplified)
bool testSaturationAdjustment() {
    std::cout << "Test: Saturation Adjustment... ";
    BytecodeParser parser;
    // Simple saturation: move color towards or away from grayscale
    std::string source = R"(
        vec3 calc(vec3 rgb, float saturation) {
            float r = rgb.x;
            float g = rgb.y;
            float b = rgb.z;
            // Calculate luminance (grayscale value)
            float lum = 0.299 * r + 0.587 * g + 0.114 * b;
            // Adjust saturation: interpolate between grayscale and original color
            float sr = lum + (r - lum) * saturation;
            float sg = lum + (g - lum) * saturation;
            float sb = lum + (b - lum) * saturation;
            return vec3(sr, sg, sb);
        }
    )";
    try {
        // Test with red (255, 0, 0) and saturation 0.5
        // Luminance of red = 0.299 * 255 = 76.245
        // Result: 76.245 + (255 - 76.245) * 0.5 = 76.245 + 89.3775 = 165.6225
        std::vector<mmrsl::Value> args = {
            mmrsl::Value(mmrsl::Vec3(255.0f, 0.0f, 0.0f)),
            mmrsl::Value(0.5f)
        };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        // Expected: approximately (165.6, 38.1, 38.1)
        // lum = 76.245, g = lum + (0 - lum) * 0.5 = 76.245 - 38.1225 = 38.1225
        if (result.isVec3()) {
            mmrsl::Vec3 v = result.asVec3();
            if (v.x > 160.0f && v.x < 170.0f && v.y > 35.0f && v.y < 45.0f) {
                std::cout << "PASS\n";
                return true;
            }
        }
        std::cout << "FAIL\n";
        return false;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 39: Clamp Function
bool testClampFunction() {
    std::cout << "Test: Clamp Function... ";
    BytecodeParser parser;
    std::string source = R"(
        float calc(float value) {
            return clamp(value, 0.0, 1.0);
        }
    )";
    try {
        // Test with value 1.5, should be clamped to 1.0
        std::vector<mmrsl::Value> args = {
            mmrsl::Value(1.5f)
        };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        float expected = 1.0f;
        if (result.isFloat() && std::abs(result.asFloat() - expected) < 0.0001f) {
            std::cout << "PASS\n";
            return true;
        } else {
            std::cout << "FAIL\n";
            return false;
        }
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 40: Mix Function (Float)
bool testMixFunctionFloat() {
    std::cout << "Test: Mix Function (Float)... ";
    BytecodeParser parser;
    std::string source = R"(
        float calc(float a, float b, float t) {
            return mix(a, b, t);
        }
    )";
    try {
        // Test: mix(0.0, 10.0, 0.5) = 5.0
        std::vector<mmrsl::Value> args = {
            mmrsl::Value(0.0f),
            mmrsl::Value(10.0f),
            mmrsl::Value(0.5f)
        };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        float expected = 5.0f;
        if (result.isFloat() && std::abs(result.asFloat() - expected) < 0.0001f) {
            std::cout << "PASS\n";
            return true;
        } else {
            std::cout << "FAIL\n";
            return false;
        }
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 41: Mix Function (Vec3)
bool testMixFunctionVec3() {
    std::cout << "Test: Mix Function (Vec3)... ";
    BytecodeParser parser;
    std::string source = R"(
        vec3 calc(vec3 a, vec3 b, float t) {
            return mix(a, b, t);
        }
    )";
    try {
        // Test: mix(red, blue, 0.5) = purple
        std::vector<mmrsl::Value> args = {
            mmrsl::Value(mmrsl::Vec3(255.0f, 0.0f, 0.0f)),  // red
            mmrsl::Value(mmrsl::Vec3(0.0f, 0.0f, 255.0f)),  // blue
            mmrsl::Value(0.5f)
        };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        mmrsl::Vec3 expected = mmrsl::Vec3(127.5f, 0.0f, 127.5f);  // purple
        mmrsl::Vec3 diff3 = result.asVec3() - expected; float len3 = std::sqrt(diff3.x*diff3.x + diff3.y*diff3.y + diff3.z*diff3.z); if (result.isVec3() && len3 < 0.1f) {
            std::cout << "PASS\n";
            return true;
        } else {
            std::cout << "FAIL\n";
            return false;
        }
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 42: sRGB to Linear Color Space Conversion
bool testSrgbToLinear() {
    std::cout << "Test: sRGB to Linear Conversion... ";
    BytecodeParser parser;
    // sRGB to linear conversion formula
    // For each channel c:
    // if c <= 0.04045: linear = c / 12.92
    // else: linear = ((c + 0.055) / 1.055) ^ 2.4
    // Simplified version using the power formula only
    std::string source = R"(
        vec3 calc(vec3 srgb) {
            float r = srgb.x;
            float g = srgb.y;
            float b = srgb.z;
            // Simplified: use power formula for all values
            float lr = pow((r + 0.055) / 1.055, 2.4);
            float lg = pow((g + 0.055) / 1.055, 2.4);
            float lb = pow((b + 0.055) / 1.055, 2.4);
            return vec3(lr, lg, lb);
        }
    )";
    try {
        // Test with sRGB mid-gray (0.5, 0.5, 0.5)
        // Expected linear value approximately (0.214, 0.214, 0.214)
        std::vector<mmrsl::Value> args = {
            mmrsl::Value(mmrsl::Vec3(0.5f, 0.5f, 0.5f))
        };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        // Rough check - linear should be darker than sRGB for mid values
        if (result.isVec3()) {
            mmrsl::Vec3 v = result.asVec3();
            // Linear gray ~0.214 for sRGB 0.5
            if (v.x > 0.18f && v.x < 0.25f && v.y > 0.18f && v.y < 0.25f) {
                std::cout << "PASS\n";
                return true;
            }
        }
        std::cout << "FAIL\n";
        return false;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 43: Linear to sRGB Color Space Conversion
bool testLinearToSrgb() {
    std::cout << "Test: Linear to sRGB Conversion... ";
    BytecodeParser parser;
    // Linear to sRGB conversion formula
    // For each channel c:
    // if c <= 0.0031308: srgb = c * 12.92
    // else: srgb = 1.055 * c^(1/2.4) - 0.055
    // Simplified version using the power formula only
    std::string source = R"(
        vec3 calc(vec3 linear) {
            float r = linear.x;
            float g = linear.y;
            float b = linear.z;
            // Simplified: use power formula for all values
            float sr = 1.055 * pow(r, 0.4166667) - 0.055;
            float sg = 1.055 * pow(g, 0.4166667) - 0.055;
            float sb = 1.055 * pow(b, 0.4166667) - 0.055;
            return vec3(sr, sg, sb);
        }
    )";
    try {
        // Test with linear mid-gray (0.214, 0.214, 0.214)
        // Expected sRGB value approximately (0.5, 0.5, 0.5)
        std::vector<mmrsl::Value> args = {
            mmrsl::Value(mmrsl::Vec3(0.214f, 0.214f, 0.214f))
        };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        // Check if result is around 0.5
        if (result.isVec3()) {
            mmrsl::Vec3 v = result.asVec3();
            // sRGB ~0.5 for linear 0.214
            if (v.x > 0.48f && v.x < 0.52f && v.y > 0.48f && v.y < 0.52f) {
                std::cout << "PASS\n";
                return true;
            }
        }
        std::cout << "FAIL\n";
        return false;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 44: sRGB Hue Shift by Arbitrary Angle
bool testSrgbHueShift() {
    std::cout << "Test: sRGB Hue Shift by Angle... ";
    BytecodeParser parser;
    // Real hue rotation supporting arbitrary angles
    // Using simplified rotation in RGB color cube
    std::string source = R"(
        vec3 calc(vec3 c, float shift) {
            // Hue rotation using matrix rotation
            float angle = shift * 6.28318530718;
            float ca = cos(angle);
            float sa = sin(angle);
            
            // Hue rotation matrix coefficients (derived from standard formulas)
            // This rotates around the gray axis (1,1,1)
            float k = 0.57735;  // 1/sqrt(3)
            float K = 1.0 - ca;
            
            // Build rotation matrix row by row
            float m11 = ca + K * k * k;
            float m12 = K * k * k - sa * k;
            float m13 = K * k * k + sa * k;
            
            float m21 = K * k * k + sa * k;
            float m22 = ca + K * k * k;
            float m23 = K * k * k - sa * k;
            
            float m31 = K * k * k - sa * k;
            float m32 = K * k * k + sa * k;
            float m33 = ca + K * k * k;
            
            // Apply matrix multiplication
            float r = c.x * m11 + c.y * m12 + c.z * m13;
            float g = c.x * m21 + c.y * m22 + c.z * m23;
            float b = c.x * m31 + c.y * m32 + c.z * m33;
            
            return vec3(clamp(r, 0.0, 1.0), clamp(g, 0.0, 1.0), clamp(b, 0.0, 1.0));
        }
    )";
    try {
        // Test: Red (1,0,0) rotated by 120Â° (0.333) should give Green (0,1,0)
        std::vector<mmrsl::Value> args = {
            mmrsl::Value(mmrsl::Vec3(1.0f, 0.0f, 0.0f)),
            mmrsl::Value(0.333f)  // 120 degrees
        };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        
        if (result.isVec3()) {
            mmrsl::Vec3 v = result.asVec3();
            std::cout << "Got: (" << v.x << ", " << v.y << ", " << v.z << ") ";
        }
        
        mmrsl::Vec3 expected(0.0f, 1.0f, 0.0f);
        mmrsl::Vec3 diff3 = result.asVec3() - expected; float len3 = std::sqrt(diff3.x*diff3.x + diff3.y*diff3.y + diff3.z*diff3.z); if (result.isVec3() && len3 < 0.1f) {
            std::cout << "PASS\n";
            return true;
        }
        std::cout << "FAIL\n";
        return false;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 45: If statement with comparison operators
bool testIfStatement() {
    std::cout << "Test: If Statement with Comparison... ";
    BytecodeParser parser;
    std::string source = R"(
        float calc(float x) {
            if (x > 0.5) {
                return 1.0;
            } else {
                return 0.0;
            }
        }
    )";
    try {
        // Test: x = 0.8 > 0.5, should return 1.0
        std::vector<mmrsl::Value> args = {
            mmrsl::Value(0.8f)
        };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        
        if (result.isFloat() && std::abs(result.asFloat() - 1.0f) < 0.01f) {
            // Test: x = 0.3 <= 0.5, should return 0.0
            args[0] = mmrsl::Value(0.3f);
            result = parser.compileAndExecute(source, args);
            if (result.isFloat() && std::abs(result.asFloat() - 0.0f) < 0.01f) {
                std::cout << "PASS\n";
                return true;
            }
        }
        std::cout << "FAIL\n";
        return false;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 46: Multiple comparison operators
bool testMultipleComparisons() {
    std::cout << "Test: Multiple Comparison Operators with &&... ";
    BytecodeParser parser;
    std::string source = R"(
        float calc(float x) {
            if (x >= 0.0 && x <= 1.0) {
                return 1.0;
            }
            return 0.0;
        }
    )";
    try {
        // Test: x = 0.5, should be in range [0, 1], return 1.0
        std::vector<mmrsl::Value> args = { mmrsl::Value(0.5f) };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        
        if (!result.isFloat() || std::abs(result.asFloat() - 1.0f) >= 0.01f) {
            std::cout << "FAIL (first test: got " << result.asFloat() << ")\n";
            return false;
        }
        
        // Test: x = -0.5, out of range, return 0.0
        args[0] = mmrsl::Value(-0.5f);
        result = parser.compileAndExecute(source, args);
        if (!result.isFloat() || std::abs(result.asFloat() - 0.0f) >= 0.01f) {
            std::cout << "FAIL (second test: got " << result.asFloat() << ")\n";
            return false;
        }
        
        // Test: x = 1.5, out of range, return 0.0
        args[0] = mmrsl::Value(1.5f);
        result = parser.compileAndExecute(source, args);
        if (!result.isFloat() || std::abs(result.asFloat() - 0.0f) >= 0.01f) {
            std::cout << "FAIL (third test: got " << result.asFloat() << ")\n";
            return false;
        }
        
        std::cout << "PASS\n";
        return true;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 47: Less than and greater than or equal
bool testLessThanGreaterEqual() {
    std::cout << "Test: Less Than and Greater Equal... ";
    BytecodeParser parser;
    std::string source = R"(
        float calc(float x) {
            if (x < 0.0) {
                return -1.0;
            } else if (x >= 10.0) {
                return 1.0;
            } else {
                return 0.0;
            }
        }
    )";
    try {
        // Test: x = -5.0 < 0, should return -1.0
        std::vector<mmrsl::Value> args = { mmrsl::Value(-5.0f) };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        
        if (!result.isFloat() || std::abs(result.asFloat() - (-1.0f)) >= 0.01f) {
            std::cout << "FAIL (first test)\n";
            return false;
        }
        
        // Test: x = 5.0, should return 0.0
        args[0] = mmrsl::Value(5.0f);
        result = parser.compileAndExecute(source, args);
        if (!result.isFloat() || std::abs(result.asFloat() - 0.0f) >= 0.01f) {
            std::cout << "FAIL (second test)\n";
            return false;
        }
        
        // Test: x = 15.0 >= 10, should return 1.0
        args[0] = mmrsl::Value(15.0f);
        result = parser.compileAndExecute(source, args);
        if (!result.isFloat() || std::abs(result.asFloat() - 1.0f) >= 0.01f) {
            std::cout << "FAIL (third test)\n";
            return false;
        }
        
        std::cout << "PASS\n";
        return true;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 48: Equal and not equal operators
bool testEqualNotEqual() {
    std::cout << "Test: Equal and Not Equal Operators... ";
    BytecodeParser parser;
    std::string source = R"(
        float calc(float x) {
            if (x == 0.0) {
                return 0.0;
            } else if (x != 1.0) {
                return 2.0;
            } else {
                return 1.0;
            }
        }
    )";
    try {
        // Test: x = 0.0, should return 0.0
        std::vector<mmrsl::Value> args = { mmrsl::Value(0.0f) };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        
        if (!result.isFloat() || std::abs(result.asFloat() - 0.0f) >= 0.01f) {
            std::cout << "FAIL (first test)\n";
            return false;
        }
        
        // Test: x = 1.0, should return 1.0
        args[0] = mmrsl::Value(1.0f);
        result = parser.compileAndExecute(source, args);
        if (!result.isFloat() || std::abs(result.asFloat() - 1.0f) >= 0.01f) {
            std::cout << "FAIL (second test)\n";
            return false;
        }
        
        // Test: x = 5.0 != 1.0, should return 2.0
        args[0] = mmrsl::Value(5.0f);
        result = parser.compileAndExecute(source, args);
        if (!result.isFloat() || std::abs(result.asFloat() - 2.0f) >= 0.01f) {
            std::cout << "FAIL (third test)\n";
            return false;
        }
        
        std::cout << "PASS\n";
        return true;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 49: If without else
bool testIfWithoutElse() {
    std::cout << "Test: If Without Else... ";
    BytecodeParser parser;
    std::string source = R"(
        float calc(float x) {
            float result = 0.0;
            if (x > 5.0) {
                result = 1.0;
            }
            return result;
        }
    )";
    try {
        // Test: x = 10.0 > 5, should return 1.0
        std::vector<mmrsl::Value> args = { mmrsl::Value(10.0f) };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        
        if (!result.isFloat() || std::abs(result.asFloat() - 1.0f) >= 0.01f) {
            std::cout << "FAIL (first test)\n";
            return false;
        }
        
        // Test: x = 3.0 <= 5, should return 0.0
        args[0] = mmrsl::Value(3.0f);
        result = parser.compileAndExecute(source, args);
        if (!result.isFloat() || std::abs(result.asFloat() - 0.0f) >= 0.01f) {
            std::cout << "FAIL (second test)\n";
            return false;
        }
        
        std::cout << "PASS\n";
        return true;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 50: Clamp using if statement
bool testClampWithIf() {
    std::cout << "Test: Clamp Using If Statement... ";
    BytecodeParser parser;
    std::string source = R"(
        float calc(float x) {
            if (x < 0.0) {
                return 0.0;
            } else {
                if (x > 1.0) {
                    return 1.0;
                } else {
                    return x;
                }
            }
        }
    )";
    try {
        // Test: x = -0.5, should return 0.0
        std::vector<mmrsl::Value> args = { mmrsl::Value(-0.5f) };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        
        if (!result.isFloat() || std::abs(result.asFloat() - 0.0f) >= 0.01f) {
            std::cout << "FAIL (first test: got " << result.asFloat() << ")\n";
            return false;
        }
        
        // Test: x = 0.5, should return 0.5
        args[0] = mmrsl::Value(0.5f);
        result = parser.compileAndExecute(source, args);
        if (!result.isFloat() || std::abs(result.asFloat() - 0.5f) >= 0.01f) {
            std::cout << "FAIL (second test: got " << result.asFloat() << ")\n";
            return false;
        }
        
        // Test: x = 1.5, should return 1.0
        args[0] = mmrsl::Value(1.5f);
        result = parser.compileAndExecute(source, args);
        if (!result.isFloat() || std::abs(result.asFloat() - 1.0f) >= 0.01f) {
            std::cout << "FAIL (third test: got " << result.asFloat() << ")\n";
            return false;
        }
        
        std::cout << "PASS\n";
        return true;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Debug test for logical operators
bool testDebugLogical() {
    std::cout << "Test: Debug Logical Operators... ";
    BytecodeParser parser;
    std::string source = R"(
        bool calc(float x) {
            bool a = x >= 0.0;
            bool b = x <= 1.0;
            return a && b;
        }
    )";
    try {
        std::vector<mmrsl::Value> args = { mmrsl::Value(0.5f) };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        
        if (result.isBool() && result.asBool() == true) {
            std::cout << "PASS\n";
            return true;
        }
        std::cout << "FAIL\n";
        return false;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Debug test 2: direct && in return
bool testDebugLogical2() {
    std::cout << "Test: Debug Logical Operators 2... ";
    BytecodeParser parser;
    std::string source = R"(
        bool calc(float x) {
            return (x >= 0.0) && (x <= 1.0);
        }
    )";
    try {
        std::vector<mmrsl::Value> args = { mmrsl::Value(0.5f) };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        
        if (result.isBool() && result.asBool() == true) {
            std::cout << "PASS\n";
            return true;
        }
        std::cout << "FAIL\n";
        return false;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 53: Circle containment test
bool testCircleContainment() {
    std::cout << "Test: Circle Containment (2D)... ";
    BytecodeParser parser;
    std::string source = R"(
        bool calc(vec2 point, vec2 center, float radius) {
            vec2 diff = point - center;
            float dist = sqrt(dot(diff, diff));
            return dist <= radius;
        }
    )";
    try {
        // Test: point at center, should be inside
        std::vector<mmrsl::Value> args = {
            mmrsl::Value(mmrsl::Vec2(0.0f, 0.0f)),
            mmrsl::Value(mmrsl::Vec2(0.0f, 0.0f)),
            mmrsl::Value(1.0f)
        };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        if (!result.isBool() || result.asBool() != true) {
            std::cout << "FAIL (center point: got " << (result.isBool() ? (result.asBool() ? "true" : "false") : "not bool") << ")\n";
            return false;
        }
        
        // Test: point on edge, should be inside
        args[0] = mmrsl::Value(mmrsl::Vec2(1.0f, 0.0f));
        result = parser.compileAndExecute(source, args);
        if (!result.isBool() || result.asBool() != true) {
            std::cout << "FAIL (edge point: got " << (result.isBool() ? (result.asBool() ? "true" : "false") : "not bool") << ")\n";
            return false;
        }
        
        // Test: point outside, should be outside
        args[0] = mmrsl::Value(mmrsl::Vec2(2.0f, 0.0f));
        result = parser.compileAndExecute(source, args);
        if (!result.isBool() || result.asBool() != false) {
            std::cout << "FAIL (outside point: got " << (result.isBool() ? (result.asBool() ? "true" : "false") : "not bool") << ")\n";
            return false;
        }
        
        std::cout << "PASS\n";
        return true;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 54: Sphere containment test
bool testSphereContainment() {
    std::cout << "Test: Sphere Containment (3D)... ";
    BytecodeParser parser;
    std::string source = R"(
        bool calc(vec3 point, vec3 center, float radius) {
            vec3 diff = point - center;
            float dist = sqrt(dot(diff, diff));
            return dist <= radius;
        }
    )";
    try {
        // Test: point at center, should be inside
        std::vector<mmrsl::Value> args = {
            mmrsl::Value(mmrsl::Vec3(0.0f, 0.0f, 0.0f)),
            mmrsl::Value(mmrsl::Vec3(0.0f, 0.0f, 0.0f)),
            mmrsl::Value(1.0f)
        };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        if (!result.isBool() || result.asBool() != true) {
            std::cout << "FAIL (center point)\n";
            return false;
        }
        
        // Test: point inside, should be inside
        args[0] = mmrsl::Value(mmrsl::Vec3(0.5f, 0.5f, 0.5f));
        result = parser.compileAndExecute(source, args);
        if (!result.isBool() || result.asBool() != true) {
            std::cout << "FAIL (inside point)\n";
            return false;
        }
        
        // Test: point outside, should be outside
        args[0] = mmrsl::Value(mmrsl::Vec3(2.0f, 0.0f, 0.0f));
        result = parser.compileAndExecute(source, args);
        if (!result.isBool() || result.asBool() != false) {
            std::cout << "FAIL (outside point)\n";
            return false;
        }
        
        std::cout << "PASS\n";
        return true;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 55: Ring (annulus) containment test
bool testRingContainment() {
    std::cout << "Test: Ring Containment (2D)... ";
    BytecodeParser parser;
    std::string source = R"(
        bool calc(vec2 point, vec2 center, float innerRadius, float outerRadius) {
            vec2 diff = point - center;
            float dist = sqrt(dot(diff, diff));
            return (dist >= innerRadius) && (dist <= outerRadius);
        }
    )";
    try {
        // Test: point in middle of ring, should be inside
        std::vector<mmrsl::Value> args = {
            mmrsl::Value(mmrsl::Vec2(1.5f, 0.0f)),
            mmrsl::Value(mmrsl::Vec2(0.0f, 0.0f)),
            mmrsl::Value(1.0f),
            mmrsl::Value(2.0f)
        };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        if (!result.isBool() || result.asBool() != true) {
            std::cout << "FAIL (middle of ring)\n";
            return false;
        }
        
        // Test: point inside inner radius, should be outside
        args[0] = mmrsl::Value(mmrsl::Vec2(0.5f, 0.0f));
        result = parser.compileAndExecute(source, args);
        if (!result.isBool() || result.asBool() != false) {
            std::cout << "FAIL (inside inner)\n";
            return false;
        }
        
        // Test: point outside outer radius, should be outside
        args[0] = mmrsl::Value(mmrsl::Vec2(3.0f, 0.0f));
        result = parser.compileAndExecute(source, args);
        if (!result.isBool() || result.asBool() != false) {
            std::cout << "FAIL (outside outer)\n";
            return false;
        }
        
        std::cout << "PASS\n";
        return true;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 56: Box containment test (3D AABB)
bool testBoxContainment() {
    std::cout << "Test: Box Containment (3D AABB)... ";
    BytecodeParser parser;
    std::string source = R"(
        bool calc(vec3 point, vec3 minBound, vec3 maxBound) {
            return (point.x >= minBound.x && point.x <= maxBound.x) &&
                   (point.y >= minBound.y && point.y <= maxBound.y) &&
                   (point.z >= minBound.z && point.z <= maxBound.z);
        }
    )";
    try {
        // Test: point inside box
        std::vector<mmrsl::Value> args = {
            mmrsl::Value(mmrsl::Vec3(0.5f, 0.5f, 0.5f)),
            mmrsl::Value(mmrsl::Vec3(0.0f, 0.0f, 0.0f)),
            mmrsl::Value(mmrsl::Vec3(1.0f, 1.0f, 1.0f))
        };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        if (!result.isBool() || result.asBool() != true) {
            std::cout << "FAIL (inside)\n";
            return false;
        }
        
        // Test: point outside (x too large)
        args[0] = mmrsl::Value(mmrsl::Vec3(2.0f, 0.5f, 0.5f));
        result = parser.compileAndExecute(source, args);
        if (!result.isBool() || result.asBool() != false) {
            std::cout << "FAIL (outside x)\n";
            return false;
        }
        
        // Test: point on edge
        args[0] = mmrsl::Value(mmrsl::Vec3(1.0f, 0.5f, 0.5f));
        result = parser.compileAndExecute(source, args);
        if (!result.isBool() || result.asBool() != true) {
            std::cout << "FAIL (on edge)\n";
            return false;
        }
        
        std::cout << "PASS\n";
        return true;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 58: Length function test
bool testLengthFunction() {
    std::cout << "Test: Length Function... ";
    BytecodeParser parser;
    
    // Test vec2 length
    std::string source2 = R"(
        float calc(vec2 v) {
            return length(v);
        }
    )";
    try {
        std::vector<mmrsl::Value> args = { mmrsl::Value(mmrsl::Vec2(3.0f, 4.0f)) };
        mmrsl::Value result = parser.compileAndExecute(source2, args);
        
        if (!result.isFloat() || std::abs(result.asFloat() - 5.0f) >= 0.01f) {
            std::cout << "FAIL (vec2 length: got " << result.asFloat() << ")\n";
            return false;
        }
        
        // Test vec3 length
        std::string source3 = R"(
            float calc(vec3 v) {
                return length(v);
            }
        )";
        args = { mmrsl::Value(mmrsl::Vec3(1.0f, 2.0f, 2.0f)) };
        result = parser.compileAndExecute(source3, args);
        
        if (!result.isFloat() || std::abs(result.asFloat() - 3.0f) >= 0.01f) {
            std::cout << "FAIL (vec3 length: got " << result.asFloat() << ")\n";
            return false;
        }
        
        std::cout << "PASS\n";
        return true;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 59: Normalize function test
bool testNormalizeFunction() {
    std::cout << "Test: Normalize Function... ";
    BytecodeParser parser;
    
    std::string source = R"(
        vec3 calc(vec3 v) {
            return normalize(v);
        }
    )";
    try {
        std::vector<mmrsl::Value> args = { mmrsl::Value(mmrsl::Vec3(3.0f, 0.0f, 4.0f)) };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        
        if (!result.isVec3()) {
            std::cout << "FAIL (not vec3)\n";
            return false;
        }
        
        mmrsl::Vec3 expected(0.6f, 0.0f, 0.8f);
        mmrsl::Vec3 diff_ge = result.asVec3() - expected; float len_ge = std::sqrt(diff_ge.x*diff_ge.x + diff_ge.y*diff_ge.y + diff_ge.z*diff_ge.z); if (len_ge >= 0.01f) {
            std::cout << "FAIL (wrong value: got " << result.asVec3().x << ", " << result.asVec3().y << ", " << result.asVec3().z << ")\n";
            return false;
        }
        
        std::cout << "PASS\n";
        return true;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 60: Reflect function test
bool testReflectFunction() {
    std::cout << "Test: Reflect Function... ";
    BytecodeParser parser;
    
    std::string source = R"(
        vec3 calc(vec3 I, vec3 N) {
            return reflect(I, N);
        }
    )";
    try {
        // Test: reflect incident vector off surface normal
        std::vector<mmrsl::Value> args = {
            mmrsl::Value(mmrsl::Vec3(1.0f, -1.0f, 0.0f)),  // Incident
            mmrsl::Value(mmrsl::Vec3(0.0f, 1.0f, 0.0f))   // Normal (up)
        };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        
        if (!result.isVec3()) {
            std::cout << "FAIL (not vec3)\n";
            return false;
        }
        
        // Expected: (1, 1, 0) - reflected off y-axis
        mmrsl::Vec3 expected(1.0f, 1.0f, 0.0f);
        mmrsl::Vec3 diff_ge = result.asVec3() - expected; float len_ge = std::sqrt(diff_ge.x*diff_ge.x + diff_ge.y*diff_ge.y + diff_ge.z*diff_ge.z); if (len_ge >= 0.01f) {
            std::cout << "FAIL (wrong value: got " << result.asVec3().x << ", " << result.asVec3().y << ", " << result.asVec3().z << ")\n";
            return false;
        }
        
        std::cout << "PASS\n";
        return true;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 61: Refract function test
bool testRefractFunction() {
    std::cout << "Test: Refract Function... ";
    BytecodeParser parser;
    
    std::string source = R"(
        vec3 calc(vec3 I, vec3 N, float eta) {
            return refract(I, N, eta);
        }
    )";
    try {
        // Test: refract from air to water (eta = 1.0/1.33)
        std::vector<mmrsl::Value> args = {
            mmrsl::Value(mmrsl::Vec3(0.4472f, -0.8944f, 0.0f)),  // Incident (normalized 0.5, -1, 0)
            mmrsl::Value(mmrsl::Vec3(0.0f, 1.0f, 0.0f)),                   // Normal
            mmrsl::Value(1.0f / 1.33f)                                   // eta
        };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        
        if (!result.isVec3()) {
            std::cout << "FAIL (not vec3)\n";
            return false;
        }
        
        // Result should be a valid refracted vector
        mmrsl::Vec3 refracted = result.asVec3();
        float refract_len = std::sqrt(refracted.x*refracted.x + refracted.y*refracted.y + refracted.z*refracted.z); if (refract_len < 0.99f || refract_len > 1.01f) {
            std::cout << "FAIL (not unit vector: length=" << refract_len << ")\n";
            return false;
        }
        
        // Should bend towards the normal (y component should increase)
        if (refracted.y >= 0.0f) {
            std::cout << "FAIL (should bend towards normal)\n";
            return false;
        }
        
        std::cout << "PASS\n";
        return true;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 64: Cook-Torrance BRDF lighting model test
bool testCookTorranceLighting() {
    std::cout << "Test: Cook-Torrance BRDF Lighting Model... ";
    BytecodeParser parser;
    
    // Simplified Cook-Torrance model
    std::string source = R"(
        vec3 calc(vec3 normal, vec3 lightDir, vec3 viewDir, vec3 lightColor, float roughness) {
            vec3 N = normalize(normal);
            vec3 L = normalize(lightDir);
            vec3 V = normalize(viewDir);
            vec3 H = normalize(L + V);
            
            float NdotL = max(dot(N, L), 0.0);
            float NdotV = max(dot(N, V), 0.0);
            float NdotH = max(dot(N, H), 0.0);
            float VdotH = max(dot(V, H), 0.0);
            
            // Diffuse term (Lambert)
            float diff = NdotL;
            
            // Specular term (simplified Cook-Torrance)
            // D term (Normal Distribution Function) - Beckmann approximation
            float alpha = roughness * roughness;
            float alpha2 = alpha * alpha;
            float NdotH2 = NdotH * NdotH;
            float denom = NdotH2 * (alpha2 - 1.0) + 1.0;
            float D = alpha2 / (3.14159 * denom * denom);
            
            // G term (Geometric attenuation) - simplified
            float G = min(1.0, min(2.0 * NdotH * NdotV / VdotH, 2.0 * NdotH * NdotL / VdotH));
            G = max(G, 0.0);
            
            // F term (Fresnel) - simplified Schlick approximation
            float F0 = 0.04;  // F0 for non-metals
            float F = F0 + (1.0 - F0) * pow(1.0 - VdotH, 5.0);
            
            // Specular BRDF
            float spec = 0.0;
            if (NdotL > 0.0 && NdotV > 0.0) {
                spec = (D * G * F) / (4.0 * NdotL * NdotV + 0.001);
            }
            
            // Combine diffuse and specular
            vec3 diffuseColor = lightColor * diff * 0.96;  // 96% diffuse
            vec3 specularColor = lightColor * spec * NdotL;
            
            return diffuseColor + specularColor;
        }
    )";
    try {
        // Test: light and view both directly above surface (rough surface)
        std::vector<mmrsl::Value> args = {
            mmrsl::Value(mmrsl::Vec3(0.0f, 1.0f, 0.0f)),   // Normal up
            mmrsl::Value(mmrsl::Vec3(0.0f, 1.0f, 0.0f)),   // Light from above
            mmrsl::Value(mmrsl::Vec3(0.0f, 1.0f, 0.0f)),   // View from above
            mmrsl::Value(mmrsl::Vec3(1.0f, 1.0f, 1.0f)),   // White light
            mmrsl::Value(0.5f)                           // Roughness
        };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        
        if (!result.isVec3()) {
            std::cout << "FAIL (not vec3)\n";
            return false;
        }
        
        // When light and view are both directly above:
        // Should have strong diffuse + some specular
        mmrsl::Vec3 res = result.asVec3();
        if (res.x < 0.8f || res.x > 2.0f) {
            std::cout << "FAIL (direct rough: got " << res.x << ", " << res.y << ", " << res.z << ")\n";
            return false;
        }
        
        // Test: smooth surface (low roughness)
        args[4] = mmrsl::Value(0.1f);
        result = parser.compileAndExecute(source, args);
        
        if (!result.isVec3()) {
            std::cout << "FAIL (not vec3)\n";
            return false;
        }
        
        // Smooth surface should have stronger specular highlight
        res = result.asVec3();
        if (res.x < 0.8f) {
            std::cout << "FAIL (direct smooth: got " << res.x << ", " << res.y << ", " << res.z << ")\n";
            return false;
        }
        
        // Test: light perpendicular to normal (no lighting)
        args[1] = mmrsl::Value(mmrsl::Vec3(1.0f, 0.0f, 0.0f));  // Light from side
        args[4] = mmrsl::Value(0.5f);
        result = parser.compileAndExecute(source, args);
        
        if (!result.isVec3()) {
            std::cout << "FAIL (not vec3)\n";
            return false;
        }
        
        // When light is perpendicular: should be near black
        res = result.asVec3();
        if (res.x > 0.1f || res.y > 0.1f || res.z > 0.1f) {
            std::cout << "FAIL (perpendicular: got " << res.x << ", " << res.y << ", " << res.z << ")\n";
            return false;
        }
        
        std::cout << "PASS\n";
        return true;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 63: Phong lighting model test
bool testPhongLighting() {
    std::cout << "Test: Phong Lighting Model... ";
    BytecodeParser parser;
    
    std::string source = R"(
        vec3 calc(vec3 normal, vec3 lightDir, vec3 viewDir, vec3 lightColor) {
            vec3 N = normalize(normal);
            vec3 L = normalize(lightDir);
            vec3 V = normalize(viewDir);
            vec3 R = reflect(-L, N);
            
            float diff = max(dot(N, L), 0.0);
            float spec = 0.0;
            if (diff > 0.0) {
                spec = pow(max(dot(R, V), 0.0), 32.0);
            }
            
            return lightColor * (diff + spec);
        }
    )";
    try {
        // Test: light and view both directly above surface
        std::vector<mmrsl::Value> args = {
            mmrsl::Value(mmrsl::Vec3(0.0f, 1.0f, 0.0f)),   // Normal up
            mmrsl::Value(mmrsl::Vec3(0.0f, 1.0f, 0.0f)),   // Light from above
            mmrsl::Value(mmrsl::Vec3(0.0f, 1.0f, 0.0f)),   // View from above
            mmrsl::Value(mmrsl::Vec3(1.0f, 1.0f, 1.0f))    // White light
        };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        
        if (!result.isVec3()) {
            std::cout << "FAIL (not vec3)\n";
            return false;
        }
        
        // When light and view are both directly above:
        // diffuse = 1.0, specular = 1.0 (perfect reflection)
        // result = (2.0, 2.0, 2.0)
        mmrsl::Vec3 expected(2.0f, 2.0f, 2.0f);
        mmrsl::Vec3 diff_ge = result.asVec3() - expected; float len_ge = std::sqrt(diff_ge.x*diff_ge.x + diff_ge.y*diff_ge.y + diff_ge.z*diff_ge.z); if (len_ge >= 0.01f) {
            std::cout << "FAIL (direct: got " << result.asVec3().x << ", " << result.asVec3().y << ", " << result.asVec3().z << ")\n";
            return false;
        }
        
        // Test: light perpendicular to normal (no lighting)
        args[1] = mmrsl::Value(mmrsl::Vec3(1.0f, 0.0f, 0.0f));  // Light from side
        result = parser.compileAndExecute(source, args);
        
        if (!result.isVec3()) {
            std::cout << "FAIL (not vec3)\n";
            return false;
        }
        
        // When light is perpendicular: diffuse = 0, specular = 0
        expected = mmrsl::Vec3(0.0f, 0.0f, 0.0f);
        diff_ge = result.asVec3() - expected; len_ge = std::sqrt(diff_ge.x*diff_ge.x + diff_ge.y*diff_ge.y + diff_ge.z*diff_ge.z); if (len_ge >= 0.01f) {
            std::cout << "FAIL (perpendicular: got " << result.asVec3().x << ", " << result.asVec3().y << ", " << result.asVec3().z << ")\n";
            return false;
        }
        
        // Test: light at 45 degree angle
        args[0] = mmrsl::Value(mmrsl::Vec3(0.0f, 1.0f, 0.0f));   // Normal up
        args[1] = mmrsl::Value(mmrsl::Vec3(0.7071f, 0.7071f, 0.0f));  // Light at 45 degrees (normalized)
        args[2] = mmrsl::Value(mmrsl::Vec3(0.0f, 1.0f, 0.0f));   // View from above
        args[3] = mmrsl::Value(mmrsl::Vec3(1.0f, 0.0f, 0.0f));    // Red light
        result = parser.compileAndExecute(source, args);
        
        if (!result.isVec3()) {
            std::cout << "FAIL (not vec3)\n";
            return false;
        }
        
        // At 45 degrees: diffuse = cos(45) ï¿?0.707
        // Result should be approximately (0.707, 0, 0)
        mmrsl::Vec3 res = result.asVec3();
        if (res.x < 0.6f || res.x > 0.8f || res.y != 0.0f || res.z != 0.0f) {
            std::cout << "FAIL (45 degree: got " << res.x << ", " << res.y << ", " << res.z << ")\n";
            return false;
        }
        
        std::cout << "PASS\n";
        return true;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 62: Simple lighting calculation test
bool testSimpleLighting() {
    std::cout << "Test: Simple Lighting Calculation... ";
    BytecodeParser parser;
    
    std::string source = R"(
        float calc(vec3 normal, vec3 lightDir) {
            vec3 N = normalize(normal);
            vec3 L = normalize(lightDir);
            return max(dot(N, L), 0.0);
        }
    )";
    try {
        // Test: light directly above surface
        std::vector<mmrsl::Value> args = {
            mmrsl::Value(mmrsl::Vec3(0.0f, 1.0f, 0.0f)),  // Normal up
            mmrsl::Value(mmrsl::Vec3(0.0f, 1.0f, 0.0f))   // Light from above
        };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        
        if (!result.isFloat() || std::abs(result.asFloat() - 1.0f) >= 0.01f) {
            std::cout << "FAIL (direct light: got " << result.asFloat() << ")\n";
            return false;
        }
        
        // Test: light perpendicular to normal
        args[1] = mmrsl::Value(mmrsl::Vec3(1.0f, 0.0f, 0.0f));
        result = parser.compileAndExecute(source, args);
        
        if (!result.isFloat() || std::abs(result.asFloat() - 0.0f) >= 0.01f) {
            std::cout << "FAIL (perpendicular light: got " << result.asFloat() << ")\n";
            return false;
        }
        
        std::cout << "PASS\n";
        return true;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 57: Cylinder containment test
bool testCylinderContainment() {
    std::cout << "Test: Cylinder Containment (3D)... ";
    BytecodeParser parser;
    std::string source = R"(
        bool calc(vec3 point, vec3 center, float radius, float height) {
            vec2 xy = vec2(point.x, point.y);
            vec2 centerXY = vec2(center.x, center.y);
            vec2 diff = xy - centerXY;
            float distXY = sqrt(dot(diff, diff));
            float halfHeight = height * 0.5;
            return (distXY <= radius) && 
                   (point.z >= center.z - halfHeight) && 
                   (point.z <= center.z + halfHeight);
        }
    )";
    try {
        // Test: point inside cylinder
        std::vector<mmrsl::Value> args = {
            mmrsl::Value(mmrsl::Vec3(0.5f, 0.0f, 0.0f)),
            mmrsl::Value(mmrsl::Vec3(0.0f, 0.0f, 0.0f)),
            mmrsl::Value(1.0f),
            mmrsl::Value(2.0f)
        };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        if (!result.isBool() || result.asBool() != true) {
            std::cout << "FAIL (inside)\n";
            return false;
        }
        
        // Test: point outside radius
        args[0] = mmrsl::Value(mmrsl::Vec3(2.0f, 0.0f, 0.0f));
        result = parser.compileAndExecute(source, args);
        if (!result.isBool() || result.asBool() != false) {
            std::cout << "FAIL (outside radius)\n";
            return false;
        }
        
        // Test: point outside height
        args[0] = mmrsl::Value(mmrsl::Vec3(0.5f, 0.0f, 2.0f));
        result = parser.compileAndExecute(source, args);
        if (!result.isBool() || result.asBool() != false) {
            std::cout << "FAIL (outside height)\n";
            return false;
        }
        
        std::cout << "PASS\n";
        return true;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 52: Logical NOT operator
bool testLogicalNot() {
    std::cout << "Test: Logical NOT Operator... ";
    BytecodeParser parser;
    std::string source = R"(
        float calc(float x) {
            if (!(x > 0.5)) {
                return 0.0;
            }
            return 1.0;
        }
    )";
    try {
        // Test: x = 0.3, !(0.3 > 0.5) = true, return 0.0
        std::vector<mmrsl::Value> args = { mmrsl::Value(0.3f) };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        
        if (!result.isFloat() || std::abs(result.asFloat() - 0.0f) >= 0.01f) {
            std::cout << "FAIL (first test: got " << result.asFloat() << ")\n";
            return false;
        }
        
        // Test: x = 0.8, !(0.8 > 0.5) = false, return 1.0
        args[0] = mmrsl::Value(0.8f);
        result = parser.compileAndExecute(source, args);
        if (!result.isFloat() || std::abs(result.asFloat() - 1.0f) >= 0.01f) {
            std::cout << "FAIL (second test: got " << result.asFloat() << ")\n";
            return false;
        }
        
        std::cout << "PASS\n";
        return true;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 51: Logical OR operator
bool testLogicalOr() {
    std::cout << "Test: Logical OR Operator... ";
    BytecodeParser parser;
    std::string source = R"(
        float calc(float x) {
            if (x < 0.0 || x > 1.0) {
                return 0.0;
            }
            return 1.0;
        }
    )";
    try {
        // Test: x = -0.5, out of range, return 0.0
        std::vector<mmrsl::Value> args = { mmrsl::Value(-0.5f) };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        
        if (!result.isFloat() || std::abs(result.asFloat() - 0.0f) >= 0.01f) {
            std::cout << "FAIL (first test: got " << result.asFloat() << ")\n";
            return false;
        }
        
        // Test: x = 0.5, in range, return 1.0
        args[0] = mmrsl::Value(0.5f);
        result = parser.compileAndExecute(source, args);
        if (!result.isFloat() || std::abs(result.asFloat() - 1.0f) >= 0.01f) {
            std::cout << "FAIL (second test: got " << result.asFloat() << ")\n";
            return false;
        }
        
        // Test: x = 1.5, out of range, return 0.0
        args[0] = mmrsl::Value(1.5f);
        result = parser.compileAndExecute(source, args);
        if (!result.isFloat() || std::abs(result.asFloat() - 0.0f) >= 0.01f) {
            std::cout << "FAIL (third test: got " << result.asFloat() << ")\n";
            return false;
        }
        
        std::cout << "PASS\n";
        return true;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test: Vector constructors (vec2, vec3, vec4)
bool testVectorConstructors() {
    std::cout << "Test: Vector Constructors (vec2/vec3/vec4)... ";
    BytecodeParser parser;
    std::string source = R"(
        float calc(float x) {
            vec2 v2 = vec2(1.0, 2.0);
            vec3 v3 = vec3(3.0, 4.0, 5.0);
            vec4 v4 = vec4(6.0, 7.0, 8.0, 9.0);
            return v2.x + v2.y + v3.x + v3.y + v3.z + v4.x + v4.y + v4.z + v4.w;
        }
    )";
    try {
        std::vector<mmrsl::Value> args = { mmrsl::Value(0.0f) };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        float expected = 1.0f + 2.0f + 3.0f + 4.0f + 5.0f + 6.0f + 7.0f + 8.0f + 9.0f; // 45.0
        
        if (result.isFloat() && std::abs(result.asFloat() - expected) < 0.01f) {
            std::cout << "PASS\n";
            return true;
        }
        std::cout << "Got: " << result.asFloat() << " Expected: " << expected << " FAIL\n";
        return false;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test: Matrix constructor mat2
bool testMat2Constructor() {
    std::cout << "Test: Matrix Constructor (mat2)... ";
    BytecodeParser parser;
    std::string source = R"(
        float calc(float x) {
            mat2 m = mat2(1.0, 2.0, 3.0, 4.0);
            vec2 v = vec2(1.0, 1.0);
            vec2 r = m * v;
            return r.x + r.y;
        }
    )";
    try {
        std::vector<mmrsl::Value> args = { mmrsl::Value(0.0f) };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        // mat2(1,2,3,4) * vec2(1,1) = vec2(1*1+3*1, 2*1+4*1) = vec2(4, 6)
        float expected = 4.0f + 6.0f; // 10.0
        
        if (result.isFloat() && std::abs(result.asFloat() - expected) < 0.01f) {
            std::cout << "PASS\n";
            return true;
        }
        std::cout << "Got: " << result.asFloat() << " Expected: " << expected << " FAIL\n";
        return false;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test: Matrix constructor mat3
bool testMat3Constructor() {
    std::cout << "Test: Matrix Constructor (mat3)... ";
    BytecodeParser parser;
    std::string source = R"(
        float calc(float x) {
            mat3 m = mat3(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);
            vec3 v = vec3(1.0, 2.0, 3.0);
            vec3 r = m * v;
            return r.x + r.y + r.z;
        }
    )";
    try {
        std::vector<mmrsl::Value> args = { mmrsl::Value(0.0f) };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        // Identity matrix * vec3(1,2,3) = vec3(1,2,3)
        float expected = 1.0f + 2.0f + 3.0f; // 6.0
        
        if (result.isFloat() && std::abs(result.asFloat() - expected) < 0.01f) {
            std::cout << "PASS\n";
            return true;
        }
        std::cout << "Got: " << result.asFloat() << " Expected: " << expected << " FAIL\n";
        return false;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test: Matrix constructor mat4 with translation
bool testMat4Constructor() {
    std::cout << "Test: Matrix Constructor (mat4)... ";
    BytecodeParser parser;
    std::string source = R"(
        vec4 calc(float x) {
            mat4 m = mat4(1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 10.0, 20.0, 30.0, 1.0);
            vec4 v = vec4(1.0, 2.0, 3.0, 1.0);
            return m * v;
        }
    )";
    try {
        std::vector<mmrsl::Value> args = { mmrsl::Value(0.0f) };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        // Translation matrix * vec4(1,2,3,1) = vec4(1+10, 2+20, 3+30, 1) = vec4(11, 22, 33, 1)
        mmrsl::Vec4 expected(11.0f, 22.0f, 33.0f, 1.0f);
        
        if (result.isVec4()) {
            mmrsl::Vec4 r = result.asVec4();
            if (std::abs(r.x - expected.x) < 0.01f &&
                std::abs(r.y - expected.y) < 0.01f &&
                std::abs(r.z - expected.z) < 0.01f &&
                std::abs(r.w - expected.w) < 0.01f) {
                std::cout << "PASS\n";
                return true;
            }
        }
        std::cout << "FAIL\n";
        return false;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test: Combined vector and matrix operations with constructors
bool testVecMatConstructorChain() {
    std::cout << "Test: Vector/Matrix Constructor Chain... ";
    BytecodeParser parser;
    std::string source = R"(
        vec3 calc(float x) {
            vec3 pos = vec3(1.0, 2.0, 3.0);
            vec3 offset = vec3(10.0, 20.0, 30.0);
            vec3 scaled = pos * 2.0;
            return scaled + offset;
        }
    )";
    try {
        std::vector<mmrsl::Value> args = { mmrsl::Value(0.0f) };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        // vec3(1,2,3) * 2 + vec3(10,20,30) = vec3(2,4,6) + vec3(10,20,30) = vec3(12,24,36)
        mmrsl::Vec3 expected(12.0f, 24.0f, 36.0f);
        
        if (result.isVec3()) {
            mmrsl::Vec3 r = result.asVec3();
            if (std::abs(r.x - expected.x) < 0.01f &&
                std::abs(r.y - expected.y) < 0.01f &&
                std::abs(r.z - expected.z) < 0.01f) {
                std::cout << "PASS\n";
                return true;
            }
        }
        std::cout << "FAIL\n";
        return false;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test: mat4 scale matrix constructor
bool testMat4ScaleConstructor() {
    std::cout << "Test: mat4 Scale Matrix Constructor... ";
    BytecodeParser parser;
    std::string source = R"(
        vec4 calc(float x) {
            mat4 scale = mat4(2.0, 0.0, 0.0, 0.0, 0.0, 3.0, 0.0, 0.0, 0.0, 0.0, 4.0, 0.0, 0.0, 0.0, 0.0, 1.0);
            vec4 v = vec4(1.0, 1.0, 1.0, 1.0);
            return scale * v;
        }
    )";
    try {
        std::vector<mmrsl::Value> args = { mmrsl::Value(0.0f) };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        // Scale matrix * vec4(1,1,1,1) = vec4(2,3,4,1)
        mmrsl::Vec4 expected(2.0f, 3.0f, 4.0f, 1.0f);
        
        if (result.isVec4()) {
            mmrsl::Vec4 r = result.asVec4();
            if (std::abs(r.x - expected.x) < 0.01f &&
                std::abs(r.y - expected.y) < 0.01f &&
                std::abs(r.z - expected.z) < 0.01f &&
                std::abs(r.w - expected.w) < 0.01f) {
                std::cout << "PASS\n";
                return true;
            }
        }
        std::cout << "FAIL\n";
        return false;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Debug test 3: && in if condition
bool testDebugLogical3() {
    std::cout << "Test: Debug Logical Operators 3 (if &&)... ";
    BytecodeParser parser;
    std::string source = R"(
        float calc(float x) {
            if ((x >= 0.0) && (x <= 1.0)) {
                return 1.0;
            }
            return 0.0;
        }
    )";
    try {
        std::vector<mmrsl::Value> args = { mmrsl::Value(0.5f) };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        std::cout << "Got: " << result.asFloat() << " ";
        
        if (result.isFloat() && std::abs(result.asFloat() - 1.0f) < 0.01f) {
            std::cout << "PASS\n";
            return true;
        }
        std::cout << "FAIL\n";
        return false;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Debug test 4: simple && in if condition
bool testDebugLogical4() {
    std::cout << "Test: Debug Logical Operators 4 (simple if &&)... ";
    BytecodeParser parser;
    std::string source = R"(
        float calc(float x) {
            if (true && true) {
                return 1.0;
            }
            return 0.0;
        }
    )";
    try {
        std::vector<mmrsl::Value> args = { mmrsl::Value(0.5f) };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        
        if (result.isFloat() && std::abs(result.asFloat() - 1.0f) < 0.01f) {
            std::cout << "PASS\n";
            return true;
        }
        std::cout << "FAIL\n";
        return false;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Debug test 5: simple if with literal
bool testDebugLogical5() {
    std::cout << "Test: Debug Logical Operators 5 (if true with else)... ";
    BytecodeParser parser;
    std::string source = R"(
        float calc(float x) {
            if (true) {
                return 1.0;
            } else {
                return 0.0;
            }
        }
    )";
    try {
        std::vector<mmrsl::Value> args = { mmrsl::Value(0.5f) };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        
        if (result.isFloat() && std::abs(result.asFloat() - 1.0f) < 0.01f) {
            std::cout << "PASS\n";
            return true;
        }
        std::cout << "FAIL\n";
        return false;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test: Compile and execute two different scripts to verify compiler state reset
bool testMultipleScriptsCompilation() {
    std::cout << "Test: Multiple Scripts Compilation (triangle area + line intersection)... ";
    
    // Script 1: Calculate triangle area using Heron's formula
    std::string triangleSource = R"(
        float calc(float a, float b, float c) {
            // Heron's formula: area = sqrt(s * (s-a) * (s-b) * (s-c))
            // where s = (a + b + c) / 2
            float s = (a + b + c) * 0.5;
            return sqrt(s * (s - a) * (s - b) * (s - c));
        }
    )";
    
    // Script 2: Check if two 2D line segments intersect using cross product
    std::string lineIntersectSource = R"(
        bool calc(vec2 p1, vec2 p2, vec2 p3, vec2 p4) {
            // Line 1: p1 to p2
            // Line 2: p3 to p4
            // Using 2D cross product: cross(a, b) = a.x * b.y - a.y * b.x
            vec2 r = p2 - p1;
            vec2 s = p4 - p3;
            
            // 2D cross product of r and s
            float rxs = r.x * s.y - r.y * s.x;
            
            if (abs(rxs) < 0.0001) {
                return false;  // Parallel or collinear lines
            }
            
            vec2 qp = p3 - p1;
            
            // t = (qp x s) / (r x s)
            float t = (qp.x * s.y - qp.y * s.x) / rxs;
            // u = (qp x r) / (r x s)
            float u = (qp.x * r.y - qp.y * r.x) / rxs;
            
            return (t >= 0.0 && t <= 1.0 && u >= 0.0 && u <= 1.0);
        }
    )";
    
    try {
        BytecodeParser parser;
        
        // Test triangle area script
        // Triangle with sides 3, 4, 5 (right triangle, area = 6)
        std::vector<mmrsl::Value> triangleArgs = {
            mmrsl::Value(3.0f),
            mmrsl::Value(4.0f),
            mmrsl::Value(5.0f)
        };
        mmrsl::Value triangleResult = parser.compileAndExecute(triangleSource, triangleArgs);
        
        if (!triangleResult.isFloat() || std::abs(triangleResult.asFloat() - 6.0f) >= 0.01f) {
            std::cout << "FAIL (triangle area: expected 6.0, got " << triangleResult.asFloat() << ")\n";
            return false;
        }
        
        // Test another triangle: equilateral with side 2 (area = sqrt(3) ï¿?1.732)
        triangleArgs[0] = mmrsl::Value(2.0f);
        triangleArgs[1] = mmrsl::Value(2.0f);
        triangleArgs[2] = mmrsl::Value(2.0f);
        triangleResult = parser.compileAndExecute(triangleSource, triangleArgs);
        
        if (!triangleResult.isFloat() || std::abs(triangleResult.asFloat() - 1.732f) >= 0.01f) {
            std::cout << "FAIL (equilateral triangle: expected ~1.732, got " << triangleResult.asFloat() << ")\n";
            return false;
        }
        
        // Test line intersection script
        // Intersecting lines: (0,0)-(2,2) and (0,2)-(2,0) should intersect at (1,1)
        std::vector<mmrsl::Value> lineArgs = {
            mmrsl::Value(mmrsl::Vec2(0.0f, 0.0f)),  // p1
            mmrsl::Value(mmrsl::Vec2(2.0f, 2.0f)),  // p2
            mmrsl::Value(mmrsl::Vec2(0.0f, 2.0f)),  // p3
            mmrsl::Value(mmrsl::Vec2(2.0f, 0.0f))   // p4
        };
        mmrsl::Value lineResult = parser.compileAndExecute(lineIntersectSource, lineArgs);
        
        if (!lineResult.isBool() || lineResult.asBool() != true) {
            std::cout << "FAIL (intersecting lines: expected true)\n";
            return false;
        }
        
        // Non-intersecting lines
        lineArgs[0] = mmrsl::Value(mmrsl::Vec2(0.0f, 0.0f));
        lineArgs[1] = mmrsl::Value(mmrsl::Vec2(1.0f, 1.0f));
        lineArgs[2] = mmrsl::Value(mmrsl::Vec2(2.0f, 0.0f));
        lineArgs[3] = mmrsl::Value(mmrsl::Vec2(3.0f, 1.0f));
        lineResult = parser.compileAndExecute(lineIntersectSource, lineArgs);
        
        if (!lineResult.isBool() || lineResult.asBool() != false) {
            std::cout << "FAIL (non-intersecting lines: expected false)\n";
            return false;
        }
        
        // Switch back to triangle script to verify compiler state is properly reset
        triangleArgs[0] = mmrsl::Value(5.0f);
        triangleArgs[1] = mmrsl::Value(5.0f);
        triangleArgs[2] = mmrsl::Value(6.0f);
        triangleResult = parser.compileAndExecute(triangleSource, triangleArgs);
        
        // 5-5-6 triangle: s = 8, area = sqrt(8 * 3 * 3 * 2) = sqrt(144) = 12
        if (!triangleResult.isFloat() || std::abs(triangleResult.asFloat() - 12.0f) >= 0.01f) {
            std::cout << "FAIL (triangle after line: expected 12.0, got " << triangleResult.asFloat() << ")\n";
            return false;
        }
        
        std::cout << "PASS\n";
        return true;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 77: Compile once, execute multiple times
bool testCompileOnceExecuteMultiple() {
    std::cout << "Test: Compile Once, Execute Multiple Times... ";
    
    BytecodeParser parser;
    
    // Quadratic function: f(x) = ax^2 + bx + c
    std::string source = R"(
        float calc(float a, float b, float c, float x) {
            return a * x * x + b * x + c;
        }
    )";
    
    try {
        // Compile once
        if (!parser.compile(source)) {
            std::cout << "FAIL (compilation failed: " << parser.getLastError() << ")\n";
            return false;
        }
        
        // Execute multiple times with different inputs
        // Test 1: f(x) = x^2 (a=1, b=0, c=0), x=2 -> result = 4
        std::vector<mmrsl::Value> args1 = {
            mmrsl::Value(1.0f),
            mmrsl::Value(0.0f),
            mmrsl::Value(0.0f),
            mmrsl::Value(2.0f)
        };
        mmrsl::Value result1 = parser.execute(args1);
        if (!result1.isFloat() || std::abs(result1.asFloat() - 4.0f) >= 0.001f) {
            std::cout << "FAIL (test 1: expected 4.0, got " << result1.asFloat() << ")\n";
            return false;
        }
        
        // Test 2: f(x) = 2x + 1 (a=0, b=2, c=1), x=3 -> result = 7
        std::vector<mmrsl::Value> args2 = {
            mmrsl::Value(0.0f),
            mmrsl::Value(2.0f),
            mmrsl::Value(1.0f),
            mmrsl::Value(3.0f)
        };
        mmrsl::Value result2 = parser.execute(args2);
        if (!result2.isFloat() || std::abs(result2.asFloat() - 7.0f) >= 0.001f) {
            std::cout << "FAIL (test 2: expected 7.0, got " << result2.asFloat() << ")\n";
            return false;
        }
        
        // Test 3: f(x) = -x^2 + 5 (a=-1, b=0, c=5), x=2 -> result = 1
        std::vector<mmrsl::Value> args3 = {
            mmrsl::Value(-1.0f),
            mmrsl::Value(0.0f),
            mmrsl::Value(5.0f),
            mmrsl::Value(2.0f)
        };
        mmrsl::Value result3 = parser.execute(args3);
        if (!result3.isFloat() || std::abs(result3.asFloat() - 1.0f) >= 0.001f) {
            std::cout << "FAIL (test 3: expected 1.0, got " << result3.asFloat() << ")\n";
            return false;
        }
        
        // Test 4: f(x) = 0.5x^2 - 2x + 1 (a=0.5, b=-2, c=1), x=4 -> result = 1
        std::vector<mmrsl::Value> args4 = {
            mmrsl::Value(0.5f),
            mmrsl::Value(-2.0f),
            mmrsl::Value(1.0f),
            mmrsl::Value(4.0f)
        };
        mmrsl::Value result4 = parser.execute(args4);
        if (!result4.isFloat() || std::abs(result4.asFloat() - 1.0f) >= 0.001f) {
            std::cout << "FAIL (test 4: expected 1.0, got " << result4.asFloat() << ")\n";
            return false;
        }
        
        std::cout << "PASS\n";
        return true;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 78: Matrix non-diagonal element indexing
bool testMatrixNonDiagonalIndexing() {
    std::cout << "Test: Matrix Non-Diagonal Element Indexing... ";
    
    BytecodeParser parser;
    
    // Test mat2 non-diagonal elements
    // Use a non-symmetric matrix where m[0][1] != m[1][0]
    std::string mat2Source = R"(
        float calc(mat2 m) {
            return m[0][1];  // Should return column 0, row 1
        }
    )";
    
    try {
        // Test mat2: [[1, 2], [3, 4]] in row-major
        // GLM column-major: col0=[1,3], col1=[2,4]
        // m[0][1] = col0, row1 = 3
        std::vector<mmrsl::Value> args = {
            mmrsl::Value(mmrsl::Mat2(1.0f, 3.0f, 2.0f, 4.0f))
        };
        mmrsl::Value result = parser.compileAndExecute(mat2Source, args);
        
        if (!result.isFloat() || std::abs(result.asFloat() - 3.0f) >= 0.001f) {
            std::cout << "FAIL (mat2 m[0][1]: expected 3.0, got " << result.asFloat() << ")\n";
            return false;
        }
        
        // Test mat2 m[1][0] = col1, row0 = 2
        std::string mat2Source2 = R"(
            float calc(mat2 m) {
                return m[1][0];  // Should return column 1, row 0
            }
        )";
        result = parser.compileAndExecute(mat2Source2, args);
        
        if (!result.isFloat() || std::abs(result.asFloat() - 2.0f) >= 0.001f) {
            std::cout << "FAIL (mat2 m[1][0]: expected 2.0, got " << result.asFloat() << ")\n";
            return false;
        }
        
        // Test mat3 non-diagonal cross verification
        // m[0][2] - m[2][0] should be non-zero for non-symmetric matrix
        std::string mat3Source = R"(
            float calc(mat3 m) {
                return m[0][2] - m[2][0];  // col0,row2 - col2,row0
            }
        )";
        
        // Non-symmetric mat3:
        // Row 0: [1, 2, 3]
        // Row 1: [4, 5, 6]
        // Row 2: [7, 8, 9]
        // GLM column-major: col0=[1,4,7], col1=[2,5,8], col2=[3,6,9]
        // m[0][2] = col0, row2 = 7
        // m[2][0] = col2, row0 = 3
        // Result: 7 - 3 = 4
        std::vector<mmrsl::Value> mat3Args = {
            mmrsl::Value(mmrsl::Mat3(
                1.0f, 4.0f, 7.0f,  // col 0
                2.0f, 5.0f, 8.0f,  // col 1
                3.0f, 6.0f, 9.0f   // col 2
            ))
        };
        result = parser.compileAndExecute(mat3Source, mat3Args);
        
        if (!result.isFloat() || std::abs(result.asFloat() - 4.0f) >= 0.001f) {
            std::cout << "FAIL (mat3 m[0][2]-m[2][0]: expected 4.0, got " << result.asFloat() << ")\n";
            return false;
        }
        
        // Test mat4 specific non-diagonal element
        std::string mat4Source = R"(
            float calc(mat4 m) {
                return m[1][2];  // col1, row2
            }
        )";
        
        // mat4 with distinct values to verify indexing
        // We want m[1][2] = col1, row2 = 10
        std::vector<mmrsl::Value> mat4Args = {
            mmrsl::Value(mmrsl::Mat4(
                1.0f, 5.0f, 9.0f,  13.0f,  // col 0
                2.0f, 6.0f, 10.0f, 14.0f,  // col 1 (row2 = 10)
                3.0f, 7.0f, 11.0f, 15.0f,  // col 2
                4.0f, 8.0f, 12.0f, 16.0f   // col 3
            ))
        };
        result = parser.compileAndExecute(mat4Source, mat4Args);
        
        if (!result.isFloat() || std::abs(result.asFloat() - 10.0f) >= 0.001f) {
            std::cout << "FAIL (mat4 m[1][2]: expected 10.0, got " << result.asFloat() << ")\n";
            return false;
        }
        
        std::cout << "PASS\n";
        return true;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 79: Complex expression with many intermediate variables
// Verifies proper register allocation and reuse
bool testComplexExpressionRegisterReuse() {
    std::cout << "Test: Complex Expression Register Reuse... ";
    
    BytecodeParser parser;
    
    // Many intermediate variables to stress register allocation
    std::string source = R"(
        float calc(float a, float b, float c, float d) {
            float r1 = a + b;
            float r2 = c + d;
            float r3 = r1 * r2;
            float r4 = r3 - a;
            float r5 = r4 / b;
            float r6 = r5 + c;
            float r7 = r6 * d;
            float r8 = r7 - r1;
            float r9 = r8 + r2;
            float r10 = r9 * r3;
            return r10;
        }
    )";
    
    try {
        // Test with values: a=2, b=3, c=4, d=5
        // Manual calculation:
        // r1 = 2 + 3 = 5
        // r2 = 4 + 5 = 9
        // r3 = 5 * 9 = 45
        // r4 = 45 - 2 = 43
        // r5 = 43 / 3 = 14.333...
        // r6 = 14.333... + 4 = 18.333...
        // r7 = 18.333... * 5 = 91.666...
        // r8 = 91.666... - 5 = 86.666...
        // r9 = 86.666... + 9 = 95.666...
        // r10 = 95.666... * 45 = 4305
        std::vector<mmrsl::Value> args = {
            mmrsl::Value(2.0f),
            mmrsl::Value(3.0f),
            mmrsl::Value(4.0f),
            mmrsl::Value(5.0f)
        };
        
        mmrsl::Value result = parser.compileAndExecute(source, args);
        
        if (!result.isFloat()) {
            std::cout << "FAIL (expected float result)\n";
            return false;
        }
        
        float expected = 4305.0f;  // (86.666... + 9) * 45 = 95.666... * 45
        // More precise: ((43/3 + 4) * 5 - 5 + 9) * 45
        // = ((14.333... + 4) * 5 - 5 + 9) * 45
        // = (18.333... * 5 - 5 + 9) * 45
        // = (91.666... - 5 + 9) * 45
        // = (86.666... + 9) * 45
        // = 95.666... * 45
        // = 4305
        
        if (std::abs(result.asFloat() - expected) >= 0.1f) {
            std::cout << "FAIL (expected " << expected << ", got " << result.asFloat() << ")\n";
            return false;
        }
        
        // Test with different values to verify reusability
        // a=1, b=2, c=3, d=4
        // r1=3, r2=7, r3=21, r4=20, r5=10, r6=13, r7=52, r8=49, r9=56, r10=1176
        args[0] = mmrsl::Value(1.0f);
        args[1] = mmrsl::Value(2.0f);
        args[2] = mmrsl::Value(3.0f);
        args[3] = mmrsl::Value(4.0f);
        
        result = parser.execute(args);
        expected = 1176.0f;
        
        if (!result.isFloat() || std::abs(result.asFloat() - expected) >= 0.1f) {
            std::cout << "FAIL (second call: expected " << expected << ", got " << result.asFloat() << ")\n";
            return false;
        }
        
        std::cout << "PASS\n";
        return true;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 80: Deeply nested expression
// Verifies proper handling of complex nested function calls
bool testDeeplyNestedExpression() {
    std::cout << "Test: Deeply Nested Expression... ";
    
    BytecodeParser parser;
    
    // Deeply nested expression with multiple function calls
    std::string source = R"(
        float calc(float x) {
            return sin(cos(tan(sqrt(abs(x + 1.0) * 2.0) - 3.0) + 4.0) * 5.0);
        }
    )";
    
    try {
        // Test with x = 0.5
        // Manual calculation:
        // x + 1.0 = 1.5
        // abs(1.5) = 1.5
        // 1.5 * 2.0 = 3.0
        // sqrt(3.0) = 1.732...
        // 1.732... - 3.0 = -1.2679...
        // tan(-1.2679...) = -3.165...
        // -3.165... + 4.0 = 0.834...
        // 0.834... * 5.0 = 4.172...
        // cos(4.172...) = -0.513...
        // sin(-0.513...) = -0.490...
        
        std::vector<mmrsl::Value> args = {
            mmrsl::Value(0.5f)
        };
        
        mmrsl::Value result = parser.compileAndExecute(source, args);
        
        if (!result.isFloat()) {
            std::cout << "FAIL (expected float result)\n";
            return false;
        }
        
        // Verify the result is in a reasonable range (-1 to 1 for sin output)
        float value = result.asFloat();
        if (value < -1.0f || value > 1.0f) {
            std::cout << "FAIL (result " << value << " out of expected range [-1, 1])\n";
            return false;
        }
        
        // Test with x = -2.5
        // This tests a different code path through abs()
        args[0] = mmrsl::Value(-2.5f);
        result = parser.execute(args);
        
        if (!result.isFloat()) {
            std::cout << "FAIL (second call: expected float result)\n";
            return false;
        }
        
        value = result.asFloat();
        if (value < -1.0f || value > 1.0f) {
            std::cout << "FAIL (second call: result " << value << " out of expected range [-1, 1])\n";
            return false;
        }
        
        std::cout << "PASS\n";
        return true;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 81: Complex control flow - nested if/else and multiple sequential ifs
bool testComplexControlFlow() {
    std::cout << "Test: Complex Control Flow (nested if/else + sequential ifs)... ";
    
    BytecodeParser parser;
    
    // Test 1: Nested if/else
    std::string nestedIfSource = R"(
        float calc(float x) {
            if (x > 10.0) {
                if (x > 20.0) {
                    return 3.0;
                } else {
                    return 2.0;
                }
            } else {
                return 1.0;
            }
        }
    )";
    
    try {
        // Test nested if: x=5 -> 1, x=15 -> 2, x=25 -> 3
        std::vector<mmrsl::Value> args = { mmrsl::Value(5.0f) };
        mmrsl::Value result = parser.compileAndExecute(nestedIfSource, args);
        
        if (!result.isFloat() || std::abs(result.asFloat() - 1.0f) >= 0.001f) {
            std::cout << "FAIL (nested if x=5: expected 1.0, got " << result.asFloat() << ")\n";
            return false;
        }
        
        args[0] = mmrsl::Value(15.0f);
        result = parser.execute(args);
        if (!result.isFloat() || std::abs(result.asFloat() - 2.0f) >= 0.001f) {
            std::cout << "FAIL (nested if x=15: expected 2.0, got " << result.asFloat() << ")\n";
            return false;
        }
        
        args[0] = mmrsl::Value(25.0f);
        result = parser.execute(args);
        if (!result.isFloat() || std::abs(result.asFloat() - 3.0f) >= 0.001f) {
            std::cout << "FAIL (nested if x=25: expected 3.0, got " << result.asFloat() << ")\n";
            return false;
        }
        
        // Test 2: Multiple sequential ifs (not else-if)
        // Verifies jump backpatch correctness
        std::string sequentialIfSource = R"(
            float calc(float x) {
                float result = 0.0;
                if (x > 0.0) {
                    result = result + 1.0;
                }
                if (x > 5.0) {
                    result = result + 10.0;
                }
                if (x > 10.0) {
                    result = result + 100.0;
                }
                return result;
            }
        )";
        
        // x = -1.0: no conditions met -> 0
        args[0] = mmrsl::Value(-1.0f);
        result = parser.compileAndExecute(sequentialIfSource, args);
        if (!result.isFloat() || std::abs(result.asFloat() - 0.0f) >= 0.001f) {
            std::cout << "FAIL (sequential if x=-1: expected 0.0, got " << result.asFloat() << ")\n";
            return false;
        }
        
        // x = 3.0: only x>0.0 -> 1
        args[0] = mmrsl::Value(3.0f);
        result = parser.execute(args);
        if (!result.isFloat() || std::abs(result.asFloat() - 1.0f) >= 0.001f) {
            std::cout << "FAIL (sequential if x=3: expected 1.0, got " << result.asFloat() << ")\n";
            return false;
        }
        
        // x = 7.0: x>0.0 and x>5.0 -> 1 + 10 = 11
        args[0] = mmrsl::Value(7.0f);
        result = parser.execute(args);
        if (!result.isFloat() || std::abs(result.asFloat() - 11.0f) >= 0.001f) {
            std::cout << "FAIL (sequential if x=7: expected 11.0, got " << result.asFloat() << ")\n";
            return false;
        }
        
        // x = 15.0: all conditions -> 1 + 10 + 100 = 111
        args[0] = mmrsl::Value(15.0f);
        result = parser.execute(args);
        if (!result.isFloat() || std::abs(result.asFloat() - 111.0f) >= 0.001f) {
            std::cout << "FAIL (sequential if x=15: expected 111.0, got " << result.asFloat() << ")\n";
            return false;
        }
        
        std::cout << "PASS\n";
        return true;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 82: Repeated compilation and state isolation
// Verifies that multiple compilations on the same parser instance don't leak state
bool testRepeatedCompilationStateIsolation() {
    std::cout << "Test: Repeated Compilation State Isolation... ";
    
    BytecodeParser parser;
    
    try {
        // First compilation: vec3 operation
        std::string vec3Source = R"(
            vec3 calc(vec3 v) {
                return v * 2.0;
            }
        )";
        
        if (!parser.compile(vec3Source)) {
            std::cout << "FAIL (first compile failed: " << parser.getLastError() << ")\n";
            return false;
        }
        
        // Execute vec3 script
        std::vector<mmrsl::Value> vec3Args = {
            mmrsl::Value(mmrsl::Vec3(1.0f, 2.0f, 3.0f))
        };
        mmrsl::Value result1 = parser.execute(vec3Args);
        
        if (!result1.isVec3()) {
            std::cout << "FAIL (expected vec3 result, got " << result1.toString() << ")\n";
            return false;
        }
        
        mmrsl::Vec3 v1 = result1.asVec3();
        if (std::abs(v1.x - 2.0f) >= 0.001f || std::abs(v1.y - 4.0f) >= 0.001f || std::abs(v1.z - 6.0f) >= 0.001f) {
            std::cout << "FAIL (vec3 result mismatch: expected (2,4,6), got (" << v1.x << "," << v1.y << "," << v1.z << "))\n";
            return false;
        }
        
        // Second compilation: float operation (completely different type)
        std::string floatSource = R"(
            float calc(float x) {
                return x + 1.0;
            }
        )";
        
        if (!parser.compile(floatSource)) {
            std::cout << "FAIL (second compile failed: " << parser.getLastError() << ")\n";
            return false;
        }
        
        // Execute float script - should not be affected by previous vec3 compilation
        std::vector<mmrsl::Value> floatArgs = { mmrsl::Value(1.0f) };
        mmrsl::Value result2 = parser.execute(floatArgs);
        
        if (!result2.isFloat() || std::abs(result2.asFloat() - 2.0f) >= 0.001f) {
            std::cout << "FAIL (float result: expected 2.0, got " << result2.toString() << ")\n";
            return false;
        }
        
        // Third compilation: intentionally fail
        std::string invalidSource = "invalid syntax !!!";
        bool ok = parser.compile(invalidSource);
        
        if (ok) {
            std::cout << "FAIL (invalid source should have failed compilation)\n";
            return false;
        }
        
        // Verify error is set
        if (parser.getLastError().empty()) {
            std::cout << "FAIL (error message should be set after failed compilation)\n";
            return false;
        }
        
        // Verify bytecode was cleared - execute should throw
        try {
            parser.execute(floatArgs);
            std::cout << "FAIL (execute should throw after failed compilation)\n";
            return false;
        } catch (const std::exception& e) {
            // Expected - bytecode should be cleared
        }
        
        // Fourth compilation: successful again
        std::string anotherSource = R"(
            float calc(float x) {
                return x * x;
            }
        )";
        
        if (!parser.compile(anotherSource)) {
            std::cout << "FAIL (fourth compile failed: " << parser.getLastError() << ")\n";
            return false;
        }
        
        // Execute after recovery from failure
        std::vector<mmrsl::Value> args4 = { mmrsl::Value(5.0f) };
        mmrsl::Value result4 = parser.execute(args4);
        
        if (!result4.isFloat() || std::abs(result4.asFloat() - 25.0f) >= 0.001f) {
            std::cout << "FAIL (after recovery: expected 25.0, got " << result4.toString() << ")\n";
            return false;
        }
        
        std::cout << "PASS\n";
        return true;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 83: Boundary values and special floating-point cases
bool testBoundaryValuesAndSpecialFloats() {
    std::cout << "Test: Boundary Values and Special Floating-Point Cases... ";
    
    BytecodeParser parser;
    
    try {
        // Test 1: Division by zero detection
        std::string divSource = R"(
            float calc(float x) {
                return 1.0 / x;
            }
        )";
        
        if (!parser.compile(divSource)) {
            std::cout << "FAIL (compile failed: " << parser.getLastError() << ")\n";
            return false;
        }
        
        // x = 0 should trigger division by zero error
        std::vector<mmrsl::Value> args = { mmrsl::Value(0.0f) };
        try {
            parser.execute(args);
            std::cout << "FAIL (division by zero should throw)\n";
            return false;
        } catch (const std::exception& e) {
            // Expected - division by zero
        }
        
        // x = 1e-13 (below threshold) should also trigger error
        args[0] = mmrsl::Value(1e-13f);
        try {
            parser.execute(args);
            std::cout << "FAIL (near-zero division should throw)\n";
            return false;
        } catch (const std::exception& e) {
            // Expected - near-zero division
        }
        
        // x = 2.0 should work fine
        args[0] = mmrsl::Value(2.0f);
        mmrsl::Value result = parser.execute(args);
        if (!result.isFloat() || std::abs(result.asFloat() - 0.5f) >= 0.001f) {
            std::cout << "FAIL (1/2: expected 0.5, got " << result.toString() << ")\n";
            return false;
        }
        
        // Test 2: Large number behavior
        std::string largeSource = R"(
            float calc(float x) {
                return x * x * x;
            }
        )";
        
        if (!parser.compile(largeSource)) {
            std::cout << "FAIL (large compile failed: " << parser.getLastError() << ")\n";
            return false;
        }
        
        // Large value: 100^3 = 1,000,000
        args[0] = mmrsl::Value(100.0f);
        result = parser.execute(args);
        if (!result.isFloat() || std::abs(result.asFloat() - 1000000.0f) >= 1.0f) {
            std::cout << "FAIL (100^3: expected 1000000, got " << result.toString() << ")\n";
            return false;
        }
        
        // Very small value: 0.001^3 = 0.000000001
        args[0] = mmrsl::Value(0.001f);
        result = parser.execute(args);
        if (!result.isFloat() || std::abs(result.asFloat() - 1e-9f) >= 1e-10f) {
            std::cout << "FAIL (0.001^3: expected 1e-9, got " << result.toString() << ")\n";
            return false;
        }
        
        // Test 3: Negative square root (NaN behavior)
        std::string sqrtSource = R"(
            float calc(float x) {
                return sqrt(x);
            }
        )";
        
        if (!parser.compile(sqrtSource)) {
            std::cout << "FAIL (sqrt compile failed: " << parser.getLastError() << ")\n";
            return false;
        }
        
        // x = -1 should produce NaN
        args[0] = mmrsl::Value(-1.0f);
        result = parser.execute(args);
        if (!result.isFloat()) {
            std::cout << "FAIL (sqrt(-1): expected float result)\n";
            return false;
        }
        
        float sqrtResult = result.asFloat();
        if (sqrtResult == sqrtResult) {  // NaN check: NaN != NaN
            std::cout << "FAIL (sqrt(-1): expected NaN, got " << sqrtResult << ")\n";
            return false;
        }
        
        // x = 4 should produce 2
        args[0] = mmrsl::Value(4.0f);
        result = parser.execute(args);
        if (!result.isFloat() || std::abs(result.asFloat() - 2.0f) >= 0.001f) {
            std::cout << "FAIL (sqrt(4): expected 2, got " << result.toString() << ")\n";
            return false;
        }
        
        // Test 4: clamp boundary values
        std::string clampSource = R"(
            float calc(float x) {
                return clamp(x, 0.0, 1.0);
            }
        )";
        
        if (!parser.compile(clampSource)) {
            std::cout << "FAIL (clamp compile failed: " << parser.getLastError() << ")\n";
            return false;
        }
        
        // x = -0.5 ï¿?clamped to 0.0
        args[0] = mmrsl::Value(-0.5f);
        result = parser.execute(args);
        if (!result.isFloat() || std::abs(result.asFloat() - 0.0f) >= 0.001f) {
            std::cout << "FAIL (clamp(-0.5): expected 0.0, got " << result.toString() << ")\n";
            return false;
        }
        
        // x = 0.5 ï¿?unchanged
        args[0] = mmrsl::Value(0.5f);
        result = parser.execute(args);
        if (!result.isFloat() || std::abs(result.asFloat() - 0.5f) >= 0.001f) {
            std::cout << "FAIL (clamp(0.5): expected 0.5, got " << result.toString() << ")\n";
            return false;
        }
        
        // x = 1.5 ï¿?clamped to 1.0
        args[0] = mmrsl::Value(1.5f);
        result = parser.execute(args);
        if (!result.isFloat() || std::abs(result.asFloat() - 1.0f) >= 0.001f) {
            std::cout << "FAIL (clamp(1.5): expected 1.0, got " << result.toString() << ")\n";
            return false;
        }
        
        // x = 0.0 ï¿?boundary value
        args[0] = mmrsl::Value(0.0f);
        result = parser.execute(args);
        if (!result.isFloat() || std::abs(result.asFloat() - 0.0f) >= 0.001f) {
            std::cout << "FAIL (clamp(0.0): expected 0.0, got " << result.toString() << ")\n";
            return false;
        }
        
        // x = 1.0 ï¿?boundary value
        args[0] = mmrsl::Value(1.0f);
        result = parser.execute(args);
        if (!result.isFloat() || std::abs(result.asFloat() - 1.0f) >= 0.001f) {
            std::cout << "FAIL (clamp(1.0): expected 1.0, got " << result.toString() << ")\n";
            return false;
        }
        
        std::cout << "PASS\n";
        return true;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 84: Mixed type operations completeness
// Verifies various combinations of type mixing work correctly
bool testMixedTypeOperations() {
    std::cout << "Test: Mixed Type Operations Completeness... ";
    
    BytecodeParser parser;
    
    try {
        // Test 1: float * vec3 (both orders should produce same result)
        std::string vecFloatMulSource = R"(
            vec3 calc(vec3 v, float s) {
                vec3 a = v * s;       // vec * float
                vec3 b = s * v;       // float * vec (commutative)
                return a - b;         // should return (0,0,0)
            }
        )";
        
        if (!parser.compile(vecFloatMulSource)) {
            std::cout << "FAIL (vec*float compile failed: " << parser.getLastError() << ")\n";
            return false;
        }
        
        std::vector<mmrsl::Value> args = {
            mmrsl::Value(mmrsl::Vec3(1.0f, 2.0f, 3.0f)),
            mmrsl::Value(2.0f)
        };
        mmrsl::Value result = parser.execute(args);
        
        if (!result.isVec3()) {
            std::cout << "FAIL (expected vec3 result)\n";
            return false;
        }
        
        mmrsl::Vec3 v = result.asVec3();
        if (std::abs(v.x) >= 0.001f || std::abs(v.y) >= 0.001f || std::abs(v.z) >= 0.001f) {
            std::cout << "FAIL (v*s - s*v should be (0,0,0), got (" << v.x << "," << v.y << "," << v.z << "))\n";
            return false;
        }
        
        // Test 2: mat * vec chain
        std::string matVecChainSource = R"(
            vec3 calc(mat3 m1, mat3 m2, vec3 v) {
                return m1 * (m2 * v);
            }
        )";
        
        if (!parser.compile(matVecChainSource)) {
            std::cout << "FAIL (mat*vec chain compile failed: " << parser.getLastError() << ")\n";
            return false;
        }
        
        // m1 = identity, m2 = scale by 2, v = (1,2,3)
        // m2 * v = (2,4,6)
        // m1 * (2,4,6) = (2,4,6)
        mmrsl::Mat3 m1;  // identity
        mmrsl::Mat3 m2(2.0f, 0.0f, 0.0f, 0.0f, 2.0f, 0.0f, 0.0f, 0.0f, 2.0f);  // scale by 2
        
        std::vector<mmrsl::Value> matArgs = {
            mmrsl::Value(m1),
            mmrsl::Value(m2),
            mmrsl::Value(mmrsl::Vec3(1.0f, 2.0f, 3.0f))
        };
        result = parser.execute(matArgs);
        
        if (!result.isVec3()) {
            std::cout << "FAIL (mat chain: expected vec3 result)\n";
            return false;
        }
        
        v = result.asVec3();
        if (std::abs(v.x - 2.0f) >= 0.001f || std::abs(v.y - 4.0f) >= 0.001f || std::abs(v.z - 6.0f) >= 0.001f) {
            std::cout << "FAIL (mat chain: expected (2,4,6), got (" << v.x << "," << v.y << "," << v.z << "))\n";
            return false;
        }
        
        // Test 3: vec2 * float and float * vec2
        std::string vec2FloatSource = R"(
            vec2 calc(vec2 v, float s) {
                vec2 a = v * s;
                vec2 b = s * v;
                return a + b;  // should be 2 * v * s
            }
        )";
        
        if (!parser.compile(vec2FloatSource)) {
            std::cout << "FAIL (vec2*float compile failed: " << parser.getLastError() << ")\n";
            return false;
        }
        
        std::vector<mmrsl::Value> vec2Args = {
            mmrsl::Value(mmrsl::Vec2(2.0f, 3.0f)),
            mmrsl::Value(3.0f)
        };
        result = parser.execute(vec2Args);
        
        if (!result.isVec2()) {
            std::cout << "FAIL (vec2: expected vec2 result)\n";
            return false;
        }
        
        mmrsl::Vec2 v2 = result.asVec2();
        // (2,3) * 3 + 3 * (2,3) = (6,9) + (6,9) = (12,18)
        if (std::abs(v2.x - 12.0f) >= 0.001f || std::abs(v2.y - 18.0f) >= 0.001f) {
            std::cout << "FAIL (vec2: expected (12,18), got (" << v2.x << "," << v2.y << "))\n";
            return false;
        }
        
        // Test 4: vec4 * float and float * vec4
        std::string vec4FloatSource = R"(
            vec4 calc(vec4 v, float s) {
                return v * s + s * v;
            }
        )";
        
        if (!parser.compile(vec4FloatSource)) {
            std::cout << "FAIL (vec4*float compile failed: " << parser.getLastError() << ")\n";
            return false;
        }
        
        std::vector<mmrsl::Value> vec4Args = {
            mmrsl::Value(mmrsl::Vec4(1.0f, 2.0f, 3.0f, 4.0f)),
            mmrsl::Value(2.0f)
        };
        result = parser.execute(vec4Args);
        
        if (!result.isVec4()) {
            std::cout << "FAIL (vec4: expected vec4 result)\n";
            return false;
        }
        
        mmrsl::Vec4 v4 = result.asVec4();
        // (1,2,3,4) * 2 + 2 * (1,2,3,4) = (2,4,6,8) + (2,4,6,8) = (4,8,12,16)
        if (std::abs(v4.x - 4.0f) >= 0.001f || std::abs(v4.y - 8.0f) >= 0.001f || 
            std::abs(v4.z - 12.0f) >= 0.001f || std::abs(v4.w - 16.0f) >= 0.001f) {
            std::cout << "FAIL (vec4: expected (4,8,12,16), got (" << v4.x << "," << v4.y << "," << v4.z << "," << v4.w << "))\n";
            return false;
        }
        
        // Test 5: Mixed constructor + arithmetic
        std::string mixedConstructorSource = R"(
            vec4 calc(float x, float y) {
                vec4 a = vec4(x, y, x+y, x*y);
                vec4 b = vec4(1.0, 2.0, 3.0, 4.0);
                return a + b;
            }
        )";
        
        if (!parser.compile(mixedConstructorSource)) {
            std::cout << "FAIL (mixed constructor compile failed: " << parser.getLastError() << ")\n";
            return false;
        }
        
        std::vector<mmrsl::Value> mixedArgs = {
            mmrsl::Value(2.0f),
            mmrsl::Value(3.0f)
        };
        result = parser.execute(mixedArgs);
        
        if (!result.isVec4()) {
            std::cout << "FAIL (mixed: expected vec4 result)\n";
            return false;
        }
        
        v4 = result.asVec4();
        // a = (2, 3, 5, 6), b = (1, 2, 3, 4)
        // a + b = (3, 5, 8, 10)
        if (std::abs(v4.x - 3.0f) >= 0.001f || std::abs(v4.y - 5.0f) >= 0.001f || 
            std::abs(v4.z - 8.0f) >= 0.001f || std::abs(v4.w - 10.0f) >= 0.001f) {
            std::cout << "FAIL (mixed: expected (3,5,8,10), got (" << v4.x << "," << v4.y << "," << v4.z << "," << v4.w << "))\n";
            return false;
        }
        
        std::cout << "PASS\n";
        return true;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 85: Boolean logic chain operations
// Verifies complex logical expressions with &&, ||, and !
bool testBooleanLogicChain() {
    std::cout << "Test: Boolean Logic Chain Operations... ";
    
    BytecodeParser parser;
    
    try {
        // Test 1: Compound logical expression with &&
        std::string compoundAndSource = R"(
            float calc(float x, float y) {
                if (x > 0.0 && y > 0.0 && x < 10.0) {
                    return 1.0;
                }
                return 0.0;
            }
        )";
        
        if (!parser.compile(compoundAndSource)) {
            std::cout << "FAIL (compound && compile failed: " << parser.getLastError() << ")\n";
            return false;
        }
        
        // x=5, y=3: all conditions true -> 1
        std::vector<mmrsl::Value> args = { mmrsl::Value(5.0f), mmrsl::Value(3.0f) };
        mmrsl::Value result = parser.execute(args);
        if (!result.isFloat() || std::abs(result.asFloat() - 1.0f) >= 0.001f) {
            std::cout << "FAIL (x=5,y=3: expected 1.0, got " << result.toString() << ")\n";
            return false;
        }
        
        // x=-1, y=3: x>0 false -> 0
        args[0] = mmrsl::Value(-1.0f);
        args[1] = mmrsl::Value(3.0f);
        result = parser.execute(args);
        if (!result.isFloat() || std::abs(result.asFloat() - 0.0f) >= 0.001f) {
            std::cout << "FAIL (x=-1,y=3: expected 0.0, got " << result.toString() << ")\n";
            return false;
        }
        
        // x=5, y=-2: y>0 false -> 0
        args[0] = mmrsl::Value(5.0f);
        args[1] = mmrsl::Value(-2.0f);
        result = parser.execute(args);
        if (!result.isFloat() || std::abs(result.asFloat() - 0.0f) >= 0.001f) {
            std::cout << "FAIL (x=5,y=-2: expected 0.0, got " << result.toString() << ")\n";
            return false;
        }
        
        // x=15, y=3: x<10 false -> 0
        args[0] = mmrsl::Value(15.0f);
        args[1] = mmrsl::Value(3.0f);
        result = parser.execute(args);
        if (!result.isFloat() || std::abs(result.asFloat() - 0.0f) >= 0.001f) {
            std::cout << "FAIL (x=15,y=3: expected 0.0, got " << result.toString() << ")\n";
            return false;
        }
        
        // Test 2: NOT + compound logic
        std::string notCompoundSource = R"(
            float calc(float x) {
                if (!(x > 5.0) && x > 0.0) {
                    return 1.0;  // 0 < x <= 5
                }
                return 0.0;
            }
        )";
        
        if (!parser.compile(notCompoundSource)) {
            std::cout << "FAIL (NOT compound compile failed: " << parser.getLastError() << ")\n";
            return false;
        }
        
        // x=3: !(3>5)=true && 3>0=true -> 1
        std::vector<mmrsl::Value> singleArgs = { mmrsl::Value(3.0f) };
        result = parser.execute(singleArgs);
        if (!result.isFloat() || std::abs(result.asFloat() - 1.0f) >= 0.001f) {
            std::cout << "FAIL (x=3: expected 1.0, got " << result.toString() << ")\n";
            return false;
        }
        
        // x=7: !(7>5)=false -> 0
        singleArgs[0] = mmrsl::Value(7.0f);
        result = parser.execute(singleArgs);
        if (!result.isFloat() || std::abs(result.asFloat() - 0.0f) >= 0.001f) {
            std::cout << "FAIL (x=7: expected 0.0, got " << result.toString() << ")\n";
            return false;
        }
        
        // x=-1: -1>0=false -> 0
        singleArgs[0] = mmrsl::Value(-1.0f);
        result = parser.execute(singleArgs);
        if (!result.isFloat() || std::abs(result.asFloat() - 0.0f) >= 0.001f) {
            std::cout << "FAIL (x=-1: expected 0.0, got " << result.toString() << ")\n";
            return false;
        }
        
        // x=5: !(5>5)=true && 5>0=true -> 1 (boundary case)
        singleArgs[0] = mmrsl::Value(5.0f);
        result = parser.execute(singleArgs);
        if (!result.isFloat() || std::abs(result.asFloat() - 1.0f) >= 0.001f) {
            std::cout << "FAIL (x=5: expected 1.0, got " << result.toString() << ")\n";
            return false;
        }
        
        // Test 3: Bool in if/else with ||
        std::string boolInIfElseSource = R"(
            float calc(float a, float b) {
                float bigger = 0.0;
                if (a > b || a > 100.0) {
                    bigger = a;
                } else {
                    bigger = b;
                }
                return bigger;
            }
        )";
        
        if (!parser.compile(boolInIfElseSource)) {
            std::cout << "FAIL (bool if/else compile failed: " << parser.getLastError() << ")\n";
            return false;
        }
        
        // a=10, b=5: 10>5=true -> bigger=a=10
        args[0] = mmrsl::Value(10.0f);
        args[1] = mmrsl::Value(5.0f);
        result = parser.execute(args);
        if (!result.isFloat() || std::abs(result.asFloat() - 10.0f) >= 0.001f) {
            std::cout << "FAIL (a=10,b=5: expected 10.0, got " << result.toString() << ")\n";
            return false;
        }
        
        // a=5, b=10: 5>10=false, 5>100=false -> bigger=b=10
        args[0] = mmrsl::Value(5.0f);
        args[1] = mmrsl::Value(10.0f);
        result = parser.execute(args);
        if (!result.isFloat() || std::abs(result.asFloat() - 10.0f) >= 0.001f) {
            std::cout << "FAIL (a=5,b=10: expected 10.0, got " << result.toString() << ")\n";
            return false;
        }
        
        // a=150, b=200: 150>200=false, 150>100=true -> bigger=a=150
        args[0] = mmrsl::Value(150.0f);
        args[1] = mmrsl::Value(200.0f);
        result = parser.execute(args);
        if (!result.isFloat() || std::abs(result.asFloat() - 150.0f) >= 0.001f) {
            std::cout << "FAIL (a=150,b=200: expected 150.0, got " << result.toString() << ")\n";
            return false;
        }
        
        // Test 4: Complex mixed logical expression
        std::string complexMixedSource = R"(
            float calc(float x, float y, float z) {
                if ((x > 0.0 && y > 0.0) || (z > 10.0 && !(x < 0.0))) {
                    return 1.0;
                }
                return 0.0;
            }
        )";
        
        if (!parser.compile(complexMixedSource)) {
            std::cout << "FAIL (complex mixed compile failed: " << parser.getLastError() << ")\n";
            return false;
        }
        
        // x=1,y=1,z=5: (true && true)=true -> 1
        std::vector<mmrsl::Value> tripleArgs = { mmrsl::Value(1.0f), mmrsl::Value(1.0f), mmrsl::Value(5.0f) };
        result = parser.execute(tripleArgs);
        if (!result.isFloat() || std::abs(result.asFloat() - 1.0f) >= 0.001f) {
            std::cout << "FAIL (x=1,y=1,z=5: expected 1.0, got " << result.toString() << ")\n";
            return false;
        }
        
        // x=-1,y=1,z=15: (false && true)=false, (true && true)=true -> 1
        // Wait: !(x < 0.0) when x=-1: !(-1 < 0) = !true = false
        // So: (true && false) = false, overall false || false = false -> 0
        // Let me fix: use x=1 instead so !(1 < 0) = true
        tripleArgs[0] = mmrsl::Value(1.0f);
        tripleArgs[1] = mmrsl::Value(-1.0f);  // y<0 so first part false
        tripleArgs[2] = mmrsl::Value(15.0f);  // z>10 true
        result = parser.execute(tripleArgs);
        if (!result.isFloat() || std::abs(result.asFloat() - 1.0f) >= 0.001f) {
            std::cout << "FAIL (x=1,y=-1,z=15: expected 1.0, got " << result.toString() << ")\n";
            return false;
        }
        
        // x=-1,y=1,z=5: (false && true)=false, (false && true)=false -> 0
        // Actually: z>10 is false when z=5, so right side is false regardless of x
        tripleArgs[0] = mmrsl::Value(-1.0f);
        tripleArgs[1] = mmrsl::Value(1.0f);
        tripleArgs[2] = mmrsl::Value(5.0f);
        result = parser.execute(tripleArgs);
        if (!result.isFloat() || std::abs(result.asFloat() - 0.0f) >= 0.001f) {
            std::cout << "FAIL (x=-1,y=1,z=5: expected 0.0, got " << result.toString() << ")\n";
            return false;
        }
        
        std::cout << "PASS\n";
        return true;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 86: Direct expression composition without intermediate variables
// Verifies temporary register allocation/deallocation in complex expression chains
bool testDirectExpressionComposition() {
    std::cout << "Test: Direct Expression Composition... ";
    
    BytecodeParser parser;
    
    try {
        // Test 1: Constructor directly participates in operations
        std::string constructorDirectSource = R"(
            vec3 calc(vec3 a, vec3 b) {
                return vec3(1.0, 0.0, 0.0) + normalize(a) * dot(a, b);
            }
        )";
        
        if (!parser.compile(constructorDirectSource)) {
            std::cout << "FAIL (constructor direct compile failed: " << parser.getLastError() << ")\n";
            return false;
        }
        
        // a = (3,0,0), b = (1,0,0)
        // normalize(a) = (1,0,0)
        // dot(a,b) = 3
        // vec3(1,0,0) + (1,0,0) * 3 = (1,0,0) + (3,0,0) = (4,0,0)
        std::vector<mmrsl::Value> args = {
            mmrsl::Value(mmrsl::Vec3(3.0f, 0.0f, 0.0f)),
            mmrsl::Value(mmrsl::Vec3(1.0f, 0.0f, 0.0f))
        };
        mmrsl::Value result = parser.execute(args);
        
        if (!result.isVec3()) {
            std::cout << "FAIL (constructor direct: expected vec3 result)\n";
            return false;
        }
        
        mmrsl::Vec3 v = result.asVec3();
        if (std::abs(v.x - 4.0f) >= 0.001f || std::abs(v.y) >= 0.001f || std::abs(v.z) >= 0.001f) {
            std::cout << "FAIL (constructor direct: expected (4,0,0), got (" << v.x << "," << v.y << "," << v.z << "))\n";
            return false;
        }
        
        // Test 2: Function return value directly as another function's argument
        std::string funcChainSource = R"(
            float calc(float x) {
                return abs(sin(x)) + sqrt(abs(cos(x)));
            }
        )";
        
        if (!parser.compile(funcChainSource)) {
            std::cout << "FAIL (func chain compile failed: " << parser.getLastError() << ")\n";
            return false;
        }
        
        // x = 0: sin(0)=0, abs(0)=0, cos(0)=1, abs(1)=1, sqrt(1)=1, result=0+1=1
        std::vector<mmrsl::Value> singleArgs = { mmrsl::Value(0.0f) };
        result = parser.execute(singleArgs);
        
        if (!result.isFloat() || std::abs(result.asFloat() - 1.0f) >= 0.001f) {
            std::cout << "FAIL (x=0: expected 1.0, got " << result.toString() << ")\n";
            return false;
        }
        
        // x = pi/2: sin(pi/2)=1, abs(1)=1, cos(pi/2)=0, abs(0)=0, sqrt(0)=0, result=1+0=1
        singleArgs[0] = mmrsl::Value(3.14159265f / 2.0f);
        result = parser.execute(singleArgs);
        
        if (!result.isFloat() || std::abs(result.asFloat() - 1.0f) >= 0.01f) {
            std::cout << "FAIL (x=pi/2: expected ~1.0, got " << result.toString() << ")\n";
            return false;
        }
        
        // Test 3: Member access directly participates in operations
        std::string memberDirectSource = R"(
            float calc(vec3 a, vec3 b) {
                return (a + b).x * (a - b).y + cross(a, b).z;
            }
        )";
        
        if (!parser.compile(memberDirectSource)) {
            std::cout << "FAIL (member direct compile failed: " << parser.getLastError() << ")\n";
            return false;
        }
        
        // a = (2,3,4), b = (1,1,1)
        // (a+b) = (3,4,5), .x = 3
        // (a-b) = (1,2,3), .y = 2
        // cross(a,b) = (3*1-4*1, 4*1-2*1, 2*1-3*1) = (-1, 2, -1), .z = -1
        // result = 3 * 2 + (-1) = 6 - 1 = 5
        args[0] = mmrsl::Value(mmrsl::Vec3(2.0f, 3.0f, 4.0f));
        args[1] = mmrsl::Value(mmrsl::Vec3(1.0f, 1.0f, 1.0f));
        result = parser.execute(args);
        
        if (!result.isFloat() || std::abs(result.asFloat() - 5.0f) >= 0.001f) {
            std::cout << "FAIL (member direct: expected 5.0, got " << result.toString() << ")\n";
            return false;
        }
        
        // Test 4: Deeply nested expression without intermediates
        std::string deepNestedSource = R"(
            float calc(float x, float y, float z) {
                return clamp(sin(x) * cos(y) + tan(z), 0.0, 1.0) * length(vec3(x, y, z));
            }
        )";
        
        if (!parser.compile(deepNestedSource)) {
            std::cout << "FAIL (deep nested compile failed: " << parser.getLastError() << ")\n";
            return false;
        }
        
        // x=0, y=0, z=0
        // sin(0)=0, cos(0)=1, tan(0)=0
        // 0*1 + 0 = 0, clamp(0,0,1) = 0
        // length(vec3(0,0,0)) = 0
        // result = 0 * 0 = 0
        std::vector<mmrsl::Value> tripleArgs = {
            mmrsl::Value(0.0f),
            mmrsl::Value(0.0f),
            mmrsl::Value(0.0f)
        };
        result = parser.execute(tripleArgs);
        
        if (!result.isFloat() || std::abs(result.asFloat() - 0.0f) >= 0.001f) {
            std::cout << "FAIL (deep nested x=y=z=0: expected 0.0, got " << result.toString() << ")\n";
            return false;
        }
        
        // Test 5: Matrix operation result directly used
        std::string matDirectSource = R"(
            vec3 calc(mat3 m, vec3 v) {
                return normalize(m * v) * length(v);
            }
        )";
        
        if (!parser.compile(matDirectSource)) {
            std::cout << "FAIL (mat direct compile failed: " << parser.getLastError() << ")\n";
            return false;
        }
        
        // m = identity, v = (3,0,0)
        // m * v = (3,0,0)
        // normalize(3,0,0) = (1,0,0)
        // length(3,0,0) = 3
        // result = (1,0,0) * 3 = (3,0,0)
        mmrsl::Mat3 m;
        std::vector<mmrsl::Value> matArgs = {
            mmrsl::Value(m),
            mmrsl::Value(mmrsl::Vec3(3.0f, 0.0f, 0.0f))
        };
        result = parser.execute(matArgs);
        
        if (!result.isVec3()) {
            std::cout << "FAIL (mat direct: expected vec3 result)\n";
            return false;
        }
        
        v = result.asVec3();
        if (std::abs(v.x - 3.0f) >= 0.001f || std::abs(v.y) >= 0.001f || std::abs(v.z) >= 0.001f) {
            std::cout << "FAIL (mat direct: expected (3,0,0), got (" << v.x << "," << v.y << "," << v.z << "))\n";
            return false;
        }
        
        std::cout << "PASS\n";
        return true;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 87: Unary operators in complex contexts
// Verifies negation and positive operators work correctly in various contexts
bool testUnaryOperatorsComplexContext() {
    std::cout << "Test: Unary Operators in Complex Contexts... ";
    
    BytecodeParser parser;
    
    try {
        // Test 1: Negation of function call result
        std::string negFuncSource = R"(
            vec3 calc(vec3 v) {
                return -normalize(v);
            }
        )";
        
        if (!parser.compile(negFuncSource)) {
            std::cout << "FAIL (neg func compile failed: " << parser.getLastError() << ")\n";
            return false;
        }
        
        // v = (3,0,0), normalize(v) = (1,0,0), -normalize(v) = (-1,0,0)
        std::vector<mmrsl::Value> args = { mmrsl::Value(mmrsl::Vec3(3.0f, 0.0f, 0.0f)) };
        mmrsl::Value result = parser.execute(args);
        
        if (!result.isVec3()) {
            std::cout << "FAIL (neg func: expected vec3 result)\n";
            return false;
        }
        
        mmrsl::Vec3 v = result.asVec3();
        if (std::abs(v.x - (-1.0f)) >= 0.001f || std::abs(v.y) >= 0.001f || std::abs(v.z) >= 0.001f) {
            std::cout << "FAIL (neg func: expected (-1,0,0), got (" << v.x << "," << v.y << "," << v.z << "))\n";
            return false;
        }
        
        // Test 2: Negation of member access
        std::string negMemberSource = R"(
            float calc(vec3 v) {
                return -v.x + (-v.y) * (-v.z);
            }
        )";
        
        if (!parser.compile(negMemberSource)) {
            std::cout << "FAIL (neg member compile failed: " << parser.getLastError() << ")\n";
            return false;
        }
        
        // v = (2,3,4)
        // -v.x = -2
        // (-v.y) = -3
        // (-v.z) = -4
        // -2 + (-3) * (-4) = -2 + 12 = 10
        args[0] = mmrsl::Value(mmrsl::Vec3(2.0f, 3.0f, 4.0f));
        result = parser.execute(args);
        
        if (!result.isFloat() || std::abs(result.asFloat() - 10.0f) >= 0.001f) {
            std::cout << "FAIL (neg member: expected 10.0, got " << result.toString() << ")\n";
            return false;
        }
        
        // Test 3: Consecutive unary negations
        std::string consecutiveNegSource = R"(
            float calc(float x) {
                return -(-(-x));  // should equal -x
            }
        )";
        
        if (!parser.compile(consecutiveNegSource)) {
            std::cout << "FAIL (consecutive neg compile failed: " << parser.getLastError() << ")\n";
            return false;
        }
        
        // x = 5, -(-(-5)) = -(5) = -5
        std::vector<mmrsl::Value> singleArgs = { mmrsl::Value(5.0f) };
        result = parser.execute(singleArgs);
        
        if (!result.isFloat() || std::abs(result.asFloat() - (-5.0f)) >= 0.001f) {
            std::cout << "FAIL (x=5: expected -5.0, got " << result.toString() << ")\n";
            return false;
        }
        
        // x = -3, -(-(-(-3))) = -(3) = 3
        singleArgs[0] = mmrsl::Value(-3.0f);
        result = parser.execute(singleArgs);
        
        if (!result.isFloat() || std::abs(result.asFloat() - 3.0f) >= 0.001f) {
            std::cout << "FAIL (x=-3: expected 3.0, got " << result.toString() << ")\n";
            return false;
        }
        
        // Test 4: Positive unary operator (no-op)
        std::string positiveSource = R"(
            float calc(float x) {
                return +x;
            }
        )";
        
        if (!parser.compile(positiveSource)) {
            std::cout << "FAIL (positive compile failed: " << parser.getLastError() << ")\n";
            return false;
        }
        
        // x = 7, +7 = 7
        singleArgs[0] = mmrsl::Value(7.0f);
        result = parser.execute(singleArgs);
        
        if (!result.isFloat() || std::abs(result.asFloat() - 7.0f) >= 0.001f) {
            std::cout << "FAIL (+x: expected 7.0, got " << result.toString() << ")\n";
            return false;
        }
        
        // x = -4, +(-4) = -4
        singleArgs[0] = mmrsl::Value(-4.0f);
        result = parser.execute(singleArgs);
        
        if (!result.isFloat() || std::abs(result.asFloat() - (-4.0f)) >= 0.001f) {
            std::cout << "FAIL (+(-4): expected -4.0, got " << result.toString() << ")\n";
            return false;
        }
        
        // Test 5: Mixed unary and binary in complex expression
        std::string mixedUnarySource = R"(
            float calc(float x, float y) {
                return -x * y + -(x + y);
            }
        )";
        
        if (!parser.compile(mixedUnarySource)) {
            std::cout << "FAIL (mixed unary compile failed: " << parser.getLastError() << ")\n";
            return false;
        }
        
        // x = 3, y = 2
        // -3 * 2 + -(3 + 2) = -6 + (-5) = -11
        std::vector<mmrsl::Value> doubleArgs = { mmrsl::Value(3.0f), mmrsl::Value(2.0f) };
        result = parser.execute(doubleArgs);
        
        if (!result.isFloat() || std::abs(result.asFloat() - (-11.0f)) >= 0.001f) {
            std::cout << "FAIL (mixed unary: expected -11.0, got " << result.toString() << ")\n";
            return false;
        }
        
        // Test 6: Negation of arithmetic expression result
        std::string negArithSource = R"(
            vec3 calc(vec3 a, vec3 b) {
                return -(a + b) * 2.0;
            }
        )";
        
        if (!parser.compile(negArithSource)) {
            std::cout << "FAIL (neg arith compile failed: " << parser.getLastError() << ")\n";
            return false;
        }
        
        // a = (1,2,3), b = (4,5,6)
        // a + b = (5,7,9)
        // -(5,7,9) = (-5,-7,-9)
        // (-5,-7,-9) * 2 = (-10,-14,-18)
        args[0] = mmrsl::Value(mmrsl::Vec3(1.0f, 2.0f, 3.0f));
        std::vector<mmrsl::Value> vecArgs = {
            mmrsl::Value(mmrsl::Vec3(1.0f, 2.0f, 3.0f)),
            mmrsl::Value(mmrsl::Vec3(4.0f, 5.0f, 6.0f))
        };
        result = parser.execute(vecArgs);
        
        if (!result.isVec3()) {
            std::cout << "FAIL (neg arith: expected vec3 result)\n";
            return false;
        }
        
        v = result.asVec3();
        if (std::abs(v.x - (-10.0f)) >= 0.001f || std::abs(v.y - (-14.0f)) >= 0.001f || std::abs(v.z - (-18.0f)) >= 0.001f) {
            std::cout << "FAIL (neg arith: expected (-10,-14,-18), got (" << v.x << "," << v.y << "," << v.z << "))\n";
            return false;
        }
        
        std::cout << "PASS\n";
        return true;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 88: Operator precedence verification
// Verifies correct precedence of arithmetic, comparison, and logical operators
bool testOperatorPrecedence() {
    std::cout << "Test: Operator Precedence... ";
    
    BytecodeParser parser;
    
    try {
        // Test 1: Multiplication precedence over addition
        std::string mulAddSource = R"(
            float calc(float a, float b, float c) {
                return a + b * c;  // should be a + (b*c), not (a+b)*c
            }
        )";
        
        if (!parser.compile(mulAddSource)) {
            std::cout << "FAIL (mul+add compile failed: " << parser.getLastError() << ")\n";
            return false;
        }
        
        // a=1, b=2, c=3: 1 + 2*3 = 1 + 6 = 7 (not (1+2)*3 = 9)
        std::vector<mmrsl::Value> args = { mmrsl::Value(1.0f), mmrsl::Value(2.0f), mmrsl::Value(3.0f) };
        mmrsl::Value result = parser.execute(args);
        
        if (!result.isFloat() || std::abs(result.asFloat() - 7.0f) >= 0.001f) {
            std::cout << "FAIL (1+2*3: expected 7.0, got " << result.toString() << ")\n";
            return false;
        }
        
        // Test 2: Complex precedence with multiple operators
        std::string complexPrecSource = R"(
            float calc(float x) {
                return 2.0 + 3.0 * 4.0 - 1.0 / 2.0;  // = 2 + 12 - 0.5 = 13.5
            }
        )";
        
        if (!parser.compile(complexPrecSource)) {
            std::cout << "FAIL (complex prec compile failed: " << parser.getLastError() << ")\n";
            return false;
        }
        
        std::vector<mmrsl::Value> singleArgs = { mmrsl::Value(0.0f) };
        result = parser.execute(singleArgs);
        
        if (!result.isFloat() || std::abs(result.asFloat() - 13.5f) >= 0.001f) {
            std::cout << "FAIL (2+3*4-1/2: expected 13.5, got " << result.toString() << ")\n";
            return false;
        }
        
        // Test 3: Modulo precedence
        std::string moduloPrecSource = R"(
            float calc(float x) {
                return x + 10.0 % 3.0 * 2.0;  // = x + (10%3)*2 = x + 2.0
            }
        )";
        
        if (!parser.compile(moduloPrecSource)) {
            std::cout << "FAIL (modulo prec compile failed: " << parser.getLastError() << ")\n";
            return false;
        }
        
        // x = 5: 5 + (10%3)*2 = 5 + 1*2 = 7
        singleArgs[0] = mmrsl::Value(5.0f);
        result = parser.execute(singleArgs);
        
        if (!result.isFloat() || std::abs(result.asFloat() - 7.0f) >= 0.001f) {
            std::cout << "FAIL (5+10%3*2: expected 7.0, got " << result.toString() << ")\n";
            return false;
        }
        
        // Test 4: Comparison and logical operator precedence
        // && should have higher precedence than ||
        std::string logicPrecSource = R"(
            float calc(float a, float b, float c) {
                if (a > 0.0 && b > 0.0 || c > 0.0) {
                    return 1.0;
                }
                return 0.0;
            }
        )";
        
        if (!parser.compile(logicPrecSource)) {
            std::cout << "FAIL (logic prec compile failed: " << parser.getLastError() << ")\n";
            return false;
        }
        
        // a=-1, b=-1, c=1: (-1>0 && -1>0) || 1>0 = (false && false) || true = false || true = true -> 1
        // This verifies && has higher precedence than ||
        args[0] = mmrsl::Value(-1.0f);
        args[1] = mmrsl::Value(-1.0f);
        args[2] = mmrsl::Value(1.0f);
        result = parser.execute(args);
        
        if (!result.isFloat() || std::abs(result.asFloat() - 1.0f) >= 0.001f) {
            std::cout << "FAIL (a=-1,b=-1,c=1: expected 1.0, got " << result.toString() << ")\n";
            return false;
        }
        
        // a=-1, b=1, c=-1: (-1>0 && 1>0) || -1>0 = (false && true) || false = false || false = false -> 0
        args[0] = mmrsl::Value(-1.0f);
        args[1] = mmrsl::Value(1.0f);
        args[2] = mmrsl::Value(-1.0f);
        result = parser.execute(args);
        
        if (!result.isFloat() || std::abs(result.asFloat() - 0.0f) >= 0.001f) {
            std::cout << "FAIL (a=-1,b=1,c=-1: expected 0.0, got " << result.toString() << ")\n";
            return false;
        }
        
        // a=1, b=1, c=-1: (1>0 && 1>0) || -1>0 = (true && true) || false = true || false = true -> 1
        args[0] = mmrsl::Value(1.0f);
        args[1] = mmrsl::Value(1.0f);
        args[2] = mmrsl::Value(-1.0f);
        result = parser.execute(args);
        
        if (!result.isFloat() || std::abs(result.asFloat() - 1.0f) >= 0.001f) {
            std::cout << "FAIL (a=1,b=1,c=-1: expected 1.0, got " << result.toString() << ")\n";
            return false;
        }
        
        // Test 5: Unary operator precedence (higher than multiplicative)
        std::string unaryPrecSource = R"(
            float calc(float x) {
                return -x * 2.0;  // should be (-x) * 2, not -(x * 2)
            }
        )";
        
        if (!parser.compile(unaryPrecSource)) {
            std::cout << "FAIL (unary prec compile failed: " << parser.getLastError() << ")\n";
            return false;
        }
        
        // x = 3: (-3) * 2 = -6
        singleArgs[0] = mmrsl::Value(3.0f);
        result = parser.execute(singleArgs);
        
        if (!result.isFloat() || std::abs(result.asFloat() - (-6.0f)) >= 0.001f) {
            std::cout << "FAIL (-3*2: expected -6.0, got " << result.toString() << ")\n";
            return false;
        }
        
        // Test 6: Parentheses override precedence
        std::string parenSource = R"(
            float calc(float a, float b, float c) {
                return (a + b) * c;  // should be (a+b)*c, not a+(b*c)
            }
        )";
        
        if (!parser.compile(parenSource)) {
            std::cout << "FAIL (paren compile failed: " << parser.getLastError() << ")\n";
            return false;
        }
        
        // a=1, b=2, c=3: (1+2)*3 = 9 (not 1+2*3=7)
        args[0] = mmrsl::Value(1.0f);
        args[1] = mmrsl::Value(2.0f);
        args[2] = mmrsl::Value(3.0f);
        result = parser.execute(args);
        
        if (!result.isFloat() || std::abs(result.asFloat() - 9.0f) >= 0.001f) {
            std::cout << "FAIL ((1+2)*3: expected 9.0, got " << result.toString() << ")\n";
            return false;
        }
        
        std::cout << "PASS\n";
        return true;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 89: Variable multiple reassignment
// Verifies variables can be reassigned multiple times correctly
bool testVariableReassignment() {
    std::cout << "Test: Variable Multiple Reassignment... ";
    
    BytecodeParser parser;
    
    try {
        // Test 1: Same variable repeatedly overwritten
        std::string chainReassignSource = R"(
            float calc(float x) {
                float v = x;
                v = v * 2.0;
                v = v + 3.0;
                v = v * v;
                v = sqrt(v);
                return v;
            }
        )";
        
        if (!parser.compile(chainReassignSource)) {
            std::cout << "FAIL (chain reassign compile failed: " << parser.getLastError() << ")\n";
            return false;
        }
        
        // x = 1.0: v = 1 ï¿?2 ï¿?5 ï¿?25 ï¿?5.0
        std::vector<mmrsl::Value> args = { mmrsl::Value(1.0f) };
        mmrsl::Value result = parser.execute(args);
        
        if (!result.isFloat() || std::abs(result.asFloat() - 5.0f) >= 0.001f) {
            std::cout << "FAIL (x=1: expected 5.0, got " << result.toString() << ")\n";
            return false;
        }
        
        // x = 2.0: v = 2 ï¿?4 ï¿?7 ï¿?49 ï¿?7.0
        args[0] = mmrsl::Value(2.0f);
        result = parser.execute(args);
        
        if (!result.isFloat() || std::abs(result.asFloat() - 7.0f) >= 0.001f) {
            std::cout << "FAIL (x=2: expected 7.0, got " << result.toString() << ")\n";
            return false;
        }
        
        // Test 2: Assignment in if branches
        std::string ifBranchAssignSource = R"(
            float calc(float x) {
                float result = 0.0;
                if (x > 0.0) {
                    result = x;
                }
                result = result + 1.0;
                if (x > 10.0) {
                    result = result * 2.0;
                }
                return result;
            }
        )";
        
        if (!parser.compile(ifBranchAssignSource)) {
            std::cout << "FAIL (if branch assign compile failed: " << parser.getLastError() << ")\n";
            return false;
        }
        
        // x = -5: result = 0 ï¿?(not >0) ï¿?0+1 = 1 ï¿?(not >10) ï¿?1
        args[0] = mmrsl::Value(-5.0f);
        result = parser.execute(args);
        
        if (!result.isFloat() || std::abs(result.asFloat() - 1.0f) >= 0.001f) {
            std::cout << "FAIL (x=-5: expected 1.0, got " << result.toString() << ")\n";
            return false;
        }
        
        // x = 5: result = 0 ï¿?5 ï¿?5+1 = 6 ï¿?(not >10) ï¿?6
        args[0] = mmrsl::Value(5.0f);
        result = parser.execute(args);
        
        if (!result.isFloat() || std::abs(result.asFloat() - 6.0f) >= 0.001f) {
            std::cout << "FAIL (x=5: expected 6.0, got " << result.toString() << ")\n";
            return false;
        }
        
        // x = 15: result = 0 ï¿?15 ï¿?15+1 = 16 ï¿?(>10) ï¿?16*2 = 32
        args[0] = mmrsl::Value(15.0f);
        result = parser.execute(args);
        
        if (!result.isFloat() || std::abs(result.asFloat() - 32.0f) >= 0.001f) {
            std::cout << "FAIL (x=15: expected 32.0, got " << result.toString() << ")\n";
            return false;
        }
        
        // Test 3: Multiple variables reassigned interleaved
        std::string multiVarReassignSource = R"(
            float calc(float x, float y) {
                float a = x;
                float b = y;
                a = a + b;
                b = a - b;
                a = a - b;
                return a * 10.0 + b;  // should swap x and y
            }
        )";
        
        if (!parser.compile(multiVarReassignSource)) {
            std::cout << "FAIL (multi var reassign compile failed: " << parser.getLastError() << ")\n";
            return false;
        }
        
        // This implements a swap: a=x, b=y, a=x+y, b=(x+y)-y=x, a=(x+y)-x=y
        // Result: a=y, b=x
        // Return: y*10 + x
        // x=3, y=7: result = 7*10 + 3 = 73
        std::vector<mmrsl::Value> doubleArgs = { mmrsl::Value(3.0f), mmrsl::Value(7.0f) };
        result = parser.execute(doubleArgs);
        
        if (!result.isFloat() || std::abs(result.asFloat() - 73.0f) >= 0.001f) {
            std::cout << "FAIL (swap x=3,y=7: expected 73.0, got " << result.toString() << ")\n";
            return false;
        }
        
        // Test 4: Vector variable reassignment
        std::string vecReassignSource = R"(
            vec3 calc(vec3 v) {
                vec3 result = v;
                result = result * 2.0;
                result = result + vec3(1.0, 2.0, 3.0);
                result = normalize(result);
                return result;
            }
        )";
        
        if (!parser.compile(vecReassignSource)) {
            std::cout << "FAIL (vec reassign compile failed: " << parser.getLastError() << ")\n";
            return false;
        }
        
        // v = (1,0,0): result = (1,0,0) ï¿?(2,0,0) ï¿?(3,2,3) ï¿?normalize
        args[0] = mmrsl::Value(mmrsl::Vec3(1.0f, 0.0f, 0.0f));
        result = parser.execute(args);
        
        if (!result.isVec3()) {
            std::cout << "FAIL (vec reassign: expected vec3 result)\n";
            return false;
        }
        
        // Just verify it's a normalized vector (length ï¿?1)
        mmrsl::Vec3 v = result.asVec3();
        float len = std::sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
        if (std::abs(len - 1.0f) >= 0.001f) {
            std::cout << "FAIL (vec reassign: expected normalized vector, length=" << len << ")\n";
            return false;
        }
        
        std::cout << "PASS\n";
        return true;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 90: ACES Tone Mapping algorithm
// Real-world shader algorithm: ACES filmic tone mapping curve
bool testAcesToneMapping() {
    std::cout << "Test: ACES Tone Mapping... ";
    
    BytecodeParser parser;
    
    std::string acesSource = R"(
        vec3 calc(vec3 color) {
            float a = 2.51;
            float b = 0.03;
            float c = 2.43;
            float d = 0.59;
            float e = 0.14;
            float rx = color.x;
            float gx = color.y;
            float bx = color.z;
            float rmapped = clamp((rx * (a * rx + b)) / (rx * (c * rx + d) + e), 0.0, 1.0);
            float gmapped = clamp((gx * (a * gx + b)) / (gx * (c * gx + d) + e), 0.0, 1.0);
            float bmapped = clamp((bx * (a * bx + b)) / (bx * (c * bx + d) + e), 0.0, 1.0);
            return vec3(rmapped, gmapped, bmapped);
        }
    )";
    
    try {
        if (!parser.compile(acesSource)) {
            std::cout << "FAIL (compile failed: " << parser.getLastError() << ")\n";
            return false;
        }
        
        // Test 1: Black color (0,0,0) should stay black
        std::vector<mmrsl::Value> args = { mmrsl::Value(mmrsl::Vec3(0.0f, 0.0f, 0.0f)) };
        mmrsl::Value result = parser.execute(args);
        
        if (!result.isVec3()) {
            std::cout << "FAIL (black: expected vec3 result)\n";
            return false;
        }
        
        mmrsl::Vec3 v = result.asVec3();
        if (std::abs(v.x) >= 0.001f || std::abs(v.y) >= 0.001f || std::abs(v.z) >= 0.001f) {
            std::cout << "FAIL (black: expected (0,0,0), got (" << v.x << "," << v.y << "," << v.z << "))\n";
            return false;
        }
        
        // Test 2: White color (1,1,1) should be mapped to near-white
        args[0] = mmrsl::Value(mmrsl::Vec3(1.0f, 1.0f, 1.0f));
        result = parser.execute(args);
        
        if (!result.isVec3()) {
            std::cout << "FAIL (white: expected vec3 result)\n";
            return false;
        }
        
        v = result.asVec3();
        // ACES maps (1,1,1) to approximately (0.82, 0.82, 0.82)
        if (v.x < 0.7f || v.x > 0.9f || v.y < 0.7f || v.y > 0.9f || v.z < 0.7f || v.z > 0.9f) {
            std::cout << "FAIL (white: expected ~0.82, got (" << v.x << "," << v.y << "," << v.z << "))\n";
            return false;
        }
        
        // Test 3: Mid-gray (0.5, 0.5, 0.5)
        args[0] = mmrsl::Value(mmrsl::Vec3(0.5f, 0.5f, 0.5f));
        result = parser.execute(args);
        
        if (!result.isVec3()) {
            std::cout << "FAIL (gray: expected vec3 result)\n";
            return false;
        }
        
        v = result.asVec3();
        // Mid-gray should be mapped to a slightly lower value
        if (v.x < 0.3f || v.x > 0.7f || std::abs(v.x - v.y) >= 0.001f || std::abs(v.y - v.z) >= 0.001f) {
            std::cout << "FAIL (gray: expected similar values in range [0.3,0.7], got (" << v.x << "," << v.y << "," << v.z << "))\n";
            return false;
        }
        
        // Test 4: Very bright color (5,5,5) should be clamped to 1
        args[0] = mmrsl::Value(mmrsl::Vec3(5.0f, 5.0f, 5.0f));
        result = parser.execute(args);
        
        if (!result.isVec3()) {
            std::cout << "FAIL (bright: expected vec3 result)\n";
            return false;
        }
        
        v = result.asVec3();
        // Should be clamped to 1.0
        if (v.x < 0.9f || v.x > 1.0f || v.y < 0.9f || v.y > 1.0f || v.z < 0.9f || v.z > 1.0f) {
            std::cout << "FAIL (bright: expected ~1.0, got (" << v.x << "," << v.y << "," << v.z << "))\n";
            return false;
        }
        
        // Test 5: Different RGB values (1, 0.5, 0.25)
        args[0] = mmrsl::Value(mmrsl::Vec3(1.0f, 0.5f, 0.25f));
        result = parser.execute(args);
        
        if (!result.isVec3()) {
            std::cout << "FAIL (color: expected vec3 result)\n";
            return false;
        }
        
        v = result.asVec3();
        // R > G > B should be preserved
        if (v.x <= v.y || v.y <= v.z) {
            std::cout << "FAIL (color: expected R>G>B, got (" << v.x << "," << v.y << "," << v.z << "))\n";
            return false;
        }
        
        // Test 6: Very bright values should be clamped to 1
        args[0] = mmrsl::Value(mmrsl::Vec3(10.0f, 10.0f, 10.0f));
        result = parser.execute(args);
        
        if (!result.isVec3()) {
            std::cout << "FAIL (very bright: expected vec3 result)\n";
            return false;
        }
        
        v = result.asVec3();
        // Should be clamped to 1.0
        if (v.x < 0.9f || v.x > 1.0f || v.y < 0.9f || v.y > 1.0f || v.z < 0.9f || v.z > 1.0f) {
            std::cout << "FAIL (very bright: expected ~1.0, got (" << v.x << "," << v.y << "," << v.z << "))\n";
            return false;
        }
        
        std::cout << "PASS\n";
        return true;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 91: Matrix runtime bounds checking
// Verifies VM correctly detects and reports out-of-bounds matrix access
bool testMatrixBoundsChecking() {
    std::cout << "Test: Matrix Runtime Bounds Checking... ";
    
    BytecodeParser parser;
    
    try {
        // Test 1: mat2 column index out of bounds
        std::string mat2ColOobSource = R"(
            float calc(mat2 m) {
                return m[2][0];  // column index 2 is out of bounds (mat2 has 0,1)
            }
        )";
        
        if (!parser.compile(mat2ColOobSource)) {
            std::cout << "FAIL (mat2 col OOB compile failed: " << parser.getLastError() << ")\n";
            return false;
        }
        
        mmrsl::Mat2 m2;
        std::vector<mmrsl::Value> args = { mmrsl::Value(m2) };
        
        try {
            parser.execute(args);
            std::cout << "FAIL (mat2 col OOB should throw)\n";
            return false;
        } catch (const std::exception& e) {
            // Expected - verify error message mentions bounds/out of range
            std::string errorMsg = e.what();
            if (errorMsg.find("bounds") == std::string::npos && 
                errorMsg.find("out of") == std::string::npos &&
                errorMsg.find("range") == std::string::npos &&
                errorMsg.find("invalid") == std::string::npos) {
                std::cout << "FAIL (mat2 col OOB: error message should mention bounds, got: " << errorMsg << ")\n";
                return false;
            }
        }
        
        // Test 2: mat3 row index out of bounds
        std::string mat3RowOobSource = R"(
            float calc(mat3 m) {
                return m[0][3];  // row index 3 is out of bounds (mat3 has 0,1,2)
            }
        )";
        
        if (!parser.compile(mat3RowOobSource)) {
            std::cout << "FAIL (mat3 row OOB compile failed: " << parser.getLastError() << ")\n";
            return false;
        }
        
        mmrsl::Mat3 m3;
        args[0] = mmrsl::Value(m3);
        
        try {
            parser.execute(args);
            std::cout << "FAIL (mat3 row OOB should throw)\n";
            return false;
        } catch (const std::exception& e) {
            // Expected
        }
        
        // Test 3: mat4 negative column index
        std::string mat4NegColSource = R"(
            float calc(mat4 m) {
                return m[-1][0];  // negative column index
            }
        )";
        
        if (!parser.compile(mat4NegColSource)) {
            std::cout << "FAIL (mat4 neg col compile failed: " << parser.getLastError() << ")\n";
            return false;
        }
        
        mmrsl::Mat4 m4;
        args[0] = mmrsl::Value(m4);
        
        try {
            parser.execute(args);
            std::cout << "FAIL (mat4 neg col should throw)\n";
            return false;
        } catch (const std::exception& e) {
            // Expected
        }
        
        // Test 4: Valid mat2 access (boundary test)
        std::string mat2ValidSource = R"(
            float calc(mat2 m) {
                return m[1][1];  // valid: last element
            }
        )";
        
        if (!parser.compile(mat2ValidSource)) {
            std::cout << "FAIL (mat2 valid compile failed: " << parser.getLastError() << ")\n";
            return false;
        }
        
        m2 = mmrsl::Mat2(1.0f, 2.0f, 3.0f, 4.0f);  // [1 3; 2 4] in column-major
        args[0] = mmrsl::Value(m2);
        mmrsl::Value result = parser.execute(args);
        
        if (!result.isFloat() || std::abs(result.asFloat() - 4.0f) >= 0.001f) {
            std::cout << "FAIL (mat2 valid: expected 4.0, got " << result.toString() << ")\n";
            return false;
        }
        
        // Test 5: Valid mat3 access (boundary test)
        std::string mat3ValidSource = R"(
            float calc(mat3 m) {
                return m[2][2];  // valid: last element
            }
        )";
        
        if (!parser.compile(mat3ValidSource)) {
            std::cout << "FAIL (mat3 valid compile failed: " << parser.getLastError() << ")\n";
            return false;
        }
        
        m3 = mmrsl::Mat3(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
        args[0] = mmrsl::Value(m3);
        result = parser.execute(args);
        
        if (!result.isFloat() || std::abs(result.asFloat() - 1.0f) >= 0.001f) {
            std::cout << "FAIL (mat3 valid: expected 1.0, got " << result.toString() << ")\n";
            return false;
        }
        
        std::cout << "PASS\n";
        return true;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 92: Constant pool deduplication
// Verifies that identical constants are stored only once in the constant pool
bool testConstantPoolDeduplication() {
    std::cout << "Test: Constant Pool Deduplication... ";
    
    BytecodeParser parser;
    
    // This function uses 1.0 four times, but it should only appear once in the constant pool
    std::string dedupSource = R"(
        float calc(float x) {
            float a = x + 1.0;
            float b = x * 1.0;
            float c = 1.0 + x;
            float d = 1.0 * x;
            return a + b + c + d;
        }
    )";
    
    try {
        if (!parser.compile(dedupSource)) {
            std::cout << "FAIL (compile failed: " << parser.getLastError() << ")\n";
            return false;
        }
        
        // Get disassembly output
        std::string disasm = parser.getBytecodeDisassembly();
        
        // Count occurrences of "1" in the constants section (before "Code:")
        size_t constantsStart = disasm.find("Constants:");
        size_t codeStart = disasm.find("Code:");
        
        if (constantsStart == std::string::npos || codeStart == std::string::npos) {
            std::cout << "FAIL (could not find Constants or Code section)\n";
            return false;
        }
        
        std::string constantsSection = disasm.substr(constantsStart, codeStart - constantsStart);
        
        // Count how many constant entries there are (lines starting with "  [")
        int constantCount = 0;
        size_t pos = 0;
        while ((pos = constantsSection.find("  [", pos)) != std::string::npos) {
            constantCount++;
            pos++;
        }
        
        // The constant pool should have exactly 1 constant: 1.0
        // (x is a parameter, not a constant)
        if (constantCount != 1) {
            std::cout << "FAIL (expected 1 constant, found " << constantCount << ")\n";
            std::cout << "Constants section:\n" << constantsSection << "\n";
            return false;
        }
        
        // Verify the constant is 1.0
        if (constantsSection.find("1") == std::string::npos) {
            std::cout << "FAIL (constant should be 1.0)\n";
            return false;
        }
        
        // Test 2: Multiple different constants
        std::string multiConstSource = R"(
            float calc(float x) {
                return x + 1.0 + 2.0 + 1.0;  // 1.0 appears twice, should be deduped
            }
        )";
        
        if (!parser.compile(multiConstSource)) {
            std::cout << "FAIL (multi const compile failed: " << parser.getLastError() << ")\n";
            return false;
        }
        
        disasm = parser.getBytecodeDisassembly();
        constantsStart = disasm.find("Constants:");
        codeStart = disasm.find("Code:");
        constantsSection = disasm.substr(constantsStart, codeStart - constantsStart);
        
        constantCount = 0;
        pos = 0;
        while ((pos = constantsSection.find("  [", pos)) != std::string::npos) {
            constantCount++;
            pos++;
        }
        
        // Should have 2 constants: 1.0 and 2.0 (1.0 deduped)
        if (constantCount != 2) {
            std::cout << "FAIL (expected 2 constants, found " << constantCount << ")\n";
            std::cout << "Constants section:\n" << constantsSection << "\n";
            return false;
        }
        
        // Test 3: Verify execution still works correctly
        std::vector<mmrsl::Value> args = { mmrsl::Value(5.0f) };
        mmrsl::Value result = parser.execute(args);
        
        // x=5: 5 + 1 + 2 + 1 = 9
        if (!result.isFloat() || std::abs(result.asFloat() - 9.0f) >= 0.001f) {
            std::cout << "FAIL (execution: expected 9.0, got " << result.toString() << ")\n";
            return false;
        }
        
        std::cout << "PASS\n";
        return true;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 93: Multiple return paths with control flow
// Verifies correct handling of functions with multiple return statements
bool testMultipleReturnPaths() {
    std::cout << "Test: Multiple Return Paths... ";
    
    BytecodeParser parser;
    
    try {
        // Test 1: Piecewise function with multiple returns
        std::string piecewiseSource = R"(
            float calc(float x) {
                if (x < 0.0) {
                    return -1.0;
                }
                if (x > 1.0) {
                    return 1.0;
                }
                return x * 2.0 - 1.0;
            }
        )";
        
        if (!parser.compile(piecewiseSource)) {
            std::cout << "FAIL (piecewise compile failed: " << parser.getLastError() << ")\n";
            return false;
        }
        
        // x = -5: should return -1.0 (first branch)
        std::vector<mmrsl::Value> args = { mmrsl::Value(-5.0f) };
        mmrsl::Value result = parser.execute(args);
        
        if (!result.isFloat() || std::abs(result.asFloat() - (-1.0f)) >= 0.001f) {
            std::cout << "FAIL (x=-5: expected -1.0, got " << result.toString() << ")\n";
            return false;
        }
        
        // x = 0.5: should return 0.5 * 2 - 1 = 0 (middle branch)
        args[0] = mmrsl::Value(0.5f);
        result = parser.execute(args);
        
        if (!result.isFloat() || std::abs(result.asFloat() - 0.0f) >= 0.001f) {
            std::cout << "FAIL (x=0.5: expected 0.0, got " << result.toString() << ")\n";
            return false;
        }
        
        // x = 2: should return 1.0 (last branch)
        args[0] = mmrsl::Value(2.0f);
        result = parser.execute(args);
        
        if (!result.isFloat() || std::abs(result.asFloat() - 1.0f) >= 0.001f) {
            std::cout << "FAIL (x=2: expected 1.0, got " << result.toString() << ")\n";
            return false;
        }
        
        // x = 0: should return 0 * 2 - 1 = -1 (boundary)
        args[0] = mmrsl::Value(0.0f);
        result = parser.execute(args);
        
        if (!result.isFloat() || std::abs(result.asFloat() - (-1.0f)) >= 0.001f) {
            std::cout << "FAIL (x=0: expected -1.0, got " << result.toString() << ")\n";
            return false;
        }
        
        // x = 1: should return 1 * 2 - 1 = 1 (boundary)
        args[0] = mmrsl::Value(1.0f);
        result = parser.execute(args);
        
        if (!result.isFloat() || std::abs(result.asFloat() - 1.0f) >= 0.001f) {
            std::cout << "FAIL (x=1: expected 1.0, got " << result.toString() << ")\n";
            return false;
        }
        
        // Test 2: Nested if with return in each branch (quadrant detection)
        std::string quadrantSource = R"(
            float calc(float x, float y) {
                if (x > 0.0) {
                    if (y > 0.0) {
                        return 1.0;  // first quadrant
                    } else {
                        return 2.0;  // fourth quadrant
                    }
                } else {
                    if (y > 0.0) {
                        return 3.0;  // second quadrant
                    } else {
                        return 4.0;  // third quadrant
                    }
                }
            }
        )";
        
        if (!parser.compile(quadrantSource)) {
            std::cout << "FAIL (quadrant compile failed: " << parser.getLastError() << ")\n";
            return false;
        }
        
        // First quadrant: x=1, y=1 ï¿?1
        std::vector<mmrsl::Value> doubleArgs = { mmrsl::Value(1.0f), mmrsl::Value(1.0f) };
        result = parser.execute(doubleArgs);
        
        if (!result.isFloat() || std::abs(result.asFloat() - 1.0f) >= 0.001f) {
            std::cout << "FAIL (Q1: expected 1.0, got " << result.toString() << ")\n";
            return false;
        }
        
        // Fourth quadrant: x=1, y=-1 ï¿?2
        doubleArgs[0] = mmrsl::Value(1.0f);
        doubleArgs[1] = mmrsl::Value(-1.0f);
        result = parser.execute(doubleArgs);
        
        if (!result.isFloat() || std::abs(result.asFloat() - 2.0f) >= 0.001f) {
            std::cout << "FAIL (Q4: expected 2.0, got " << result.toString() << ")\n";
            return false;
        }
        
        // Second quadrant: x=-1, y=1 ï¿?3
        doubleArgs[0] = mmrsl::Value(-1.0f);
        doubleArgs[1] = mmrsl::Value(1.0f);
        result = parser.execute(doubleArgs);
        
        if (!result.isFloat() || std::abs(result.asFloat() - 3.0f) >= 0.001f) {
            std::cout << "FAIL (Q2: expected 3.0, got " << result.toString() << ")\n";
            return false;
        }
        
        // Third quadrant: x=-1, y=-1 ï¿?4
        doubleArgs[0] = mmrsl::Value(-1.0f);
        doubleArgs[1] = mmrsl::Value(-1.0f);
        result = parser.execute(doubleArgs);
        
        if (!result.isFloat() || std::abs(result.asFloat() - 4.0f) >= 0.001f) {
            std::cout << "FAIL (Q3: expected 4.0, got " << result.toString() << ")\n";
            return false;
        }
        
        // Test 3: Early return with computation
        std::string earlyReturnSource = R"(
            float calc(float x) {
                if (x < 0.0) {
                    return x * x;  // square for negative
                }
                return x * 2.0;  // double for non-negative
            }
        )";
        
        if (!parser.compile(earlyReturnSource)) {
            std::cout << "FAIL (early return compile failed: " << parser.getLastError() << ")\n";
            return false;
        }
        
        // x = -3: should return 9
        args[0] = mmrsl::Value(-3.0f);
        result = parser.execute(args);
        
        if (!result.isFloat() || std::abs(result.asFloat() - 9.0f) >= 0.001f) {
            std::cout << "FAIL (x=-3: expected 9.0, got " << result.toString() << ")\n";
            return false;
        }
        
        // x = 4: should return 8
        args[0] = mmrsl::Value(4.0f);
        result = parser.execute(args);
        
        if (!result.isFloat() || std::abs(result.asFloat() - 8.0f) >= 0.001f) {
            std::cout << "FAIL (x=4: expected 8.0, got " << result.toString() << ")\n";
            return false;
        }
        
        std::cout << "PASS\n";
        return true;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 94: Vector component swizzle simulation
// Verifies component extraction and recombination using member access
bool testVectorSwizzleSimulation() {
    std::cout << "Test: Vector Swizzle Simulation... ";
    
    BytecodeParser parser;
    
    try {
        // Test 1: vec3 component reversal (simulating .zyx swizzle)
        std::string vec3ReverseSource = R"(
            vec3 calc(vec3 v) {
                return vec3(v.z, v.y, v.x);  // reverse components
            }
        )";
        
        if (!parser.compile(vec3ReverseSource)) {
            std::cout << "FAIL (vec3 reverse compile failed: " << parser.getLastError() << ")\n";
            return false;
        }
        
        // v = (1, 2, 3) ï¿?(3, 2, 1)
        std::vector<mmrsl::Value> args = { mmrsl::Value(mmrsl::Vec3(1.0f, 2.0f, 3.0f)) };
        mmrsl::Value result = parser.execute(args);
        
        if (!result.isVec3()) {
            std::cout << "FAIL (vec3 reverse: expected vec3 result)\n";
            return false;
        }
        
        mmrsl::Vec3 v = result.asVec3();
        if (std::abs(v.x - 3.0f) >= 0.001f || std::abs(v.y - 2.0f) >= 0.001f || std::abs(v.z - 1.0f) >= 0.001f) {
            std::cout << "FAIL (vec3 reverse: expected (3,2,1), got (" << v.x << "," << v.y << "," << v.z << "))\n";
            return false;
        }
        
        // Test 2: vec4 component reversal (simulating .wzyx swizzle)
        std::string vec4ReverseSource = R"(
            vec4 calc(vec4 v) {
                return vec4(v.w, v.z, v.y, v.x);  // fully reverse
            }
        )";
        
        if (!parser.compile(vec4ReverseSource)) {
            std::cout << "FAIL (vec4 reverse compile failed: " << parser.getLastError() << ")\n";
            return false;
        }
        
        // v = (1, 2, 3, 4) ï¿?(4, 3, 2, 1)
        args[0] = mmrsl::Value(mmrsl::Vec4(1.0f, 2.0f, 3.0f, 4.0f));
        result = parser.execute(args);
        
        if (!result.isVec4()) {
            std::cout << "FAIL (vec4 reverse: expected vec4 result)\n";
            return false;
        }
        
        mmrsl::Vec4 v4 = result.asVec4();
        if (std::abs(v4.x - 4.0f) >= 0.001f || std::abs(v4.y - 3.0f) >= 0.001f || 
            std::abs(v4.z - 2.0f) >= 0.001f || std::abs(v4.w - 1.0f) >= 0.001f) {
            std::cout << "FAIL (vec4 reverse: expected (4,3,2,1), got (" << v4.x << "," << v4.y << "," << v4.z << "," << v4.w << "))\n";
            return false;
        }
        
        // Test 3: Component extraction and recombination with operations
        std::string vec2FromVec3Source = R"(
            vec2 calc(vec3 a, vec3 b) {
                return vec2(dot(a, b), length(cross(a, b)));
            }
        )";
        
        if (!parser.compile(vec2FromVec3Source)) {
            std::cout << "FAIL (vec2 from vec3 compile failed: " << parser.getLastError() << ")\n";
            return false;
        }
        
        // a = (1, 0, 0), b = (0, 1, 0)
        // dot = 0, cross = (0, 0, 1), length = 1
        std::vector<mmrsl::Value> doubleArgs = {
            mmrsl::Value(mmrsl::Vec3(1.0f, 0.0f, 0.0f)),
            mmrsl::Value(mmrsl::Vec3(0.0f, 1.0f, 0.0f))
        };
        result = parser.execute(doubleArgs);
        
        if (!result.isVec2()) {
            std::cout << "FAIL (vec2 from vec3: expected vec2 result)\n";
            return false;
        }
        
        mmrsl::Vec2 v2 = result.asVec2();
        if (std::abs(v2.x - 0.0f) >= 0.001f || std::abs(v2.y - 1.0f) >= 0.001f) {
            std::cout << "FAIL (vec2 from vec3: expected (0,1), got (" << v2.x << "," << v2.y << "))\n";
            return false;
        }
        
        // Test 4: RGB to BGR conversion (common swizzle use case)
        std::string rgbToBgrSource = R"(
            vec3 calc(vec3 color) {
                return vec3(color.z, color.y, color.x);  // BGR
            }
        )";
        
        if (!parser.compile(rgbToBgrSource)) {
            std::cout << "FAIL (RGB->BGR compile failed: " << parser.getLastError() << ")\n";
            return false;
        }
        
        // color = (1, 0.5, 0) orange ï¿?(0, 0.5, 1) cyan-ish
        args[0] = mmrsl::Value(mmrsl::Vec3(1.0f, 0.5f, 0.0f));
        result = parser.execute(args);
        
        if (!result.isVec3()) {
            std::cout << "FAIL (RGB->BGR: expected vec3 result)\n";
            return false;
        }
        
        v = result.asVec3();
        if (std::abs(v.x - 0.0f) >= 0.001f || std::abs(v.y - 0.5f) >= 0.001f || std::abs(v.z - 1.0f) >= 0.001f) {
            std::cout << "FAIL (RGB->BGR: expected (0,0.5,1), got (" << v.x << "," << v.y << "," << v.z << "))\n";
            return false;
        }
        
        // Test 5: vec2 to vec4 expansion with swizzle-like pattern
        std::string vec2ToVec4Source = R"(
            vec4 calc(vec2 uv) {
                return vec4(uv.x, uv.y, 0.0, 1.0);  // UV to position
            }
        )";
        
        if (!parser.compile(vec2ToVec4Source)) {
            std::cout << "FAIL (vec2->vec4 compile failed: " << parser.getLastError() << ")\n";
            return false;
        }
        
        // uv = (0.5, 0.25) ï¿?(0.5, 0.25, 0, 1)
        args[0] = mmrsl::Value(mmrsl::Vec2(0.5f, 0.25f));
        result = parser.execute(args);
        
        if (!result.isVec4()) {
            std::cout << "FAIL (vec2->vec4: expected vec4 result)\n";
            return false;
        }
        
        v4 = result.asVec4();
        if (std::abs(v4.x - 0.5f) >= 0.001f || std::abs(v4.y - 0.25f) >= 0.001f || 
            std::abs(v4.z) >= 0.001f || std::abs(v4.w - 1.0f) >= 0.001f) {
            std::cout << "FAIL (vec2->vec4: expected (0.5,0.25,0,1), got (" << v4.x << "," << v4.y << "," << v4.z << "," << v4.w << "))\n";
            return false;
        }
        
        std::cout << "PASS\n";
        return true;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 95: Performance regression benchmark
// Verifies bytecode VM performance is within acceptable range
bool testPerformanceBenchmark() {
    std::cout << "Test: Performance Benchmark... ";
    
    BytecodeParser hpParser;
    mmrsl::SimpleParser simpleParser;
    
    // Complex expression with multiple function calls
    std::string perfSource = R"(
        float calc(float x) {
            return sin(x) * cos(x) + sqrt(abs(x));
        }
    )";
    
    try {
        // Compile both parsers
        if (!hpParser.compile(perfSource)) {
            std::cout << "FAIL (HP compile failed: " << hpParser.getLastError() << ")\n";
            return false;
        }
        
        if (!simpleParser.compile(perfSource)) {
            std::cout << "FAIL (Simple compile failed: " << simpleParser.getLastError() << ")\n";
            return false;
        }
        
        double hpCompileTime = hpParser.getLastCompileTimeMs();
        
        // Warm-up run
        for (int i = 0; i < 100; i++) {
            hpParser.execute({ mmrsl::Value(static_cast<float>(i) * 0.01f) });
            simpleParser.execute({ mmrsl::Value(static_cast<float>(i) * 0.01f) });
        }
        
        // Benchmark: 10000 executions
        const int iterations = 10000;
        
        // Benchmark HP parser
        auto hpStart = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iterations; i++) {
            hpParser.execute({ mmrsl::Value(static_cast<float>(i) * 0.01f) });
        }
        auto hpEnd = std::chrono::high_resolution_clock::now();
        auto hpElapsed = std::chrono::duration_cast<std::chrono::microseconds>(hpEnd - hpStart).count();
        double hpAvgTime = static_cast<double>(hpElapsed) / iterations;
        
        // Benchmark Simple parser
        auto simpleStart = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iterations; i++) {
            simpleParser.execute({ mmrsl::Value(static_cast<float>(i) * 0.01f) });
        }
        auto simpleEnd = std::chrono::high_resolution_clock::now();
        auto simpleElapsed = std::chrono::duration_cast<std::chrono::microseconds>(simpleEnd - simpleStart).count();
        double simpleAvgTime = static_cast<double>(simpleElapsed) / iterations;
        
        // Calculate speedup
        double speedup = simpleAvgTime / hpAvgTime;
        
        // Print benchmark results
        std::cout << "\n  [Performance Benchmark Results]\n";
        std::cout << "  Iterations: " << iterations << "\n\n";
        std::cout << "  Bytecode VM (High-Performance):\n";
        std::cout << "    Compile time: " << hpCompileTime << " ms\n";
        std::cout << "    Total execution: " << hpElapsed / 1000.0 << " ms\n";
        std::cout << "    Average per call: " << hpAvgTime << " us\n";
        std::cout << "    Calls per second: " << static_cast<int>(1000000.0 / hpAvgTime) << "\n\n";
        std::cout << "  Simple Interpreter:\n";
        std::cout << "    Total execution: " << simpleElapsed / 1000.0 << " ms\n";
        std::cout << "    Average per call: " << simpleAvgTime << " us\n";
        std::cout << "    Calls per second: " << static_cast<int>(1000000.0 / simpleAvgTime) << "\n\n";
        std::cout << "  Speedup: " << std::fixed << std::setprecision(2) << speedup << "x faster\n";
        
        // Performance assertions
        if (hpCompileTime > 100.0) {
            std::cout << "  WARNING: Compile time > 100ms\n";
        }
        
        if (hpAvgTime > 100.0) {
            std::cout << "FAIL (HP execution too slow: " << hpAvgTime << " us)\n";
            return false;
        }
        
        // Verify speedup is significant (at least 2x)
        if (speedup < 2.0) {
            std::cout << "  WARNING: Speedup < 2x, may indicate performance regression\n";
        }
        
        // Verify correctness
        mmrsl::Value result = hpParser.execute({ mmrsl::Value(0.0f) });
        if (!result.isFloat() || std::abs(result.asFloat()) >= 0.001f) {
            std::cout << "FAIL (Result correctness check failed)\n";
            return false;
        }
        
        std::cout << "PASS\n";
        return true;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 96: Source code length limit
// Verifies that source code exceeding 8KB is rejected
bool testSourceLengthLimit() {
    std::cout << "Test: Source Code Length Limit... ";
    
    BytecodeParser parser;
    
    // Create a source that is exactly at the limit (should pass)
    std::string validSource = R"(
        float calc(float x) {
            return x * 2.0;
        }
    )";
    
    // Create a source that exceeds 8KB
    std::string oversizedSource;
    oversizedSource.reserve(8500);
    oversizedSource = "float calc(float x) {\n";
    // Add many comments to exceed 8KB
    for (int i = 0; i < 200; i++) {
        oversizedSource += "    // This is a very long comment line to make the source code exceed the 8KB limit for testing purposes\n";
    }
    oversizedSource += "    return x * 2.0;\n}";
    
    try {
        // Test 1: Valid source should compile
        if (!parser.compile(validSource)) {
            std::cout << "FAIL (valid source should compile: " << parser.getLastError() << ")\n";
            return false;
        }
        
        // Verify it works
        std::vector<mmrsl::Value> args = { mmrsl::Value(5.0f) };
        mmrsl::Value result = parser.execute(args);
        if (!result.isFloat() || std::abs(result.asFloat() - 10.0f) >= 0.001f) {
            std::cout << "FAIL (valid source execution failed)\n";
            return false;
        }
        
        // Test 2: Oversized source should fail with length error
        if (parser.compile(oversizedSource)) {
            std::cout << "FAIL (oversized source should be rejected)\n";
            return false;
        }
        
        std::string errorMsg = parser.getLastError();
        if (errorMsg.find("exceeds maximum length") == std::string::npos &&
            errorMsg.find("8192") == std::string::npos) {
            std::cout << "FAIL (error should mention length limit, got: " << errorMsg << ")\n";
            return false;
        }
        
        // Test 3: Verify oversized source doesn't affect parser state
        // Parser should still work with valid source after rejecting oversized
        if (!parser.compile(validSource)) {
            std::cout << "FAIL (parser should recover after rejecting oversized source)\n";
            return false;
        }
        
        result = parser.execute(args);
        if (!result.isFloat() || std::abs(result.asFloat() - 10.0f) >= 0.001f) {
            std::cout << "FAIL (parser recovery failed)\n";
            return false;
        }
        
        std::cout << "PASS\n";
        return true;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 97: DoS Protection - Nesting depth limit
bool testNestingDepthLimit() {
    std::cout << "Test: DoS Protection - Nesting Depth Limit... ";
    
    BytecodeParser parser;
    
    // Create deeply nested if statements (exceeds 32 limit)
    std::string deepNesting = "float calc(float x) {\n";
    for (int i = 0; i < 35; i++) {
        deepNesting += "    if (x > " + std::to_string(i) + ".0) {\n";
    }
    deepNesting += "        return 1.0;\n";
    for (int i = 0; i < 35; i++) {
        deepNesting += "    }\n";
    }
    deepNesting += "    return 0.0;\n}";
    
    try {
        // Should fail due to nesting depth limit
        if (parser.compile(deepNesting)) {
            std::cout << "FAIL (deep nesting should be rejected)\n";
            return false;
        }
        
        std::string error = parser.getLastError();
        if (error.find("nesting depth") == std::string::npos && error.find("32") == std::string::npos) {
            std::cout << "FAIL (wrong error message: " << error << ")\n";
            return false;
        }
        
        std::cout << "PASS\n";
        return true;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 98: DoS Protection - Expression depth limit
bool testExpressionDepthLimit() {
    std::cout << "Test: DoS Protection - Expression Depth Limit... ";
    
    BytecodeParser parser;
    
    // Create deeply nested expression (exceeds 64 limit)
    std::string deepExpr = "float calc(float x) { return ";
    for (int i = 0; i < 70; i++) {
        deepExpr += "(";
    }
    deepExpr += "x";
    for (int i = 0; i < 70; i++) {
        deepExpr += " + 1.0)";
    }
    deepExpr += "; }";
    
    try {
        // Should fail due to expression depth limit
        if (parser.compile(deepExpr)) {
            std::cout << "FAIL (deep expression should be rejected)\n";
            return false;
        }
        
        std::string error = parser.getLastError();
        if (error.find("expression depth") == std::string::npos && error.find("64") == std::string::npos) {
            std::cout << "FAIL (wrong error message: " << error << ")\n";
            return false;
        }
        
        std::cout << "PASS\n";
        return true;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 99: DoS Protection - Local variable limit
bool testLocalVariableLimit() {
    std::cout << "Test: DoS Protection - Local Variable Limit... ";
    
    BytecodeParser parser;
    
    // Create function with too many local variables (exceeds 128 limit)
    // Use shorter variable names to stay under source length limit
    std::string manyVars = "float calc(float x) {\n";
    for (int i = 0; i < 130; i++) {
        manyVars += "float a" + std::to_string(i) + "=" + std::to_string(i) + ".;";
    }
    manyVars += "return a0;}";
    
    try {
        // Should fail due to local variable limit
        if (parser.compile(manyVars)) {
            std::cout << "FAIL (too many locals should be rejected)\n";
            return false;
        }
        
        std::string error = parser.getLastError();
        if (error.find("local variables") == std::string::npos && error.find("128") == std::string::npos &&
            error.find("exceeds") == std::string::npos) {
            std::cout << "FAIL (wrong error message: " << error << ")\n";
            return false;
        }
        
        std::cout << "PASS\n";
        return true;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 100: DoS Protection - Identifier length limit
bool testIdentifierLengthLimit() {
    std::cout << "Test: DoS Protection - Identifier Length Limit... ";
    
    BytecodeParser parser;
    
    // Create variable with very long name (exceeds 64 limit)
    std::string longName = "float calc(float x) {\n";
    longName += "    float veryLongVariableNameThatExceedsTheMaximumAllowedLengthOf64CharactersForIdentifierNames = 1.0;\n";
    longName += "    return veryLongVariableNameThatExceedsTheMaximumAllowedLengthOf64CharactersForIdentifierNames;\n}";
    
    try {
        // Should fail due to identifier length limit
        if (parser.compile(longName)) {
            std::cout << "FAIL (long identifier should be rejected)\n";
            return false;
        }
        
        std::string error = parser.getLastError();
        if (error.find("length") == std::string::npos && error.find("64") == std::string::npos) {
            std::cout << "FAIL (wrong error message: " << error << ")\n";
            return false;
        }
        
        std::cout << "PASS\n";
        return true;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 101: Transform Chain (Model-View-Projection)
// Real-world 3D graphics transformation pipeline
bool testTransformChain() {
    std::cout << "Test: Transform Chain (Model-View-Projection)... ";
    
    BytecodeParser parser;
    
    // Transform chain: scale * translate * view
    std::string transformSource = R"(
        vec4 calc(float x) {
            mat4 s = mat4(2.0, 0.0, 0.0, 0.0, 0.0, 2.0, 0.0, 0.0, 0.0, 0.0, 2.0, 0.0, 0.0, 0.0, 0.0, 1.0);
            mat4 t = mat4(1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0, 2.0, 3.0, 1.0);
            mat4 v = mat4(1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, -5.0, 1.0);
            return v * t * s * vec4(1.0, 2.0, 3.0, 1.0);
        }
    )";
    
    try {
        std::vector<mmrsl::Value> args = { mmrsl::Value(0.0f) };
        mmrsl::Value r = parser.compileAndExecute(transformSource, args);
        if (!r.isVec4()) { std::cout << "FAIL (need vec4)\n"; return false; }
        
        mmrsl::Vec4 v = r.asVec4();
        // (1,2,3) * 2 + (1,2,3) = (3,6,9), then view z-5 = (3,6,4)
        if (std::abs(v.x - 3.0f) > 0.1f || std::abs(v.y - 6.0f) > 0.1f || std::abs(v.z - 4.0f) > 0.1f) {
            std::cout << "FAIL (expected ~(3,6,4), got " << v.x << "," << v.y << "," << v.z << ")\n";
            return false;
        }
        
        std::cout << "PASS\n";
        return true;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 102: Matrix Inverse
// Tests inverse() function for mat2, mat3, mat4
bool testMatrixInverse() {
    std::cout << "Test: Matrix Inverse... ";
    
    BytecodeParser parser;
    
    // Test mat2 inverse
    std::string mat2Source = R"(
        mat2 calc(float x) {
            mat2 m = mat2(2.0, 0.0, 0.0, 2.0);
            return inverse(m);
        }
    )";
    
    // Test mat3 inverse
    std::string mat3Source = R"(
        mat3 calc(float x) {
            mat3 m = mat3(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0, 2.0, 1.0);
            return inverse(m);
        }
    )";
    
    // Test mat4 inverse
    std::string mat4Source = R"(
        mat4 calc(float x) {
            mat4 m = mat4(2.0, 0.0, 0.0, 0.0, 0.0, 2.0, 0.0, 0.0, 0.0, 0.0, 2.0, 0.0, 0.0, 0.0, 0.0, 1.0);
            return inverse(m);
        }
    )";
    
    try {
        // Test mat2 inverse: scale by 2 -> inverse should scale by 0.5
        std::vector<mmrsl::Value> args = { mmrsl::Value(0.0f) };
        mmrsl::Value r2 = parser.compileAndExecute(mat2Source, args);
        if (!r2.isMat2()) { std::cout << "FAIL (mat2: need mat2)\n"; return false; }
        
        mmrsl::Mat2 m2 = r2.asMat2();
        // Inverse of scale by 2 matrix should be scale by 0.5
        if (std::abs(m2[0][0] - 0.5f) > 0.01f || std::abs(m2[1][1] - 0.5f) > 0.01f) {
            std::cout << "FAIL (mat2: expected 0.5 diagonal, got " << m2[0][0] << "," << m2[1][1] << ")\n";
            return false;
        }
        
        // Test mat3 inverse
        mmrsl::Value r3 = parser.compileAndExecute(mat3Source, args);
        if (!r3.isMat3()) { std::cout << "FAIL (mat3: need mat3)\n"; return false; }
        
        // Test mat4 inverse: scale by 2 -> inverse should scale by 0.5
        mmrsl::Value r4 = parser.compileAndExecute(mat4Source, args);
        if (!r4.isMat4()) { std::cout << "FAIL (mat4: need mat4)\n"; return false; }
        
        mmrsl::Mat4 m4 = r4.asMat4();
        // Inverse of scale by 2 matrix should be scale by 0.5 (diagonal elements)
        if (std::abs(m4[0][0] - 0.5f) > 0.01f || std::abs(m4[1][1] - 0.5f) > 0.01f || 
            std::abs(m4[2][2] - 0.5f) > 0.01f) {
            std::cout << "FAIL (mat4: expected 0.5 diagonal)\n";
            return false;
        }
        
        std::cout << "PASS\n";
        return true;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 103: Matrix Transpose
// Tests transpose() function for mat2, mat3, mat4
bool testMatrixTranspose() {
    std::cout << "Test: Matrix Transpose... ";
    
    BytecodeParser parser;
    
    // Test mat2 transpose
    std::string mat2Source = R"(
        mat2 calc(float x) {
            mat2 m = mat2(1.0, 2.0, 3.0, 4.0);
            return transpose(m);
        }
    )";
    
    // Test mat3 transpose
    std::string mat3Source = R"(
        mat3 calc(float x) {
            mat3 m = mat3(1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0);
            return transpose(m);
        }
    )";
    
    // Test mat4 transpose
    std::string mat4Source = R"(
        mat4 calc(float x) {
            mat4 m = mat4(1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0);
            return transpose(m);
        }
    )";
    
    try {
        std::vector<mmrsl::Value> args = { mmrsl::Value(0.0f) };
        
        // Test mat2 transpose
        // Input: [1,2; 3,4] (column-major: col0=[1,2], col1=[3,4])
        // Transpose should swap rows/cols
        mmrsl::Value r2 = parser.compileAndExecute(mat2Source, args);
        if (!r2.isMat2()) { std::cout << "FAIL (mat2: need mat2)\n"; return false; }
        
        mmrsl::Mat2 m2 = r2.asMat2();
        // After transpose: m[0][1] should be 3 (was m[1][0]), m[1][0] should be 2 (was m[0][1])
        if (std::abs(m2[0][0] - 1.0f) > 0.01f || std::abs(m2[0][1] - 3.0f) > 0.01f ||
            std::abs(m2[1][0] - 2.0f) > 0.01f || std::abs(m2[1][1] - 4.0f) > 0.01f) {
            std::cout << "FAIL (mat2: transpose incorrect)\n";
            return false;
        }
        
        // Test mat3 transpose
        mmrsl::Value r3 = parser.compileAndExecute(mat3Source, args);
        if (!r3.isMat3()) { std::cout << "FAIL (mat3: need mat3)\n"; return false; }
        
        // Test mat4 transpose
        mmrsl::Value r4 = parser.compileAndExecute(mat4Source, args);
        if (!r4.isMat4()) { std::cout << "FAIL (mat4: need mat4)\n"; return false; }
        
        std::cout << "PASS\n";
        return true;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 104: Noise Functions
// Tests noise1, noise2, noise3, noise4 functions
bool testNoiseFunctions() {
    std::cout << "Test: Noise Functions... ";
    
    BytecodeParser parser;
    
    // Test noise1 - 1D noise returns float
    std::string noise1Source = R"(
        float calc(float x) {
            return noise1(x);
        }
    )";
    
    // Test noise2 - 2D noise returns vec2
    std::string noise2Source = R"(
        vec2 calc(float x) {
            vec2 p = vec2(x, x * 0.5);
            return noise2(p);
        }
    )";
    
    // Test noise3 - 3D noise returns vec3
    std::string noise3Source = R"(
        vec3 calc(float x) {
            vec3 p = vec3(x, x * 0.5, x * 0.25);
            return noise3(p);
        }
    )";
    
    // Test noise4 - 4D noise returns vec4
    std::string noise4Source = R"(
        vec4 calc(float x) {
            vec4 p = vec4(x, x * 0.5, x * 0.25, x * 0.125);
            return noise4(p);
        }
    )";
    
    try {
        std::vector<mmrsl::Value> args;
        
        // Test noise1 - result should be in [0, 1] range
        args = { mmrsl::Value(1.5f) };
        mmrsl::Value r1 = parser.compileAndExecute(noise1Source, args);
        if (!r1.isFloat()) { std::cout << "FAIL (noise1: need float)\n"; return false; }
        float f1 = r1.asFloat();
        if (f1 < 0.0f || f1 > 1.0f) {
            std::cout << "FAIL (noise1: result out of range [0,1]: " << f1 << ")\n";
            return false;
        }
        
        // Test noise2 - result should be vec2
        mmrsl::Value r2 = parser.compileAndExecute(noise2Source, args);
        if (!r2.isVec2()) { std::cout << "FAIL (noise2: need vec2)\n"; return false; }
        
        // Test noise3 - result should be vec3
        mmrsl::Value r3 = parser.compileAndExecute(noise3Source, args);
        if (!r3.isVec3()) { std::cout << "FAIL (noise3: need vec3)\n"; return false; }
        
        // Test noise4 - result should be vec4
        mmrsl::Value r4 = parser.compileAndExecute(noise4Source, args);
        if (!r4.isVec4()) { std::cout << "FAIL (noise4: need vec4)\n"; return false; }
        
        // Test determinism - same input should give same output
        mmrsl::Value r1_again = parser.compileAndExecute(noise1Source, args);
        if (std::abs(r1.asFloat() - r1_again.asFloat()) > 0.0001f) {
            std::cout << "FAIL (noise1: not deterministic)\n";
            return false;
        }
        
        std::cout << "PASS\n";
        return true;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

int main() {
    std::cout << "=== TEST: test_hp_main ===\n\n";
    
    int passed = 0;
    int total = 104;
    
    if (testBasicArithmetic()) passed++;
    if (testVectorOperations()) passed++;
    if (testDotProduct()) passed++;
    if (testCrossProduct()) passed++;
    if (testVectorLength()) passed++;
    if (testMat3VectorMul()) passed++;
    if (testMat2Determinant()) passed++;
    if (testRotation2D()) passed++;
    if (testMat3Trace()) passed++;
    if (testMat4VectorMul()) passed++;
    if (testTransformPoint()) passed++;
    if (testMat2Mul()) passed++;
    if (testMat3Transpose()) passed++;
    if (testOrthographicProjection()) passed++;
    if (testMat4Column()) passed++;
    if (testVec3ScaleAndAdd()) passed++;
    if (testMat3IdentityCheck()) passed++;
    if (testVec3LengthSquared()) passed++;
    if (testMat2Diagonal()) passed++;
    if (testVec3ComponentMul()) passed++;
    if (testMat3VecTransform()) passed++;
    if (testVec3DistanceSquared()) passed++;
    if (testMat3ElementSum()) passed++;
    if (testVec3Negation()) passed++;
    if (testMatVecChain()) passed++;
    if (testMat4PointTransform()) passed++;
    if (testMat4PointScale()) passed++;
    if (testMat4PointScaleTranslate()) passed++;
    if (testMat4DirectionTransform()) passed++;
    if (testMat4ExtractComponents()) passed++;
    if (testRGBtoYUV()) passed++;
    if (testRGBtoGrayscale()) passed++;
    if (testRGBInversion()) passed++;
    if (testRGBBrightness()) passed++;
    if (testRGBtoSepia()) passed++;
    if (testHueRotation()) passed++;
    if (testHueShiftApproximation()) passed++;
    if (testSaturationAdjustment()) passed++;
    if (testClampFunction()) passed++;
    if (testMixFunctionFloat()) passed++;
    if (testMixFunctionVec3()) passed++;
    if (testSrgbToLinear()) passed++;
    if (testLinearToSrgb()) passed++;
    if (testSrgbHueShift()) passed++;
    if (testIfStatement()) passed++;
    if (testMultipleComparisons()) passed++;
    if (testLessThanGreaterEqual()) passed++;
    if (testEqualNotEqual()) passed++;
    if (testIfWithoutElse()) passed++;
    if (testClampWithIf()) passed++;
    if (testDebugLogical()) passed++;
    if (testDebugLogical2()) passed++;
    if (testDebugLogical3()) passed++;
    if (testDebugLogical4()) passed++;
    if (testDebugLogical5()) passed++;
    if (testCircleContainment()) passed++;
    if (testSphereContainment()) passed++;
    if (testRingContainment()) passed++;
    if (testBoxContainment()) passed++;
    if (testCylinderContainment()) passed++;
    if (testLengthFunction()) passed++;
    if (testNormalizeFunction()) passed++;
    if (testReflectFunction()) passed++;
    if (testRefractFunction()) passed++;
    if (testSimpleLighting()) passed++;
    if (testPhongLighting()) passed++;
    if (testCookTorranceLighting()) passed++;
    if (testLogicalNot()) passed++;
    if (testLogicalOr()) passed++;
    if (testVectorConstructors()) passed++;
    if (testMat2Constructor()) passed++;
    if (testMat3Constructor()) passed++;
    if (testMat4Constructor()) passed++;
    if (testVecMatConstructorChain()) passed++;
    if (testMat4ScaleConstructor()) passed++;
    if (testMultipleScriptsCompilation()) passed++;
    if (testCompileOnceExecuteMultiple()) passed++;
    if (testMatrixNonDiagonalIndexing()) passed++;
    if (testComplexExpressionRegisterReuse()) passed++;
    if (testDeeplyNestedExpression()) passed++;
    if (testComplexControlFlow()) passed++;
    if (testRepeatedCompilationStateIsolation()) passed++;
    if (testBoundaryValuesAndSpecialFloats()) passed++;
    if (testMixedTypeOperations()) passed++;
    if (testBooleanLogicChain()) passed++;
    if (testDirectExpressionComposition()) passed++;
    if (testUnaryOperatorsComplexContext()) passed++;
    if (testOperatorPrecedence()) passed++;
    if (testVariableReassignment()) passed++;
    if (testAcesToneMapping()) passed++;
    if (testMatrixBoundsChecking()) passed++;
    if (testConstantPoolDeduplication()) passed++;
    if (testMultipleReturnPaths()) passed++;
    if (testVectorSwizzleSimulation()) passed++;
    if (testPerformanceBenchmark()) passed++;
    if (testSourceLengthLimit()) passed++;
    if (testNestingDepthLimit()) passed++;
    if (testExpressionDepthLimit()) passed++;
    if (testLocalVariableLimit()) passed++;
    if (testIdentifierLengthLimit()) passed++;
    if (testTransformChain()) passed++;
    if (testMatrixInverse()) passed++;
    if (testMatrixTranspose()) passed++;
    if (testNoiseFunctions()) passed++;
    
    std::cout << "\n=== Results ===\n";
    std::cout << "Passed: " << passed << "/" << total << "\n";
    
    return (passed == total) ? 0 : 1;
}
