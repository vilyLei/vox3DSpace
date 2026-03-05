#include <iostream>
#include <cmath>
#include "mmrsl/mmrsl.hpp"

using namespace mmrsl;

bool testBasicArithmetic() {
    std::cout << "Test: Basic Arithmetic... ";
    
    SimpleParser parser;
    std::string source = R"(
        float calc(float a, float b) {
            return a + b * 2.0;
        }
    )";
    
    try {
        std::vector<Value> args = {Value(3.0f), Value(4.0f)};
        Value result = parser.compileAndExecute(source, args);
        
        if (!result.isFloat()) {
            std::cout << "FAIL: Expected float result\n";
            return false;
        }
        
        float expected = 3.0f + 4.0f * 2.0f;  // 11.0
        if (std::abs(result.asFloat() - expected) < 0.0001f) {
            std::cout << "PASS\n";
            return true;
        } else {
            std::cout << "FAIL: Expected " << expected << ", got " << result.asFloat() << "\n";
            return false;
        }
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

bool testVectorOperations() {
    std::cout << "Test: Vector Operations... ";
    
    SimpleParser parser;
    std::string source = R"(
        vec2 calc(vec2 a, vec2 b) {
            vec2 c = a + b;
            return c * 2.0;
        }
    )";
    
    try {
        std::vector<Value> args = {Value(Vec2(1.0f, 2.0f)), Value(Vec2(3.0f, 4.0f))};
        Value result = parser.compileAndExecute(source, args);
        
        if (!result.isVec2()) {
            std::cout << "FAIL: Expected vec2 result\n";
            return false;
        }
        
        Vec2 expected = (Vec2(1.0f, 2.0f) + Vec2(3.0f, 4.0f)) * 2.0f;
        Vec2 actual = result.asVec2();
        
        if (std::abs(actual.x - expected.x) < 0.0001f && 
            std::abs(actual.y - expected.y) < 0.0001f) {
            std::cout << "PASS\n";
            return true;
        } else {
            std::cout << "FAIL: Expected vec2(" << expected.x << ", " << expected.y 
                      << "), got vec2(" << actual.x << ", " << actual.y << ")\n";
            return false;
        }
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

bool testBuiltinFunctions() {
    std::cout << "Test: Built-in Functions... ";
    
    SimpleParser parser;
    std::string source = R"(
        float calc(float x) {
            return floor(x / 5.0);
        }
    )";
    
    try {
        std::vector<Value> args = {Value(17.5f)};
        Value result = parser.compileAndExecute(source, args);
        
        if (!result.isFloat()) {
            std::cout << "FAIL: Expected float result\n";
            return false;
        }
        
        float expected = std::floor(17.5f / 5.0f);  // 3.0
        if (std::abs(result.asFloat() - expected) < 0.0001f) {
            std::cout << "PASS\n";
            return true;
        } else {
            std::cout << "FAIL: Expected " << expected << ", got " << result.asFloat() << "\n";
            return false;
        }
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

bool testOriginalExample() {
    std::cout << "Test: Original Example (calcPosition)... ";
    
    SimpleParser parser;
    std::string source = R"(
        vec2 calcPosition(float i) {
            vec2 pv = vec2(30.0, 30.0);
            vec2 disV = vec2(20.0, 20.0);
            float cn = 5.0;
            float r = floor(i / 5.0);
            float c = i - (floor(i / 5.0) * cn);
            vec2 tempV = vec2(c, r);
            vec2 pos = pv + tempV * disV;
            return pos;
        }
    )";
    
    try {
        std::vector<Value> args = {Value(7.0f)};
        Value result = parser.compileAndExecute(source, args);
        
        if (!result.isVec2()) {
            std::cout << "FAIL: Expected vec2 result, got " << typeKindToString(result.kind()) << "\n";
            return false;
        }
        
        // Manual calculation:
        // i = 7
        // r = floor(7 / 5) = floor(1.4) = 1
        // c = 7 - (floor(7 / 5) * 5) = 7 - (1 * 5) = 2
        // tempV = vec2(2, 1)
        // pos = vec2(30, 30) + vec2(2, 1) * vec2(20, 20) = vec2(30, 30) + vec2(40, 20) = vec2(70, 50)
        Vec2 expected(70.0f, 50.0f);
        Vec2 actual = result.asVec2();
        
        if (std::abs(actual.x - expected.x) < 0.0001f && 
            std::abs(actual.y - expected.y) < 0.0001f) {
            std::cout << "PASS\n";
            return true;
        } else {
            std::cout << "FAIL: Expected vec2(" << expected.x << ", " << expected.y 
                      << "), got vec2(" << actual.x << ", " << actual.y << ")\n";
            return false;
        }
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

bool testErrorHandling() {
    std::cout << "Test: Error Handling... ";
    
    SimpleParser parser;
    std::string source = R"(
        float calc(float x) {
            return y + 5.0;  // y is undefined
        }
    )";
    
    try {
        std::vector<Value> args = {Value(1.0f)};
        parser.compileAndExecute(source, args);
        std::cout << "FAIL: Expected runtime error\n";
        return false;
    } catch (const mmrsl::RuntimeError& e) {
        std::cout << "PASS (caught expected error)\n";
        return true;
    } catch (const std::exception& e) {
        std::cout << "PASS (caught error: " << e.what() << ")\n";
        return true;
    }
}

bool testSyntaxError() {
    std::cout << "Test: Syntax Error... ";
    
    SimpleParser parser;
    std::string source = R"(
        float calc(float x) {
            return x +  // Missing expression
        }
    )";
    
    if (!parser.compile(source)) {
        std::cout << "PASS (compilation failed as expected)\n";
        return true;
    } else {
        std::cout << "FAIL: Expected compilation to fail\n";
        return false;
    }
}

bool testCircleArea() {
    std::cout << "Test: Circle Area Calculation... ";
    
    SimpleParser parser;
    std::string source = R"(
        float circleArea(float radius) {
            float pi = 3.14159265;
            return pi * radius * radius;
        }
    )";
    
    try {
        std::vector<Value> args = {Value(5.0f)};
        Value result = parser.compileAndExecute(source, args);
        
        if (!result.isFloat()) {
            std::cout << "FAIL: Expected float result\n";
            return false;
        }
        
        float expected = 3.14159265f * 5.0f * 5.0f;  // 78.5398...
        if (std::abs(result.asFloat() - expected) < 0.0001f) {
            std::cout << "PASS\n";
            return true;
        } else {
            std::cout << "FAIL: Expected " << expected << ", got " << result.asFloat() << "\n";
            return false;
        }
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

bool testVec2DotProduct() {
    std::cout << "Test: vec2 Dot Product... ";
    
    SimpleParser parser;
    std::string source = R"(
        float calc(vec2 a, vec2 b) {
            return dot(a, b);
        }
    )";
    
    try {
        std::vector<Value> args = {Value(Vec2(1.0f, 2.0f)), Value(Vec2(3.0f, 4.0f))};
        Value result = parser.compileAndExecute(source, args);
        
        if (!result.isFloat()) {
            std::cout << "FAIL: Expected float result\n";
            return false;
        }
        
        // dot(vec2(1, 2), vec2(3, 4)) = 1*3 + 2*4 = 11
        float expected = 1.0f * 3.0f + 2.0f * 4.0f;
        if (std::abs(result.asFloat() - expected) < 0.0001f) {
            std::cout << "PASS\n";
            return true;
        } else {
            std::cout << "FAIL: Expected " << expected << ", got " << result.asFloat() << "\n";
            return false;
        }
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

bool testVec3CrossProduct() {
    std::cout << "Test: vec3 Cross Product... ";
    
    SimpleParser parser;
    std::string source = R"(
        vec3 calc(vec3 a, vec3 b) {
            return cross(a, b);
        }
    )";
    
    try {
        // cross(vec3(1, 0, 0), vec3(0, 1, 0)) = vec3(0, 0, 1)
        std::vector<Value> args = {Value(Vec3(1.0f, 0.0f, 0.0f)), Value(Vec3(0.0f, 1.0f, 0.0f))};
        Value result = parser.compileAndExecute(source, args);
        
        if (!result.isVec3()) {
            std::cout << "FAIL: Expected vec3 result\n";
            return false;
        }
        
        Vec3 expected(0.0f, 0.0f, 1.0f);
        Vec3 actual = result.asVec3();
        
        if (std::abs(actual.x - expected.x) < 0.0001f && 
            std::abs(actual.y - expected.y) < 0.0001f &&
            std::abs(actual.z - expected.z) < 0.0001f) {
            std::cout << "PASS\n";
            return true;
        } else {
            std::cout << "FAIL: Expected vec3(" << expected.x << ", " << expected.y << ", " << expected.z
                      << "), got vec3(" << actual.x << ", " << actual.y << ", " << actual.z << ")\n";
            return false;
        }
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

bool testVec2CrossProduct() {
    std::cout << "Test: vec2 Cross Product (2D cross product)... ";
    
    SimpleParser parser;
    // For 2D vectors, cross product is computed as: cross(vec2(a,b), vec2(c,d)) = a*d - b*c
    // This gives the signed area of the parallelogram (z-component of 3D cross)
    std::string source = R"(
        float calc(vec2 a, vec2 b) {
            // 2D cross product: a.x * b.y - a.y * b.x
            return a.x * b.y - a.y * b.x;
        }
    )";
    
    try {
        // cross(vec2(1, 0), vec2(0, 1)) = 1*1 - 0*0 = 1
        std::vector<Value> args = {Value(Vec2(1.0f, 0.0f)), Value(Vec2(0.0f, 1.0f))};
        Value result = parser.compileAndExecute(source, args);
        
        if (!result.isFloat()) {
            std::cout << "FAIL: Expected float result\n";
            return false;
        }
        
        float expected = 1.0f * 1.0f - 0.0f * 0.0f;  // = 1.0
        if (std::abs(result.asFloat() - expected) < 0.0001f) {
            std::cout << "PASS\n";
            return true;
        } else {
            std::cout << "FAIL: Expected " << expected << ", got " << result.asFloat() << "\n";
            return false;
        }
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

bool testBreakOutsideLoop() {
    std::cout << "Test: break outside loop is a parse error... ";

    SimpleParser parser;
    std::string source = R"(
        float test(float x) {
            break;
            return x;
        }
    )";

    if (!parser.compile(source)) {
        std::cout << "PASS (compilation failed as expected)\n";
        return true;
    } else {
        std::cout << "FAIL: Expected parse error for 'break' outside loop\n";
        return false;
    }
}

bool testContinueOutsideLoop() {
    std::cout << "Test: continue outside loop is a parse error... ";

    SimpleParser parser;
    std::string source = R"(
        float test(float x) {
            continue;
            return x;
        }
    )";

    if (!parser.compile(source)) {
        std::cout << "PASS (compilation failed as expected)\n";
        return true;
    } else {
        std::cout << "FAIL: Expected parse error for 'continue' outside loop\n";
        return false;
    }
}

bool testNestedLoopBreak() {
    // break inside the inner loop must only exit the inner loop;
    // the outer loop continues normally.
    std::cout << "Test: break in nested for only exits inner loop... ";

    SimpleParser parser;
    // Outer loop runs i = 0..2 (3 iterations).
    // Inner loop immediately breaks after the first iteration.
    // The result accumulates outer_i values: 0 + 1 + 2 = 3.
    std::string source = R"(
        float test(float dummy) {
            float result = 0.0;
            for (int i = 0; i < 3; i = i + 1) {
                result = result + i * 1.0;
                for (int j = 0; j < 10; j = j + 1) {
                    break;
                }
            }
            return result;
        }
    )";

    try {
        std::vector<Value> args = {Value(0.0f)};
        Value result = parser.compileAndExecute(source, args);
        float expected = 0.0f + 1.0f + 2.0f;  // 3.0
        if (result.isFloat() && std::abs(result.asFloat() - expected) < 0.0001f) {
            std::cout << "PASS\n";
            return true;
        }
        std::cout << "FAIL: got " << result.asFloat() << " expected " << expected << "\n";
        return false;
    } catch (const std::exception& e) {
        std::cout << "FAIL: exception: " << e.what() << "\n";
        return false;
    }
}

bool testNestedLoopContinue() {
    // continue inside the inner loop must only skip the rest of the inner body
    // and run the inner update; the outer loop continues normally.
    std::cout << "Test: continue in nested for only affects inner loop... ";

    SimpleParser parser;
    // Outer runs i = 0..1.  Inner runs j = 0..2, skipping j==1 via continue.
    // Inner accumulates j values excluding 1: 0 + 2 = 2, twice → total = 4.
    std::string source = R"(
        float test(float dummy) {
            float result = 0.0;
            for (int i = 0; i < 2; i = i + 1) {
                for (int j = 0; j < 3; j = j + 1) {
                    if (j == 1) {
                        continue;
                    }
                    result = result + j * 1.0;
                }
            }
            return result;
        }
    )";

    try {
        std::vector<Value> args = {Value(0.0f)};
        Value result = parser.compileAndExecute(source, args);
        float expected = 4.0f;  // (0+2) * 2 iterations
        if (result.isFloat() && std::abs(result.asFloat() - expected) < 0.0001f) {
            std::cout << "PASS\n";
            return true;
        }
        std::cout << "FAIL: got " << result.asFloat() << " expected " << expected << "\n";
        return false;
    } catch (const std::exception& e) {
        std::cout << "FAIL: exception: " << e.what() << "\n";
        return false;
    }
}

int main() {
    std::cout << "=== SimpleParser Test Suite ===\n\n";

    int passed = 0;
    int total = 14;

    if (testBasicArithmetic()) passed++;
    if (testVectorOperations()) passed++;
    if (testBuiltinFunctions()) passed++;
    if (testOriginalExample()) passed++;
    if (testErrorHandling()) passed++;
    if (testSyntaxError()) passed++;
    if (testCircleArea()) passed++;
    if (testVec2DotProduct()) passed++;
    if (testVec3CrossProduct()) passed++;
    if (testVec2CrossProduct()) passed++;
    if (testBreakOutsideLoop()) passed++;
    if (testContinueOutsideLoop()) passed++;
    if (testNestedLoopBreak()) passed++;
    if (testNestedLoopContinue()) passed++;

    std::cout << "\n=== Results ===\n";
    std::cout << "Passed: " << passed << "/" << total << "\n";

    return (passed == total) ? 0 : 1;
}
