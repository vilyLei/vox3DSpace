if(VOX3DSPACE_CMAKE_VOX3DSPACE_BUILD_DEFINITIONS_CMAKE_)
  return()
endif() # VOX3DSPACE_CMAKE_VOX3DSPACE_BUILD_DEFINITIONS_CMAKE_
set(VOX3DSPACE_CMAKE_VOX3DSPACE_BUILD_DEFINITIONS_CMAKE_ 1)

# Utility for controlling the main vox3dspace library dependency. This changes in
# shared builds, and when an optional target requires a shared library build.
macro(set_vox3dspace_target)
  message("--- set_vox3dspace_target(), MSVC: ${MSVC}")
  if(MSVC)
    set(vox3dspace_dependency vox3dspace)
    set(vox3dspace_json_dependency jsoncpp_static)
    set(vox3dspace_profile_dependency vox3dspace_profile)
    set(vox3dspace_tinyObj_dependency TinyObjLoader)
    # set(vox3dspace_json_dependency "${vox3dspace_root}/libs/json/lib/jsoncpp_static.lib")
    set(vox3dspace_plugin_dependency ${vox3dspace_dependency})
    set(vox3dspace_engine_base_dependency vox3dspace_engine_base)

    ####################################################################################
    # opengl libs info define
    list(APPEND vox3dspace_opengl_libs_deps
      "glew32d"
      "glfw3"
      "opengl32"
      )
    set(vox3dspace_opengl_dependency ${vox3dspace_opengl_libs_deps})
    #
    list(APPEND vox3dspace_opengl_compute_dependency ${vox3dspace_opengl_libs_deps} vox3dspace_engine_math)
    
    ####################################################################################
    # vulkan libs info define
    list(APPEND vox3dspace_vulkan_libs_deps
      "glfw3"
      ${Vulkan_LIBRARY}
      )
    set(vox3dspace_vulkan_dependency ${vox3dspace_vulkan_libs_deps})

    ####################################################################################
    # dx11 libs info define
    list(APPEND vox3dspace_dx11_libs_deps
      "d3d11"
      "dxgi"
      "dxguid"
      "D3DCompiler"
      "d2d1"
      "dwrite"
      "winmm"
      )
    set(vox3dspace_dx11_dependency ${vox3dspace_dx11_libs_deps})

    # dx12 libs info define
    list(APPEND vox3dspace_dx12_libs_deps
      "d3d12"
      "dxgi"
      "D3DCompiler"
      )
    set(vox3dspace_dx12_dependency ${vox3dspace_dx12_libs_deps})

    # cuda libs info define
    list(APPEND vox3dspace_cuda_libs_deps
      "cudadevrt"
      "cudart_static"
      )
    set(vox3dspace_cuda_dependency ${vox3dspace_cuda_libs_deps})

  else()
    if(BUILD_SHARED_LIBS)
      set(vox3dspace_dependency vox3dspace_shared)
    else()
      set(vox3dspace_dependency vox3dspace_static)
    endif()
    set(vox3dspace_plugin_dependency vox3dspace_static)
  endif()

  if(BUILD_SHARED_LIBS)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
  endif()
endmacro()

# Configures flags and sets build system globals.
macro(vox3dspace_set_build_definitions)
  string(TOLOWER "${CMAKE_BUILD_TYPE}" build_type_lowercase)

  if(build_type_lowercase MATCHES "rel" AND VOX3DSPACE_FAST)
    if(MSVC)
      list(APPEND vox3dspace_msvc_cxx_flags "/Ox")
    else()
      list(APPEND vox3dspace_base_cxx_flags "-O3")
    endif()
  endif()

  vox3dspace_load_version_info()

  # Library version info. See the libtool docs for updating the values:
  # https://www.gnu.org/software/libtool/manual/libtool.html#Updating-version-info
  #
  # c=<current>, r=<revision>, a=<age>
  #
  # libtool generates a .so file as .so.[c-a].a.r, while -version-info c:r:a is
  # passed to libtool.
  #
  # We set VOX3DSPACE_SOVERSION = [c-a].a.r
  set(LT_CURRENT 5)
  set(LT_REVISION 0)
  set(LT_AGE 0)
  math(EXPR VOX3DSPACE_SOVERSION_MAJOR "${LT_CURRENT} - ${LT_AGE}")
  set(VOX3DSPACE_SOVERSION "${VOX3DSPACE_SOVERSION_MAJOR}.${LT_AGE}.${LT_REVISION}")
  unset(LT_CURRENT)
  unset(LT_REVISION)
  unset(LT_AGE)

  list(APPEND vox3dspace_include_paths "${vox3dspace_root}" "${vox3dspace_root}/src"
              "${vox3dspace_build}")
  list(APPEND vox3dspace_libs_include_paths "${vox3dspace_root}/libsInclude")
  list(APPEND vox3dspace_libs_paths
    "${vox3dspace_libs_root}/json/lib"
    "${vox3dspace_libs_root}/tinyObj/lib"
    "${vox3dspace_libs_root}/tbb/lib"
    )
  ####################################################################################
  # bost libs info define
  
  # set(boost_root_dir $ENV{BOOST_ROOT_DIR})
  set(boost_root_dir $ENV{BOOST_ROOT_DIR})
  file(TO_CMAKE_PATH ${boost_root_dir} boost_root_dir)
  message(STATUS "boost_root_dir: ${boost_root_dir}")

  set(boost_lib_dir $ENV{BOOST_LIB_DIR})
  file(TO_CMAKE_PATH ${boost_lib_dir} boost_lib_dir)
  message(STATUS "boost_lib_dir: ${boost_lib_dir}")

  set(vulkan_sdk_dir $ENV{VULKAN_SDK})
  file(TO_CMAKE_PATH ${vulkan_sdk_dir} vulkan_sdk_dir)
  message(STATUS "vulkan_sdk_dir: ${vulkan_sdk_dir}")

  set(tbb_root_dir $ENV{TBB_ROOT_DIR})
  file(TO_CMAKE_PATH ${tbb_root_dir} tbb_root_dir)
  message(STATUS "tbb_root_dir: ${tbb_root_dir}")

  # externals\glm
  set(glm_root_dir "${vox3dspace_root}/externals/glm")
  message(STATUS "glm_root_dir: ${glm_root_dir}")

  
  # ccplus demo libs info define
  list(APPEND vox3dspace_ccplus_libs_include_paths "${vox3dspace_root}/demos/ccplus")
  # tbb demo libs info define
  list(APPEND vox3dspace_tbb_libs_include_paths "${tbb_root_dir}/include")
  # boost demo libs info define
  list(APPEND vox3dspace_boost_libs_include_paths "${boost_root_dir}")
  list(APPEND vox3dspace_boost_libs_paths "${boost_lib_dir}")
  # opengl libs info define
  list(APPEND vox3dspace_opengl_libs_include_paths "${vox3dspace_root}/openglLibs/libx64")
  list(APPEND vox3dspace_opengl_libs_paths "${vox3dspace_root}/openglLibs/libx64/lib")

  
  # opengl compute libs info define
  list(APPEND vox3dspace_opengl_compute_libs_include_paths "${vox3dspace_root}/openglLibs/libx64" ${vox3dspace_include_paths})
  ####################################################################################
  # vulkan libs info define
  list(APPEND vox3dspace_vulkan_libs_include_paths
    "${vox3dspace_root}/openglLibs/libx64"
    ${Vulkan_INCLUDE_DIR}
    # "${vulkan_sdk_dir}/Third-Party/Include"
  )
  list(APPEND vox3dspace_vulkan_libs_paths "${vox3dspace_root}/openglLibs/libx64/lib")
  #Third-Party\glm
  
  # cuda libs info define
  list(APPEND vox3dspace_cuda_libs_include_paths "${vox3dspace_root}/demos/cuda")
  list(APPEND vox3dspace_cuda_libs_paths "${vox3dspace_root}/demos/cuda")

  if(VOX3DSPACE_ABSL)
    list(APPEND vox3dspace_include_paths "${vox3dspace_root}/third_party/abseil-cpp")
  endif()

  if(VOX3DSPACE_TRANSCODER_SUPPORTED)
    vox3dspace_setup_eigen()
    vox3dspace_setup_filesystem()
    vox3dspace_setup_tinygltf()
  endif()


  list(APPEND vox3dspace_defines "VOX3DSPACE_CMAKE=1"
              "VOX3DSPACE_FLAGS_SRCDIR=\"${vox3dspace_root}\""
              "CMAKE_CURRENT_SOURCE_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}\""
              "VOX3DSPACE_FLAGS_TMPDIR=\"/tmp\"")

  if(MSVC OR WIN32)
    list(APPEND vox3dspace_defines "_CRT_SECURE_NO_DEPRECATE=1" "NOMINMAX=1")

    if(BUILD_SHARED_LIBS)
      set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
    endif()

    if(MSVC AND NOT VOX3DSPACE_DEBUG_MSVC_WARNINGS)
      # Silence some excessively noisy MSVC warnings when not actively seeking
      # to address them. These are harmless and serve only to make the build
      # output extremely verbose. To enable these warnings set the MSVC warning
      # level to 4, or define VOX3DSPACE_DEBUG_MSVC_WARNINGS on the CMake command
      # line when configuring Vox3DSpace.

      # warning C4018: '<operator>': signed/unsigned mismatch.
      list(APPEND vox3dspace_msvc_cxx_flags /w44018)

      # warning C4146: unary minus operator applied to unsigned type, result
      # still unsigned
      list(APPEND vox3dspace_msvc_cxx_flags /w44146)

      # warning C4244: 'return': conversion from '<type>' to '<type>', possible
      # loss of data.
      list(APPEND vox3dspace_msvc_cxx_flags /w44244)

      # warning C4267: 'initializing' conversion from '<type>' to '<type>',
      # possible loss of data.
      list(APPEND vox3dspace_msvc_cxx_flags /w44267)

      # warning C4804: '<operator>': unsafe use of type '<type>' in operation.
      list(APPEND vox3dspace_msvc_cxx_flags /w44804)
    endif()
  else()
    if(${CMAKE_SIZEOF_VOID_P} EQUAL 8)
      # Ensure 64-bit platforms can support large files.
      list(APPEND vox3dspace_defines "_LARGEFILE_SOURCE" "_FILE_OFFSET_BITS=64")
    endif()
  endif()

  if(ANDROID)
    if(CMAKE_ANDROID_ARCH_ABI STREQUAL "armeabi-v7a")
      set(CMAKE_ANDROID_ARM_MODE ON)
    endif()
  endif()

  set_vox3dspace_target()

  if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "6")
      # Quiet warnings in copy-list-initialization where {} elision has always
      # been allowed.
      list(APPEND vox3dspace_clang_cxx_flags "-Wno-missing-braces")
    endif()
  endif()

  if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "7")
      if(CMAKE_SYSTEM_PROCESSOR STREQUAL "armv7")
        # Quiet gcc 6 vs 7 abi warnings:
        # https://gcc.gnu.org/bugzilla/show_bug.cgi?id=77728
        list(APPEND vox3dspace_base_cxx_flags "-Wno-psabi")
        list(APPEND ABSL_GCC_FLAGS "-Wno-psabi")
      endif()
    endif()
  endif()

  # Source file names ending in these suffixes will have the appropriate
  # compiler flags added to their compile commands to enable intrinsics.
  set(vox3dspace_neon_source_file_suffix "neon.cc")
  set(vox3dspace_sse4_source_file_suffix "sse4.cc")

  if((${CMAKE_CXX_COMPILER_ID}
      STREQUAL
      "GNU"
      AND ${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 5)
     OR (${CMAKE_CXX_COMPILER_ID}
         STREQUAL
         "Clang"
         AND ${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 4))
    message(
      WARNING "GNU/GCC < v5 or Clang/LLVM < v4, ENABLING COMPATIBILITY MODE.")
    vox3dspace_enable_feature(FEATURE "VOX3DSPACE_OLD_GCC")
  endif()
  vox3dspace_configure_sanitizer()
endmacro()
