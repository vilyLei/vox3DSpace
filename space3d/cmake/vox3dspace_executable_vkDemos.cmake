if(VOX3DSPACE_CMAKE_BUILD_EXECUTEABLE_VK_DEMOS_)
  return()
endif() # VOX3DSPACE_CMAKE_BUILD_EXECUTEABLE_VK_DEMOS_
set(VOX3DSPACE_CMAKE_BUILD_EXECUTEABLE_VK_DEMOS_ 1)

macro(build_executable_vkDemos)  

  set(test_vkDemos_folder_name "VulkanDemos")
  set(test_vkDemos_path_root "${vox3dspace_root}/testCases/vulkanDemos")

  function(vox3dspace_add_vkDemo_executableByName DEMO_NAME DEMO_SOURCES)

    # message(STATUS ">>> >>> DEMO_NAME: ${DEMO_NAME}")
    # message(STATUS ">>> >>> DEMO_SOURCES: ${DEMO_SOURCES}")

    vox3dspace_add_executable(NAME
      ${DEMO_NAME}
      SOURCES
      # "${vox3dspace_root}/testCases/vulkanDemos/${DEMO_NAME}/${DEMO_NAME}.cc"
      "${test_vkDemos_path_root}/${DEMO_NAME}/${DEMO_NAME}.cc"
      ${DEMO_SOURCES}
      FOLDER
      ${test_vkDemos_folder_name}
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

  endfunction()

  set(test_vkDemo_name "windowSurface")
  set(test_vkDemo_inner_paths "")
  

  function(build_vkDemo_innerPaths src_files out_path demo_name out_demo_path)
    # message(">>> >>> >>> src_files: ${src_files}")
    
    set(temp_demo_path "${test_vkDemos_path_root}/${demo_name}")
    set(temp_path "")
    foreach(k ${src_files})      
      # message(">>> k: ${k}")
      list(APPEND temp_path "${temp_demo_path}/${k}")
    endforeach()
    set(${out_path} "${temp_path}" PARENT_SCOPE)
    set(${out_demo_path} "${temp_demo_path}" PARENT_SCOPE)
  endfunction()

  vox3dspace_add_vkDemo_executableByName("windowSurface" "")
  vox3dspace_add_vkDemo_executableByName("swapChain" "")
  vox3dspace_add_vkDemo_executableByName("imageViews" "")
  vox3dspace_add_vkDemo_executableByName("shaderModules" "")
  vox3dspace_add_vkDemo_executableByName("fixedFunctions" "")
  vox3dspace_add_vkDemo_executableByName("renderPass" "")

  vox3dspace_add_vkDemo_executableByName("framebuffers" "")
  vox3dspace_add_vkDemo_executableByName("commandBuffers" "")
  vox3dspace_add_vkDemo_executableByName("renderingAndPresentation" "")
  vox3dspace_add_vkDemo_executableByName("framesInFlight" "")
  vox3dspace_add_vkDemo_executableByName("swapChainRecreation" "")
  vox3dspace_add_vkDemo_executableByName("indexBuffer" "")

  
  set(test_vkDemo_name "descLayoutAndBuffer")
  set(test_vkDemo_inner_srcs
    "shader.vert"
    "shader.frag"
  )
  build_vkDemo_innerPaths("${test_vkDemo_inner_srcs}" test_vkDemo_inner_paths ${test_vkDemo_name} test_vkDemo_path)
  vox3dspace_add_vkDemo_executableByName(${test_vkDemo_name} "${test_vkDemo_inner_paths}")
  
  set(test_vkDemo_name "descPoolAndSets")
  set(test_vkDemo_inner_srcs
    "shader.vert"
    "shader.frag"
  )
  build_vkDemo_innerPaths("${test_vkDemo_inner_srcs}" test_vkDemo_inner_paths ${test_vkDemo_name} ${test_vkDemo_path})
  message(">>> >>> >>> A test_vkDemo_inner_paths: ${test_vkDemo_inner_paths}")
  message(">>> >>> >>> A test_vkDemo_path: ${test_vkDemo_path}")

  vox3dspace_add_vkDemo_executableByName(${test_vkDemo_name} "${test_vkDemo_inner_paths}")
endmacro()