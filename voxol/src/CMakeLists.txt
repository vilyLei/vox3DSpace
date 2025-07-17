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


# cmake_minimum_required(VERSION 3.0)
# project(ECSVoxoletry)
# set(CMAKE_CXX_STANDARD 20)
# # 找到所有 Voxol/ 目录下的 cpp 文件
# file(GLOB_RECURSE VOXOL_SOURCES Voxol/*.cpp)
# # 添加主程序入口 main.cpp
# set(MAIN_SOURCE main.cpp)
# # 构建目标
# add_executable(voxModule ${MAIN_SOURCE} ${VOXOL_SOURCES})
# # 添加头文件搜索路径
# target_include_directories(voxModule PRIVATE ${CMAKE_SOURCE_DIR}/Voxol)
# # -s USE_WEBGL2=1 -s FULL_ES2=1
# # emscripten wasm 构建选项
# set_target_properties(voxModule PROPERTIES
#     COMPILE_FLAGS "-msimd128"
#     LINK_FLAGS "-msimd128 --bind -s WASM=1 \
#                 -s ALLOW_MEMORY_GROWTH=1 \
#                 -s USE_WEBGL2=1 \
#                 -s MODULARIZE=1 \
#                 -s EXPORTED_FUNCTIONS=['_create_rect','_create_circle','_run_intersection','_get_heap_size_bytes','_get_buffer_ptr','_set_buffer_value','_startup'] \
#                 -s EXPORT_NAME='VoxModule' \
#                 -s EXPORTED_RUNTIME_METHODS=['cwrap','HEAPU8']"
# )


cmake_minimum_required(VERSION 3.17)
project(voxolModule LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 项目子目录（你可以添加更多）
set(PROJECT_DIRS
    Voxol
)

# 所有源文件容器
set(SRC_FILES "")
set(HEADER_FILES "")
set(ALL_GROUPED_FILES "")

# 遍历每个目录
foreach(dir ${PROJECT_DIRS})
    file(GLOB_RECURSE ${dir}_SRC CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${dir}/*.cpp)
    file(GLOB_RECURSE ${dir}_HDR CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${dir}/*.h)

    list(APPEND SRC_FILES ${${dir}_SRC})
    list(APPEND HEADER_FILES ${${dir}_HDR})
    list(APPEND ALL_GROUPED_FILES ${${dir}_SRC} ${${dir}_HDR})

    # 添加到 Visual Studio 中的分组
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/${dir} PREFIX "${dir}" FILES ${${dir}_SRC} ${${dir}_HDR})
endforeach()

# main.cpp 单独添加
set(MAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
list(APPEND ALL_GROUPED_FILES ${MAIN_FILE})
source_group("" FILES ${MAIN_FILE})

# 构建目标
add_executable(voxolModule ${ALL_GROUPED_FILES})

# WebAssembly 编译配置
if(EMSCRIPTEN)
    # set(CMAKE_EXECUTABLE_SUFFIX ".html")
    # target_link_options(voxolModule PRIVATE
    #     "-sUSE_WEBGL2=1"
    #     "-sALLOW_MEMORY_GROWTH=1"
    #     "-sDISABLE_EXCEPTION_CATCHING=0"
    #     "--bind"
    # )
    # target_compile_options(voxolModule PRIVATE
    #     "-sUSE_WEBGL2=1"
    #     "-sALLOW_MEMORY_GROWTH=1"
    #     "-sDISABLE_EXCEPTION_CATCHING=0"
    #     "--bind"
    # )

    set_target_properties(voxolModule PROPERTIES
        COMPILE_FLAGS "-msimd128"
        LINK_FLAGS "-O3 -msimd128 --bind -s WASM=1 \
            -s ALLOW_MEMORY_GROWTH=1 \
            -s USE_WEBGL2=1 \
            -s MODULARIZE=1 \
            -s EXPORTED_FUNCTIONS=['_getCmds','_getCmdsTotal','_get_heap_size_bytes','_get_buffer_ptr','_set_buffer_value','_startup','_setMouseParams'] \
            -s EXPORT_NAME='VoxModule' \
            -s EXPORTED_RUNTIME_METHODS=['cwrap','HEAPU8']"
    )
endif()
