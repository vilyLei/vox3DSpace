macro(build_executable_projects)  
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
                       demo_ccplus
                       SOURCES
                       "${vox3dspace_root}/demos/ccplus/testMain.cc"
                       DEFINES
                       ${vox3dspace_defines}
                       RPOJECT_FLAGS
                       ${VOX3DSPACE_CXX_FLAGS}
                       INCLUDES
                       ${vox3dspace_ccplus_libs_include_paths})

  vox3dspace_add_executable(NAME
                       demo_vox3dspace_engine
                       SOURCES
                       "${vox3dspace_root}/demos/voxengine/voxengineTest.cc"
                       DEFINES
                       ${vox3dspace_defines}
                       RPOJECT_FLAGS
                       ${VOX3DSPACE_CXX_FLAGS}
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
                       RPOJECT_FLAGS
                       ${VOX3DSPACE_CXX_FLAGS}
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
                       RPOJECT_FLAGS
                       ${VOX3DSPACE_CXX_FLAGS}
                       INCLUDES
                       ${vox3dspace_libs_include_paths}
                       LIB_INCLUDES
                       ${vox3dspace_libs_paths}
                       LIB_DEPS
                       ${vox3dspace_json_dependency})

  # boost demo
  vox3dspace_add_executable(NAME
                      demo_boost
                      SOURCES
                      "${vox3dspace_root}/demos/boost/baseTest.cc"
                      DEFINES
                      ${vox3dspace_defines}
                      RPOJECT_FLAGS
                      ${VOX3DSPACE_CXX_FLAGS}
                      INCLUDES
                      ${vox3dspace_boost_libs_include_paths}
                      LIB_INCLUDES
                      ${vox3dspace_boost_libs_paths})
  # tiny obj demo
  vox3dspace_add_executable(NAME
                       demo_tinyObj
                       SOURCES
                       "${vox3dspace_root}/demos/tinyObj/tinyObjTest.cc"
                       DEFINES
                       ${vox3dspace_defines}
                       RPOJECT_FLAGS
                       ${VOX3DSPACE_CXX_FLAGS}
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
                       RPOJECT_FLAGS
                       ${VOX3DSPACE_CXX_FLAGS}
                       INCLUDES
                       ${vox3dspace_include_paths}
                       LIB_DEPS
                       ${vox3dspace_profile_dependency})
  
  # ogl demo
  set(ogl_demo_dir "${vox3dspace_root}/demos/opengl")
  vox3dspace_add_executable(NAME
                       demo_graphics_opengl
                       SOURCES
                      #  "${ogl_demo_dir}/oglTest.cc"
                      #  "${ogl_demo_dir}/player.cc"
                      #  "${ogl_demo_dir}/player.h"

                       "${ogl_demo_dir}/colorTri.frag"
                       "${ogl_demo_dir}/colorTri.vert"
                       "${ogl_demo_dir}/shader.cc"
                       "${ogl_demo_dir}/shader.h"
                       "${ogl_demo_dir}/colorTriMain.cc"
                       DEFINES
                       ${vox3dspace_defines}
                       RPOJECT_FLAGS
                       ${VOX3DSPACE_CXX_FLAGS}
                       INCLUDES
                       ${vox3dspace_opengl_libs_include_paths}
                       LIB_INCLUDES
                       ${vox3dspace_opengl_libs_paths}
                       LIB_DEPS
                       ${vox3dspace_opengl_dependency})
	# file(COPY "${vox3dspace_root}/demos/opengl/colorTri.vert" DESTINATION "${PROJECT_BINARY_DIR}/Debug")
	# file(COPY "${vox3dspace_root}/demos/opengl/colorTri.frag" DESTINATION "${PROJECT_BINARY_DIR}/Debug")
	# file(COPY "${vox3dspace_root}/demos/opengl/colorTri.vert" DESTINATION "${PROJECT_BINARY_DIR}")
	# file(COPY "${vox3dspace_root}/demos/opengl/colorTri.frag" DESTINATION "${PROJECT_BINARY_DIR}")
  # ogl compute demo
  set(ogl_demo_dir "${vox3dspace_root}/demos/opengl_compute")
  vox3dspace_add_executable(NAME
                       demo_graphics_opengl_compute
                       SOURCES

                       "${ogl_demo_dir}/texv.comp"
                       "${ogl_demo_dir}/texv.frag"
                       "${ogl_demo_dir}/texv.vert"
                       "${ogl_demo_dir}/buffer.h"
                       "${ogl_demo_dir}/texture.h"
                       "${ogl_demo_dir}/shader.h"
                       "${ogl_demo_dir}/vertex_array_object.h"
                       "${ogl_demo_dir}/quad.h"
                       "${ogl_demo_dir}/renderer.h"
                       "${ogl_demo_dir}/testMain.cc"
                       
                       DEFINES
                       ${vox3dspace_defines}
                       RPOJECT_FLAGS
                       ${VOX3DSPACE_CXX_FLAGS}
                       INCLUDES
                       ${vox3dspace_opengl_compute_libs_include_paths}
                       LIB_INCLUDES
                       ${vox3dspace_opengl_libs_paths}
                       LIB_DEPS
                       ${vox3dspace_opengl_compute_dependency})
  # vulkan demo
  vox3dspace_add_executable(NAME
                       demo_graphics_vulkan
                       SOURCES
                       "${vox3dspace_root}/demos/vulkan/vulkanTest.cc"
                       DEFINES
                       ${vox3dspace_defines}
                       RPOJECT_FLAGS
                       ${VOX3DSPACE_CXX_FLAGS}
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
                       RPOJECT_FLAGS
                       ${VOX3DSPACE_CXX_FLAGS}
                       EXEC_SYS
                       "WIN32"
                       LINK_FLAGS
                       "-mwindows"
                       DEFINES
                       "-DUNICODE"
                       "-D_UNICODE"
                       LIB_DEPS
                       ${vox3dspace_dx11_dependency})
  # dx12 demo, warn: copy shaders.hshd file to debug dir
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
											"${vox3dspace_root}/demos/directx12/shaders.hshd"
											DEFINES
											${vox3dspace_defines}
                      RPOJECT_FLAGS
                      ${VOX3DSPACE_CXX_FLAGS}
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
	# file(COPY "${vox3dspace_root}/demos/directx12/shaders.hshd" DESTINATION "${PROJECT_BINARY_DIR}/Debug")

  # cuda demo
  # enable_language(CUDA)  
  vox3dspace_add_executable(NAME
											demo_cuda
											SOURCES
											"${vox3dspace_root}/demos/cuda/exception.h"
											"${vox3dspace_root}/demos/cuda/helper_image.h"
											"${vox3dspace_root}/demos/cuda/helper_string.h"
											"${vox3dspace_root}/demos/cuda/helper_cuda.h"
											"${vox3dspace_root}/demos/cuda/helper_functions.h"
											"${vox3dspace_root}/demos/cuda/asyncAPI.cu"
											DEFINES
											${vox3dspace_defines}
                      INCLUDES
                      ${vox3dspace_cuda_libs_include_paths}
                      LIB_INCLUDES
                      ${vox3dspace_cuda_libs_paths}
                      LIB_DEPS
                      ${vox3dspace_cuda_dependency})
	endif()
endmacro()