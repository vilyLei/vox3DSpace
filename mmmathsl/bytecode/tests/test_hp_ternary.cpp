// Ternary Conditional Operator Tests for High-Performance mmrsl
// Tests the ?: operator functionality

#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include "mmrsl/mmrsl.hpp"

using namespace mmrsl;

// Test result tracking
int g_testsPassed = 0;
int g_testsFailed = 0;

void checkTest(bool condition, const std::string& testName) {
    if (condition) {
        std::cout << "Test: " << testName << "... PASS" << std::endl;
        g_testsPassed++;
    } else {
        std::cout << "Test: " << testName << "... FAIL" << std::endl;
        g_testsFailed++;
    }
}

bool floatEquals(float a, float b, float epsilon = 0.0001f) {
    return std::abs(a - b) < epsilon;
}

// Test 1: Basic ternary with true condition
void testBasicTernaryTrue() {
    BytecodeParser parser;
    std::string source = R"(
        float test(float x) {
            return x > 0.0 ? 1.0 : 0.0;
        }
    )";
    
    try {
        std::vector<mmrsl::Value> args = {mmrsl::Value(5.0f)};
        mmrsl::Value result = parser.compileAndExecute(source, args);
        checkTest(floatEquals(result.asFloat(), 1.0f), "Basic Ternary True (5 > 0 ? 1 : 0 = 1)");
    } catch (const std::exception& e) {
        checkTest(false, std::string("Basic Ternary True - Exception: ") + e.what());
    }
}

// Test 2: Basic ternary with false condition
void testBasicTernaryFalse() {
    BytecodeParser parser;
    std::string source = R"(
        float test(float x) {
            return x > 0.0 ? 1.0 : 0.0;
        }
    )";
    
    try {
        std::vector<mmrsl::Value> args = {mmrsl::Value(-3.0f)};
        mmrsl::Value result = parser.compileAndExecute(source, args);
        checkTest(floatEquals(result.asFloat(), 0.0f), "Basic Ternary False (-3 > 0 ? 1 : 0 = 0)");
    } catch (const std::exception& e) {
        checkTest(false, std::string("Basic Ternary False - Exception: ") + e.what());
    }
}

// Test 3: Ternary with expressions in branches
void testTernaryWithExpressions() {
    BytecodeParser parser;
    std::string source = R"(
        float test(float a, float b) {
            return a > b ? a * 2.0 : b * 3.0;
        }
    )";
    
    try {
        std::vector<mmrsl::Value> args = {mmrsl::Value(5.0f), mmrsl::Value(2.0f)};
        mmrsl::Value result = parser.compileAndExecute(source, args);
        checkTest(floatEquals(result.asFloat(), 10.0f), "Ternary with Expressions (5 > 2 ? 5*2 : 2*3 = 10)");
    } catch (const std::exception& e) {
        checkTest(false, std::string("Ternary with Expressions - Exception: ") + e.what());
    }
}

// Test 4: Nested ternary operator
void testNestedTernary() {
    BytecodeParser parser;
    std::string source = R"(
        float test(float x) {
            return x > 10.0 ? 3.0 : (x > 5.0 ? 2.0 : 1.0);
        }
    )";
    
    try {
        std::vector<mmrsl::Value> args1 = {mmrsl::Value(15.0f)};
        std::vector<mmrsl::Value> args2 = {mmrsl::Value(7.0f)};
        std::vector<mmrsl::Value> args3 = {mmrsl::Value(3.0f)};
        
        mmrsl::Value result1 = parser.compileAndExecute(source, args1);
        mmrsl::Value result2 = parser.compileAndExecute(source, args2);
        mmrsl::Value result3 = parser.compileAndExecute(source, args3);
        
        bool pass = floatEquals(result1.asFloat(), 3.0f) && 
                    floatEquals(result2.asFloat(), 2.0f) && 
                    floatEquals(result3.asFloat(), 1.0f);
        checkTest(pass, "Nested Ternary (x>10?3:(x>5?2:1))");
    } catch (const std::exception& e) {
        checkTest(false, std::string("Nested Ternary - Exception: ") + e.what());
    }
}

// Test 5: Ternary with logical operators in condition
void testTernaryWithLogicalOps() {
    BytecodeParser parser;
    std::string source = R"(
        float test(float x, float y) {
            return (x > 0.0 && y > 0.0) ? 1.0 : 0.0;
        }
    )";
    
    try {
        std::vector<mmrsl::Value> args1 = {mmrsl::Value(5.0f), mmrsl::Value(3.0f)};
        std::vector<mmrsl::Value> args2 = {mmrsl::Value(5.0f), mmrsl::Value(-3.0f)};
        std::vector<mmrsl::Value> args3 = {mmrsl::Value(-5.0f), mmrsl::Value(3.0f)};
        
        mmrsl::Value result1 = parser.compileAndExecute(source, args1);
        mmrsl::Value result2 = parser.compileAndExecute(source, args2);
        mmrsl::Value result3 = parser.compileAndExecute(source, args3);
        
        bool pass = floatEquals(result1.asFloat(), 1.0f) && 
                    floatEquals(result2.asFloat(), 0.0f) && 
                    floatEquals(result3.asFloat(), 0.0f);
        checkTest(pass, "Ternary with Logical AND");
    } catch (const std::exception& e) {
        checkTest(false, std::string("Ternary with Logical AND - Exception: ") + e.what());
    }
}

// Test 6: Ternary with vec3 return type (true branch)
void testTernaryWithVec3True() {
    BytecodeParser parser;
    std::string source = R"(
        vec3 test(float x) {
            return x > 0.0 ? vec3(1.0, 0.0, 0.0) : vec3(0.0, 1.0, 0.0);
        }
    )";
    
    try {
        std::vector<mmrsl::Value> args = {mmrsl::Value(5.0f)};
        mmrsl::Value result = parser.compileAndExecute(source, args);
        mmrsl::Vec3 v = result.asVec3();
        bool pass = floatEquals(v.x, 1.0f) && floatEquals(v.y, 0.0f) && floatEquals(v.z, 0.0f);
        checkTest(pass, "Ternary with vec3 Return (true branch)");
    } catch (const std::exception& e) {
        checkTest(false, std::string("Ternary with vec3 True - Exception: ") + e.what());
    }
}

// Test 7: Ternary with vec3 false branch
void testTernaryWithVec3False() {
    BytecodeParser parser;
    std::string source = R"(
        vec3 test(float x) {
            return x > 0.0 ? vec3(1.0, 0.0, 0.0) : vec3(0.0, 1.0, 0.0);
        }
    )";
    
    try {
        std::vector<mmrsl::Value> args = {mmrsl::Value(-5.0f)};
        mmrsl::Value result = parser.compileAndExecute(source, args);
        mmrsl::Vec3 v = result.asVec3();
        bool pass = floatEquals(v.x, 0.0f) && floatEquals(v.y, 1.0f) && floatEquals(v.z, 0.0f);
        checkTest(pass, "Ternary with vec3 Return (false branch)");
    } catch (const std::exception& e) {
        checkTest(false, std::string("Ternary with vec3 False - Exception: ") + e.what());
    }
}

// Test 8: Ternary with comparison operators
void testTernaryWithComparisons() {
    BytecodeParser parser;
    std::string source = R"(
        float test(float x) {
            return x >= 5.0 ? 10.0 : (x <= 2.0 ? 1.0 : 5.0);
        }
    )";
    
    try {
        std::vector<mmrsl::Value> args1 = {mmrsl::Value(7.0f)};
        std::vector<mmrsl::Value> args2 = {mmrsl::Value(1.0f)};
        std::vector<mmrsl::Value> args3 = {mmrsl::Value(3.0f)};
        
        mmrsl::Value result1 = parser.compileAndExecute(source, args1);
        mmrsl::Value result2 = parser.compileAndExecute(source, args2);
        mmrsl::Value result3 = parser.compileAndExecute(source, args3);
        
        bool pass = floatEquals(result1.asFloat(), 10.0f) && 
                    floatEquals(result2.asFloat(), 1.0f) && 
                    floatEquals(result3.asFloat(), 5.0f);
        checkTest(pass, "Ternary with >= and <= Comparisons");
    } catch (const std::exception& e) {
        checkTest(false, std::string("Ternary with Comparisons - Exception: ") + e.what());
    }
}

// Test 9: Ternary in variable initialization
void testTernaryInVarInit() {
    BytecodeParser parser;
    std::string source = R"(
        float test(float x) {
            float result = x > 5.0 ? 100.0 : 50.0;
            return result;
        }
    )";
    
    try {
        std::vector<mmrsl::Value> args1 = {mmrsl::Value(10.0f)};
        std::vector<mmrsl::Value> args2 = {mmrsl::Value(3.0f)};
        
        mmrsl::Value result1 = parser.compileAndExecute(source, args1);
        mmrsl::Value result2 = parser.compileAndExecute(source, args2);
        
        bool pass = floatEquals(result1.asFloat(), 100.0f) && floatEquals(result2.asFloat(), 50.0f);
        checkTest(pass, "Ternary in Variable Initialization");
    } catch (const std::exception& e) {
        checkTest(false, std::string("Ternary in Var Init - Exception: ") + e.what());
    }
}

// Test 10: Ternary with equality check
void testTernaryWithEquality() {
    BytecodeParser parser;
    std::string source = R"(
        float test(float x) {
            return x == 5.0 ? 1.0 : 0.0;
        }
    )";
    
    try {
        std::vector<mmrsl::Value> args1 = {mmrsl::Value(5.0f)};
        std::vector<mmrsl::Value> args2 = {mmrsl::Value(3.0f)};
        
        mmrsl::Value result1 = parser.compileAndExecute(source, args1);
        mmrsl::Value result2 = parser.compileAndExecute(source, args2);
        
        bool pass = floatEquals(result1.asFloat(), 1.0f) && floatEquals(result2.asFloat(), 0.0f);
        checkTest(pass, "Ternary with Equality Check (==)");
    } catch (const std::exception& e) {
        checkTest(false, std::string("Ternary with Equality - Exception: ") + e.what());
    }
}

// Test 11: Ternary with float return in both branches (absolute value)
void testTernaryFloatBranches() {
    BytecodeParser parser;
    std::string source = R"(
        float test(float x) {
            return x < 0.0 ? -x : x;
        }
    )";
    
    try {
        std::vector<mmrsl::Value> args1 = {mmrsl::Value(-5.0f)};
        std::vector<mmrsl::Value> args2 = {mmrsl::Value(5.0f)};
        
        mmrsl::Value result1 = parser.compileAndExecute(source, args1);
        mmrsl::Value result2 = parser.compileAndExecute(source, args2);
        
        bool pass = floatEquals(result1.asFloat(), 5.0f) && floatEquals(result2.asFloat(), 5.0f);
        checkTest(pass, "Ternary as Absolute Value Function");
    } catch (const std::exception& e) {
        checkTest(false, std::string("Ternary Absolute Value - Exception: ") + e.what());
    }
}

// Test 12: Ternary with complex condition
void testTernaryComplexCondition() {
    BytecodeParser parser;
    std::string source = R"(
        float test(float x, float y, float z) {
            return (x > y || y > z) ? x + y : y + z;
        }
    )";
    
    try {
        std::vector<mmrsl::Value> args1 = {mmrsl::Value(5.0f), mmrsl::Value(3.0f), mmrsl::Value(1.0f)};
        std::vector<mmrsl::Value> args2 = {mmrsl::Value(1.0f), mmrsl::Value(5.0f), mmrsl::Value(3.0f)};
        std::vector<mmrsl::Value> args3 = {mmrsl::Value(1.0f), mmrsl::Value(2.0f), mmrsl::Value(3.0f)};
        
        mmrsl::Value result1 = parser.compileAndExecute(source, args1);
        mmrsl::Value result2 = parser.compileAndExecute(source, args2);
        mmrsl::Value result3 = parser.compileAndExecute(source, args3);
        
        bool pass = floatEquals(result1.asFloat(), 8.0f) && 
                    floatEquals(result2.asFloat(), 6.0f) && 
                    floatEquals(result3.asFloat(), 5.0f);
        checkTest(pass, "Ternary with Complex OR Condition");
    } catch (const std::exception& e) {
        checkTest(false, std::string("Ternary Complex Condition - Exception: ") + e.what());
    }
}

// Main test runner
int main() {
    std::cout << "=== Ternary Conditional Operator Test Suite ===" << std::endl << std::endl;
    
    testBasicTernaryTrue();
    testBasicTernaryFalse();
    testTernaryWithExpressions();
    testNestedTernary();
    testTernaryWithLogicalOps();
    testTernaryWithVec3True();
    testTernaryWithVec3False();
    testTernaryWithComparisons();
    testTernaryInVarInit();
    testTernaryWithEquality();
    testTernaryFloatBranches();
    testTernaryComplexCondition();
    
    std::cout << std::endl << "=== Results ===" << std::endl;
    std::cout << "Passed: " << g_testsPassed << "/" << (g_testsPassed + g_testsFailed) << std::endl;
    
    return g_testsFailed > 0 ? 1 : 0;
}
