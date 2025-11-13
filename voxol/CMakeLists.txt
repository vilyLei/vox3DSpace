
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

# ================== env path setting ==================
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
set(TILE_DIR "${VOX_DIR}/Tile")
set(INTENT_DIR "${VOX_DIR}/Intent")

set(VOASS_DIR "${SRC_DIR}/Voass")
set(VOASS_RENDER_DIR "${VOASS_DIR}/Render")

set(THIRDPARTY_INCLUDE "${SRC_DIR}/thirdPart/opengl/libx64/include")
set(THIRDPARTY_LIB "${SRC_DIR}/thirdPart/opengl/libx64/lib")
set(JSON_INCLUDE_DIR "${SRC_DIR}/thirdPart/json")

# ================== .h files dir setting ==================
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
  ${TILE_DIR}
  ${INTENT_DIR}

  ${VOASS_RENDER_DIR}

  ${THIRDPARTY_INCLUDE}
  
  ${JSON_INCLUDE_DIR}
)

# ================== .cpp files dir setting ==================
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
file(GLOB TILE_SRC "${TILE_DIR}/*.cpp" "${TILE_DIR}/*.h")
file(GLOB INTENT_SRC "${INTENT_DIR}/*.cpp" "${INTENT_DIR}/*.h")

file(GLOB VOASS_RENDER_SRC "${VOASS_RENDER_DIR}/*.cpp" "${VOASS_RENDER_DIR}/*.h")

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
  ${TILE_SRC}
  ${INTENT_SRC}

  ${VOASS_RENDER_SRC}
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
source_group("Tile" FILES ${TILE_SRC})
source_group("Intent" FILES ${INTENT_SRC})
source_group("VoassRender" FILES ${VOASS_RENDER_SRC})


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
  add_compile_definitions(SRC_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

  add_library(bzip2 STATIC IMPORTED)
  set_target_properties(bzip2 PROPERTIES
    IMPORTED_LOCATION "${SRC_DIR}/thirdPart/bzip2/lib/bz2d.lib"
    INTERFACE_INCLUDE_DIRECTORIES "${SRC_DIR}/thirdPart/bzip2/include"
  )

  add_library(brotlidec STATIC IMPORTED)
  set_target_properties(brotlidec PROPERTIES
    IMPORTED_LOCATION "${SRC_DIR}/thirdPart/brotli/lib/brotlidec.lib"
    INTERFACE_INCLUDE_DIRECTORIES "${SRC_DIR}/thirdPart/brotli/include"
  )

  add_library(brotlicommon STATIC IMPORTED)
  set_target_properties(brotlicommon PROPERTIES
    IMPORTED_LOCATION "${SRC_DIR}/thirdPart/brotli/lib/brotlicommon.lib"
    INTERFACE_INCLUDE_DIRECTORIES "${SRC_DIR}/thirdPart/brotli/include"
  )

  add_library(freetype STATIC IMPORTED)
  set_target_properties(freetype PROPERTIES
    IMPORTED_LOCATION
    "${SRC_DIR}/thirdPart/freetype/lib/freetyped.lib"
    # IMPORTED_LOCATION_DEBUG
    # "${SRC_DIR}/thirdPart/freetype/lib/freetyped.lib"
    # IMPORTED_LOCATION_RELEASE
    # "${SRC_DIR}/thirdPart/freetype/lib/freetyp.lib"
    INTERFACE_INCLUDE_DIRECTORIES
    "${SRC_DIR}/thirdPart/freetype/include"
  )

  add_library(libpng STATIC IMPORTED)
  set_target_properties(libpng PROPERTIES
    IMPORTED_LOCATION "${SRC_DIR}/thirdPart/libpng/lib/libpng16d.lib"
    INTERFACE_INCLUDE_DIRECTORIES "${SRC_DIR}/thirdPart/libpng/include"
  )

  add_library(zlib STATIC IMPORTED)
  set_target_properties(zlib PROPERTIES
    IMPORTED_LOCATION "${SRC_DIR}/thirdPart/zlib/lib/zlibd.lib"
    INTERFACE_INCLUDE_DIRECTORIES "${SRC_DIR}/thirdPart/zlib/include"
  )

  add_executable(voxolModule ${ALL_SOURCES})
  target_link_directories(voxolModule PRIVATE ${THIRDPARTY_LIB})
  target_link_libraries(voxolModule PRIVATE
    opengl32
    glew32d
    glfw3
  )
  target_compile_definitions(voxolModule PRIVATE GLEW_STATIC)
  target_link_libraries(voxolModule PRIVATE freetype bzip2 brotlidec brotlicommon libpng zlib)
endif()
