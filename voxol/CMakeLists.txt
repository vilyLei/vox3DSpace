
# # WebAssembly 编译配置
# if(EMSCRIPTEN)

#     set_target_properties(voxolModule PROPERTIES
#         COMPILE_FLAGS "-msimd128"
#         LINK_FLAGS "-O3 -msimd128 --bind -s WASM=1 \
#             -s ALLOW_MEMORY_GROWTH=1 \
#             -s USE_WEBGL2=1 \
#             -s FULL_ES3=1 \
#             -s MODULARIZE=1 \
#             -s EXPORTED_FUNCTIONS=['_getCmds','_getCmdsTotal','_get_heap_size_bytes','_get_buffer_ptr','_set_buffer_value','_startup','_setMouseParams'] \
#             -s EXPORT_NAME='VoxModule' \
#             -s EXPORTED_RUNTIME_METHODS=['cwrap','HEAPU8']"
#     )
# endif()

cmake_minimum_required(VERSION 3.17)
project(voxolModule)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ================== 路径设置 ==================
set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(VOX_DIR "${SRC_DIR}/Voxol")
set(RENDER_DIR "${VOX_DIR}/Render")
set(MATH_DIR "${VOX_DIR}/Math")
set(BASE_DIR "${VOX_DIR}/Base")
set(SYSTEM_DIR "${VOX_DIR}/System")
set(CORE_DIR "${VOX_DIR}/Core")
set(MOTION_DIR "${VOX_DIR}/Motion")
set(PHYSICS_DIR "${VOX_DIR}/Physics")
set(UTIL_DIR "${VOX_DIR}/Util")
set(TEST_DIR "${VOX_DIR}/Test")
set(TASK_DIR "${VOX_DIR}/Task")
set(THIRDPARTY_INCLUDE "${SRC_DIR}/thirdPart/opengl/libx64/include")
set(THIRDPARTY_LIB "${SRC_DIR}/thirdPart/opengl/libx64/lib")

# ================== 包含头文件 ==================
include_directories(
  ${VOX_DIR}
  ${RENDER_DIR}
  ${MATH_DIR}
  ${BASE_DIR}
  ${SYSTEM_DIR}
  ${CORE_DIR}
  ${MOTION_DIR}
  ${PHYSICS_DIR}
  ${UTIL_DIR}
  ${TEST_DIR}
  ${TASK_DIR}
  ${THIRDPARTY_INCLUDE}
)

# ================== 源文件收集 ==================
file(GLOB RENDER_SRC "${RENDER_DIR}/*.cpp" "${RENDER_DIR}/*.h")
file(GLOB MATH_SRC "${MATH_DIR}/*.cpp" "${MATH_DIR}/*.h")
file(GLOB BASE_SRC "${BASE_DIR}/*.cpp" "${BASE_DIR}/*.h")
file(GLOB SYSTEM_SRC "${SYSTEM_DIR}/*.cpp" "${SYSTEM_DIR}/*.h")
file(GLOB CORE_SRC "${CORE_DIR}/*.cpp" "${CORE_DIR}/*.h")
file(GLOB MOTION_SRC "${MOTION_DIR}/*.cpp" "${MOTION_DIR}/*.h")
file(GLOB PHYSICS_SRC "${PHYSICS_DIR}/*.cpp" "${PHYSICS_DIR}/*.h")
file(GLOB UTIL_SRC "${UTIL_DIR}/*.cpp" "${UTIL_DIR}/*.h")
file(GLOB TEST_SRC "${TEST_DIR}/*.cpp" "${TEST_DIR}/*.h")
file(GLOB TASK_SRC "${TASK_DIR}/*.cpp" "${TASK_DIR}/*.h")

set(ALL_SOURCES
  ${RENDER_SRC}
  ${MATH_SRC}
  ${BASE_SRC}
  ${SYSTEM_SRC}
  ${CORE_SRC}
  ${MOTION_SRC}
  ${PHYSICS_SRC}
  ${UTIL_SRC}
  ${TEST_SRC}
  ${TASK_SRC}
)

source_group("Render" FILES ${RENDER_SRC})
source_group("Math" FILES ${MATH_SRC})
source_group("Base" FILES ${BASE_SRC})
source_group("System" FILES ${SYSTEM_SRC})
source_group("Core" FILES ${CORE_SRC})
source_group("Motion" FILES ${MOTION_SRC})
source_group("Physics" FILES ${PHYSICS_SRC})
source_group("Util" FILES ${UTIL_SRC})
source_group("Test" FILES ${TEST_SRC})
source_group("Task" FILES ${TASK_SRC})


if(CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
  set(MAIN_FILE ${SRC_DIR}/main_wasm.cpp)
  message(STATUS "apply wasm main")
else()
  message(STATUS "apply opengl main")
  set(MAIN_FILE ${SRC_DIR}/main_ogl.cpp)
endif()

list(APPEND ALL_SOURCES ${MAIN_FILE})
source_group("" FILES ${MAIN_FILE})

if(EMSCRIPTEN)
  # ---------- WebAssembly 构建 ----------
  message(STATUS "Target: WebAssembly (WebGL2)")

  # set(CMAKE_EXECUTABLE_SUFFIX ".html") # 生成html + wasm + js

  add_executable(voxolModule ${ALL_SOURCES})

  # set_target_properties(voxolModule PROPERTIES OUTPUT_NAME "index")
  # # WebGL2 / canvas / 文件系统配置等
  # target_link_options(voxolModule PRIVATE
  #     "SHELL:-sUSE_WEBGL2=1"
  #     "SHELL:-sALLOW_MEMORY_GROWTH=1"
  #     "SHELL:-sEXPORTED_FUNCTIONS=['_main']"
  #     "SHELL:-sEXPORTED_RUNTIME_METHODS=['ccall','cwrap']"
  #     "SHELL:--preload-file assets"
  #     "SHELL:--bind"
  # )
  set_target_properties(voxolModule PROPERTIES
    COMPILE_FLAGS "-msimd128"
    LINK_FLAGS "-O3 -msimd128 --bind -s WASM=1 \
            -s ALLOW_MEMORY_GROWTH=1 \
            -s USE_WEBGL2=1 \
            -s FULL_ES3=1 \
            -s MODULARIZE=1 \
            -s EXPORTED_FUNCTIONS=['_getCmds','_getCmdsTotal','_get_heap_size_bytes','_get_buffer_ptr','_set_buffer_value','_startup','_setMouseParams'] \
            -s EXPORT_NAME='VoxModule' \
            -s EXPORTED_RUNTIME_METHODS=['cwrap','HEAPU8']"
  )
else()
  # ---------- 本地桌面 OpenGL 构建 ----------
  message(STATUS "Target: Native (OpenGL)")

  add_compile_definitions(NATIVE_RUNTIME=1)
  
  add_executable(voxolModule ${ALL_SOURCES})
  target_link_directories(voxolModule PRIVATE ${THIRDPARTY_LIB})
  target_link_libraries(voxolModule PRIVATE
    opengl32
    glew32d
    glfw3
  )
  target_compile_definitions(voxolModule PRIVATE GLEW_STATIC)
endif()
