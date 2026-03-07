cmake_minimum_required(VERSION 3.25)
project(mmrsl VERSION 1.0.0 LANGUAGES CXX)

# C++20 Standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ============================================================================
# Unified mmrsl Library (zero external dependencies)
# ============================================================================

# Public headers (zero-dependency API)
set(PUBLIC_HEADERS
    include/mmrsl/mmrsl.hpp
    include/mmrsl/types.hpp
)

# Internal implementation headers
set(INTERNAL_HEADERS
    include/mmrsl/detail/lexer.hpp
    include/mmrsl/detail/parser.hpp
    include/mmrsl/detail/interpreter.hpp
    include/mmrsl/detail/compiler.hpp
    include/mmrsl/detail/vm.hpp
    include/mmrsl/detail/bytecode.hpp
)

# Source files
set(SOURCES
    src/mmrsl.cpp
    simple/src/types.cpp
    simple/src/lexer.cpp
    simple/src/parser.cpp
    simple/src/interpreter.cpp
    bytecode/src/bytecode.cpp
    bytecode/src/vm.cpp
    bytecode/src/compiler.cpp
)

# Create unified static library
add_library(mmrsl STATIC ${SOURCES} ${PUBLIC_HEADERS} ${INTERNAL_HEADERS})

target_include_directories(mmrsl
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Enable warnings
if(MSVC)
    target_compile_options(mmrsl PRIVATE /W4 /WX-)
    # Use UTF-8 encoding to suppress C4819 warnings
    target_compile_options(mmrsl PRIVATE /utf-8)
else()
    target_compile_options(mmrsl PRIVATE -Wall -Wextra -Wpedantic)
endif()

# ============================================================================
# Test Executables
# ============================================================================

# Simple version test executable
add_executable(mmrsl_test simple/tests/test_main.cpp)
target_link_libraries(mmrsl_test PRIVATE mmrsl)

# High-performance version test executable
add_executable(mmrsl_bc_test bytecode/tests/test_hp_main.cpp)
target_link_libraries(mmrsl_bc_test PRIVATE mmrsl)

# Bytecode VM stress test executable
add_executable(mmrsl_bc_stress bytecode/tests/test_hp_stress.cpp)
target_link_libraries(mmrsl_bc_stress PRIVATE mmrsl)

# Bytecode VM complex expression test executable
add_executable(mmrsl_bc_complex bytecode/tests/test_hp_complex.cpp)
target_link_libraries(mmrsl_bc_complex PRIVATE mmrsl)

# Bytecode VM built-in functions test executable
add_executable(mmrsl_bc_builtin bytecode/tests/test_hp_builtin.cpp)
target_link_libraries(mmrsl_bc_builtin PRIVATE mmrsl)

# Bytecode VM pipeline test executable
add_executable(mmrsl_bc_pipeline bytecode/tests/test_hp_pipeline.cpp)
target_link_libraries(mmrsl_bc_pipeline PRIVATE mmrsl)

# Bytecode VM ternary operator test executable
add_executable(mmrsl_bc_ternary bytecode/tests/test_hp_ternary.cpp)
target_link_libraries(mmrsl_bc_ternary PRIVATE mmrsl)

# Bytecode VM for loop test executable
add_executable(mmrsl_bc_for_loop bytecode/tests/test_hp_for_loop.cpp)
target_link_libraries(mmrsl_bc_for_loop PRIVATE mmrsl)

# Apply UTF-8 encoding to all test executables on MSVC
if(MSVC)
    target_compile_options(mmrsl_test PRIVATE /utf-8)
    target_compile_options(mmrsl_bc_test PRIVATE /utf-8)
    target_compile_options(mmrsl_bc_stress PRIVATE /utf-8)
    target_compile_options(mmrsl_bc_complex PRIVATE /utf-8)
    target_compile_options(mmrsl_bc_builtin PRIVATE /utf-8)
    target_compile_options(mmrsl_bc_pipeline PRIVATE /utf-8)
    target_compile_options(mmrsl_bc_ternary PRIVATE /utf-8)
    target_compile_options(mmrsl_bc_for_loop PRIVATE /utf-8)
endif()

# Link Windows PSAPI for memory monitoring
if(WIN32)
    target_link_libraries(mmrsl_bc_stress PRIVATE psapi)
    target_link_libraries(mmrsl_bc_complex PRIVATE psapi)
    target_link_libraries(mmrsl_bc_builtin PRIVATE psapi)
endif()

# ============================================================================
# Visual Studio specific settings
# ============================================================================
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT mmrsl_test)

# ============================================================================
# WASM target (only when building with Emscripten)
# ============================================================================
if(EMSCRIPTEN)
    add_executable(mmrsl_wasm wasm/mmrsl_wasm_bridge.cpp)
    target_link_libraries(mmrsl_wasm PRIVATE mmrsl)
    target_include_directories(mmrsl_wasm PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

    # Embind + exception support + optimization
    target_compile_options(mmrsl_wasm PRIVATE
        -std=c++20
        -O2
        -fexceptions
    )
    target_link_options(mmrsl_wasm PRIVATE
        --bind                          # enable Embind
        -fexceptions                    # C++ exception support across WASM boundary
        -sEXPORT_ES6=1                  # output ES6 module
        -sMODULARIZE=1                  # wrap in factory function: createMmrslModule()
        -sEXPORT_NAME=createMmrslModule
        -sALLOW_MEMORY_GROWTH=1         # dynamic heap growth
        -sINITIAL_MEMORY=33554432       # 32MB initial heap
        -sENVIRONMENT=web,worker,node  # target web + web worker + node (for testing)
        "--emit-tsd=mmrsl.d.ts"         # generate TypeScript type declarations
    )

    # Output to wasm/dist/
    set_target_properties(mmrsl_wasm PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/wasm/dist"
        OUTPUT_NAME "mmrsl"
        SUFFIX ".js"
    )
endif()

