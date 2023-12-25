cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

include(CheckLanguage)
check_language(CUDA)
if(NOT DEFINED CMAKE_CUDA_STANDARD)
  project(vox3dspace C CXX)
else()
  project(vox3dspace C CXX CUDA)
endif()


# thanks: https://learn.microsoft.com/en-us/cpp/build/reference/zc-cplusplus?view=msvc-170
# thanks: https://cmake.org/cmake/help/latest/prop_gbl/CMAKE_CXX_KNOWN_FEATURES.html#prop_gbl:CMAKE_CXX_KNOWN_FEATURES
set(CXX_STD_VERSION cxx_std_20)

if(NOT CMAKE_BUILD_TYPE)
  # set(CMAKE_BUILD_TYPE Release)
  set(CMAKE_BUILD_TYPE Debug)
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# set(VOX3DSPACE_VERSION 1.0.1)

message(STATUS "BOOST_ROOT_DIR: $ENV{BOOST_ROOT_DIR}")

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
# find cuda sdk
if(DEFINED CUDA_TOOLKIT_ROOT_DIR)
find_package(CUDA)
endif()
# find_package(CUDAToolkit REQUIRED)

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
include("${vox3dspace_root}/cmake/vox3dspace_sources.cmake")
include("${vox3dspace_root}/cmake/vox3dspace_libraries.cmake")
include("${vox3dspace_root}/cmake/vox3dspace_executable_project.cmake")
include("${vox3dspace_root}/cmake/vox3dspace_executable_dx12dxrDemos.cmake")
include("${vox3dspace_root}/cmake/vox3dspace_executable_vkDemos.cmake")
include("${vox3dspace_root}/cmake/vox3dspace_executable_oglDemos.cmake")
include("${vox3dspace_root}/cmake/vox3dspace_executable_multiprocessDemos.cmake")
include("${vox3dspace_root}/cmake/vox3dspace_executable_hpcDemos.cmake")

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

message(STATUS "CMAKE_GENERATOR: ${CMAKE_GENERATOR}")
message(STATUS "CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}")
message(STATUS "PROJECT_BINARY_DIR: ${PROJECT_BINARY_DIR}")
message(STATUS "Vulkan_ROOT_DIR: ${Vulkan_ROOT_DIR}")
# set( ENV{PATH} /engine/core )
message(STATUS "CMAKE_CUDA_ARCHITECTURES: ${CMAKE_CUDA_ARCHITECTURES}")
# set(CMAKE_CUDA_ARCHITECTURES 50-virtual)
set(CMAKE_CUDA_ARCHITECTURES 50 52 60 61)
# string(APPEND CMAKE_CUDA_FLAGS " -gencode arch=compute_50,code=sm_50")
if($ENV{VOX_ENGINE_ROOT_DIR})
  message(STATUS "find engine root dir.")
else()
  message(STATUS "find not engine root dir.")
endif()
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

build_projects_sources()

# Vox3DSpace targets.
#
set(EXEC_BUILD_FLAG true)
if(EXEC_BUILD_FLAG)
  build_libraries()
  # # Library targets that consume the object collections.
  build_executable_projects()
  build_executable_dx12dxrDemos()
  build_executable_vkDemos()
  build_executable_oglDemos()
  build_executable_multprocessDemos()
  build_executable_hpcDemos()
  # vox3dspace_setup_install_target()
  # vox3dspace_setup_test_targets()
endif()

if(VOX3DSPACE_VERBOSE)
  vox3dspace_dump_cmake_flag_variables()
  vox3dspace_dump_tracked_configuration_variables()
  # vox3dspace_dump_options()
endif()
add_subdirectory("${vox3dspace_root}/cmakeSubTest")