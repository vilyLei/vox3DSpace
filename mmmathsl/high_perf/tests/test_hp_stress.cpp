// Stress tests for High-Performance mmrsl
// Tests performance, memory stability, and correctness under high execution density

#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <chrono>
#include <iomanip>
#include <memory>
#include <thread>
#include <cstdio>
#include <cstring>

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#endif

#include "mmrsl/mmrsl.hpp"

using namespace mmrsl;

// Memory usage monitoring structure
struct MemoryStats {
    size_t workingSetMB;      // Physical memory used (MB)
    size_t privateBytesMB;    // Private memory used (MB)
    size_t peakWorkingSetMB;  // Peak physical memory (MB)
    
    void print(const std::string& label) const {
        std::cout << "  [Memory] " << label << ": "
                  << "WorkingSet=" << workingSetMB << "MB, "
                  << "Private=" << privateBytesMB << "MB, "
                  << "Peak=" << peakWorkingSetMB << "MB\n";
    }
};

// Get current memory usage
MemoryStats getMemoryUsage() {
    MemoryStats stats = {0, 0, 0};
    
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        stats.workingSetMB = pmc.WorkingSetSize / (1024 * 1024);
        stats.peakWorkingSetMB = pmc.PeakWorkingSetSize / (1024 * 1024);
        // Private bytes approximation
        stats.privateBytesMB = (pmc.PagefileUsage + pmc.WorkingSetSize) / (1024 * 1024);
    }
#else
    // Linux: read from /proc/self/status
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
        std::cout << "\n";
        startStats.print("Before " + testName);
    }
    
    ~MemoryTracker() {
        MemoryStats endStats = getMemoryUsage();
        endStats.print("After " + testName);
        
        long delta = static_cast<long>(endStats.workingSetMB) - static_cast<long>(startStats.workingSetMB);
        std::cout << "  [Memory] Delta for " << testName << ": " 
                  << (delta >= 0 ? "+" : "") << delta << "MB\n";
        
        // Warning if memory grew significantly
        if (delta > 50) {
            std::cout << "  [WARNING] Significant memory increase detected!\n";
        }
    }
};

// Test 105: High-Frequency Compilation Stress
// Tests rapid repeated compilation to check for memory leaks
bool testHighFrequencyCompilation() {
    std::cout << "Stress Test: High-Frequency Compilation (10,000 iterations)... ";
    MemoryTracker tracker("HighFreqCompile");
    
    const int iterations = 10000;
    std::string source = R"(
        float calc(float x) {
            return sin(x) * cos(x) + x * x;
        }
    )";
    
    auto start = std::chrono::high_resolution_clock::now();
    
    try {
        for (int i = 0; i < iterations; i++) {
            HighPerfParser parser;
            std::vector<mmrsl::Value> args = { mmrsl::Value(1.5f) };
            mmrsl::Value result = parser.compileAndExecute(source, args);
            
            if (i % 1000 == 0) {
                // Periodic verification
                float expected = std::sin(1.5f) * std::cos(1.5f) + 1.5f * 1.5f;
                if (std::abs(result.asFloat() - expected) > 0.0001f) {
                    std::cout << "FAIL (incorrect result at iteration " << i << ")\n";
                    return false;
                }
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

// Test 106: Large Constant Pool Stress
// Tests handling of scripts with many unique constants (within expression depth limit)
bool testLargeConstantPool() {
    std::cout << "Stress Test: Large Constant Pool (30 constants)...\n";
    MemoryTracker tracker("LargeConstantPool");
    
    // Build a source with many unique constants (within depth limit)
    // Use multiple statements instead of one huge expression
    std::string source = "float calc(float x) {\n";
    source += "    float sum = 0.0;\n";
    for (int i = 0; i < 30; i++) {
        source += "    sum = sum + " + std::to_string(i * 0.1f).substr(0, 3) + ";\n";
    }
    source += "    return sum;\n}";
    
    try {
        HighPerfParser parser;
        std::vector<mmrsl::Value> args = { mmrsl::Value(0.0f) };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        
        float expected = 0.0f;
        for (int i = 0; i < 30; i++) {
            expected += i * 0.1f;
        }
        
        if (std::abs(result.asFloat() - expected) > 0.1f) {
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

// Test 107: Deep Expression Nesting Stress
// Tests deep expression trees (within 64 depth limit)
bool testDeepExpressionNesting() {
    std::cout << "Stress Test: Deep Expression Nesting (50 levels)...\n";
    MemoryTracker tracker("DeepNesting");
    
    // Build deeply nested expression: (((((x + 0.01) + 0.01) + 0.01)...)
    std::string source = "float calc(float x) { return ";
    for (int i = 0; i < 50; i++) {
        source += "(";
    }
    source += "x";
    for (int i = 0; i < 50; i++) {
        source += " + 0.01)";
    }
    source += "; }";
    
    try {
        HighPerfParser parser;
        std::vector<mmrsl::Value> args = { mmrsl::Value(1.0f) };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        
        float expected = 1.0f + 50.0f * 0.01f; // 1.5
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

// Test 108: Many Local Variables Stress
// Tests scripts with many local variables (within 8KB limit)
bool testManyLocalVariables() {
    std::cout << "Stress Test: Many Local Variables (50 vars)...\n";
    MemoryTracker tracker("ManyLocalVars");
    
    std::string source = "float calc(float x) {\n";
    for (int i = 0; i < 50; i++) {
        source += "    float v" + std::to_string(i) + " = " + std::to_string(i * 0.1f).substr(0, 3) + ";\n";
    }
    source += "    float sum = 0.0;\n";
    for (int i = 0; i < 50; i++) {
        source += "    sum = sum + v" + std::to_string(i) + ";\n";
    }
    source += "    return sum;\n}";
    
    try {
        HighPerfParser parser;
        std::vector<mmrsl::Value> args = { mmrsl::Value(0.0f) };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        
        float expected = 0.0f;
        for (int i = 0; i < 50; i++) {
            expected += i * 0.1f;
        }
        
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

// Test 109: Complex Nested Control Flow Stress
// Tests deeply nested if/else chains
bool testComplexNestedControlFlow() {
    std::cout << "Stress Test: Complex Nested Control Flow...\n";
    MemoryTracker tracker("NestedControlFlow");
    
    std::string source = R"(
        float calc(float x) {
            if (x < 10.0) {
                if (x < 5.0) {
                    if (x < 2.5) {
                        if (x < 1.25) {
                            return 1.0;
                        } else {
                            return 2.0;
                        }
                    } else {
                        return 3.0;
                    }
                } else {
                    return 4.0;
                }
            } else {
                return 5.0;
            }
        }
    )";
    
    try {
        HighPerfParser parser;
        
        // Test different branches
        struct TestCase { float input; float expected; };
        TestCase cases[] = {
            {0.5f, 1.0f}, {1.5f, 2.0f}, {3.0f, 3.0f}, {7.0f, 4.0f}, {15.0f, 5.0f}
        };
        
        for (const auto& tc : cases) {
            std::vector<mmrsl::Value> args = { mmrsl::Value(tc.input) };
            mmrsl::Value result = parser.compileAndExecute(source, args);
            
            if (std::abs(result.asFloat() - tc.expected) > 0.0001f) {
                std::cout << "FAIL (input=" << tc.input << " expected " << tc.expected 
                          << ", got " << result.asFloat() << ")\n";
                return false;
            }
        }
        
        std::cout << "PASS\n";
        return true;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 110: Repeated Execution Stress
// Tests compile-once-execute-many pattern
bool testRepeatedExecutionStress() {
    std::cout << "Stress Test: Repeated Execution (100,000 calls)...\n";
    MemoryTracker tracker("RepeatedExec");
    
    std::string source = R"(
        vec3 calc(float t) {
            float x = sin(t) * cos(t * 0.5);
            float y = cos(t) * sin(t * 0.3);
            float z = sin(t * 0.7) * cos(t * 0.2);
            return vec3(x, y, z);
        }
    )";
    
    try {
        HighPerfParser parser;
        parser.compile(source);
        
        auto start = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < 100000; i++) {
            float t = i * 0.001f;
            std::vector<mmrsl::Value> args = { mmrsl::Value(t) };
            mmrsl::Value result = parser.execute(args);
            
            if (!result.isVec3()) {
                std::cout << "FAIL (iteration " << i << ": expected vec3)\n";
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

// Test 111: Long Duration Stress Test
// Continuous compilation and execution for 1 million iterations
bool testLongDurationStress() {
    std::cout << "Stress Test: Long Duration (1,000,000 iterations)... ";
    MemoryTracker tracker("LongDuration");
    
    const int iterations = 1000000;
    std::string source = R"(
        float calc(float x) {
            vec3 v = vec3(x, x * 0.5, x * 0.25);
            float len = sqrt(dot(v, v));
            return sin(len) * cos(len * 0.5);
        }
    )";
    
    auto start = std::chrono::high_resolution_clock::now();
    int errors = 0;
    
    try {
        for (int i = 0; i < iterations; i++) {
            HighPerfParser parser;
            float input = i * 0.001f;
            std::vector<mmrsl::Value> args = { mmrsl::Value(input) };
            
            try {
                mmrsl::Value result = parser.compileAndExecute(source, args);
                
                // Verify result is valid (not NaN or Inf)
                float f = result.asFloat();
                if (std::isnan(f) || std::isinf(f)) {
                    errors++;
                }
            } catch (...) {
                errors++;
            }
            
            // Progress report every 100k iterations
            if (i > 0 && i % 100000 == 0) {
                std::cout << "." << std::flush;
            }
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);
        
        if (errors > 0) {
            std::cout << "FAIL (" << errors << " errors)\n";
            return false;
        }
        
        std::cout << "PASS (" << duration.count() << "s)\n";
        return true;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 112: Memory Pressure Test
// Tests with maximum allowed constants and local variables
bool testMemoryPressure() {
    std::cout << "Stress Test: Memory Pressure (60 vars & constants)...\n";
    MemoryTracker tracker("MemoryPressure");
    
    // Use many local variables within 8KB limit
    std::string source = "float calc(float x) {\n";
    
    // Add local variables with short names
    for (int i = 0; i < 60; i++) {
        source += "    float v" + std::to_string(i) + "=" + std::to_string(i * 0.01f).substr(0, 3) + ";\n";
    }
    
    // Complex calculation using all variables
    source += "    float r=0.0;\n";
    for (int i = 0; i < 60; i++) {
        source += "    r=r+v" + std::to_string(i);
        if (i < 59) source += "*0.99";
        source += ";\n";
    }
    source += "    return r;\n}";
    
    try {
        HighPerfParser parser;
        std::vector<mmrsl::Value> args = { mmrsl::Value(0.0f) };
        
        auto start = std::chrono::high_resolution_clock::now();
        mmrsl::Value result = parser.compileAndExecute(source, args);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        // Just verify it completes without crash
        if (result.isFloat()) {
            std::cout << "PASS (" << duration.count() << "ms)\n";
            return true;
        } else {
            std::cout << "FAIL (wrong return type)\n";
            return false;
        }
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 113: Rapid Script Variation Test
// Tests rapid compilation of different scripts
bool testRapidScriptVariation() {
    std::cout << "Stress Test: Rapid Script Variation (5,000 unique scripts)... ";
    MemoryTracker tracker("RapidScriptVar");
    
    const int iterations = 5000;
    int errors = 0;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    try {
        for (int i = 0; i < iterations; i++) {
            // Generate slightly different script each iteration
            std::string source = "float calc(float x) {\n";
            source += "    float a = " + std::to_string(i * 0.001f) + ";\n";
            source += "    float b = " + std::to_string((i + 1) * 0.001f) + ";\n";
            source += "    return sin(x + a) * cos(x + b) + a * b;\n}";
            
            HighPerfParser parser;
            std::vector<mmrsl::Value> args = { mmrsl::Value(1.0f) };
            
            try {
                mmrsl::Value result = parser.compileAndExecute(source, args);
                float f = result.asFloat();
                if (std::isnan(f) || std::isinf(f)) {
                    errors++;
                }
            } catch (...) {
                errors++;
            }
            
            if (i > 0 && i % 1000 == 0) {
                std::cout << "." << std::flush;
            }
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        if (errors > 0) {
            std::cout << "FAIL (" << errors << " errors)\n";
            return false;
        }
        
        std::cout << "PASS (" << duration.count() << "ms)\n";
        return true;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 114: Mixed Workload Stress Test
// Random combination of complex operations
bool testMixedWorkload() {
    std::cout << "Stress Test: Mixed Workload (1,000 complex scripts)...\n";
    MemoryTracker tracker("MixedWorkload");
    
    const int iterations = 1000;
    int errors = 0;
    
    // Various complex script templates
    const char* templates[] = {
        R"(
            float calc(float x) {
                mat4 m = mat4(1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0);
                vec4 v = vec4(x, x*0.5, x*0.25, 1.0);
                vec4 r = m * v;
                return r.x + r.y + r.z;
            }
        )",
        R"(
            float calc(float x) {
                vec3 a = vec3(x, sin(x), cos(x));
                vec3 b = normalize(a);
                return dot(a, b) + length(a);
            }
        )",
        R"(
            float calc(float x) {
                if (x < 0.0) return 0.0;
                if (x < 1.0) return x * x;
                if (x < 2.0) return 2.0 * x - 1.0;
                return 3.0;
            }
        )",
        R"(
            float calc(float x) {
                vec3 p = vec3(x, x*0.5, x*0.25);
                return noise1(length(p));
            }
        )",
        R"(
            float calc(float x) {
                float s = sin(x);
                float c = cos(x);
                float t = tan(x * 0.5);
                return s*s + c*c + t*0.1;
            }
        )"
    };
    const int numTemplates = sizeof(templates) / sizeof(templates[0]);
    
    auto start = std::chrono::high_resolution_clock::now();
    
    try {
        for (int i = 0; i < iterations; i++) {
            // Cycle through different script types
            const char* source = templates[i % numTemplates];
            
            HighPerfParser parser;
            float input = (i % 100) * 0.1f;
            std::vector<mmrsl::Value> args = { mmrsl::Value(input) };
            
            try {
                mmrsl::Value result = parser.compileAndExecute(source, args);
                float f = result.asFloat();
                if (std::isnan(f) || std::isinf(f)) {
                    errors++;
                }
            } catch (...) {
                errors++;
            }
            
            if (i > 0 && i % 200 == 0) {
                std::cout << "." << std::flush;
            }
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        if (errors > 0) {
            std::cout << "FAIL (" << errors << " errors)\n";
            return false;
        }
        
        std::cout << "PASS (" << duration.count() << "ms)\n";
        return true;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 115: Deep Recursion Simulation Test
// Tests deeply nested control flow and expression evaluation
bool testDeepRecursionSimulation() {
    std::cout << "Stress Test: Deep Recursion Simulation (32 levels nesting)...\n";
    MemoryTracker tracker("DeepRecursion");
    
    // Build a deeply nested if-else chain (up to MAX_NESTING_DEPTH = 32)
    std::string source = "float calc(float x) {\n";
    for (int i = 0; i < 32; i++) {
        source += "    if (x > " + std::to_string(i * 0.1f) + ") {\n";
    }
    source += "        return " + std::to_string(32.0f) + ";\n";
    for (int i = 0; i < 32; i++) {
        source += "    }\n";
    }
    source += "    return 0.0;\n}";
    
    try {
        HighPerfParser parser;
        
        // Test at boundary
        std::vector<mmrsl::Value> args = { mmrsl::Value(3.5f) };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        
        if (!result.isFloat() || std::abs(result.asFloat() - 32.0f) > 0.0001f) {
            std::cout << "FAIL (expected 32.0, got " << result.asFloat() << ")\n";
            return false;
        }
        
        std::cout << "PASS\n";
        return true;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 116: Heavy Matrix Operations Stress Test
// Tests intensive matrix computations
bool testHeavyMatrixOperations() {
    std::cout << "Stress Test: Heavy Matrix Operations (10,000 mat4 ops)...\n";
    MemoryTracker tracker("HeavyMatrixOps");
    
    std::string source = R"(
        vec4 calc(vec4 v) {
            mat4 m1 = mat4(1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0);
            mat4 m2 = mat4(2.0, 0.0, 0.0, 0.0, 0.0, 2.0, 0.0, 0.0, 0.0, 0.0, 2.0, 0.0, 0.0, 0.0, 0.0, 1.0);
            mat4 m3 = transpose(m2);
            vec4 r = m1 * v;
            r = m2 * r;
            r = m3 * r;
            return r;
        }
    )";
    
    try {
        HighPerfParser parser;
        parser.compile(source);
        
        auto start = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < 10000; i++) {
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

// Test 117: Concurrent Compilation Simulation Test
// Simulates rapid concurrent-like compilation (single-threaded burst)
bool testConcurrentCompilationSimulation() {
    std::cout << "Stress Test: Concurrent Compilation Simulation (1,000 rapid compiles)...\n";
    MemoryTracker tracker("ConcurrentCompile");
    
    const int iterations = 1000;
    int errors = 0;
    
    // Different script types to simulate varied workload
    const char* scripts[] = {
        "float calc(float x) { return sin(x) + cos(x); }",
        "float calc(float x) { vec3 v = vec3(x, x*2.0, x*3.0); return length(v); }",
        "float calc(float x) { if (x > 0.5) return x * 2.0; else return x * 0.5; }",
        "float calc(float x) { mat2 m = mat2(1.0, 0.0, 0.0, 1.0); vec2 v = vec2(x, x); return (m * v).x; }",
        "float calc(float x) { return noise1(x) + noise1(x + 1.0); }"
    };
    const int numScripts = sizeof(scripts) / sizeof(scripts[0]);
    
    auto start = std::chrono::high_resolution_clock::now();
    
    try {
        for (int i = 0; i < iterations; i++) {
            // Rapidly create and destroy parsers
            HighPerfParser parser;
            const char* source = scripts[i % numScripts];
            std::vector<mmrsl::Value> args = { mmrsl::Value(0.5f) };
            
            try {
                mmrsl::Value result = parser.compileAndExecute(source, args);
                if (std::isnan(result.asFloat()) || std::isinf(result.asFloat())) {
                    errors++;
                }
            } catch (...) {
                errors++;
            }
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        if (errors > 0) {
            std::cout << "FAIL (" << errors << " errors)\n";
            return false;
        }
        
        std::cout << "PASS (" << duration.count() << "ms)\n";
        return true;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}

// Test 118: Max Local Variables Deterministic Test
// Tests 110 local variables (near 128 limit) with complex computation
// Verifies deterministic correctness across multiple runs
bool testMaxLocalVariablesDeterministic() {
    std::cout << "Stress Test: Max Local Variables Deterministic (110 vars, 10 runs)...\n";
    MemoryTracker tracker("MaxLocalVarsDet");
    
    // Build a complex expression with 110 local variables (compact format)
    std::string source = "float calc(float s){\n";
    
    // Initialize 110 variables with compact format
    for (int i = 0; i < 110; i++) {
        source += "float a" + std::to_string(i) + "=";
        source += "s*" + std::to_string(i + 1) + "+" + std::to_string(i).substr(0, 3) + ";";
        if (i % 4 == 3) source += "\n";
    }
    
    // Complex computation
    source += "float r=0;";
    for (int i = 0; i < 110; i++) {
        if (i % 3 == 0) source += "r=r+a" + std::to_string(i) + "*0.1;";
        else if (i % 3 == 1) source += "r=r+sin(a" + std::to_string(i) + ")*0.5;";
        else source += "r=r+cos(a" + std::to_string(i) + ")*0.3;";
        if (i % 4 == 3) source += "\n";
    }
    
    source += "r=r*0.01;";
    source += "if(r>100)r=100;";
    source += "if(r<-100)r=-100;";
    source += "return r;}";
    
    // Check source size
    if (source.length() > 8192) {
        std::cout << "SKIP (source " << source.length() << " bytes > 8192 limit)\n";
        return true; // Skip but don't fail
    }
    
    try {
        // Run multiple times with same seed to verify determinism
        const int numRuns = 10;
        float seed = 3.14159f;
        float firstResult = 0.0f;
        bool firstRun = true;
        
        auto start = std::chrono::high_resolution_clock::now();
        
        for (int run = 0; run < numRuns; ++run) {
            HighPerfParser parser;
            std::vector<mmrsl::Value> args = { mmrsl::Value(seed) };
            mmrsl::Value result = parser.compileAndExecute(source, args);
            
            if (!result.isFloat()) {
                std::cout << "FAIL (run " << run << ": expected float)\n";
                return false;
            }
            
            float f = result.asFloat();
            
            if (firstRun) {
                firstResult = f;
                firstRun = false;
            } else {
                // Verify deterministic: all runs should produce identical result
                if (std::abs(f - firstResult) > 0.0001f) {
                    std::cout << "FAIL (non-deterministic: run " << run << " got " << f 
                              << ", expected " << firstResult << ")\n";
                    return false;
                }
            }
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        std::cout << "PASS (" << duration.count() << "ms, result=" << firstResult << ")\n";
        return true;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}
int runStressTests() {
    std::cout << "\n=== High-Performance mmrsl Stress Test Suite ===\n\n";
    
    int passed = 0;
    int total = 14;
    
    if (testHighFrequencyCompilation()) passed++;
    if (testLargeConstantPool()) passed++;
    if (testDeepExpressionNesting()) passed++;
    if (testManyLocalVariables()) passed++;
    if (testComplexNestedControlFlow()) passed++;
    if (testRepeatedExecutionStress()) passed++;
    if (testLongDurationStress()) passed++;
    if (testMemoryPressure()) passed++;
    if (testRapidScriptVariation()) passed++;
    if (testMixedWorkload()) passed++;
    if (testDeepRecursionSimulation()) passed++;
    if (testHeavyMatrixOperations()) passed++;
    if (testConcurrentCompilationSimulation()) passed++;
    if (testMaxLocalVariablesDeterministic()) passed++;
    
    std::cout << "\n=== Stress Test Results ===\n";
    std::cout << "Passed: " << passed << "/" << total << "\n";
    
    // Final memory summary
    std::cout << "\n=== Memory Usage Summary ===\n";
    MemoryStats finalStats = getMemoryUsage();
    finalStats.print("Final");
    
    return (passed == total) ? 0 : 1;
}

// Main entry point for stress tests
int main() {
    return runStressTests();
}
