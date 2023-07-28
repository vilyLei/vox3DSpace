macro(build_executable_dx12dxrDemos)

  set(test_dx12dxrDemos_folder_name "DXRDemos")

  function(vox3dspace_add_dxrDemo_executable DEMO_NAME DEMO_SOURCES)

    # message(STATUS ">>> >>> DEMO_NAME: ${DEMO_NAME}")
    # message(STATUS ">>> >>> DEMO_SOURCES: ${DEMO_SOURCES}")
    vox3dspace_add_executable(NAME
      ${DEMO_NAME}
      SOURCES
      "${vox3dspace_root}/testCases/dx12dxrDemos/framework/Framework.cc"
      "${vox3dspace_root}/testCases/dx12dxrDemos/framework/Framework.h"
      ${DEMO_SOURCES}
      FOLDER
      ${test_dx12dxrDemos_folder_name}
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
      INCLUDES
      ${vox3dspace_dx12demos_include_paths}
      LIB_DEPS
      ${vox3dspace_dx12_dependency})

endfunction()
if(WIN32)
  # vox3dspace_add_executable(NAME
  #     createWindow
  #     SOURCES
  #     "${vox3dspace_root}/testCases/dx12dxrDemos/framework/Framework.cc"
  #     "${vox3dspace_root}/testCases/dx12dxrDemos/framework/Framework.h"
  #     "${vox3dspace_root}/testCases/dx12dxrDemos/createWindow/CreateWindow.cc"
  #     "${vox3dspace_root}/testCases/dx12dxrDemos/createWindow/CreateWindow.h"
  #     FOLDER
  #     ${test_dx12dxrDemos_folder_name}
  #     DEFINES
  #     ${vox3dspace_defines}
  #     RPOJECT_FLAGS
  #     ${VOX3DSPACE_CXX_FLAGS}
  #     EXEC_SYS
  #     "WIN32"
  #     LINK_FLAGS
  #     "-mwindows"
  #     DEFINES
  #     "-DUNICODE"
  #     "-D_UNICODE"
  #     INCLUDES
  #     ${vox3dspace_dx12demos_include_paths}
  #     LIB_DEPS
  #     ${vox3dspace_dx12_dependency})
  #
  list(APPEND createWindow_sources
    "${vox3dspace_root}/testCases/dx12dxrDemos/createWindow/CreateWindow.cc"
    "${vox3dspace_root}/testCases/dx12dxrDemos/createWindow/CreateWindow.h"
    )
  vox3dspace_add_dxrDemo_executable("demo01_createWindow" ${createWindow_sources})
  #
  list(APPEND initDXR_sources
    "${vox3dspace_root}/testCases/dx12dxrDemos/initDXR/InitDXR.cc"
    "${vox3dspace_root}/testCases/dx12dxrDemos/initDXR/InitDXR.h"
    )
  vox3dspace_add_dxrDemo_executable("demo02_initDXR" ${initDXR_sources})
  #
  list(APPEND accelerationStructure_sources
    "${vox3dspace_root}/testCases/dx12dxrDemos/accelerationStructure/AccelerationStructure.cc"
    "${vox3dspace_root}/testCases/dx12dxrDemos/accelerationStructure/AccelerationStructure.h"
    )
  vox3dspace_add_dxrDemo_executable("demo03_accelerationStructure" ${accelerationStructure_sources})
	#
  list(APPEND rtPipelineState_sources
    "${vox3dspace_root}/testCases/dx12dxrDemos/rtPipelineState/RtPipelineState.cc"
    "${vox3dspace_root}/testCases/dx12dxrDemos/rtPipelineState/RtPipelineState.h"
    )
  vox3dspace_add_dxrDemo_executable("demo04_rtPipelineState" ${rtPipelineState_sources})
  #
  list(APPEND shaderTable_sources
    "${vox3dspace_root}/testCases/dx12dxrDemos/shaderTable/ShaderTable.cc"
    "${vox3dspace_root}/testCases/dx12dxrDemos/shaderTable/ShaderTable.h"
    )
  vox3dspace_add_dxrDemo_executable("demo05_shaderTable" ${shaderTable_sources})
  #
  list(APPEND raytrace_sources
    "${vox3dspace_root}/testCases/dx12dxrDemos/raytrace/Raytrace.cc"
    "${vox3dspace_root}/testCases/dx12dxrDemos/raytrace/Raytrace.h"
    )
  vox3dspace_add_dxrDemo_executable("demo06_raytrace" ${raytrace_sources})
  # basicShaders
  list(APPEND basicShaders_sources
    "${vox3dspace_root}/testCases/dx12dxrDemos/basicShaders/BasicShaders.cc"
    "${vox3dspace_root}/testCases/dx12dxrDemos/basicShaders/BasicShaders.h"
    )
    vox3dspace_add_dxrDemo_executable("demo07_basicShaders" ${basicShaders_sources})
  #
  # instancing
  list(APPEND instancing_sources
    "${vox3dspace_root}/testCases/dx12dxrDemos/instancing/Instancing.cc"
    "${vox3dspace_root}/testCases/dx12dxrDemos/instancing/Instancing.h"
    )
    vox3dspace_add_dxrDemo_executable("demo08_instancing" ${instancing_sources})
  #
  # constantBuffer
  list(APPEND constantBuffer_sources
    "${vox3dspace_root}/testCases/dx12dxrDemos/constantBuffer/ConstantBuffer.cc"
    "${vox3dspace_root}/testCases/dx12dxrDemos/constantBuffer/ConstantBuffer.h"
    )
    vox3dspace_add_dxrDemo_executable("demo09_constantBuffer" ${constantBuffer_sources})
  #
  # perInstanceConstantBuffer
  list(APPEND perInstanceConstantBuffer_sources
    "${vox3dspace_root}/testCases/dx12dxrDemos/perInstanceConstantBuffer/PerInstanceConstantBuffer.cc"
    "${vox3dspace_root}/testCases/dx12dxrDemos/perInstanceConstantBuffer/PerInstanceConstantBuffer.h"
    )
    vox3dspace_add_dxrDemo_executable("demo10_perInstanceConstantBuffer" ${perInstanceConstantBuffer_sources})
  #
  # secondGeometry
  list(APPEND secondGeometry_sources
    "${vox3dspace_root}/testCases/dx12dxrDemos/secondGeometry/SecondGeometry.cc"
    "${vox3dspace_root}/testCases/dx12dxrDemos/secondGeometry/SecondGeometry.h"
    )
    vox3dspace_add_dxrDemo_executable("demo11_secondGeometry" ${secondGeometry_sources})
  #
  # perGeometryHitShader
  list(APPEND perGeometryHitShader_sources
    "${vox3dspace_root}/testCases/dx12dxrDemos/perGeometryHitShader/PerGeometryHitShader.cc"
    "${vox3dspace_root}/testCases/dx12dxrDemos/perGeometryHitShader/PerGeometryHitShader.h"
    )
    vox3dspace_add_dxrDemo_executable("demo12_perGeometryHitShader" ${perGeometryHitShader_sources})
  #
  # secondRayType
  list(APPEND secondRayType_sources
    "${vox3dspace_root}/testCases/dx12dxrDemos/secondRayType/SecondRayType.cc"
    "${vox3dspace_root}/testCases/dx12dxrDemos/secondRayType/SecondRayType.h"
    )
    vox3dspace_add_dxrDemo_executable("demo13_secondRayType" ${secondRayType_sources})
  #
  # refit
  list(APPEND refit_sources
  "${vox3dspace_root}/testCases/dx12dxrDemos/refit/Refit.cc"
  "${vox3dspace_root}/testCases/dx12dxrDemos/refit/Refit.h"
  )
  vox3dspace_add_dxrDemo_executable("demo14_refit" ${refit_sources})

  #######################################################################################################
  file(GLOB dxrDemo_dlls "${vox3dspace_root}/testCases/dx12dxrDemos/framework/externals/dxcompiler/*.dll")
  file(COPY ${dxrDemo_dlls} DESTINATION "${PROJECT_BINARY_DIR}/")
	#
	endif()
endmacro()