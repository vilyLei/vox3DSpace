macro(build_executable_vkDemos)  

  set(test_vkDemos_folder_name "VulkanDemos")

  function(vox3dspace_add_vkDemo_executableByName DEMO_NAME DEMO_SOURCES)

    # message(STATUS ">>> >>> DEMO_NAME: ${DEMO_NAME}")
    # message(STATUS ">>> >>> DEMO_SOURCES: ${DEMO_SOURCES}")

    vox3dspace_add_executable(NAME
      ${DEMO_NAME}
      SOURCES
      "${vox3dspace_root}/testCases/vulkanDemos/${DEMO_NAME}/${DEMO_NAME}.cc"
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
endmacro()