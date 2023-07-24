if(VOX3DSPACE_CMAKE_VOX3DSPACE_INSTALL_CMAKE_)
  return()
endif() # VOX3DSPACE_CMAKE_VOX3DSPACE_INSTALL_CMAKE_
set(VOX3DSPACE_CMAKE_VOX3DSPACE_INSTALL_CMAKE_ 1)

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

# Sets up the vox3dspace install targets. Must be called after the static library
# target is created.
macro(vox3dspace_setup_install_target)
  set(bin_path "${CMAKE_INSTALL_FULL_BINDIR}")
  set(data_path "${CMAKE_INSTALL_FULL_DATAROOTDIR}")
  set(includes_path "${CMAKE_INSTALL_FULL_INCLUDEDIR}")
  set(libs_path "${CMAKE_INSTALL_FULL_LIBDIR}")

  foreach(file ${vox3dspace_sources})
    if(file MATCHES "h$")
      list(APPEND vox3dspace_api_includes ${file})
    endif()
  endforeach()

  list(REMOVE_DUPLICATES vox3dspace_api_includes)

  # Strip $vox3dspace_src_root from the file paths: we need to install relative to
  # $include_directory.
  list(TRANSFORM vox3dspace_api_includes REPLACE "${vox3dspace_src_root}/" "")

  foreach(vox3dspace_api_include ${vox3dspace_api_includes})
    get_filename_component(file_directory ${vox3dspace_api_include} DIRECTORY)
    set(target_directory "${includes_path}/vox3dspace/${file_directory}")
    install(FILES ${vox3dspace_src_root}/${vox3dspace_api_include}
            DESTINATION "${target_directory}")
  endforeach()

  install(
    FILES "${vox3dspace_build}/vox3dspace/vox3dspace_features.h"
    DESTINATION "${includes_path}/vox3dspace/")

  install(TARGETS vox3dspace_engine DESTINATION "${bin_path}")

  if(MSVC)
    install(
      TARGETS vox3dspace
      EXPORT vox3dspaceExport
      RUNTIME DESTINATION "${bin_path}"
      ARCHIVE DESTINATION "${libs_path}"
      LIBRARY DESTINATION "${libs_path}")
  else()
    install(
        TARGETS vox3dspace_static
        EXPORT vox3dspaceExport
        DESTINATION "${libs_path}")

    if(BUILD_SHARED_LIBS)
      install(
        TARGETS vox3dspace_shared
        EXPORT vox3dspaceExport
        RUNTIME DESTINATION "${bin_path}"
        ARCHIVE DESTINATION "${libs_path}"
        LIBRARY DESTINATION "${libs_path}")
    endif()
  endif()

  if(VOX3DSPACE_UNITY_PLUGIN)
    install(TARGETS vox3dspacedec_unity DESTINATION "${libs_path}")
  endif()

  if(VOX3DSPACE_MAYA_PLUGIN)
    install(TARGETS vox3dspace_maya_wrapper DESTINATION "${libs_path}")
  endif()

  # pkg-config: vox3dspace.pc
  configure_file("${vox3dspace_root}/cmake/vox3dspace.pc.template"
                 "${vox3dspace_build}/vox3dspace.pc" @ONLY NEWLINE_STYLE UNIX)
  install(FILES "${vox3dspace_build}/vox3dspace.pc" DESTINATION "${libs_path}/pkgconfig")

  # CMake config: vox3dspace-config.cmake
  configure_package_config_file(
    "${vox3dspace_root}/cmake/vox3dspace-config.cmake.template"
    "${vox3dspace_build}/vox3dspace-config.cmake"
    INSTALL_DESTINATION "${data_path}/cmake/vox3dspace")

  write_basic_package_version_file(
    "${vox3dspace_build}/vox3dspace-config-version.cmake"
    VERSION ${VOX3DSPACE_VERSION}
    COMPATIBILITY AnyNewerVersion)

  export(
    EXPORT vox3dspaceExport
    NAMESPACE vox3dspace::
    FILE "${vox3dspace_build}/vox3dspace-targets.cmake")

  install(
    EXPORT vox3dspaceExport
    NAMESPACE vox3dspace::
    FILE vox3dspace-targets.cmake
    DESTINATION "${data_path}/cmake/vox3dspace")

  install(
    FILES
      "${vox3dspace_build}/vox3dspace-config.cmake"
      "${vox3dspace_build}/vox3dspace-config-version.cmake"
    DESTINATION "${data_path}/cmake/vox3dspace")
endmacro()
