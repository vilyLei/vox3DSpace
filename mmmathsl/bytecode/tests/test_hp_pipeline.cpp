// Pipeline Tests for Multi-Script Sequential Execution
// Tests complex color space transformations and multi-stage processing

#include "mmrsl/mmrsl.hpp"
#include <iostream>
#include <cmath>
#include <vector>
#include <memory>

using namespace mmrsl;

// Helper to check if two floats are approximately equal
bool approxEqual(float a, float b, float epsilon = 1e-5f) {
    return std::abs(a - b) < epsilon;
}

// Helper to check if two vec3 are approximately equal
bool approxEqual(const Vec3& a, const Vec3& b, float epsilon = 1e-5f) {
    return approxEqual(a.x, b.x, epsilon) && 
           approxEqual(a.y, b.y, epsilon) && 
           approxEqual(a.z, b.z, epsilon);
}

// Test 1: Simple multi-stage pipeline (vec3 operations)
bool testSimplePipeline() {
    std::cout << "Pipeline Test: Simple Multi-Stage Pipeline... ";
    
    // Stage 1: Scale color
    const char* stage1Script = R"(
        vec3 scale(vec3 c, float s) {
            return c * s;
        }
    )";
    
    // Stage 2: Add offset
    const char* stage2Script = R"(
        vec3 offset(vec3 c, float o) {
            return c + vec3(o, o, o);
        }
    )";
    
    // Stage 3: Just pass through (simplified)
    const char* stage3Script = R"(
        vec3 clamp(vec3 c) {
            return c;
        }
    )";
    
    try {
        // Stage 1: Scale
        BytecodeParser stage1;
        if (!stage1.compile(stage1Script)) {
            std::cout << "FAIL (stage1 compile: " << stage1.getLastError() << ")" << std::endl;
            return false;
        }
        
        Vec3 inputColor(0.5f, 0.3f, 0.2f);
        std::vector<Value> args1 = { Value(inputColor), Value(1.5f) };
        auto result1 = stage1.execute(args1);
        if (!result1.isVec3()) {
            std::cout << "FAIL (stage1 result not vec3)" << std::endl;
            return false;
        }
        Vec3 scaledColor = result1.asVec3();
        
        // Stage 2: Offset
        BytecodeParser stage2;
        if (!stage2.compile(stage2Script)) {
            std::cout << "FAIL (stage2 compile: " << stage2.getLastError() << ")" << std::endl;
            return false;
        }
        
        std::vector<Value> args2 = { Value(scaledColor), Value(0.1f) };
        auto result2 = stage2.execute(args2);
        if (!result2.isVec3()) {
            std::cout << "FAIL (stage2 result not vec3)" << std::endl;
            return false;
        }
        Vec3 offsetColor = result2.asVec3();
        
        // Stage 3: Clamp
        BytecodeParser stage3;
        if (!stage3.compile(stage3Script)) {
            std::cout << "FAIL (stage3 compile: " << stage3.getLastError() << ")" << std::endl;
            return false;
        }
        
        std::vector<Value> args3 = { Value(offsetColor) };
        auto result3 = stage3.execute(args3);
        if (!result3.isVec3()) {
            std::cout << "FAIL (stage3 result not vec3)" << std::endl;
            return false;
        }
        Vec3 finalColor = result3.asVec3();
        
        // Verify result is in valid range [0, 1]
        if (finalColor.x < 0.0f || finalColor.x > 1.0f ||
            finalColor.y < 0.0f || finalColor.y > 1.0f ||
            finalColor.z < 0.0f || finalColor.z > 1.0f) {
            std::cout << "FAIL (result out of range: " << finalColor.x << ", " << finalColor.y << ", " << finalColor.z << ")" << std::endl;
            return false;
        }
        
        std::cout << "PASS (result: " << finalColor.x << ", " << finalColor.y << ", " << finalColor.z << ")" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cout << "FAIL (exception: " << e.what() << ")" << std::endl;
        return false;
    }
}

// Test 2: Matrix transformation pipeline
bool testMatrixPipeline() {
    std::cout << "Pipeline Test: Matrix Transformation Pipeline... ";
    
    // Stage 1: Apply rotation matrix
    const char* stage1Script = R"(
        vec3 rotate(vec3 v, float angle) {
            float c = cos(angle);
            float s = sin(angle);
            mat3 m = mat3(c, -s, 0.0, s, c, 0.0, 0.0, 0.0, 1.0);
            return m * v;
        }
    )";
    
    // Stage 2: Apply scale
    const char* stage2Script = R"(
        vec3 scale(vec3 v, float s) {
            return v * s;
        }
    )";
    
    // Stage 3: Apply translation
    const char* stage3Script = R"(
        vec3 translate(vec3 v, vec3 t) {
            return v + t;
        }
    )";
    
    try {
        // Stage 1: Rotate
        BytecodeParser stage1;
        if (!stage1.compile(stage1Script)) {
            std::cout << "FAIL (stage1 compile: " << stage1.getLastError() << ")" << std::endl;
            return false;
        }
        
        Vec3 input(1.0f, 0.0f, 0.0f);
        std::vector<Value> args1 = { Value(input), Value(3.14159f / 2.0f) };  // 90 degrees
        auto result1 = stage1.execute(args1);
        if (!result1.isVec3()) {
            std::cout << "FAIL (stage1 result not vec3)" << std::endl;
            return false;
        }
        Vec3 rotated = result1.asVec3();
        
        // Stage 2: Scale
        BytecodeParser stage2;
        if (!stage2.compile(stage2Script)) {
            std::cout << "FAIL (stage2 compile: " << stage2.getLastError() << ")" << std::endl;
            return false;
        }
        
        std::vector<Value> args2 = { Value(rotated), Value(2.0f) };
        auto result2 = stage2.execute(args2);
        if (!result2.isVec3()) {
            std::cout << "FAIL (stage2 result not vec3)" << std::endl;
            return false;
        }
        Vec3 scaled = result2.asVec3();
        
        // Stage 3: Translate
        BytecodeParser stage3;
        if (!stage3.compile(stage3Script)) {
            std::cout << "FAIL (stage3 compile: " << stage3.getLastError() << ")" << std::endl;
            return false;
        }
        
        std::vector<Value> args3 = { Value(scaled), Value(Vec3(0.5f, 0.5f, 0.5f)) };
        auto result3 = stage3.execute(args3);
        if (!result3.isVec3()) {
            std::cout << "FAIL (stage3 result not vec3)" << std::endl;
            return false;
        }
        Vec3 finalVec = result3.asVec3();
        
        std::cout << "PASS (result: " << finalVec.x << ", " << finalVec.y << ", " << finalVec.z << ")" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cout << "FAIL (exception: " << e.what() << ")" << std::endl;
        return false;
    }
}

// Test 3: Reuse compiled scripts multiple times
bool testCompiledScriptReuse() {
    std::cout << "Pipeline Test: Reuse Compiled Scripts (10 iterations)... ";
    
    const char* script = R"(
        vec3 process(vec3 input, float scale, float offset) {
            return input * scale + vec3(offset, offset, offset);
        }
    )";
    
    try {
        BytecodeParser parser;
        if (!parser.compile(script)) {
            std::cout << "FAIL (compile: " << parser.getLastError() << ")" << std::endl;
            return false;
        }
        
        // Execute same compiled script multiple times with different inputs
        for (int i = 0; i < 10; ++i) {
            Vec3 input(i * 0.1f, i * 0.05f, i * 0.02f);
            std::vector<Value> args = { Value(input), Value(0.5f), Value(0.25f) };
            auto result = parser.execute(args);
            if (!result.isVec3()) {
                std::cout << "FAIL (iteration " << i << " result not vec3)" << std::endl;
                return false;
            }
        }
        
        std::cout << "PASS" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cout << "FAIL (exception: " << e.what() << ")" << std::endl;
        return false;
    }
}

// Test 4: Chain multiple operations in single script vs pipeline
bool testSingleScriptVsPipeline() {
    std::cout << "Pipeline Test: Single Script vs Multi-Script Pipeline... ";
    
    // Single script doing everything
    const char* singleScript = R"(
        vec3 process(vec3 input) {
            vec3 scaled = input * 2.0;
            vec3 offset = scaled + vec3(0.5, 0.5, 0.5);
            return offset;
        }
    )";
    
    // Multi-script pipeline
    const char* script1 = R"(
        vec3 scale(vec3 input) {
            return input * 2.0;
        }
    )";
    
    const char* script2 = R"(
        vec3 offset(vec3 input) {
            return input + vec3(0.5, 0.5, 0.5);
        }
    )";
    
    const char* script3 = R"(
        vec3 clamp(vec3 input) {
            return input;
        }
    )";
    
    try {
        // Single script approach
        BytecodeParser single;
        if (!single.compile(singleScript)) {
            std::cout << "FAIL (single compile: " << single.getLastError() << ")" << std::endl;
            return false;
        }
        Vec3 input(0.3f, 0.4f, 0.5f);
        std::vector<Value> args = { Value(input) };
        auto singleResult = single.execute(args);
        Vec3 singleVec = singleResult.asVec3();
        
        // Pipeline approach
        BytecodeParser stage1, stage2, stage3;
        if (!stage1.compile(script1) || !stage2.compile(script2) || !stage3.compile(script3)) {
            std::cout << "FAIL (pipeline compile)" << std::endl;
            return false;
        }
        
        std::vector<Value> args1 = { Value(input) };
        auto r1 = stage1.execute(args1);
        std::vector<Value> args2 = { Value(r1.asVec3()) };
        auto r2 = stage2.execute(args2);
        std::vector<Value> args3 = { Value(r2.asVec3()) };
        auto r3 = stage3.execute(args3);
        Vec3 pipelineVec = r3.asVec3();
        
        // Results should be approximately equal
        if (!approxEqual(singleVec, pipelineVec, 1e-4f)) {
            std::cout << "FAIL (results differ: single=" << singleVec.x << "," << singleVec.y << "," << singleVec.z 
                      << " pipeline=" << pipelineVec.x << "," << pipelineVec.y << "," << pipelineVec.z << ")" << std::endl;
            return false;
        }
        
        std::cout << "PASS (both approaches yield identical results)" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cout << "FAIL (exception: " << e.what() << ")" << std::endl;
        return false;
    }
}

// Test 5: Data passing between stages with different types
bool testMixedTypePipeline() {
    std::cout << "Pipeline Test: Mixed Type Pipeline (float -> vec3 -> vec4)... ";
    
    // Stage 1: Create vec3 from float
    const char* stage1Script = R"(
        vec3 createColor(float intensity) {
            return vec3(intensity, intensity * 0.5, intensity * 0.25);
        }
    )";
    
    // Stage 2: Convert vec3 to vec4 with alpha
    const char* stage2Script = R"(
        vec4 addAlpha(vec3 color, float alpha) {
            return vec4(color.x, color.y, color.z, alpha);
        }
    )";
    
    // Stage 3: Premultiply alpha
    const char* stage3Script = R"(
        vec4 premultiply(vec4 color) {
            return vec4(color.x * color.w, color.y * color.w, color.z * color.w, color.w);
        }
    )";
    
    try {
        // Stage 1: Create color
        BytecodeParser stage1;
        if (!stage1.compile(stage1Script)) {
            std::cout << "FAIL (stage1 compile: " << stage1.getLastError() << ")" << std::endl;
            return false;
        }
        
        std::vector<Value> args1 = { Value(0.8f) };
        auto result1 = stage1.execute(args1);
        if (!result1.isVec3()) {
            std::cout << "FAIL (stage1 result not vec3)" << std::endl;
            return false;
        }
        Vec3 color = result1.asVec3();
        
        // Stage 2: Add alpha
        BytecodeParser stage2;
        if (!stage2.compile(stage2Script)) {
            std::cout << "FAIL (stage2 compile: " << stage2.getLastError() << ")" << std::endl;
            return false;
        }
        
        std::vector<Value> args2 = { Value(color), Value(0.75f) };
        auto result2 = stage2.execute(args2);
        if (!result2.isVec4()) {
            std::cout << "FAIL (stage2 result not vec4)" << std::endl;
            return false;
        }
        Vec4 colorWithAlpha = result2.asVec4();
        
        // Stage 3: Premultiply
        BytecodeParser stage3;
        if (!stage3.compile(stage3Script)) {
            std::cout << "FAIL (stage3 compile: " << stage3.getLastError() << ")" << std::endl;
            return false;
        }
        
        std::vector<Value> args3 = { Value(colorWithAlpha) };
        auto result3 = stage3.execute(args3);
        if (!result3.isVec4()) {
            std::cout << "FAIL (stage3 result not vec4)" << std::endl;
            return false;
        }
        Vec4 finalColor = result3.asVec4();
        
        std::cout << "PASS (result: " << finalColor.x << ", " << finalColor.y << ", " << finalColor.z << ", " << finalColor.w << ")" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cout << "FAIL (exception: " << e.what() << ")" << std::endl;
        return false;
    }
}

int main() {
    std::cout << "=== mmrsl Pipeline Test Suite ===\n\n";
    
    int passed = 0;
    int total = 5;
    
    if (testSimplePipeline()) passed++;
    if (testMatrixPipeline()) passed++;
    if (testCompiledScriptReuse()) passed++;
    if (testSingleScriptVsPipeline()) passed++;
    if (testMixedTypePipeline()) passed++;
    
    std::cout << "\n=== Results ===" << std::endl;
    std::cout << "Passed: " << passed << "/" << total << std::endl;
    
    return (passed == total) ? 0 : 1;
}
