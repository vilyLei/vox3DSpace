cmake_minimum_required(VERSION 3.21 FATAL_ERROR)
project(vox3dspace C CXX)

if(NOT CMAKE_BUILD_TYPE)
  # set(CMAKE_BUILD_TYPE Release)
  set(CMAKE_BUILD_TYPE Debug)
endif()

# set(VOX3DSPACE_VERSION 1.0.1)

set(vox3dspace_root "${CMAKE_CURRENT_SOURCE_DIR}")
set(vox3dspace_src_root "${vox3dspace_root}/src")
set(vox3dspace_libs_root "${vox3dspace_root}/libs")
set(vox3dspace_build "${CMAKE_BINARY_DIR}")

if("${vox3dspace_root}" STREQUAL "${vox3dspace_build}")
  message(
    FATAL_ERROR "Building from within the Vox3DSpace source tree is not supported.\n"
                "Hint: Run these commands\n"
                "$ rm -rf CMakeCache.txt CMakeFiles\n"
                "$ mkdir -p ../vox3dspace_build\n" "$ cd ../vox3dspace_build\n"
                "And re-run CMake from the vox3dspace_build directory.")
endif()
# find vulkan sdk
find_package(Vulkan)
# message("--- 2 Vulkan_FOUND: ${Vulkan_FOUND}")
# message("--- 2 Vulkan: ${Vulkan}")
# message("--- 2 Vulkan_LIBRARY: ${Vulkan_LIBRARY}")
# message("--- 2 Vulkan_INCLUDE_DIR: ${Vulkan_INCLUDE_DIR}")
message("--- 3 PROJECT_BINARY_DIR: ${PROJECT_BINARY_DIR}")
message("--- 3 CMAKE_CURRENT_BINARY_DIR: ${CMAKE_CURRENT_BINARY_DIR}")

# if(WIN32)
# 	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_USE_PLATFORM_WIN32_KHR")
# endif()

find_package(PythonInterp REQUIRED)

include(FindPythonInterp)
include("${vox3dspace_root}/cmake/vox3dspace_build_definitions.cmake")
include("${vox3dspace_root}/cmake/vox3dspace_helpers.cmake")
include("${vox3dspace_root}/cmake/vox3dspace_install.cmake")
include("${vox3dspace_root}/cmake/vox3dspace_intrinsics.cmake")
include("${vox3dspace_root}/cmake/vox3dspace_flags.cmake")
include("${vox3dspace_root}/cmake/vox3dspace_sanitizer.cmake")
include("${vox3dspace_root}/cmake/vox3dspace_targets.cmake")
include("${vox3dspace_root}/cmake/vox3dspace_variables.cmake")

execute_process(
  COMMAND
      ${PYTHON_EXECUTABLE} "-c" "print('Hello, world!')"
  RESULT_VARIABLE _status
  OUTPUT_VARIABLE _hello_world
  ERROR_QUIET
  OUTPUT_STRIP_TRAILING_WHITESPACE
  )
message(STATUS "RESULT_VARIABLE is: ${_status}")
message(STATUS "OUTPUT_VARIABLE is: ${_hello_world}")
# C++ and linker flags.
vox3dspace_track_configuration_variable(VOX3DSPACE_CXX_FLAGS)
vox3dspace_track_configuration_variable(VOX3DSPACE_EXE_LINKER_FLAGS)

# Sanitizer integration.
vox3dspace_track_configuration_variable(VOX3DSPACE_SANITIZE)

# Generated source file directory.
vox3dspace_track_configuration_variable(VOX3DSPACE_GENERATED_SOURCES_DIRECTORY)

# Controls use of std::mutex and absl::Mutex in ThreadPool.
vox3dspace_track_configuration_variable(VOX3DSPACE_THREADPOOL_USE_STD_MUTEX)

if(VOX3DSPACE_VERBOSE)
  vox3dspace_dump_cmake_flag_variables()
  vox3dspace_dump_tracked_configuration_variables()
  # vox3dspace_dump_options()
endif()

# Compiler/linker flags must be lists, but come in from the environment as
# strings. Break them up:
if(NOT "${VOX3DSPACE_CXX_FLAGS}" STREQUAL "")
  separate_arguments(VOX3DSPACE_CXX_FLAGS)
endif()
if(NOT "${VOX3DSPACE_EXE_LINKER_FLAGS}" STREQUAL "")
  separate_arguments(VOX3DSPACE_EXE_LINKER_FLAGS)
endif()

vox3dspace_reset_target_lists()
# vox3dspace_setup_options()
vox3dspace_set_build_definitions()
vox3dspace_set_cxx_flags()
vox3dspace_set_exe_linker_flags()
# vox3dspace_generate_features_h()

# Vox3DSpace source file listing variables.
list(
  APPEND
    vox3dspace_common_base_sources
    "${vox3dspace_src_root}/common/base/commonDef.h"
    "${vox3dspace_src_root}/common/base/commonType.h"
    "${vox3dspace_src_root}/common/base/preCompileDef.cc"
    "${vox3dspace_src_root}/common/base/preCompileDef.h"
  )
list(
  APPEND
    vox3dspace_common_math_sources
    "${vox3dspace_src_root}/common/math/Box.cc"
    "${vox3dspace_src_root}/common/math/Box.h"
    "${vox3dspace_src_root}/common/math/half.h"
    "${vox3dspace_src_root}/common/math/mat2.h"
    "${vox3dspace_src_root}/common/math/mat3.h"
    "${vox3dspace_src_root}/common/math/mat4.h"
    "${vox3dspace_src_root}/common/math/math.cc"
    "${vox3dspace_src_root}/common/math/math.h"
    "${vox3dspace_src_root}/common/math/mathutil.cc"
    "${vox3dspace_src_root}/common/math/mathutil.h"
    "${vox3dspace_src_root}/common/math/mdefine.h"
    "${vox3dspace_src_root}/common/math/Plane.cc"
    "${vox3dspace_src_root}/common/math/Plane.h"
    "${vox3dspace_src_root}/common/math/quat.h"
    "${vox3dspace_src_root}/common/math/Ray.h"
    "${vox3dspace_src_root}/common/math/scalar.h"
    "${vox3dspace_src_root}/common/math/Sphere.h"
    "${vox3dspace_src_root}/common/math/TMatHelpers.h"
    "${vox3dspace_src_root}/common/math/TQuatHelpers.h"
    "${vox3dspace_src_root}/common/math/TVecHelpers.h"
    "${vox3dspace_src_root}/common/math/vec2.h"
    "${vox3dspace_src_root}/common/math/vec3.h"
    "${vox3dspace_src_root}/common/math/vec4.h"
  )
list(
    APPEND
      vox3dspace_engine_data_stream_sources
      "${vox3dspace_src_root}/voxengine/data/stream/streamDef.h"
      "${vox3dspace_src_root}/voxengine/data/stream/ArrayBuffer.cc"
      "${vox3dspace_src_root}/voxengine/data/stream/ArrayBuffer.h"
      "${vox3dspace_src_root}/voxengine/data/stream/DataView.cc"
      "${vox3dspace_src_root}/voxengine/data/stream/DataView.h"
      "${vox3dspace_src_root}/voxengine/data/stream/Float32Array.cc"
      "${vox3dspace_src_root}/voxengine/data/stream/Float32Array.h"
      "${vox3dspace_src_root}/voxengine/data/stream/Int8Array.cc"
      "${vox3dspace_src_root}/voxengine/data/stream/Int8Array.h"
      "${vox3dspace_src_root}/voxengine/data/stream/Int16Array.cc"
      "${vox3dspace_src_root}/voxengine/data/stream/Int16Array.h"
      "${vox3dspace_src_root}/voxengine/data/stream/Int32Array.cc"
      "${vox3dspace_src_root}/voxengine/data/stream/Int32Array.h"
      "${vox3dspace_src_root}/voxengine/data/stream/Uint8Array.cc"
      "${vox3dspace_src_root}/voxengine/data/stream/Uint8Array.h"
      "${vox3dspace_src_root}/voxengine/data/stream/Uint16Array.cc"
      "${vox3dspace_src_root}/voxengine/data/stream/Uint16Array.h"
      "${vox3dspace_src_root}/voxengine/data/stream/Uint32Array.cc"
      "${vox3dspace_src_root}/voxengine/data/stream/Uint32Array.h"
      "${vox3dspace_src_root}/voxengine/data/stream/streamDemo.h"
    )    
list(
  APPEND
    vox3dspace_engine_data_ctm_sources
    "${vox3dspace_src_root}/voxengine/data/ctm/ctmDef.h"
    "${vox3dspace_src_root}/voxengine/data/ctm/ctmUtils.h"
    "${vox3dspace_src_root}/voxengine/data/ctm/ctmDemo.h"
    "${vox3dspace_src_root}/voxengine/data/ctm/CTM.cc"
    "${vox3dspace_src_root}/voxengine/data/ctm/CTM.h"
    "${vox3dspace_src_root}/voxengine/data/ctm/CTMFile.cc"
    "${vox3dspace_src_root}/voxengine/data/ctm/CTMFile.h"
    "${vox3dspace_src_root}/voxengine/data/ctm/CTMFileBody.cc"
    "${vox3dspace_src_root}/voxengine/data/ctm/CTMFileBody.h"
    "${vox3dspace_src_root}/voxengine/data/ctm/CTMStream.cc"
    "${vox3dspace_src_root}/voxengine/data/ctm/CTMStream.h"
    "${vox3dspace_src_root}/voxengine/data/ctm/FileHeader.cc"
    "${vox3dspace_src_root}/voxengine/data/ctm/FileHeader.h"
    "${vox3dspace_src_root}/voxengine/data/ctm/FileMG2Header.cc"
    "${vox3dspace_src_root}/voxengine/data/ctm/FileMG2Header.h"
    "${vox3dspace_src_root}/voxengine/data/ctm/ICTMRStream.h"
    "${vox3dspace_src_root}/voxengine/data/ctm/ICTMWStream.h"
    "${vox3dspace_src_root}/voxengine/data/ctm/IDecoder.h"
    "${vox3dspace_src_root}/voxengine/data/ctm/InterleavedStream.cc"
    "${vox3dspace_src_root}/voxengine/data/ctm/InterleavedStream.h"
    "${vox3dspace_src_root}/voxengine/data/ctm/IReaderHeader.h"
    "${vox3dspace_src_root}/voxengine/data/ctm/LZMA.cc"
    "${vox3dspace_src_root}/voxengine/data/ctm/LZMA.h"
    "${vox3dspace_src_root}/voxengine/data/ctm/OutWindow.cc"
    "${vox3dspace_src_root}/voxengine/data/ctm/OutWindow.h"
    "${vox3dspace_src_root}/voxengine/data/ctm/RangeDecoder.cc"
    "${vox3dspace_src_root}/voxengine/data/ctm/RangeDecoder.h"
    "${vox3dspace_src_root}/voxengine/data/ctm/ReaderMG1.cc"
    "${vox3dspace_src_root}/voxengine/data/ctm/ReaderMG1.h"
    "${vox3dspace_src_root}/voxengine/data/ctm/ReaderMG2.cc"
    "${vox3dspace_src_root}/voxengine/data/ctm/ReaderMG2.h"
    "${vox3dspace_src_root}/voxengine/data/ctm/ReaderRAW.cc"
    "${vox3dspace_src_root}/voxengine/data/ctm/ReaderRAW.h"
  )

list(
  APPEND
  vox3dspace_engine_math_sources
    "${vox3dspace_src_root}/voxengine/math/mathDefine.h"
    "${vox3dspace_src_root}/voxengine/math/Float.h"
    "${vox3dspace_src_root}/voxengine/math/vec3.cc"
    "${vox3dspace_src_root}/voxengine/math/vec3.h"
    "${vox3dspace_src_root}/voxengine/math/EulerOrder.h"
    "${vox3dspace_src_root}/voxengine/math/IEuler.h"
    "${vox3dspace_src_root}/voxengine/math/Euler.cc"
    "${vox3dspace_src_root}/voxengine/math/Euler.h"
    "${vox3dspace_src_root}/voxengine/math/IMatrix4.h"
    "${vox3dspace_src_root}/voxengine/math/Matrix4.cc"
    "${vox3dspace_src_root}/voxengine/math/Matrix4.h"
    "${vox3dspace_src_root}/voxengine/math/OBB.cc"
    "${vox3dspace_src_root}/voxengine/math/OBB.h"
  )
list(
  APPEND
  vox3dspace_engine_text_sources
    "${vox3dspace_src_root}/voxengine/text/BaseText.cc"
    "${vox3dspace_src_root}/voxengine/text/BaseText.h"
)
list(
  APPEND
  vox3dspace_engine_view_sources
    "${vox3dspace_src_root}/voxengine/view/Camera.cc"
    "${vox3dspace_src_root}/voxengine/view/Camera.h"
)
list(
  APPEND
  vox3dspace_profile_sources
    "${vox3dspace_src_root}/profile/log/color/consoleColor.cc"
    "${vox3dspace_src_root}/profile/log/color/consoleColor.h"
    "${vox3dspace_src_root}/profile/log/def/dllmain.cc"
    "${vox3dspace_src_root}/profile/log/def/framework.h"
    "${vox3dspace_src_root}/profile/log/def/pch.cc"
    "${vox3dspace_src_root}/profile/log/def/pch.h"
)
#
# Vox3DSpace targets.
#
set(EXEC_BUILD_FLAG true)
if(EXEC_BUILD_FLAG)

  # list(APPEND vox3dspace_engine_base_src
  #   ${vox3dspace_data_stream_sources}
  #   ${vox3dspace_math_sources}
  # )
  # Standard Vox3DSpace libs, encoder and decoder. Object collections that mirror the
  # Vox3DSpace directory structure.
  
  vox3dspace_add_library(NAME vox3dspace_common_base TYPE OBJECT SOURCES
                    ${vox3dspace_common_base_sources}
                    DEFINES
                    ${vox3dspace_defines}
                    )
  vox3dspace_add_library(NAME vox3dspace_common_math TYPE OBJECT SOURCES
                    ${vox3dspace_common_math_sources}
                    DEFINES
                    ${vox3dspace_defines}
                    LIB_DEPS
                    ${vox3dspace_common_base}
                    )
  vox3dspace_add_library(NAME vox3dspace_engine_data_stream TYPE OBJECT SOURCES
                    ${vox3dspace_engine_data_stream_sources}
                    DEFINES
                    ${vox3dspace_defines}
                    )
  vox3dspace_add_library(NAME vox3dspace_engine_data_ctm TYPE OBJECT SOURCES
                    ${vox3dspace_engine_data_ctm_sources}
                    DEFINES
                    ${vox3dspace_defines}
                    )
  vox3dspace_add_library(NAME vox3dspace_engine_math TYPE OBJECT SOURCES
                    ${vox3dspace_engine_math_sources}
                    DEFINES
                    ${vox3dspace_defines}
                    )
  vox3dspace_add_library(NAME vox3dspace_engine_text TYPE OBJECT SOURCES
                    ${vox3dspace_engine_text_sources}
                    DEFINES
                    ${vox3dspace_defines}
                    )
  vox3dspace_add_library(NAME vox3dspace_engine_view TYPE OBJECT SOURCES
                    ${vox3dspace_engine_view_sources}
                    DEFINES
                    ${vox3dspace_defines}
                    )
  vox3dspace_add_library(NAME vox3dspace_profile TYPE SHARED SOURCES
                    ${vox3dspace_profile_sources}
                    DEFINES
                    ${vox3dspace_defines}
                    )
  # vox3dspace_add_library(NAME vox3dspace_engine_base TYPE OBJECT SOURCES
  #                   ${vox3dspace_engine_base_src}
  #                   DEFINES
  #                   ${vox3dspace_defines}
  #                   )

  
  # list(APPEND vox3dspace_engine_library_deps
  #   vox3dspace_engine_base
  #   vox3dspace_common_math
  #   )
  list(APPEND vox3dspace_engine_library_deps
    vox3dspace_engine_data_stream
    vox3dspace_engine_math
    vox3dspace_common_math
    vox3dspace_engine_text
    vox3dspace_engine_view
    )
  list(APPEND vox3dspace_ctm_library_deps
    vox3dspace_engine_data_stream
    vox3dspace_engine_data_ctm
    )
  # Library targets that consume the object collections.
  if(MSVC)
    # In order to produce a DLL and import library the Windows tools require
    # that the exported symbols are part of the DLL target. The unfortunate side
    # effect of this is that a single configuration cannot output both the
    # static library and the DLL: This results in an either/or situation.
    # Windows users of the vox3dspace build can have a DLL and an import library, or
    # they can have a static library; they cannot have both from a single
    # configuration of the build.
    if(BUILD_SHARED_LIBS)
      set(vox3dspace_lib_type SHARED)
    else()
      set(vox3dspace_lib_type STATIC)
    endif()
  endif()
  # Vox3DSpace app targets.
  vox3dspace_add_executable(NAME
                       demo_vox3dspace_engine
                       SOURCES
                       "${vox3dspace_root}/demos/voxengine/voxengineTest.cc"
                       DEFINES
                       ${vox3dspace_defines}
                       INCLUDES
                       ${vox3dspace_include_paths}
                       LIB_DEPS
                       ${vox3dspace_engine_library_deps})
  # ctm demo
  vox3dspace_add_executable(NAME
                       demo_ctm
                       SOURCES
                       "${vox3dspace_root}/demos/ctm/ctmTest.cc"
                       DEFINES
                       ${vox3dspace_defines}
                       INCLUDES
                       ${vox3dspace_include_paths}
                       LIB_DEPS
                       ${vox3dspace_ctm_library_deps})
  # json demo
  vox3dspace_add_executable(NAME
                       demo_json
                       SOURCES
                       "${vox3dspace_root}/demos/json/jsonTest.cc"
                       DEFINES
                       ${vox3dspace_defines}
                       INCLUDES
                       ${vox3dspace_libs_include_paths}
                       LIB_INCLUDES
                       ${vox3dspace_libs_paths}
                       LIB_DEPS
                       ${vox3dspace_json_dependency})
  # tiny obj demo
  vox3dspace_add_executable(NAME
                       demo_tinyObj
                       SOURCES
                       "${vox3dspace_root}/demos/tinyObj/tinyObjTest.cc"
                       DEFINES
                       ${vox3dspace_defines}
                       INCLUDES
                       ${vox3dspace_libs_include_paths}
                       LIB_INCLUDES
                       ${vox3dspace_libs_paths}
                       LIB_DEPS
                       ${vox3dspace_tinyObj_dependency})
  # profile log dll demo
  vox3dspace_add_executable(NAME
                       demo_profile_log
                       SOURCES
                       "${vox3dspace_root}/demos/profile/logerTest.cc"
                       DEFINES
                       ${vox3dspace_defines}
                       INCLUDES
                       ${vox3dspace_include_paths}
                       LIB_DEPS
                       ${vox3dspace_profile_dependency})
  
  # ogl demo
  vox3dspace_add_executable(NAME
                       demo_graphics_opengl
                       SOURCES
                       "${vox3dspace_root}/demos/opengl/oglTest.cc"
                       "${vox3dspace_root}/demos/opengl/player.cc"
                       "${vox3dspace_root}/demos/opengl/player.h"
                       DEFINES
                       ${vox3dspace_defines}
                       INCLUDES
                       ${vox3dspace_opengl_libs_include_paths}
                       LIB_INCLUDES
                       ${vox3dspace_opengl_libs_paths}
                       LIB_DEPS
                       ${vox3dspace_opengl_dependency})
  # vulkan demo
  vox3dspace_add_executable(NAME
                       demo_graphics_vulkan
                       SOURCES
                       "${vox3dspace_root}/demos/vulkan/vulkanTest.cc"
                       DEFINES
                       ${vox3dspace_defines}
                       INCLUDES
                       ${vox3dspace_vulkan_libs_include_paths}
                       LIB_INCLUDES
                       ${vox3dspace_vulkan_libs_paths}
                       LIB_DEPS
                       ${vox3dspace_vulkan_dependency})
	if(WIN32)
  # dx11 demo
  vox3dspace_add_executable(NAME
                       demo_graphics_d3dx11
                       SOURCES
                       "${vox3dspace_root}/demos/directx11/d3dApp.cc"
                       "${vox3dspace_root}/demos/directx11/d3dApp.h"
                       "${vox3dspace_root}/demos/directx11/d3dUtil.cc"
                       "${vox3dspace_root}/demos/directx11/d3dUtil.h"
                       "${vox3dspace_root}/demos/directx11/DXTrace.cc"
                       "${vox3dspace_root}/demos/directx11/DXTrace.h"
                       "${vox3dspace_root}/demos/directx11/GameApp.cc"
                       "${vox3dspace_root}/demos/directx11/GameApp.h"
                       "${vox3dspace_root}/demos/directx11/GameTimer.cc"
                       "${vox3dspace_root}/demos/directx11/GameTimer.h"
                       "${vox3dspace_root}/demos/directx11/Main.cc"
                       DEFINES
                       ${vox3dspace_defines}
                       EXEC_SYS
                       "WIN32"
                       LINK_FLAGS
                       "-mwindows"
                       DEFINES
                       "-DUNICODE"
                       "-D_UNICODE"
                       LIB_DEPS
                       ${vox3dspace_dx11_dependency})
  # dx12 demo, warn: copy shaders.hlsl file to debug dir
  vox3dspace_add_executable(NAME
											demo_graphics_d3dx12
											SOURCES
											"${vox3dspace_root}/demos/directx12/D3D12HelloTriangle.cc"
											"${vox3dspace_root}/demos/directx12/D3D12HelloTriangle.h"
											"${vox3dspace_root}/demos/directx12/d3dx12.h"
											"${vox3dspace_root}/demos/directx12/DXSample.cc"
											"${vox3dspace_root}/demos/directx12/DXSample.h"
											"${vox3dspace_root}/demos/directx12/DXSampleHelper.h"
											"${vox3dspace_root}/demos/directx12/Main.cc"
											"${vox3dspace_root}/demos/directx12/stdafx.cc"
											"${vox3dspace_root}/demos/directx12/stdafx.h"
											"${vox3dspace_root}/demos/directx12/Win32Application.h"
											"${vox3dspace_root}/demos/directx12/Win32Application.cc"
											DEFINES
											${vox3dspace_defines}
											EXEC_SYS
											"WIN32"
											LINK_FLAGS
											"-mwindows"
											DEFINES
											"-DUNICODE"
											"-D_UNICODE"
											LIB_DEPS
											${vox3dspace_dx12_dependency})
	#
	file(COPY "${vox3dspace_root}/demos/directx12/shaders.hlsl" DESTINATION "${PROJECT_BINARY_DIR}/Debug")
	endif()
  # cuda demo
  # vox3dspace_setup_install_target()
  # vox3dspace_setup_test_targets()
endif()

if(VOX3DSPACE_VERBOSE)
  vox3dspace_dump_cmake_flag_variables()
  vox3dspace_dump_tracked_configuration_variables()
  # vox3dspace_dump_options()
endif()
