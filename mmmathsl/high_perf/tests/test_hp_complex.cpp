// Complex Expression Tests for High-Performance mmrsl
// Tests expressions approaching system limits with 8KB source code support

#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <chrono>
#include <iomanip>
#include <cstdio>
#include <cstring>

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#endif

#include "mmrsl/mmrsl.hpp"

using namespace mmrsl;

// Memory statistics structure
struct MemoryStats {
    size_t workingSetMB;
    size_t privateBytesMB;
    size_t peakWorkingSetMB;
};

// Get current memory usage
MemoryStats getMemoryUsage() {
    MemoryStats stats = {0, 0, 0};
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        stats.workingSetMB = pmc.WorkingSetSize / (1024 * 1024);
        stats.peakWorkingSetMB = pmc.PeakWorkingSetSize / (1024 * 1024);
        stats.privateBytesMB = (pmc.PagefileUsage + pmc.WorkingSetSize) / (1024 * 1024);
    }
#else
    FILE* fp = fopen("/proc/self/status", "r");
    if (fp) {
        char line[256];
        while (fgets(line, sizeof(line), fp)) {
            if (strncmp(line, "VmRSS:", 6) == 0) {
                long rss_kb;
                sscanf(line, "VmRSS: %ld", &rss_kb);
                stats.workingSetMB = rss_kb / 1024;
            }
            if (strncmp(line, "VmPeak:", 7) == 0) {
                long peak_kb;
                sscanf(line, "VmPeak: %ld", &peak_kb);
                stats.peakWorkingSetMB = peak_kb / 1024;
            }
        }
        fclose(fp);
    }
    stats.privateBytesMB = stats.workingSetMB;
#endif
    return stats;
}

// Memory change tracking
class MemoryTracker {
    MemoryStats startStats;
    std::string testName;
    
public:
    explicit MemoryTracker(const std::string& name) : testName(name) {
        startStats = getMemoryUsage();
        std::cout << "  [Memory] Before: WorkingSet=" << startStats.workingSetMB 
                  << "MB, Peak=" << startStats.peakWorkingSetMB << "MB\n";
    }
    
    ~MemoryTracker() {
        MemoryStats endStats = getMemoryUsage();
        long delta = static_cast<long>(endStats.workingSetMB) - static_cast<long>(startStats.workingSetMB);
        std::cout << "  [Memory] After:  WorkingSet=" << endStats.workingSetMB 
                  << "MB, Peak=" << endStats.peakWorkingSetMB << "MB";
        std::cout << " (Delta: " << (delta >= 0 ? "+" : "") << delta << "MB)\n";
        
        if (delta > 10) {
            std::cout << "  [WARNING] Significant memory increase!\n";
        }
    }
};

// Test 1: Maximum Local Variables (120 vars with 8KB limit)
bool testMaxLocalVars120() {
    std::cout << "Complex Test: Max Local Variables (120 vars, 8KB)...\n";
    MemoryTracker tracker("MaxLocalVars120");
    
    std::string source = "float calc(float s){\n";
    
    // 120 variables
    for (int i = 0; i < 120; i++) {
        source += "float a" + std::to_string(i) + "=";
        source += "s*" + std::to_string(i + 1) + "+" + std::to_string(i) + ";";
        if (i % 4 == 3) source += "\n";
    }
    
    // Complex computation
    source += "float r=0;";
    for (int i = 0; i < 120; i++) {
        if (i % 3 == 0) source += "r=r+a" + std::to_string(i) + "*0.1;";
        else if (i % 3 == 1) source += "r=r+sin(a" + std::to_string(i) + ")*0.5;";
        else source += "r=r+cos(a" + std::to_string(i) + ")*0.3;";
        if (i % 4 == 3) source += "\n";
    }
    source += "r=r*0.01;if(r>100)r=100;if(r<-100)r=-100;return r;}";
    
    std::cout << "Source: " << source.length() << " bytes\n";
    
    try {
        // Verify determinism with 5 runs
        float seed = 3.14159f;
        float expected = 0.0f;
        
        for (int run = 0; run < 5; ++run) {
            HighPerfParser parser;
            std::vector<mmrsl::Value> args = { mmrsl::Value(seed) };
            mmrsl::Value result = parser.compileAndExecute(source, args);
            
            if (!result.isFloat()) {
                std::cout << "  FAIL (run " << run << ": expected float)\n";
                return false;
            }
            
            float f = result.asFloat();
            if (run == 0) {
                expected = f;
            } else if (std::abs(f - expected) > 0.0001f) {
                std::cout << "  FAIL (non-deterministic: run " << run << " got " << f << ")\n";
                return false;
            }
        }
        
        std::cout << "  PASS (result: " << expected << ")\n";
        return true;
    } catch (const std::exception& e) {
        std::cout << "  FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 2: Deep Expression Nesting (60 levels)
bool testDeepNesting60() {
    std::cout << "Complex Test: Deep Expression Nesting (60 levels)...\n";
    MemoryTracker tracker("DeepNesting60");
    
    std::string source = "float calc(float x){return ";
    for (int i = 0; i < 60; i++) source += "(";
    source += "x";
    for (int i = 0; i < 60; i++) source += "+0.01)";
    source += ";}";
    
    try {
        HighPerfParser parser;
        std::vector<mmrsl::Value> args = { mmrsl::Value(1.0f) };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        
        float expected = 1.0f + 60.0f * 0.01f;
        if (std::abs(result.asFloat() - expected) > 0.001f) {
            std::cout << "FAIL (expected " << expected << ", got " << result.asFloat() << ")\n";
            return false;
        }
        
        std::cout << "PASS\n";
        return true;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 3: Large Constant Pool (100 constants)
bool testLargeConstantPool100() {
    std::cout << "Complex Test: Large Constant Pool (100 constants)...\n";
    MemoryTracker tracker("LargeConstantPool100");
    
    std::string source = "float calc(float x){float s=0;";
    for (int i = 0; i < 100; i++) {
        source += "s=s+" + std::to_string(i * 0.01f).substr(0, 4) + ";";
    }
    source += "return s;}";
    
    try {
        HighPerfParser parser;
        std::vector<mmrsl::Value> args = { mmrsl::Value(0.0f) };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        
        float expected = 0.0f;
        for (int i = 0; i < 100; i++) expected += i * 0.01f;
        
        if (std::abs(result.asFloat() - expected) > 0.5f) {
            std::cout << "FAIL (expected " << expected << ", got " << result.asFloat() << ")\n";
            return false;
        }
        
        std::cout << "PASS\n";
        return true;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 4: Complex Mixed Expression
bool testComplexMixedExpression() {
    std::cout << "Complex Test: Mixed Expression (vec+mat+noise)...\n";
    MemoryTracker tracker("ComplexMixed");
    
    std::string source = R"(
        float calc(float x) {
            vec3 p = vec3(x, x*0.5, x*0.25);
            vec3 n = normalize(p);
            float d = dot(p, n);
            float l = length(p);
            mat4 m = mat4(1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0);
            vec4 v = vec4(p.x, p.y, p.z, 1.0);
            vec4 r = m * v;
            float noise = noise1(l);
            float result = sin(d) * cos(l) + noise * 0.1 + r.x + r.y + r.z;
            if (result > 10.0) result = 10.0;
            if (result < -10.0) result = -10.0;
            return result;
        }
    )";
    
    try {
        HighPerfParser parser;
        std::vector<mmrsl::Value> args = { mmrsl::Value(2.0f) };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        
        if (!result.isFloat()) {
            std::cout << "FAIL (expected float)\n";
            return false;
        }
        
        float f = result.asFloat();
        if (std::isnan(f) || std::isinf(f)) {
            std::cout << "FAIL (result is NaN or Inf)\n";
            return false;
        }
        
        std::cout << "PASS (result: " << f << ")\n";
        return true;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 5: Many Matrix Operations
bool testManyMatrixOperations() {
    std::cout << "Complex Test: Many Matrix Operations (500 ops)...\n";
    MemoryTracker tracker("ManyMatrixOps");
    
    std::string source = R"(
        vec4 calc(vec4 v) {
            mat4 m = mat4(1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0);
            vec4 r = v;
            r = m * r;
            r = m * r;
            r = m * r;
            return r;
        }
    )";
    
    try {
        HighPerfParser parser;
        parser.compile(source);
        
        auto start = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < 500; i++) {
            mmrsl::Vec4 input(1.0f, 2.0f, 3.0f, 1.0f);
            std::vector<mmrsl::Value> args = { mmrsl::Value(input) };
            mmrsl::Value result = parser.execute(args);
            
            if (!result.isVec4()) {
                std::cout << "FAIL (iteration " << i << ": expected vec4)\n";
                return false;
            }
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        std::cout << "PASS (" << duration.count() << "ms)\n";
        return true;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 6: Deep If Nesting (30 levels)
// Tests deeply nested if-else control flow near the 32 limit
bool testDeepIfNesting30() {
    std::cout << "Complex Test: Deep If Nesting (30 levels)...\n";
    MemoryTracker tracker("DeepIfNesting30");
    
    // Build 30 levels of nested if statements (within 32 limit)
    std::string source = "float calc(float x) {\n";
    
    // Open 30 if blocks
    for (int i = 0; i < 30; i++) {
        source += "    if (x > " + std::to_string(i * 0.1f) + ") {\n";
    }
    
    // Innermost return
    source += "        return 30.0;\n";
    
    // Close all if blocks
    for (int i = 0; i < 30; i++) {
        source += "    }\n";
    }
    
    // Default return
    source += "    return 0.0;\n";
    source += "}";
    
    std::cout << "  Source: " << source.length() << " bytes, " 
              << std::count(source.begin(), source.end(), '{') << " blocks\n";
    
    try {
        HighPerfParser parser;
        
        // Test value that reaches innermost level
        std::vector<mmrsl::Value> args = { mmrsl::Value(5.0f) };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        
        if (!result.isFloat()) {
            std::cout << "  FAIL (expected float)\n";
            return false;
        }
        
        float f = result.asFloat();
        if (std::abs(f - 30.0f) > 0.0001f) {
            std::cout << "  FAIL (expected 30.0, got " << f << ")\n";
            return false;
        }
        
        // Test value that doesn't enter any if
        std::vector<mmrsl::Value> args2 = { mmrsl::Value(-1.0f) };
        mmrsl::Value result2 = parser.compileAndExecute(source, args2);
        
        if (std::abs(result2.asFloat() - 0.0f) > 0.0001f) {
            std::cout << "  FAIL (expected 0.0 for negative input, got " << result2.asFloat() << ")\n";
            return false;
        }
        
        std::cout << "  PASS (innermost=30.0, default=0.0)\n";
        return true;
    } catch (const std::exception& e) {
        std::cout << "  FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 7: Deep Logical Expression Nesting (60 levels)
// Tests deeply nested boolean logic expressions (within 64 depth limit)
bool testDeepLogicalNesting60() {
    std::cout << "Complex Test: Deep Logical Expression Nesting (60 levels)...\n";
    MemoryTracker tracker("DeepLogicalNesting60");
    
    // Build 60 levels of nested logical expressions: (x > 0 && (x > 1 && (x > 2 && ...)))
    std::string source = "bool calc(float x) { return ";
    
    // Open 60 conditions
    for (int i = 0; i < 60; i++) {
        source += "(x > " + std::to_string(i * 0.01f).substr(0, 4) + " && ";
    }
    
    // Innermost true
    source += "true";
    
    // Close all conditions
    for (int i = 0; i < 60; i++) {
        source += ")";
    }
    
    source += "; }";
    
    std::cout << "  Source: " << source.length() << " bytes\n";
    
    try {
        HighPerfParser parser;
        
        // Test value that satisfies all conditions
        std::vector<mmrsl::Value> args = { mmrsl::Value(1.0f) };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        
        if (!result.isBool()) {
            std::cout << "  FAIL (expected bool, got " << (result.isFloat() ? "float" : "other") << ")\n";
            return false;
        }
        
        if (!result.asBool()) {
            std::cout << "  FAIL (expected true for x=1.0, got false)\n";
            return false;
        }
        
        // Test value that fails early
        std::vector<mmrsl::Value> args2 = { mmrsl::Value(0.005f) };
        mmrsl::Value result2 = parser.compileAndExecute(source, args2);
        
        if (result2.asBool()) {
            std::cout << "  FAIL (expected false for x=0.005, got true)\n";
            return false;
        }
        
        std::cout << "  PASS (x=1.0->true, x=0.005->false)\n";
        return true;
    } catch (const std::exception& e) {
        std::cout << "  FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test runner
int runComplexTests() {
    std::cout << "=== TEST: test_hp_complex ===\n\n";
    
    MemoryStats startStats = getMemoryUsage();
    std::cout << "[Memory] Initial: WorkingSet=" << startStats.workingSetMB 
              << "MB, Peak=" << startStats.peakWorkingSetMB << "MB\n\n";
    
    int passed = 0;
    int total = 7;  // Updated to 7 tests
    
    // Run tests 3 times to verify memory stability
    for (int round = 0; round < 3; ++round) {
        std::cout << "\n========== Round " << (round + 1) << "/3 ==========\n";
        
        if (testMaxLocalVars120()) passed++;
        if (testDeepNesting60()) passed++;
        if (testLargeConstantPool100()) passed++;
        if (testComplexMixedExpression()) passed++;
        if (testManyMatrixOperations()) passed++;
        if (testDeepIfNesting30()) passed++;
        if (testDeepLogicalNesting60()) passed++;  // New test
        
        MemoryStats midStats = getMemoryUsage();
        std::cout << "[Memory] After Round " << (round + 1) << ": WorkingSet=" 
                  << midStats.workingSetMB << "MB\n";
    }
    
    std::cout << "\n=== Complex Test Results ===\n";
    std::cout << "Passed: " << passed << "/" << (total * 3) << "\n";
    
    MemoryStats endStats = getMemoryUsage();
    std::cout << "[Memory] Final:   WorkingSet=" << endStats.workingSetMB 
              << "MB, Peak=" << endStats.peakWorkingSetMB << "MB\n";
    
    long delta = static_cast<long>(endStats.workingSetMB) - static_cast<long>(startStats.workingSetMB);
    std::cout << "Memory Delta: " << (delta >= 0 ? "+" : "") << delta << "MB\n";
    
    if (delta <= 2) {
        std::cout << "[OK] Memory stable (Debug mode overhead is normal)\n";
    } else if (delta > 10) {
        std::cout << "[WARNING] Possible memory leak detected!\n";
    }
    
    return (passed == total * 3) ? 0 : 1;
}

// Main entry point
int main() {
    return runComplexTests();
}
