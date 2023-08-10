if(VOX3DSPACE_CMAKE_BUILD_EXECUTEABLE_MULTIPROC_DEMOS_)
  return()
endif() # VOX3DSPACE_CMAKE_BUILD_EXECUTEABLE_MULTIPROC_DEMOS_
set(VOX3DSPACE_CMAKE_BUILD_EXECUTEABLE_MULTIPROC_DEMOS_ 1)

macro(build_executable_multprocessDemos)  

  set(test_multprocessDemos_folder_name "Multiprocess")
  set(multprocessDemo_dir "${vox3dspace_root}/testCases/multiprocess/")

  function(vox3dspace_add_multprocessDemo_executableByName DEMO_NAME DEMO_SOURCES)

    # message(STATUS ">>> >>> DEMO_NAME: ${DEMO_NAME}")
    # message(STATUS ">>> >>> DEMO_SOURCES: ${DEMO_SOURCES}")

    set(demoDir "${multprocessDemo_dir}/${DEMO_NAME}/")
    vox3dspace_add_executable(NAME
      ${DEMO_NAME}
      SOURCES
      "${demoDir}/${DEMO_NAME}.cc"
      ${DEMO_SOURCES}
      FOLDER
      ${test_multprocessDemos_folder_name}
      DEFINES
      ${vox3dspace_defines}
      RPOJECT_FLAGS
      ${VOX3DSPACE_CXX_FLAGS}
      INCLUDES
      ${demoDir}
      "${vox3dspace_root}/libsInclude")
    
  endfunction()

  set(src_demoName "sharedMemory")
  set(src_demoDir "${multprocessDemo_dir}/${src_demoName}/")
  list(APPEND multprocesssrc_demo_sources
  "${src_demoDir}/proc/testCreateProcess.h"
    )
  vox3dspace_add_multprocessDemo_executableByName(${src_demoName} "${multprocesssrc_demo_sources}")
endmacro()