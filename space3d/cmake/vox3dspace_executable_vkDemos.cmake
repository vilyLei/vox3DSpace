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

  # set(test_vkDemo_name "windowSurface")
  # set(test_vkDemo_inner_paths "")
  
  function(build_vkDemo_innerPaths src_files out_path demo_name)
    # message(">>> >>> >>> src_files: ${src_files}")
    
    set(temp_demo_path "${test_vkDemos_path_root}/${demo_name}")
    set(temp_path "")
    foreach(k ${src_files})      
      # message(">>> k: ${k}")
      list(APPEND temp_path "${temp_demo_path}/${k}")
    endforeach()
    set(${out_path} "${temp_path}" PARENT_SCOPE)
    # set(${out_demo_path} "${temp_demo_path}" PARENT_SCOPE)
  endfunction()
  
  function(add_vkDemo_executableByNameFunc inner_srcs demo_name)
    
    set(inner_paths "")
    build_vkDemo_innerPaths("${inner_srcs}" inner_paths ${demo_name})
    # message(">>> >>> >>> add_vkDemo_executableByNameFunc() inner_paths: ${inner_paths}")
    vox3dspace_add_vkDemo_executableByName(${demo_name} "${inner_paths}")
  endfunction()

  set(test_vkDemo_inner_srcs
    "shader.vert"
    "shader.frag"
    "shader.bat"
  )
  # vox3dspace_add_vkDemo_executableByName("windowSurface" "")
  # vox3dspace_add_vkDemo_executableByName("swapChain" "")
  # vox3dspace_add_vkDemo_executableByName("imageViews" "")
  # vox3dspace_add_vkDemo_executableByName("shaderModules" "")
  # vox3dspace_add_vkDemo_executableByName("fixedFunctions" "")
  # vox3dspace_add_vkDemo_executableByName("renderPass" "")

  # vox3dspace_add_vkDemo_executableByName("framebuffers" "")
  # vox3dspace_add_vkDemo_executableByName("commandBuffers" "")
  # vox3dspace_add_vkDemo_executableByName("renderingAndPresentation" "")
  # vox3dspace_add_vkDemo_executableByName("framesInFlight" "")
  # vox3dspace_add_vkDemo_executableByName("swapChainRecreation" "")
  # vox3dspace_add_vkDemo_executableByName("indexBuffer" "")


  add_vkDemo_executableByNameFunc("" "windowSurface")
  add_vkDemo_executableByNameFunc("" "swapChain")
  add_vkDemo_executableByNameFunc("" "imageViews")
  add_vkDemo_executableByNameFunc("${test_vkDemo_inner_srcs}" "shaderModules")
  add_vkDemo_executableByNameFunc("${test_vkDemo_inner_srcs}" "fixedFunctions")
  add_vkDemo_executableByNameFunc("${test_vkDemo_inner_srcs}" "renderPass")
  
  add_vkDemo_executableByNameFunc("${test_vkDemo_inner_srcs}" "framebuffers")
  add_vkDemo_executableByNameFunc("${test_vkDemo_inner_srcs}" "commandBuffers")
  add_vkDemo_executableByNameFunc("${test_vkDemo_inner_srcs}" "renderingAndPresentation")
  add_vkDemo_executableByNameFunc("${test_vkDemo_inner_srcs}" "framesInFlight")
  add_vkDemo_executableByNameFunc("${test_vkDemo_inner_srcs}" "swapChainRecreation")
  add_vkDemo_executableByNameFunc("${test_vkDemo_inner_srcs}" "indexBuffer")

  add_vkDemo_executableByNameFunc("${test_vkDemo_inner_srcs}" "descLayoutAndBuffer")
  add_vkDemo_executableByNameFunc("${test_vkDemo_inner_srcs}" "descPoolAndSets")

endmacro()