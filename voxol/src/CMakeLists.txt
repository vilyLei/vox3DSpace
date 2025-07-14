# cmake_minimum_required(VERSION 3.0)
# project(MyWasmApp)
# set(CMAKE_CXX_STANDARD 17)
# add_executable(my_app main.cpp)
# # Emscripten-specific options
# set_target_properties(my_app PROPERTIES
#     LINK_FLAGS "-s WASM=1 -s EXPORTED_FUNCTIONS=['_add'] -s EXPORTED_RUNTIME_METHODS=['cwrap']"
# )


# cmake_minimum_required(VERSION 3.0)
# project(ECSVoxoletry)
# set(CMAKE_CXX_STANDARD 17)
# # 编译为 WebAssembly 的目标名
# add_executable(ecs_geom
#     src/main.cpp
# )
# # 设置 emscripten 的链接参数
# set_target_properties(ecs_geom PROPERTIES
#     LINK_FLAGS "-s WASM=1 \
#                 -s ALLOW_MEMORY_GROWTH=1 \
#                 -s EXPORTED_FUNCTIONS=['_create_rect','_create_circle','_run_intersection'] \
#                 -s EXPORTED_RUNTIME_METHODS=['cwrap']"
# )

cmake_minimum_required(VERSION 3.0)
project(ECSVoxoletry)

set(CMAKE_CXX_STANDARD 20)

# 找到所有 Voxol/ 目录下的 cpp 文件
file(GLOB_RECURSE VOXOL_SOURCES Voxol/*.cpp)

# 添加主程序入口 main.cpp
set(MAIN_SOURCE main.cpp)

# 构建目标
add_executable(voxModule ${MAIN_SOURCE} ${VOXOL_SOURCES})

# 添加头文件搜索路径
target_include_directories(voxModule PRIVATE ${CMAKE_SOURCE_DIR}/Voxol)
# -s USE_WEBGL2=1 -s FULL_ES2=1
# emscripten wasm 构建选项
set_target_properties(voxModule PROPERTIES
    COMPILE_FLAGS "-msimd128"
    LINK_FLAGS "-msimd128 --bind -s WASM=1 \
                -s ALLOW_MEMORY_GROWTH=1 \
                -s USE_WEBGL2=1 \
                -s MODULARIZE=1 \
                -s EXPORTED_FUNCTIONS=['_create_rect','_create_circle','_run_intersection','_get_heap_size_bytes','_get_buffer_ptr','_set_buffer_value','_startup'] \
                -s EXPORT_NAME='VoxModule' \
                -s EXPORTED_RUNTIME_METHODS=['cwrap','HEAPU8']"
)