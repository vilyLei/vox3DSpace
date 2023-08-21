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
  set(test_demo_folder_name "TestDemo")
  # Vox3DSpace app targets.
  set(ccplus_dir "${vox3dspace_root}/demos/ccplus")
  # thanks: https://learn.microsoft.com/zh-cn/cpp/build/reference/zc-conformance?view=msvc-170
  vox3dspace_add_executable(NAME
                       demo_ccplus
                       SOURCES
                       "${ccplus_dir}/thread/testAtomic.h"
                       "${ccplus_dir}/thread/testAtomic2.h"
                       "${ccplus_dir}/thread/testMutex.h"
                       "${ccplus_dir}/thread/testMemoryOrder.h"
                       "${ccplus_dir}/thread/testMemoryFence.h"
                       "${ccplus_dir}/thread/testSyncConcurrent.h"
                       "${ccplus_dir}/thread/testAsync.h"
                       "${ccplus_dir}/thread/testThrFuture.h"
                       "${ccplus_dir}/base/testConstexpr.h"
                       "${ccplus_dir}/parallel/testExecutionPar.h"
                       "${ccplus_dir}/parallel/futureFastSort01.h"
                       "${ccplus_dir}/parallel/baseNonLock.h"
                       "${ccplus_dir}/parallel/conditionAndLock.h"
                       "${ccplus_dir}/base/testForward.h"
                       "${ccplus_dir}/base/testTypeInfo.h"
                       "${ccplus_dir}/base/testString.h"
                       "${ccplus_dir}/base/testClass.h"
                       "${ccplus_dir}/base/testMacro.h"
                       "${ccplus_dir}/base/testMemory.h"
                       "${ccplus_dir}/coding/testCodecvt.h"
                       "${ccplus_dir}/exception1/testExcptBase.h"
                       "${ccplus_dir}/msvcAsm/testInlineAsm.h"
                       "${ccplus_dir}/demoTemplate/testTemplate.h"
                       "${ccplus_dir}/ccplusMain.cc"
                       FOLDER
                       ${test_demo_folder_name}
                       DEFINES
                       ${vox3dspace_defines}
                       RPOJECT_FLAGS
                       ${VOX3DSPACE_CXX_FLAGS}
                       "/Zc:__cplusplus"
                       INCLUDES
                       ${vox3dspace_ccplus_libs_include_paths})

  set(ccwindows_dir "${vox3dspace_root}/demos/ccwindows")  
  vox3dspace_add_executable(NAME
                       demo_ccwindows
                       SOURCES
                       "${ccwindows_dir}/thread/threadMain.h"
                       "${ccwindows_dir}/thread/mutexMain.h"
                       "${ccwindows_dir}/ccwindowsMain.cc"
                       FOLDER
                       ${test_demo_folder_name}
                       DEFINES
                       ${vox3dspace_defines}
                       RPOJECT_FLAGS
                       ${VOX3DSPACE_CXX_FLAGS}
                       "/Zc:__cplusplus"
                       INCLUDES
                       ${vox3dspace_ccwindows_libs_include_paths})
  # voxengine demos
  vox3dspace_add_executable(NAME
                       demo_vox3dspace_engine
                       SOURCES
                       "${vox3dspace_root}/demos/voxengine/voxengineTest.cc"
                       FOLDER
                       ${test_demo_folder_name}
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
                       FOLDER
                       ${test_demo_folder_name}
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
                       FOLDER
                       ${test_demo_folder_name}
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

  # tbb demo
  vox3dspace_add_executable(NAME
                       demo_tbb
                       SOURCES
                       "${vox3dspace_root}/demos/tbb/testMain.cc"
                       "${vox3dspace_root}/demos/tbb/tbbAtomic.h"
                       "${vox3dspace_root}/demos/tbb/tbbAtomic2.h"
                       "${vox3dspace_root}/demos/tbb/testMutex.h"
                       "${vox3dspace_root}/demos/tbb/testTask.h"
                       "${vox3dspace_root}/demos/tbb/testParallel.h"
                       "${tbb_root_dir}/src/tbb/co_context.h"
                       FOLDER
                       ${test_demo_folder_name}
                       DEFINES
                       ${vox3dspace_defines}
                       RPOJECT_FLAGS
                       ${VOX3DSPACE_CXX_FLAGS}
                       INCLUDES
                       ${vox3dspace_tbb_libs_include_paths}
                       LIB_INCLUDES
                       ${vox3dspace_libs_paths})
  
	file(COPY "${vox3dspace_root}/libs/tbb/lib/" DESTINATION "${PROJECT_BINARY_DIR}/${CMAKE_BUILD_TYPE}")
	file(COPY "${vox3dspace_root}/libs/tbb/lib/" DESTINATION "${PROJECT_BINARY_DIR}/")
  # boost demo
  vox3dspace_add_executable(NAME
                      demo_boost
                      SOURCES
                      "${vox3dspace_root}/demos/boost/baseTest.cc"
                      FOLDER
                      ${test_demo_folder_name}
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
                       FOLDER
                       ${test_demo_folder_name}
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
                       FOLDER
                       ${test_demo_folder_name}
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
                       "${ogl_demo_dir}/oglTest.cc"
                       "${ogl_demo_dir}/player.cc"
                       "${ogl_demo_dir}/player.h"
                       FOLDER
                       ${test_demo_folder_name}
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
  
  # ogl rendering demo
  set(ogl_r_demo_dir "${vox3dspace_root}/demos/opengl_rendering")
  vox3dspace_add_executable(NAME
                       demo_graphics_opengl_rendering
                       SOURCES
                       "${ogl_r_demo_dir}/colorTri.frag"
                       "${ogl_r_demo_dir}/colorTri.vert"
                       "${ogl_r_demo_dir}/shader.cc"
                       "${ogl_r_demo_dir}/shader.h"
                       "${ogl_r_demo_dir}/colorTriMain.cc"
                       FOLDER
                       ${test_demo_folder_name}
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
  # opengl_binShader
  set(ogl_bs_demo_dir "${vox3dspace_root}/demos/opengl_binShader")
  vox3dspace_add_executable(NAME
                       demo_graphics_opengl_binShader
                       SOURCES
                       "${ogl_bs_demo_dir}/colorTri.frag"
                       "${ogl_bs_demo_dir}/colorTri.vert"
                       "${ogl_bs_demo_dir}/shader.cc"
                       "${ogl_bs_demo_dir}/shader.h"
                       "${ogl_bs_demo_dir}/colorTriMain.cc"
                       FOLDER
                       ${test_demo_folder_name}
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

	# file(COPY "${vox3dspace_root}/demos/opengl/colorTri.frag" DESTINATION "${PROJECT_BINARY_DIR}")
  # ogl compute compute demo
  set(ogl_c_demo_dir "${vox3dspace_root}/demos/opengl_compute")
  vox3dspace_add_executable(NAME
                       demo_graphics_opengl_compute
                       SOURCES

                       "${ogl_c_demo_dir}/texv.comp"
                       "${ogl_c_demo_dir}/texv.frag"
                       "${ogl_c_demo_dir}/texv.vert"
                       "${ogl_c_demo_dir}/buffer.h"
                       "${ogl_c_demo_dir}/texture.h"
                       "${ogl_c_demo_dir}/shader.h"
                       "${ogl_c_demo_dir}/vertex_array_object.h"
                       "${ogl_c_demo_dir}/quad.h"
                       "${ogl_c_demo_dir}/renderer.h"
                       "${ogl_c_demo_dir}/testMain.cc"
                       
                       FOLDER
                       ${test_demo_folder_name}
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
                       FOLDER
                       ${test_demo_folder_name}
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
  # vulkan rendering demo
  vox3dspace_add_executable(NAME
                       demo_graphics_vulkan_rendering
                       SOURCES
                       "${vox3dspace_root}/demos/vulkan_rendering/renderingMain.cc"
                       FOLDER
                       ${test_demo_folder_name}
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
  # vulkan compute shader demo
  set(vlk_c_demo_dir "${vox3dspace_root}/demos/vulkan_compute")
  vox3dspace_add_executable(NAME
                       demo_graphics_vulkan_compute
                       SOURCES
                       "${vlk_c_demo_dir}/computeMain.cc"
                       "${vlk_c_demo_dir}/test.vert"
                       "${vlk_c_demo_dir}/test.frag"
                       "${vlk_c_demo_dir}/test.comp"
                       FOLDER
                       ${test_demo_folder_name}
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
  set(dx11_demo_dir "${vox3dspace_root}/demos/directx11/")
  vox3dspace_add_executable(NAME
                       demo_graphics_d3dx11
                       SOURCES
                       "${dx11_demo_dir}/d3dApp.cc"
                       "${dx11_demo_dir}/d3dApp.h"
                       "${dx11_demo_dir}/d3dUtil.cc"
                       "${dx11_demo_dir}/d3dUtil.h"
                       "${dx11_demo_dir}/DXTrace.cc"
                       "${dx11_demo_dir}/DXTrace.h"
                       "${dx11_demo_dir}/GameApp.cc"
                       "${dx11_demo_dir}/GameApp.h"
                       "${dx11_demo_dir}/GameTimer.cc"
                       "${dx11_demo_dir}/GameTimer.h"
                       "${dx11_demo_dir}/Main.cc"
                       FOLDER
                       ${test_demo_folder_name}
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
  # dx11 rendering demo
  set(dx11_r_demo_dir "${vox3dspace_root}/demos/directx11_rendering/")
  vox3dspace_add_executable(NAME
                       demo_graphics_d3dx11_rendering
                       SOURCES
                       "${dx11_r_demo_dir}/d3dApp.cc"
                       "${dx11_r_demo_dir}/d3dApp.h"
                       "${dx11_r_demo_dir}/d3dUtil.cc"
                       "${dx11_r_demo_dir}/d3dUtil.h"
                       "${dx11_r_demo_dir}/DXTrace.cc"
                       "${dx11_r_demo_dir}/DXTrace.h"
                       "${dx11_r_demo_dir}/GameApp.cc"
                       "${dx11_r_demo_dir}/GameApp.h"
                       "${dx11_r_demo_dir}/GameTimer.cc"
                       "${dx11_r_demo_dir}/GameTimer.h"
                       "${dx11_r_demo_dir}/Main.cc"
                       FOLDER
                       ${test_demo_folder_name}
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
                      FOLDER
                      ${test_demo_folder_name}
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
  
  message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
  message(STATUS "CMAKE_INTDIR: ${CMAKE_INTDIR}")
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
                      FOLDER
                      ${test_demo_folder_name}
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