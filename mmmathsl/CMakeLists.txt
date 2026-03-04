cmake_minimum_required(VERSION 3.25)
project(mmrsl VERSION 1.0.0 LANGUAGES CXX)

# C++20 Standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ============================================================================
# GLM Configuration (B + D方案: CMake选项 + FetchContent)
# ============================================================================

# Option 1: CMake 选项（优先级最高）
set(GLM_ROOT "" CACHE PATH "Path to GLM library (optional)")

# Option 2: 环境变量回退
if(NOT GLM_ROOT)
    set(GLM_ROOT "$ENV{GLM_ROOT}")
endif()

# Option 3: 自动查找常见位置
if(NOT GLM_ROOT OR NOT EXISTS "${GLM_ROOT}")
    find_path(GLM_INCLUDE_DIR glm/glm.hpp
        PATHS
            "${CMAKE_CURRENT_SOURCE_DIR}/../externals/glm"
            "${CMAKE_CURRENT_SOURCE_DIR}/externals/glm"
            "/usr/include"
            "/usr/local/include"
        DOC "GLM include directory"
    )
    if(GLM_INCLUDE_DIR)
        set(GLM_ROOT "${GLM_INCLUDE_DIR}")
    endif()
endif()

# Option 4: FetchContent 自动下载
if(NOT GLM_ROOT OR NOT EXISTS "${GLM_ROOT}")
    include(FetchContent)
    FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 0.9.9.8
    )
    FetchContent_MakeAvailable(glm)
    set(GLM_ROOT "${glm_SOURCE_DIR}")
    message(STATUS "GLM fetched via FetchContent: ${GLM_ROOT}")
endif()

# 最终验�?
if(NOT EXISTS "${GLM_ROOT}/glm/glm.hpp")
    message(FATAL_ERROR "GLM not found at ${GLM_ROOT}. Please set GLM_ROOT via:\n"
        "  - cmake -DGLM_ROOT=/path/to/glm\n"
        "  - Environment variable GLM_ROOT\n"
        "  - Place GLM in ${CMAKE_CURRENT_SOURCE_DIR}/externals/glm")
endif()

message(STATUS "GLM root: ${GLM_ROOT}")

# ============================================================================
# Unified mmrsl Library
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
    high_perf/src/bytecode.cpp
    high_perf/src/vm.cpp
    high_perf/src/compiler.cpp
)

# Create unified static library
add_library(mmrsl STATIC ${SOURCES} ${PUBLIC_HEADERS} ${INTERNAL_HEADERS})

target_include_directories(mmrsl
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${GLM_ROOT}
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
add_executable(mmrsl_hp_test high_perf/tests/test_hp_main.cpp)
target_link_libraries(mmrsl_hp_test PRIVATE mmrsl)

# High-performance version stress test executable
add_executable(mmrsl_hp_stress high_perf/tests/test_hp_stress.cpp)
target_link_libraries(mmrsl_hp_stress PRIVATE mmrsl)

# High-performance version complex expression test executable
add_executable(mmrsl_hp_complex high_perf/tests/test_hp_complex.cpp)
target_link_libraries(mmrsl_hp_complex PRIVATE mmrsl)

# High-performance version built-in functions test executable
add_executable(mmrsl_hp_builtin high_perf/tests/test_hp_builtin.cpp)
target_link_libraries(mmrsl_hp_builtin PRIVATE mmrsl)

# High-performance version pipeline test executable
add_executable(mmrsl_hp_pipeline high_perf/tests/test_hp_pipeline.cpp)
target_link_libraries(mmrsl_hp_pipeline PRIVATE mmrsl)

# High-performance version ternary operator test executable
add_executable(mmrsl_hp_ternary high_perf/tests/test_hp_ternary.cpp)
target_link_libraries(mmrsl_hp_ternary PRIVATE mmrsl)

# High-performance version for loop test executable
add_executable(mmrsl_hp_for_loop high_perf/tests/test_hp_for_loop.cpp)
target_link_libraries(mmrsl_hp_for_loop PRIVATE mmrsl)

# Apply UTF-8 encoding to all test executables on MSVC
if(MSVC)
    target_compile_options(mmrsl_test PRIVATE /utf-8)
    target_compile_options(mmrsl_hp_test PRIVATE /utf-8)
    target_compile_options(mmrsl_hp_stress PRIVATE /utf-8)
    target_compile_options(mmrsl_hp_complex PRIVATE /utf-8)
    target_compile_options(mmrsl_hp_builtin PRIVATE /utf-8)
    target_compile_options(mmrsl_hp_pipeline PRIVATE /utf-8)
    target_compile_options(mmrsl_hp_ternary PRIVATE /utf-8)
    target_compile_options(mmrsl_hp_for_loop PRIVATE /utf-8)
endif()

# Link Windows PSAPI for memory monitoring
if(WIN32)
    target_link_libraries(mmrsl_hp_stress PRIVATE psapi)
    target_link_libraries(mmrsl_hp_complex PRIVATE psapi)
    target_link_libraries(mmrsl_hp_builtin PRIVATE psapi)
endif()

# ============================================================================
# Visual Studio specific settings
# ============================================================================
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT mmrsl_test)
