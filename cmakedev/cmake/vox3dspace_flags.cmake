if(VOX3DSPACE_CMAKE_VOX3DSPACE_FLAGS_CMAKE_)
  return()
endif() # VOX3DSPACE_CMAKE_VOX3DSPACE_FLAGS_CMAKE_
set(VOX3DSPACE_CMAKE_VOX3DSPACE_FLAGS_CMAKE_ 1)

include(CheckCXXCompilerFlag)
include(CheckCXXSourceCompiles)

# Adds compiler flags specified by FLAGS to the sources specified by SOURCES:
#
# vox3dspace_set_compiler_flags_for_sources(SOURCES <sources> FLAGS <flags>)
macro(vox3dspace_set_compiler_flags_for_sources)
  unset(compiler_SOURCES)
  unset(compiler_FLAGS)
  unset(optional_args)
  unset(single_value_args)
  set(multi_value_args SOURCES FLAGS)
  cmake_parse_arguments(compiler "${optional_args}" "${single_value_args}"
                        "${multi_value_args}" ${ARGN})

  if(NOT (compiler_SOURCES AND compiler_FLAGS))
    vox3dspace_die("vox3dspace_set_compiler_flags_for_sources: SOURCES and "
              "FLAGS required.")
  endif()

  set_source_files_properties(${compiler_SOURCES} PROPERTIES COMPILE_FLAGS
                              ${compiler_FLAGS})

  if(VOX3DSPACE_VERBOSE GREATER 1)
    foreach(source ${compiler_SOURCES})
      foreach(flag ${compiler_FLAGS})
        message("vox3dspace_set_compiler_flags_for_sources: source:${source} "
                "flag:${flag}")
      endforeach()
    endforeach()
  endif()
endmacro()

# Tests compiler flags stored in list(s) specified by FLAG_LIST_VAR_NAMES, adds
# flags to $VOX3DSPACE_CXX_FLAGS when tests pass. Terminates configuration if
# FLAG_REQUIRED is specified and any flag check fails.
#
# ~~~
# vox3dspace_test_cxx_flag(<FLAG_LIST_VAR_NAMES <flag list variable(s)>>
#                       [FLAG_REQUIRED])
# ~~~
macro(vox3dspace_test_cxx_flag)
  unset(cxx_test_FLAG_LIST_VAR_NAMES)
  unset(cxx_test_FLAG_REQUIRED)
  unset(single_value_args)
  set(optional_args FLAG_REQUIRED)
  set(multi_value_args FLAG_LIST_VAR_NAMES)
  cmake_parse_arguments(cxx_test "${optional_args}" "${single_value_args}"
                        "${multi_value_args}" ${ARGN})

  if(NOT cxx_test_FLAG_LIST_VAR_NAMES)
    vox3dspace_die("vox3dspace_test_cxx_flag: FLAG_LIST_VAR_NAMES required")
  endif()

  unset(cxx_flags)
  foreach(list_var ${cxx_test_FLAG_LIST_VAR_NAMES})
    if(VOX3DSPACE_VERBOSE)
      message("vox3dspace_test_cxx_flag: adding ${list_var} to cxx_flags")
    endif()
    list(APPEND cxx_flags ${${list_var}})
  endforeach()

  if(VOX3DSPACE_VERBOSE)
    message("CXX test: all flags: ${cxx_flags}")
  endif()

  unset(all_cxx_flags)
  list(APPEND all_cxx_flags ${VOX3DSPACE_CXX_FLAGS} ${cxx_flags})

  # Turn off output from check_cxx_source_compiles. Print status directly
  # instead since the logging messages from check_cxx_source_compiles can be
  # quite confusing.
  set(CMAKE_REQUIRED_QUIET TRUE)

  # Run the actual compile test.
  unset(vox3dspace_all_cxx_flags_pass CACHE)
  message("--- Running combined CXX flags test, flags: ${all_cxx_flags}")

  # check_cxx_compiler_flag() requires that the flags are a string. When flags
  # are passed as a list it will remove the list separators, and attempt to run
  # a compile command using list entries concatenated together as a single
  # argument. Avoid the problem by forcing the argument to be a string.
  vox3dspace_set_and_stringify(SOURCE_VARS all_cxx_flags DEST all_cxx_flags)
  check_cxx_compiler_flag("${all_cxx_flags}" vox3dspace_all_cxx_flags_pass)

  if(cxx_test_FLAG_REQUIRED AND NOT vox3dspace_all_cxx_flags_pass)
    vox3dspace_die("Flag test failed for required flag(s): "
              "${all_cxx_flags} and FLAG_REQUIRED specified.")
  endif()

  if(vox3dspace_all_cxx_flags_pass)
    # Test passed: update the global flag list used by the vox3dspace target creation
    # wrappers.
    set(VOX3DSPACE_CXX_FLAGS ${cxx_flags})
    list(REMOVE_DUPLICATES VOX3DSPACE_CXX_FLAGS)

    if(VOX3DSPACE_VERBOSE)
      message("VOX3DSPACE_CXX_FLAGS=${VOX3DSPACE_CXX_FLAGS}")
    endif()

    message("--- Passed combined CXX flags test")
  else()
    message("--- Failed combined CXX flags test, testing flags individually.")

    if(cxx_flags)
      message("--- Testing flags from $cxx_flags: " "${cxx_flags}")
      foreach(cxx_flag ${cxx_flags})
        # Since 3.17.0 check_cxx_compiler_flag() sets a normal variable at
        # parent scope while check_cxx_source_compiles() continues to set an
        # internal cache variable, so we unset both to avoid the failure /
        # success state persisting between checks. This has been fixed in newer
        # CMake releases, but 3.17 is pretty common: we will need this to avoid
        # weird build breakages while the fix propagates.
        unset(cxx_flag_test_passed)
        unset(cxx_flag_test_passed CACHE)
        message("--- Testing flag: ${cxx_flag}")
        check_cxx_compiler_flag("${cxx_flag}" cxx_flag_test_passed)

        if(cxx_flag_test_passed)
          message("--- Passed test for ${cxx_flag}")
        else()
          list(REMOVE_ITEM cxx_flags ${cxx_flag})
          message("--- Failed test for ${cxx_flag}, flag removed.")
        endif()
      endforeach()

      set(VOX3DSPACE_CXX_FLAGS ${cxx_flags})
    endif()
  endif()

  if(VOX3DSPACE_CXX_FLAGS)
    list(REMOVE_DUPLICATES VOX3DSPACE_CXX_FLAGS)
  endif()
endmacro()

# Tests executable linker flags stored in list specified by FLAG_LIST_VAR_NAME,
# adds flags to $VOX3DSPACE_EXE_LINKER_FLAGS when test passes. Terminates
# configuration when flag check fails. vox3dspace_set_cxx_flags() must be called
# before calling this macro because it assumes $VOX3DSPACE_CXX_FLAGS contains only
# valid CXX flags.
#
# vox3dspace_test_exe_linker_flag(<FLAG_LIST_VAR_NAME <flag list variable)>)
macro(vox3dspace_test_exe_linker_flag)
  unset(link_FLAG_LIST_VAR_NAME)
  unset(optional_args)
  unset(multi_value_args)
  set(single_value_args FLAG_LIST_VAR_NAME)
  cmake_parse_arguments(link "${optional_args}" "${single_value_args}"
                        "${multi_value_args}" ${ARGN})

  if(NOT link_FLAG_LIST_VAR_NAME)
    vox3dspace_die("vox3dspace_test_link_flag: FLAG_LIST_VAR_NAME required")
  endif()

  vox3dspace_set_and_stringify(DEST linker_flags SOURCE_VARS
                          ${link_FLAG_LIST_VAR_NAME})

  if(VOX3DSPACE_VERBOSE)
    message("EXE LINKER test: all flags: ${linker_flags}")
  endif()

  # Tests of $VOX3DSPACE_CXX_FLAGS have already passed. Include them with the linker
  # test.
  vox3dspace_set_and_stringify(DEST CMAKE_REQUIRED_FLAGS SOURCE_VARS VOX3DSPACE_CXX_FLAGS)

  # Cache the global exe linker flags.
  if(CMAKE_EXE_LINKER_FLAGS)
    set(cached_CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS})
    vox3dspace_set_and_stringify(DEST CMAKE_EXE_LINKER_FLAGS SOURCE ${linker_flags})
  endif()

  vox3dspace_set_and_stringify(DEST CMAKE_EXE_LINKER_FLAGS SOURCE ${linker_flags}
                          ${CMAKE_EXE_LINKER_FLAGS})

  # Turn off output from check_cxx_source_compiles. Print status directly
  # instead since the logging messages from check_cxx_source_compiles can be
  # quite confusing.
  set(CMAKE_REQUIRED_QUIET TRUE)

  message("--- Running EXE LINKER test for flags: ${linker_flags}")

  unset(linker_flag_test_passed CACHE)
  set(vox3dspace_cxx_main "\nint main() { return 0; }")
  check_cxx_source_compiles("${vox3dspace_cxx_main}" linker_flag_test_passed)

  if(NOT linker_flag_test_passed)
    vox3dspace_die("EXE LINKER test failed.")
  endif()

  message("--- Passed EXE LINKER flag test.")

  # Restore cached global exe linker flags.
  if(cached_CMAKE_EXE_LINKER_FLAGS)
    set(CMAKE_EXE_LINKER_FLAGS ${cached_CMAKE_EXE_LINKER_FLAGS})
  else()
    unset(CMAKE_EXE_LINKER_FLAGS)
  endif()

  list(APPEND VOX3DSPACE_EXE_LINKER_FLAGS ${${link_FLAG_LIST_VAR_NAME}})
  list(REMOVE_DUPLICATES VOX3DSPACE_EXE_LINKER_FLAGS)
endmacro()

# Runs the vox3dspace compiler tests. This macro builds up the list of list var(s)
# that is passed to vox3dspace_test_cxx_flag().
#
# Note: vox3dspace_set_build_definitions() must be called before this macro.
macro(vox3dspace_set_cxx_flags)
  unset(cxx_flag_lists)

  if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    list(APPEND cxx_flag_lists vox3dspace_base_cxx_flags)
  endif()

  # Append clang flags after the base set to allow -Wno* overrides to take
  # effect. Some of the base flags may enable a large set of warnings, e.g.,
  # -Wall.
  if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    list(APPEND cxx_flag_lists vox3dspace_clang_cxx_flags)
  endif()

  if(MSVC)
    list(APPEND cxx_flag_lists vox3dspace_msvc_cxx_flags)
  endif()

  vox3dspace_set_and_stringify(DEST cxx_flags SOURCE_VARS ${cxx_flag_lists})
  if(VOX3DSPACE_VERBOSE)
    message("vox3dspace_set_cxx_flags: internal CXX flags: ${cxx_flags}")
  endif()

  if(VOX3DSPACE_CXX_FLAGS)
    list(APPEND cxx_flag_lists VOX3DSPACE_CXX_FLAGS)
    if(VOX3DSPACE_VERBOSE)
      message("vox3dspace_set_cxx_flags: user CXX flags: ${VOX3DSPACE_CXX_FLAGS}")
    endif()
  endif()

  vox3dspace_set_and_stringify(DEST cxx_flags SOURCE_VARS ${cxx_flag_lists})

  if(cxx_flags)
    vox3dspace_test_cxx_flag(FLAG_LIST_VAR_NAMES ${cxx_flag_lists})
  endif()
endmacro()

# Collects Vox3DSpace built-in and user-specified linker flags and tests them. Halts
# configuration and reports the error when any flags cause the build to fail.
#
# Note: vox3dspace_test_exe_linker_flag() does the real work of setting the flags and
# running the test compile commands.
macro(vox3dspace_set_exe_linker_flags)
  unset(linker_flag_lists)

  if(VOX3DSPACE_VERBOSE)
    message("vox3dspace_set_exe_linker_flags: "
            "vox3dspace_base_exe_linker_flags=${vox3dspace_base_exe_linker_flags}")
  endif()

  if(vox3dspace_base_exe_linker_flags)
    list(APPEND linker_flag_lists vox3dspace_base_exe_linker_flags)
  endif()

  if(linker_flag_lists)
    unset(test_linker_flags)

    if(VOX3DSPACE_VERBOSE)
      message("vox3dspace_set_exe_linker_flags: "
              "linker_flag_lists=${linker_flag_lists}")
    endif()

    vox3dspace_set_and_stringify(DEST test_linker_flags SOURCE_VARS
                            ${linker_flag_lists})
    vox3dspace_test_exe_linker_flag(FLAG_LIST_VAR_NAME test_linker_flags)
  endif()
endmacro()
