cmake_minimum_required(VERSION 3.25)
project(vkapp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ============================================================================
# External Libraries
# ============================================================================

# Vulkan
find_package(Vulkan REQUIRED)

# GLFW3
set(GLFW3_INCLUDE_DIR "D:/dev/webdev/vox3DSpace/space3d/openglLibs/libx64/include")
set(GLFW3_LIBRARY "D:/dev/webdev/vox3DSpace/space3d/openglLibs/libx64/lib/glfw3.lib")

# GLM
set(GLM_ROOT "D:/dev/webdev/vox3DSpace/space3d/externals/glm")

# ============================================================================
# mmrsl Static Library
# ============================================================================

set(MMRSL_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/..")
add_subdirectory(${MMRSL_ROOT} ${CMAKE_CURRENT_BINARY_DIR}/mmrsl_build)

# ============================================================================
# Shader Compilation
# ============================================================================

find_program(GLSLC_EXECUTABLE glslc 
    PATHS "$ENV{VULKAN_SDK}/Bin" REQUIRED)

set(SHADER_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/shaders)
set(SHADER_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/shaders)

file(MAKE_DIRECTORY ${SHADER_BINARY_DIR})

set(SHADER_SOURCES
    ${SHADER_SOURCE_DIR}/rect.vert
    ${SHADER_SOURCE_DIR}/rect.frag
)

set(SHADER_BINARIES)

foreach(SHADER ${SHADER_SOURCES})
    get_filename_component(FILENAME ${SHADER} NAME)
    set(OUTPUT_FILE ${SHADER_BINARY_DIR}/${FILENAME}.spv)
    
    add_custom_command(
        OUTPUT ${OUTPUT_FILE}
        COMMAND ${GLSLC_EXECUTABLE} ${SHADER} -o ${OUTPUT_FILE}
        DEPENDS ${SHADER}
        COMMENT "Compiling shader ${FILENAME}"
    )
    
    list(APPEND SHADER_BINARIES ${OUTPUT_FILE})
endforeach()

add_custom_target(shaders ALL DEPENDS ${SHADER_BINARIES})

# ============================================================================
# Executable
# ============================================================================

set(SOURCES
    main.cpp
    src/application/platform/window.cpp
    src/rhi/vulkan/vulkan_device.cpp
    src/rhi/vulkan/vulkan_swapchain.cpp
    src/rhi/vulkan/vulkan_pipeline.cpp
    src/rhi/vulkan/vulkan_uniform_buffer.cpp
    src/renderer/renderer.cpp
)

set(HEADERS
    src/core/types.hpp
    src/application/platform/window.hpp
    src/rhi/vulkan/vulkan_device.hpp
    src/rhi/vulkan/vulkan_swapchain.hpp
    src/rhi/vulkan/vulkan_pipeline.hpp
    src/rhi/vulkan/vulkan_uniform_buffer.hpp
    src/renderer/renderer.hpp
)

add_executable(vkapp ${SOURCES} ${HEADERS})

# Organize files in Visual Studio
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} ${HEADERS})

# ============================================================================
# Test Executables
# ============================================================================

set(TEST_SOURCES
    src/application/platform/window.cpp
    src/rhi/vulkan/vulkan_device.cpp
    src/rhi/vulkan/vulkan_swapchain.cpp
    src/rhi/vulkan/vulkan_pipeline.cpp
    src/rhi/vulkan/vulkan_uniform_buffer.cpp
    src/renderer/renderer.cpp
)

set(SCENE_SOURCES
    src/scene/render_object.cpp
    src/scene/scene.cpp
    src/scene/script_loader.cpp
)

# Test: Window
add_executable(test_window tests/test_window.cpp ${TEST_SOURCES})
target_include_directories(test_window PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src ${Vulkan_INCLUDE_DIRS} ${GLFW3_INCLUDE_DIR} ${GLM_ROOT})
target_link_libraries(test_window PRIVATE ${Vulkan_LIBRARIES} ${GLFW3_LIBRARY})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES tests/test_window.cpp ${TEST_SOURCES})

# Test: Device
add_executable(test_device tests/test_device.cpp ${TEST_SOURCES})
target_include_directories(test_device PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src ${Vulkan_INCLUDE_DIRS} ${GLFW3_INCLUDE_DIR} ${GLM_ROOT})
target_link_libraries(test_device PRIVATE ${Vulkan_LIBRARIES} ${GLFW3_LIBRARY})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES tests/test_device.cpp ${TEST_SOURCES})

# Test: Buffer
add_executable(test_buffer tests/test_buffer.cpp ${TEST_SOURCES})
target_include_directories(test_buffer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src ${Vulkan_INCLUDE_DIRS} ${GLFW3_INCLUDE_DIR} ${GLM_ROOT})
target_link_libraries(test_buffer PRIVATE ${Vulkan_LIBRARIES} ${GLFW3_LIBRARY})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES tests/test_buffer.cpp ${TEST_SOURCES})

# Test: Color Rotation / Scene (visual)
add_executable(test_color_rotation tests/test_color_rotation.cpp ${TEST_SOURCES} ${SCENE_SOURCES})
target_include_directories(test_color_rotation PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${Vulkan_INCLUDE_DIRS}
    ${GLFW3_INCLUDE_DIR}
    ${GLM_ROOT}
    ${MMRSL_ROOT}/include
)
target_link_libraries(test_color_rotation PRIVATE ${Vulkan_LIBRARIES} ${GLFW3_LIBRARY} mmrsl)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES tests/test_color_rotation.cpp ${TEST_SOURCES} ${SCENE_SOURCES})
add_dependencies(test_color_rotation shaders)
add_custom_command(TARGET test_color_rotation POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${SHADER_BINARY_DIR} $<TARGET_FILE_DIR:test_color_rotation>/shaders
)
add_custom_command(TARGET test_color_rotation POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/scripts
    $<TARGET_FILE_DIR:test_color_rotation>/scripts
)

# Test: Visual Constant Output
add_executable(test_vis_constant tests/test_vis_constant.cpp ${TEST_SOURCES} ${SCENE_SOURCES})
target_include_directories(test_vis_constant PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${Vulkan_INCLUDE_DIRS}
    ${GLFW3_INCLUDE_DIR}
    ${GLM_ROOT}
    ${MMRSL_ROOT}/include
)
target_link_libraries(test_vis_constant PRIVATE ${Vulkan_LIBRARIES} ${GLFW3_LIBRARY} mmrsl)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES tests/test_vis_constant.cpp ${TEST_SOURCES} ${SCENE_SOURCES})
add_dependencies(test_vis_constant shaders)
add_custom_command(TARGET test_vis_constant POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${SHADER_BINARY_DIR} $<TARGET_FILE_DIR:test_vis_constant>/shaders
)
add_custom_command(TARGET test_vis_constant POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/scripts
    $<TARGET_FILE_DIR:test_vis_constant>/scripts
)

# Test: Visual Symmetry / Instance Independence
add_executable(test_vis_symmetry tests/test_vis_symmetry.cpp ${TEST_SOURCES} ${SCENE_SOURCES})
target_include_directories(test_vis_symmetry PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${Vulkan_INCLUDE_DIRS}
    ${GLFW3_INCLUDE_DIR}
    ${GLM_ROOT}
    ${MMRSL_ROOT}/include
)
target_link_libraries(test_vis_symmetry PRIVATE ${Vulkan_LIBRARIES} ${GLFW3_LIBRARY} mmrsl)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES tests/test_vis_symmetry.cpp ${TEST_SOURCES} ${SCENE_SOURCES})
add_dependencies(test_vis_symmetry shaders)
add_custom_command(TARGET test_vis_symmetry POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${SHADER_BINARY_DIR} $<TARGET_FILE_DIR:test_vis_symmetry>/shaders
)
add_custom_command(TARGET test_vis_symmetry POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/scripts
    $<TARGET_FILE_DIR:test_vis_symmetry>/scripts
)

# Test: Visual Phase Offset
add_executable(test_vis_phase_offset tests/test_vis_phase_offset.cpp ${TEST_SOURCES} ${SCENE_SOURCES})
target_include_directories(test_vis_phase_offset PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${Vulkan_INCLUDE_DIRS}
    ${GLFW3_INCLUDE_DIR}
    ${GLM_ROOT}
    ${MMRSL_ROOT}/include
)
target_link_libraries(test_vis_phase_offset PRIVATE ${Vulkan_LIBRARIES} ${GLFW3_LIBRARY} mmrsl)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES tests/test_vis_phase_offset.cpp ${TEST_SOURCES} ${SCENE_SOURCES})
add_dependencies(test_vis_phase_offset shaders)
add_custom_command(TARGET test_vis_phase_offset POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${SHADER_BINARY_DIR} $<TARGET_FILE_DIR:test_vis_phase_offset>/shaders
)
add_custom_command(TARGET test_vis_phase_offset POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/scripts
    $<TARGET_FILE_DIR:test_vis_phase_offset>/scripts
)

# Test: Visual Stress 16 Instances
add_executable(test_vis_stress_16 tests/test_vis_stress_16.cpp ${TEST_SOURCES} ${SCENE_SOURCES})
target_include_directories(test_vis_stress_16 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${Vulkan_INCLUDE_DIRS}
    ${GLFW3_INCLUDE_DIR}
    ${GLM_ROOT}
    ${MMRSL_ROOT}/include
)
target_link_libraries(test_vis_stress_16 PRIVATE ${Vulkan_LIBRARIES} ${GLFW3_LIBRARY} mmrsl)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES tests/test_vis_stress_16.cpp ${TEST_SOURCES} ${SCENE_SOURCES})
add_dependencies(test_vis_stress_16 shaders)
add_custom_command(TARGET test_vis_stress_16 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${SHADER_BINARY_DIR} $<TARGET_FILE_DIR:test_vis_stress_16>/shaders
)
add_custom_command(TARGET test_vis_stress_16 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/scripts
    $<TARGET_FILE_DIR:test_vis_stress_16>/scripts
)

# test_vis_lissajous: Lissajous figures test (verifies sin/cos compound math)
add_executable(test_vis_lissajous tests/test_vis_lissajous.cpp ${TEST_SOURCES} ${SCENE_SOURCES})
target_include_directories(test_vis_lissajous PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${Vulkan_INCLUDE_DIRS}
    ${GLFW3_INCLUDE_DIR}
    ${GLM_ROOT}
    ${MMRSL_ROOT}/include
)
target_link_libraries(test_vis_lissajous PRIVATE ${Vulkan_LIBRARIES} ${GLFW3_LIBRARY} mmrsl)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES tests/test_vis_lissajous.cpp ${TEST_SOURCES} ${SCENE_SOURCES})
add_dependencies(test_vis_lissajous shaders)
add_custom_command(TARGET test_vis_lissajous POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${SHADER_BINARY_DIR} $<TARGET_FILE_DIR:test_vis_lissajous>/shaders
)
add_custom_command(TARGET test_vis_lissajous POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/scripts
    $<TARGET_FILE_DIR:test_vis_lissajous>/scripts
)
set_property(TARGET test_vis_lissajous PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

target_include_directories(vkapp PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${Vulkan_INCLUDE_DIRS}
    ${GLFW3_INCLUDE_DIR}
    ${GLM_ROOT}
)

target_link_libraries(vkapp PRIVATE
    ${Vulkan_LIBRARIES}
    ${GLFW3_LIBRARY}
)

add_dependencies(vkapp shaders)

# Copy shaders to output directory
add_custom_command(TARGET vkapp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${SHADER_BINARY_DIR} $<TARGET_FILE_DIR:vkapp>/shaders
)

# ============================================================================
# Visual Studio Settings
# ============================================================================

set_property(TARGET vkapp PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
