// For Loop Tests for High-Performance mmrsl
// Tests C-style for loop with break, continue, int arithmetic, and type conversions

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

// Test 1: Basic counting loop - sum 0..9 = 45
void testBasicCountingLoop() {
    HighPerfParser parser;
    std::string source = R"(
        float test() {
            float acc = 0.0;
            for (int i = 0; i < 10; i++) {
                acc = acc + float(i);
            }
            return acc;
        }
    )";

    try {
        std::vector<mmrsl::Value> args = {};
        mmrsl::Value result = parser.compileAndExecute(source, args);
        checkTest(floatEquals(result.asFloat(), 45.0f), "Basic Counting Loop (sum 0..9 = 45)");
    } catch (const std::exception& e) {
        checkTest(false, std::string("Basic Counting Loop - Exception: ") + e.what());
    }
}

// Test 2: break exits early - sum 0..4 = 10
void testBreakExitsEarly() {
    HighPerfParser parser;
    std::string source = R"(
        float test() {
            float acc = 0.0;
            for (int i = 0; i < 10; i++) {
                if (i == 5) break;
                acc = acc + float(i);
            }
            return acc;
        }
    )";

    try {
        std::vector<mmrsl::Value> args = {};
        mmrsl::Value result = parser.compileAndExecute(source, args);
        checkTest(floatEquals(result.asFloat(), 10.0f), "Break Exits Early (sum 0..4 = 10)");
    } catch (const std::exception& e) {
        checkTest(false, std::string("Break Exits Early - Exception: ") + e.what());
    }
}

// Test 3: continue skips iteration - sum 0..9 excluding 3 and 7 = 45 - 3 - 7 = 35
void testContinueSkipsIteration() {
    HighPerfParser parser;
    std::string source = R"(
        float test() {
            float acc = 0.0;
            for (int i = 0; i < 10; i++) {
                if (i == 3) continue;
                if (i == 7) continue;
                acc = acc + float(i);
            }
            return acc;
        }
    )";

    try {
        std::vector<mmrsl::Value> args = {};
        mmrsl::Value result = parser.compileAndExecute(source, args);
        checkTest(floatEquals(result.asFloat(), 35.0f), "Continue Skips Iterations (sum skip 3,7 = 35)");
    } catch (const std::exception& e) {
        checkTest(false, std::string("Continue Skips Iteration - Exception: ") + e.what());
    }
}

// Test 4: Nested for loops - outer 0..2, inner 0..2, count = 3*3 = 9
void testNestedForLoops() {
    HighPerfParser parser;
    std::string source = R"(
        float test() {
            float count = 0.0;
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    count = count + 1.0;
                }
            }
            return count;
        }
    )";

    try {
        std::vector<mmrsl::Value> args = {};
        mmrsl::Value result = parser.compileAndExecute(source, args);
        checkTest(floatEquals(result.asFloat(), 9.0f), "Nested For Loops (3x3 = 9 iterations)");
    } catch (const std::exception& e) {
        checkTest(false, std::string("Nested For Loops - Exception: ") + e.what());
    }
}

// Test 5: float(i) type conversion - sum of float conversions 0..4 = 0+1+2+3+4 = 10
void testFloatTypeConversion() {
    HighPerfParser parser;
    std::string source = R"(
        float test() {
            float acc = 0.0;
            for (int i = 0; i < 5; i++) {
                float fi = float(i);
                acc = acc + fi;
            }
            return acc;
        }
    )";

    try {
        std::vector<mmrsl::Value> args = {};
        mmrsl::Value result = parser.compileAndExecute(source, args);
        checkTest(floatEquals(result.asFloat(), 10.0f), "float(i) Type Conversion (sum 0..4 = 10)");
    } catch (const std::exception& e) {
        checkTest(false, std::string("float Type Conversion - Exception: ") + e.what());
    }
}

// Test 6: Loop with vec3 accumulation - sum vec3(i,i,i) for i=0..2 = vec3(3,3,3)
void testVec3Accumulation() {
    HighPerfParser parser;
    std::string source = R"(
        vec3 test() {
            vec3 acc = vec3(0.0, 0.0, 0.0);
            for (int i = 0; i < 3; i++) {
                float fi = float(i);
                acc = acc + vec3(fi, fi, fi);
            }
            return acc;
        }
    )";

    try {
        std::vector<mmrsl::Value> args = {};
        mmrsl::Value result = parser.compileAndExecute(source, args);
        mmrsl::Vec3 v = result.asVec3();
        bool pass = floatEquals(v.x, 3.0f) && floatEquals(v.y, 3.0f) && floatEquals(v.z, 3.0f);
        checkTest(pass, "Vec3 Accumulation in Loop (sum vec3(i) for i=0..2 = vec3(3,3,3))");
    } catch (const std::exception& e) {
        checkTest(false, std::string("Vec3 Accumulation - Exception: ") + e.what());
    }
}

// Test 7: Decrement loop - sum 4..0 = 10
void testDecrementLoop() {
    HighPerfParser parser;
    std::string source = R"(
        float test() {
            float acc = 0.0;
            for (int i = 4; i >= 0; i--) {
                acc = acc + float(i);
            }
            return acc;
        }
    )";

    try {
        std::vector<mmrsl::Value> args = {};
        mmrsl::Value result = parser.compileAndExecute(source, args);
        checkTest(floatEquals(result.asFloat(), 10.0f), "Decrement Loop (sum 4..0 = 10)");
    } catch (const std::exception& e) {
        checkTest(false, std::string("Decrement Loop - Exception: ") + e.what());
    }
}

// Test 8: Zero-iteration loop (condition false from start)
void testZeroIterationLoop() {
    HighPerfParser parser;
    std::string source = R"(
        float test() {
            float acc = 99.0;
            for (int i = 0; i < 0; i++) {
                acc = acc + 1.0;
            }
            return acc;
        }
    )";

    try {
        std::vector<mmrsl::Value> args = {};
        mmrsl::Value result = parser.compileAndExecute(source, args);
        checkTest(floatEquals(result.asFloat(), 99.0f), "Zero-Iteration Loop (condition false from start)");
    } catch (const std::exception& e) {
        checkTest(false, std::string("Zero-Iteration Loop - Exception: ") + e.what());
    }
}

// Test 9: break and continue together - sum 0..9 skip 3, stop at 7 = 0+1+2+4+5+6 = 18
void testBreakAndContinueTogether() {
    HighPerfParser parser;
    std::string source = R"(
        float test() {
            float acc = 0.0;
            for (int i = 0; i < 10; i++) {
                if (i == 3) continue;
                if (i == 7) break;
                acc = acc + float(i);
            }
            return acc;
        }
    )";

    try {
        std::vector<mmrsl::Value> args = {};
        mmrsl::Value result = parser.compileAndExecute(source, args);
        // 0+1+2 (skip 3) +4+5+6 (break at 7) = 18
        checkTest(floatEquals(result.asFloat(), 18.0f), "Break and Continue Together (skip 3, break at 7 = 18)");
    } catch (const std::exception& e) {
        checkTest(false, std::string("Break and Continue Together - Exception: ") + e.what());
    }
}

// Test 10: Loop with parameter input - count steps from 0 to n
void testLoopWithParameter() {
    HighPerfParser parser;
    std::string source = R"(
        float test(float n) {
            int limit = int(n);
            float acc = 0.0;
            for (int i = 0; i < limit; i++) {
                acc = acc + 1.0;
            }
            return acc;
        }
    )";

    try {
        std::vector<mmrsl::Value> args = {mmrsl::Value(7.0f)};
        mmrsl::Value result = parser.compileAndExecute(source, args);
        checkTest(floatEquals(result.asFloat(), 7.0f), "Loop With Parameter (count 0..6 = 7)");
    } catch (const std::exception& e) {
        checkTest(false, std::string("Loop With Parameter - Exception: ") + e.what());
    }
}

// Main test runner
int main() {
    std::cout << "=== For Loop Test Suite ===" << std::endl << std::endl;

    testBasicCountingLoop();
    testBreakExitsEarly();
    testContinueSkipsIteration();
    testNestedForLoops();
    testFloatTypeConversion();
    testVec3Accumulation();
    testDecrementLoop();
    testZeroIterationLoop();
    testBreakAndContinueTogether();
    testLoopWithParameter();

    std::cout << std::endl << "=== Results ===" << std::endl;
    std::cout << "Passed: " << g_testsPassed << "/" << (g_testsPassed + g_testsFailed) << std::endl;

    return g_testsFailed > 0 ? 1 : 0;
}
