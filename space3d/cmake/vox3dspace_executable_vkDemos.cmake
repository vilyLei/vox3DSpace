macro(build_executable_vkDemos)  
  if(MSVC)
    if(BUILD_SHARED_LIBS)
      set(vox3dspace_lib_type SHARED)
    else()
      set(vox3dspace_lib_type STATIC)
    endif()
  endif()

  set(test_vkDemos_folder_name "DXRDemos")

  function(vox3dspace_add_dxrDemo_executable DEMO_NAME DEMO_SOURCES)

    # message(STATUS ">>> >>> DEMO_NAME: ${DEMO_NAME}")
    # message(STATUS ">>> >>> DEMO_SOURCES: ${DEMO_SOURCES}")
    vox3dspace_add_executable(NAME
      ${DEMO_NAME}
      SOURCES
      "${vox3dspace_root}/testCases/vkDemos/framework/Framework.cc"
      "${vox3dspace_root}/testCases/vkDemos/framework/Framework.h"
      ${DEMO_SOURCES}
      FOLDER
      ${test_vkDemos_folder_name}
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
  list(APPEND createWindow_sources
    "${vox3dspace_root}/testCases/vkDemos/createWindow/CreateWindow.cc"
    "${vox3dspace_root}/testCases/vkDemos/createWindow/CreateWindow.h"
    )
  vox3dspace_add_dxrDemo_executable("demo01_createWindow" ${createWindow_sources})
  #
	endif()
endmacro()