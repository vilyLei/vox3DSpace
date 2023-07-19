cmake_minimum_required(VERSION 3.21 FATAL_ERROR)
project(vox3dspace C CXX)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# set(VOX3DSPACE_VERSION 1.01)

set(vox3dspace_root "${CMAKE_CURRENT_SOURCE_DIR}")
set(vox3dspace_src_root "${vox3dspace_root}/src")
set(vox3dspace_build "${CMAKE_BINARY_DIR}")

if("${vox3dspace_root}" STREQUAL "${vox3dspace_build}")
  message(
    FATAL_ERROR "Building from within the Vox3DSpace source tree is not supported.\n"
                "Hint: Run these commands\n"
                "$ rm -rf CMakeCache.txt CMakeFiles\n"
                "$ mkdir -p ../vox3dspace_build\n" "$ cd ../vox3dspace_build\n"
                "And re-run CMake from the vox3dspace_build directory.")
endif()

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
      vox3dspace_data_stream_sources
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
  vox3dspace_math_sources
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

#
# Vox3DSpace targets.
#
set(EXEC_BUILD_FLAG true)
if(EXEC_BUILD_FLAG)
  list(APPEND vox3dspace_engine_base_src
    ${vox3dspace_data_stream_sources}
    ${vox3dspace_math_sources}
  )
  # Standard Vox3DSpace libs, encoder and decoder. Object collections that mirror the
  # Vox3DSpace directory structure.
  vox3dspace_add_library(NAME vox3dspace_engine_base TYPE OBJECT SOURCES
                    ${vox3dspace_engine_base_src}
                    DEFINES
                    ${vox3dspace_defines}
                    )

  
  list(APPEND vox3dspace_engine_library_deps
  vox3dspace_engine_base)
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
                       vox3dspace_engine
                       SOURCES
                       "${vox3dspace_src_root}/voxengine/demo/vox3dspace_engine.cc"
                       DEFINES
                       ${vox3dspace_defines}
                       INCLUDES
                       ${vox3dspace_include_paths}
                       LIB_DEPS
                       ${vox3dspace_engine_library_deps})

  # vox3dspace_setup_install_target()
  # vox3dspace_setup_test_targets()
endif()

if(VOX3DSPACE_VERBOSE)
  vox3dspace_dump_cmake_flag_variables()
  vox3dspace_dump_tracked_configuration_variables()
  # vox3dspace_dump_options()
endif()
