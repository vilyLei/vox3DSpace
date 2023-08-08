if(VOX3DSPACE_CMAKE_BUILD_EXECUTEABLE_HPC_DEMOS_)
  return()
endif() # VOX3DSPACE_CMAKE_BUILD_EXECUTEABLE_HPC_DEMOS_
set(VOX3DSPACE_CMAKE_BUILD_EXECUTEABLE_HPC_DEMOS_ 1)

macro(build_executable_hpcDemos)  

  set(test_hpcDemos_folder_name "HPC")
  set(hpcDemo_dir "${vox3dspace_root}/hpc/")

  function(vox3dspace_add_hpcDemo_executableByName DEMO_NAME DEMO_SOURCES)

    # message(STATUS ">>> >>> DEMO_NAME: ${DEMO_NAME}")
    # message(STATUS ">>> >>> DEMO_SOURCES: ${DEMO_SOURCES}")

    set(demoDir "${hpcDemo_dir}/${DEMO_NAME}/")
    vox3dspace_add_executable(NAME
      ${DEMO_NAME}
      SOURCES
      "${demoDir}/${DEMO_NAME}.cc"
      ${DEMO_SOURCES}
      FOLDER
      ${test_hpcDemos_folder_name}
      DEFINES
      ${vox3dspace_defines}
      RPOJECT_FLAGS
      ${VOX3DSPACE_CXX_FLAGS}
      INCLUDES
      ${demoDir}
      "${vox3dspace_root}/libsInclude")
    
  endfunction()

  set(src_demoName "cpuparallel")
  set(src_demoDir "${hpcDemo_dir}/${src_demoName}/")
  vox3dspace_add_hpcDemo_executableByName(${src_demoName}
  "${src_demoDir}/checkSSEAVX.h"
  )
endmacro()