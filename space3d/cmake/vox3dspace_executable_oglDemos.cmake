if(VOX3DSPACE_CMAKE_BUILD_EXECUTEABLE_OGL_DEMOS_)
  return()
endif() # VOX3DSPACE_CMAKE_BUILD_EXECUTEABLE_OGL_DEMOS_
set(VOX3DSPACE_CMAKE_BUILD_EXECUTEABLE_OGL_DEMOS_ 1)

macro(build_executable_oglDemos)  

  set(test_oglDemos_folder_name "OpenglDemos")
  set(oglDemo_dir "${vox3dspace_root}/testCases/openglDemos/")

  function(vox3dspace_add_oglDemo_executableByName DEMO_NAME DEMO_SOURCES DEMO_EXEC_SYS DEMO_LINK_FLAGS)

    # message(STATUS ">>> >>> DEMO_NAME: ${DEMO_NAME}")
    # message(STATUS ">>> >>> DEMO_SOURCES: ${DEMO_SOURCES}")

    set(demoDir "${oglDemo_dir}/${DEMO_NAME}/")
    vox3dspace_add_executable(NAME
      ${DEMO_NAME}
      SOURCES
      ${DEMO_SOURCES}
      "${demoDir}/${DEMO_NAME}.cc"
      FOLDER
      ${test_oglDemos_folder_name}
      EXEC_SYS
      "${DEMO_EXEC_SYS}"
      LINK_FLAGS
      "${DEMO_LINK_FLAGS}"
      DEFINES
      ${vox3dspace_defines}
      RPOJECT_FLAGS
      ${VOX3DSPACE_CXX_FLAGS}
      INCLUDES
      ${vox3dspace_opengl_libs_include_paths}
      "${vox3dspace_root}/libsInclude"
      LIB_INCLUDES
      ${vox3dspace_opengl_libs_paths}
      "${vox3dspace_libs_root}/stb/lib"
      LIB_DEPS
      ${vox3dspace_opengl_dependency})
    
  endfunction()

  set(src_demoName "textures")
  set(src_demoDir "${oglDemo_dir}/${src_demoName}/")
  set(oglsrc_demo_sources
      "${src_demoDir}/shader.cc"
      "${src_demoDir}/shader.h"
      )
  vox3dspace_add_oglDemo_executableByName(${src_demoName} "${oglsrc_demo_sources}" "" "")

  set(src_demoName "wglapp")
  set(src_demoDir "${oglDemo_dir}/${src_demoName}/")
  set(oglsrc_demo_sources
      "${src_demoDir}/sample/baseWglCtx.h"
      "${src_demoDir}/sample/baseWglCtx02.h"
      )
  vox3dspace_add_oglDemo_executableByName(${src_demoName} "${oglsrc_demo_sources}" "WIN32" "-mwindows")
  # vox3dspace_add_oglDemo_executableByName("textures" "")
endmacro()