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

add_executable(vkapp 
    main.cpp
    src/application/platform/window.cpp
    src/rhi/vulkan/vulkan_device.cpp
    src/rhi/vulkan/vulkan_swapchain.cpp
    src/rhi/vulkan/vulkan_pipeline.cpp
    src/rhi/vulkan/vulkan_uniform_buffer.cpp
    src/renderer/renderer.cpp
)

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
