// GLSL Built-in Functions Test Suite
// Tests all GLSL-style built-in math functions

#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <chrono>
#include <iomanip>

#include "mmrsl/mmrsl.hpp"

using namespace mmrsl;

// Helper function for approximate equality
bool approxEqual(float a, float b, float epsilon = 0.0001f) {
    return std::abs(a - b) < epsilon;
}

// ==================== Trigonometric Functions ====================

bool testSin() {
    std::cout << "Builtin: sin()... ";
    HighPerfParser parser;
    std::string source = R"(
        float calc(float x) {
            return sin(x);
        }
    )";
    try {
        std::vector<mmrsl::Value> args = { mmrsl::Value(3.14159265f / 2.0f) };  // π/2
        mmrsl::Value result = parser.compileAndExecute(source, args);
        if (approxEqual(result.asFloat(), 1.0f)) {
            std::cout << "PASS\n";
            return true;
        }
        std::cout << "FAIL (expected 1.0, got " << result.asFloat() << ")\n";
        return false;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

bool testCos() {
    std::cout << "Builtin: cos()... ";
    HighPerfParser parser;
    std::string source = R"(
        float calc(float x) {
            return cos(x);
        }
    )";
    try {
        std::vector<mmrsl::Value> args = { mmrsl::Value(0.0f) };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        if (approxEqual(result.asFloat(), 1.0f)) {
            std::cout << "PASS\n";
            return true;
        }
        std::cout << "FAIL (expected 1.0, got " << result.asFloat() << ")\n";
        return false;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

bool testTan() {
    std::cout << "Builtin: tan()... ";
    HighPerfParser parser;
    std::string source = R"(
        float calc(float x) {
            return tan(x);
        }
    )";
    try {
        std::vector<mmrsl::Value> args = { mmrsl::Value(3.14159265f / 4.0f) };  // π/4
        mmrsl::Value result = parser.compileAndExecute(source, args);
        if (approxEqual(result.asFloat(), 1.0f, 0.001f)) {
            std::cout << "PASS\n";
            return true;
        }
        std::cout << "FAIL (expected ~1.0, got " << result.asFloat() << ")\n";
        return false;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

bool testAsin() {
    std::cout << "Builtin: asin()... ";
    HighPerfParser parser;
    std::string source = R"(
        float calc(float x) {
            return asin(x);
        }
    )";
    try {
        std::vector<mmrsl::Value> args = { mmrsl::Value(1.0f) };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        if (approxEqual(result.asFloat(), 3.14159265f / 2.0f, 0.001f)) {
            std::cout << "PASS\n";
            return true;
        }
        std::cout << "FAIL (expected ~1.57, got " << result.asFloat() << ")\n";
        return false;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

bool testAcos() {
    std::cout << "Builtin: acos()... ";
    HighPerfParser parser;
    std::string source = R"(
        float calc(float x) {
            return acos(x);
        }
    )";
    try {
        std::vector<mmrsl::Value> args = { mmrsl::Value(1.0f) };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        if (approxEqual(result.asFloat(), 0.0f, 0.001f)) {
            std::cout << "PASS\n";
            return true;
        }
        std::cout << "FAIL (expected ~0.0, got " << result.asFloat() << ")\n";
        return false;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

bool testAtan() {
    std::cout << "Builtin: atan()... ";
    HighPerfParser parser;
    std::string source = R"(
        float calc(float x) {
            return atan(x);
        }
    )";
    try {
        std::vector<mmrsl::Value> args = { mmrsl::Value(1.0f) };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        if (approxEqual(result.asFloat(), 3.14159265f / 4.0f, 0.001f)) {
            std::cout << "PASS\n";
            return true;
        }
        std::cout << "FAIL (expected ~0.785, got " << result.asFloat() << ")\n";
        return false;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// ==================== Power and Root Functions ====================

bool testSqrt() {
    std::cout << "Builtin: sqrt()... ";
    HighPerfParser parser;
    std::string source = R"(
        float calc(float x) {
            return sqrt(x);
        }
    )";
    try {
        std::vector<mmrsl::Value> args = { mmrsl::Value(16.0f) };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        if (approxEqual(result.asFloat(), 4.0f)) {
            std::cout << "PASS\n";
            return true;
        }
        std::cout << "FAIL (expected 4.0, got " << result.asFloat() << ")\n";
        return false;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

bool testExp() {
    std::cout << "Builtin: exp()... ";
    HighPerfParser parser;
    std::string source = R"(
        float calc(float x) {
            return exp(x);
        }
    )";
    try {
        std::vector<mmrsl::Value> args = { mmrsl::Value(1.0f) };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        if (approxEqual(result.asFloat(), 2.71828183f, 0.001f)) {
            std::cout << "PASS\n";
            return true;
        }
        std::cout << "FAIL (expected ~2.718, got " << result.asFloat() << ")\n";
        return false;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

bool testLog() {
    std::cout << "Builtin: log()... ";
    HighPerfParser parser;
    std::string source = R"(
        float calc(float x) {
            return log(x);
        }
    )";
    try {
        std::vector<mmrsl::Value> args = { mmrsl::Value(2.71828183f) };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        if (approxEqual(result.asFloat(), 1.0f, 0.001f)) {
            std::cout << "PASS\n";
            return true;
        }
        std::cout << "FAIL (expected ~1.0, got " << result.asFloat() << ")\n";
        return false;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

bool testPow() {
    std::cout << "Builtin: pow()... ";
    HighPerfParser parser;
    std::string source = R"(
        float calc(float x) {
            return pow(x, 3.0);
        }
    )";
    try {
        std::vector<mmrsl::Value> args = { mmrsl::Value(2.0f) };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        if (approxEqual(result.asFloat(), 8.0f)) {
            std::cout << "PASS\n";
            return true;
        }
        std::cout << "FAIL (expected 8.0, got " << result.asFloat() << ")\n";
        return false;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

bool testRadians() {
    std::cout << "Builtin: radians()... ";
    HighPerfParser parser;
    std::string source = R"(
        float calc(float x) {
            return radians(x);
        }
    )";
    try {
        std::vector<mmrsl::Value> args = { mmrsl::Value(180.0f) };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        if (approxEqual(result.asFloat(), 3.14159265f, 0.001f)) {
            std::cout << "PASS\n";
            return true;
        }
        std::cout << "FAIL (expected ~3.14, got " << result.asFloat() << ")\n";
        return false;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

bool testDegrees() {
    std::cout << "Builtin: degrees()... ";
    HighPerfParser parser;
    std::string source = R"(
        float calc(float x) {
            return degrees(x);
        }
    )";
    try {
        std::vector<mmrsl::Value> args = { mmrsl::Value(3.14159265f) };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        if (approxEqual(result.asFloat(), 180.0f, 0.001f)) {
            std::cout << "PASS\n";
            return true;
        }
        std::cout << "FAIL (expected 180.0, got " << result.asFloat() << ")\n";
        return false;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// ==================== Rounding Functions ====================

bool testFloor() {
    std::cout << "Builtin: floor()... ";
    HighPerfParser parser;
    std::string source = R"(
        float calc(float x) {
            return floor(x);
        }
    )";
    try {
        std::vector<mmrsl::Value> args = { mmrsl::Value(3.7f) };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        if (approxEqual(result.asFloat(), 3.0f)) {
            std::cout << "PASS\n";
            return true;
        }
        std::cout << "FAIL (expected 3.0, got " << result.asFloat() << ")\n";
        return false;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

bool testCeil() {
    std::cout << "Builtin: ceil()... ";
    HighPerfParser parser;
    std::string source = R"(
        float calc(float x) {
            return ceil(x);
        }
    )";
    try {
        std::vector<mmrsl::Value> args = { mmrsl::Value(3.2f) };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        if (approxEqual(result.asFloat(), 4.0f)) {
            std::cout << "PASS\n";
            return true;
        }
        std::cout << "FAIL (expected 4.0, got " << result.asFloat() << ")\n";
        return false;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

bool testFract() {
    std::cout << "Builtin: fract()... ";
    HighPerfParser parser;
    std::string source = R"(
        float calc(float x) {
            return fract(x);
        }
    )";
    try {
        std::vector<mmrsl::Value> args = { mmrsl::Value(3.7f) };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        if (approxEqual(result.asFloat(), 0.7f)) {
            std::cout << "PASS\n";
            return true;
        }
        std::cout << "FAIL (expected 0.7, got " << result.asFloat() << ")\n";
        return false;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

bool testMod() {
    std::cout << "Builtin: mod()... ";
    HighPerfParser parser;
    std::string source = R"(
        float calc(float x) {
            return mod(x, 3.0);
        }
    )";
    try {
        std::vector<mmrsl::Value> args = { mmrsl::Value(10.0f) };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        if (approxEqual(result.asFloat(), 1.0f)) {
            std::cout << "PASS\n";
            return true;
        }
        std::cout << "FAIL (expected 1.0, got " << result.asFloat() << ")\n";
        return false;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

bool testAbs() {
    std::cout << "Builtin: abs()... ";
    HighPerfParser parser;
    std::string source = R"(
        float calc(float x) {
            return abs(x);
        }
    )";
    try {
        std::vector<mmrsl::Value> args = { mmrsl::Value(-5.0f) };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        if (approxEqual(result.asFloat(), 5.0f)) {
            std::cout << "PASS\n";
            return true;
        }
        std::cout << "FAIL (expected 5.0, got " << result.asFloat() << ")\n";
        return false;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

bool testSign() {
    std::cout << "Builtin: sign()... ";
    HighPerfParser parser;
    std::string source = R"(
        float calc(float x) {
            return sign(x);
        }
    )";
    try {
        // Test positive
        std::vector<mmrsl::Value> args1 = { mmrsl::Value(5.0f) };
        mmrsl::Value result1 = parser.compileAndExecute(source, args1);
        if (!approxEqual(result1.asFloat(), 1.0f)) {
            std::cout << "FAIL positive (expected 1.0, got " << result1.asFloat() << ")\n";
            return false;
        }
        
        // Test negative
        std::vector<mmrsl::Value> args2 = { mmrsl::Value(-3.0f) };
        mmrsl::Value result2 = parser.compileAndExecute(source, args2);
        if (!approxEqual(result2.asFloat(), -1.0f)) {
            std::cout << "FAIL negative (expected -1.0, got " << result2.asFloat() << ")\n";
            return false;
        }
        
        // Test zero
        std::vector<mmrsl::Value> args3 = { mmrsl::Value(0.0f) };
        mmrsl::Value result3 = parser.compileAndExecute(source, args3);
        if (!approxEqual(result3.asFloat(), 0.0f)) {
            std::cout << "FAIL zero (expected 0.0, got " << result3.asFloat() << ")\n";
            return false;
        }
        
        std::cout << "PASS\n";
        return true;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// ==================== Min/Max/Clamp Functions ====================

bool testMin() {
    std::cout << "Builtin: min()... ";
    HighPerfParser parser;
    std::string source = R"(
        float calc(float x) {
            return min(x, 5.0);
        }
    )";
    try {
        std::vector<mmrsl::Value> args = { mmrsl::Value(10.0f) };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        if (approxEqual(result.asFloat(), 5.0f)) {
            std::cout << "PASS\n";
            return true;
        }
        std::cout << "FAIL (expected 5.0, got " << result.asFloat() << ")\n";
        return false;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

bool testMax() {
    std::cout << "Builtin: max()... ";
    HighPerfParser parser;
    std::string source = R"(
        float calc(float x) {
            return max(x, 5.0);
        }
    )";
    try {
        std::vector<mmrsl::Value> args = { mmrsl::Value(3.0f) };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        if (approxEqual(result.asFloat(), 5.0f)) {
            std::cout << "PASS\n";
            return true;
        }
        std::cout << "FAIL (expected 5.0, got " << result.asFloat() << ")\n";
        return false;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

bool testClamp() {
    std::cout << "Builtin: clamp()... ";
    HighPerfParser parser;
    std::string source = R"(
        float calc(float x) {
            return clamp(x, 0.0, 1.0);
        }
    )";
    try {
        // Test clamping to max
        std::vector<mmrsl::Value> args1 = { mmrsl::Value(2.0f) };
        mmrsl::Value result1 = parser.compileAndExecute(source, args1);
        if (!approxEqual(result1.asFloat(), 1.0f)) {
            std::cout << "FAIL upper (expected 1.0, got " << result1.asFloat() << ")\n";
            return false;
        }
        
        // Test clamping to min
        std::vector<mmrsl::Value> args2 = { mmrsl::Value(-1.0f) };
        mmrsl::Value result2 = parser.compileAndExecute(source, args2);
        if (!approxEqual(result2.asFloat(), 0.0f)) {
            std::cout << "FAIL lower (expected 0.0, got " << result2.asFloat() << ")\n";
            return false;
        }
        
        // Test no clamping
        std::vector<mmrsl::Value> args3 = { mmrsl::Value(0.5f) };
        mmrsl::Value result3 = parser.compileAndExecute(source, args3);
        if (!approxEqual(result3.asFloat(), 0.5f)) {
            std::cout << "FAIL middle (expected 0.5, got " << result3.asFloat() << ")\n";
            return false;
        }
        
        std::cout << "PASS\n";
        return true;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// ==================== Mix (Lerp) Functions ====================

bool testMix() {
    std::cout << "Builtin: mix()... ";
    HighPerfParser parser;
    std::string source = R"(
        float calc(float x) {
            return mix(0.0, 10.0, x);
        }
    )";
    try {
        std::vector<mmrsl::Value> args = { mmrsl::Value(0.3f) };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        if (approxEqual(result.asFloat(), 3.0f)) {
            std::cout << "PASS\n";
            return true;
        }
        std::cout << "FAIL (expected 3.0, got " << result.asFloat() << ")\n";
        return false;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// ==================== Step and Smoothstep Functions ====================

bool testStep() {
    std::cout << "Builtin: step()... ";
    HighPerfParser parser;
    std::string source = R"(
        float calc(float x) {
            return step(0.5, x);
        }
    )";
    try {
        // Test below edge
        std::vector<mmrsl::Value> args1 = { mmrsl::Value(0.3f) };
        mmrsl::Value result1 = parser.compileAndExecute(source, args1);
        if (!approxEqual(result1.asFloat(), 0.0f)) {
            std::cout << "FAIL below (expected 0.0, got " << result1.asFloat() << ")\n";
            return false;
        }
        
        // Test above edge
        std::vector<mmrsl::Value> args2 = { mmrsl::Value(0.7f) };
        mmrsl::Value result2 = parser.compileAndExecute(source, args2);
        if (!approxEqual(result2.asFloat(), 1.0f)) {
            std::cout << "FAIL above (expected 1.0, got " << result2.asFloat() << ")\n";
            return false;
        }
        
        std::cout << "PASS\n";
        return true;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

bool testSmoothStep() {
    std::cout << "Builtin: smoothstep()... ";
    HighPerfParser parser;
    std::string source = R"(
        float calc(float x) {
            return smoothstep(0.0, 1.0, x);
        }
    )";
    try {
        // Test below edge0
        std::vector<mmrsl::Value> args1 = { mmrsl::Value(-0.5f) };
        mmrsl::Value result1 = parser.compileAndExecute(source, args1);
        if (!approxEqual(result1.asFloat(), 0.0f)) {
            std::cout << "FAIL below (expected 0.0, got " << result1.asFloat() << ")\n";
            return false;
        }
        
        // Test above edge1
        std::vector<mmrsl::Value> args2 = { mmrsl::Value(1.5f) };
        mmrsl::Value result2 = parser.compileAndExecute(source, args2);
        if (!approxEqual(result2.asFloat(), 1.0f)) {
            std::cout << "FAIL above (expected 1.0, got " << result2.asFloat() << ")\n";
            return false;
        }
        
        // Test middle (should be 0.5 at midpoint due to Hermite curve symmetry)
        std::vector<mmrsl::Value> args3 = { mmrsl::Value(0.5f) };
        mmrsl::Value result3 = parser.compileAndExecute(source, args3);
        if (!approxEqual(result3.asFloat(), 0.5f, 0.01f)) {
            std::cout << "FAIL middle (expected ~0.5, got " << result3.asFloat() << ")\n";
            return false;
        }
        
        std::cout << "PASS\n";
        return true;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// ==================== Vector Functions ====================

bool testDot() {
    std::cout << "Builtin: dot()... ";
    HighPerfParser parser;
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
        if (approxEqual(result.asFloat(), 32.0f)) {
            std::cout << "PASS\n";
            return true;
        }
        std::cout << "FAIL (expected 32.0, got " << result.asFloat() << ")\n";
        return false;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

bool testLength() {
    std::cout << "Builtin: length()... ";
    HighPerfParser parser;
    std::string source = R"(
        float calc(vec3 v) {
            return length(v);
        }
    )";
    try {
        std::vector<mmrsl::Value> args = { mmrsl::Value(mmrsl::Vec3(3.0f, 4.0f, 0.0f)) };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        if (approxEqual(result.asFloat(), 5.0f)) {
            std::cout << "PASS\n";
            return true;
        }
        std::cout << "FAIL (expected 5.0, got " << result.asFloat() << ")\n";
        return false;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

bool testNormalize() {
    std::cout << "Builtin: normalize()... ";
    HighPerfParser parser;
    std::string source = R"(
        vec3 calc(vec3 v) {
            return normalize(v);
        }
    )";
    try {
        std::vector<mmrsl::Value> args = { mmrsl::Value(mmrsl::Vec3(3.0f, 0.0f, 0.0f)) };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        mmrsl::Vec3 expected = mmrsl::Vec3(1.0f, 0.0f, 0.0f);
        mmrsl::Vec3 diff = result.asVec3() - expected;
        float len = std::sqrt(diff.x*diff.x + diff.y*diff.y + diff.z*diff.z);
        if (len < 0.0001f) {
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

bool testCross() {
    std::cout << "Builtin: cross()... ";
    HighPerfParser parser;
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
        mmrsl::Vec3 diff = result.asVec3() - expected;
        float len = std::sqrt(diff.x*diff.x + diff.y*diff.y + diff.z*diff.z);
        if (len < 0.0001f) {
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

bool testDistance() {
    std::cout << "Builtin: distance()... ";
    HighPerfParser parser;
    std::string source = R"(
        float calc(vec3 a, vec3 b) {
            return distance(a, b);
        }
    )";
    try {
        std::vector<mmrsl::Value> args = {
            mmrsl::Value(mmrsl::Vec3(0.0f, 0.0f, 0.0f)),
            mmrsl::Value(mmrsl::Vec3(3.0f, 4.0f, 0.0f))
        };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        if (approxEqual(result.asFloat(), 5.0f)) {
            std::cout << "PASS\n";
            return true;
        }
        std::cout << "FAIL (expected 5.0, got " << result.asFloat() << ")\n";
        return false;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// ==================== Reflect and Refract ====================

bool testReflect() {
    std::cout << "Builtin: reflect()... ";
    HighPerfParser parser;
    std::string source = R"(
        vec3 calc(vec3 i, vec3 n) {
            return reflect(i, n);
        }
    )";
    try {
        std::vector<mmrsl::Value> args = {
            mmrsl::Value(mmrsl::Vec3(1.0f, -1.0f, 0.0f)),
            mmrsl::Value(mmrsl::Vec3(0.0f, 1.0f, 0.0f))
        };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        mmrsl::Vec3 expected = mmrsl::Vec3(1.0f, 1.0f, 0.0f);
        mmrsl::Vec3 diff = result.asVec3() - expected;
        float len = std::sqrt(diff.x*diff.x + diff.y*diff.y + diff.z*diff.z);
        if (len < 0.0001f) {
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

// ==================== Test Runner ====================

int main() {
    std::cout << "\n=== GLSL Built-in Functions Test Suite ===\n\n";
    
    int passed = 0;
    int total = 30;
    
    // Trigonometric
    if (testSin()) passed++;
    if (testCos()) passed++;
    if (testTan()) passed++;
    if (testAsin()) passed++;
    if (testAcos()) passed++;
    if (testAtan()) passed++;
    
    // Power and Root
    if (testSqrt()) passed++;
    if (testExp()) passed++;
    if (testLog()) passed++;
    if (testPow()) passed++;
    
    // Rounding
    if (testFloor()) passed++;
    if (testCeil()) passed++;
    if (testFract()) passed++;
    if (testAbs()) passed++;
    if (testSign()) passed++;
    
    // Angle Conversion
    if (testRadians()) passed++;
    if (testDegrees()) passed++;
    
    // Mod
    if (testMod()) passed++;
    
    // Min/Max/Clamp
    if (testMin()) passed++;
    if (testMax()) passed++;
    if (testClamp()) passed++;
    
    // Mix
    if (testMix()) passed++;
    
    // Step and Smoothstep
    if (testStep()) passed++;
    if (testSmoothStep()) passed++;
    
    // Vector
    if (testDot()) passed++;
    if (testLength()) passed++;
    if (testDistance()) passed++;
    if (testNormalize()) passed++;
    if (testCross()) passed++;
    
    // Reflect/Refract
    if (testReflect()) passed++;
    
    std::cout << "\n=== Results ===\n";
    std::cout << "Passed: " << passed << "/" << total << "\n";
    
    return (passed == total) ? 0 : 1;
}
