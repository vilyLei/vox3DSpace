if(VOX3DSPACE_CMAKE_VOX3DSPACE_INTRINSICS_CMAKE_)
  return()
endif() # VOX3DSPACE_CMAKE_VOX3DSPACE_INTRINSICS_CMAKE_
set(VOX3DSPACE_CMAKE_VOX3DSPACE_INTRINSICS_CMAKE_ 1)

# Returns the compiler flag for the SIMD intrinsics suffix specified by the
# SUFFIX argument via the variable specified by the VARIABLE argument:
# vox3dspace_get_intrinsics_flag_for_suffix(SUFFIX <suffix> VARIABLE <var name>)
macro(vox3dspace_get_intrinsics_flag_for_suffix)
  unset(intrinsics_SUFFIX)
  unset(intrinsics_VARIABLE)
  unset(optional_args)
  unset(multi_value_args)
  set(single_value_args SUFFIX VARIABLE)
  cmake_parse_arguments(intrinsics "${optional_args}" "${single_value_args}"
                        "${multi_value_args}" ${ARGN})

  if(NOT (intrinsics_SUFFIX AND intrinsics_VARIABLE))
    message(FATAL_ERROR "vox3dspace_get_intrinsics_flag_for_suffix: SUFFIX and "
                        "VARIABLE required.")
  endif()

  if(intrinsics_SUFFIX MATCHES "neon")
    if(NOT MSVC)
      set(${intrinsics_VARIABLE} "${VOX3DSPACE_NEON_INTRINSICS_FLAG}")
    endif()
  elseif(intrinsics_SUFFIX MATCHES "sse4")
    if(NOT MSVC)
      set(${intrinsics_VARIABLE} "-msse4.1")
    endif()
  else()
    message(FATAL_ERROR "vox3dspace_get_intrinsics_flag_for_suffix: Unknown "
                        "instrinics suffix: ${intrinsics_SUFFIX}")
  endif()

  if(VOX3DSPACE_VERBOSE GREATER 1)
    message("vox3dspace_get_intrinsics_flag_for_suffix: "
            "suffix:${intrinsics_SUFFIX} flag:${${intrinsics_VARIABLE}}")
  endif()
endmacro()

# Processes source files specified by SOURCES and adds intrinsics flags as
# necessary: vox3dspace_process_intrinsics_sources(SOURCES <sources>)
#
# Detects requirement for intrinsics flags using source file name suffix.
# Currently supports only SSE4.1.
macro(vox3dspace_process_intrinsics_sources)
  unset(arg_TARGET)
  unset(arg_SOURCES)
  unset(optional_args)
  set(single_value_args TARGET)
  set(multi_value_args SOURCES)
  cmake_parse_arguments(arg "${optional_args}" "${single_value_args}"
                        "${multi_value_args}" ${ARGN})
  if(NOT (arg_TARGET AND arg_SOURCES))
    message(FATAL_ERROR "vox3dspace_process_intrinsics_sources: TARGET and "
                        "SOURCES required.")
  endif()

  if(VOX3DSPACE_ENABLE_SSE4_1 AND vox3dspace_have_sse4)
    unset(sse4_sources)
    list(APPEND sse4_sources ${arg_SOURCES})

    list(FILTER sse4_sources INCLUDE REGEX
         "${vox3dspace_sse4_source_file_suffix}$")

    if(sse4_sources)
      unset(sse4_flags)
      vox3dspace_get_intrinsics_flag_for_suffix(SUFFIX
                                             ${vox3dspace_sse4_source_file_suffix}
                                             VARIABLE sse4_flags)
      if(sse4_flags)
        vox3dspace_set_compiler_flags_for_sources(SOURCES ${sse4_sources} FLAGS
                                               ${sse4_flags})
      endif()
    endif()
  endif()

  if(VOX3DSPACE_ENABLE_NEON AND vox3dspace_have_neon)
    unset(neon_sources)
    list(APPEND neon_sources ${arg_SOURCES})
    list(FILTER neon_sources INCLUDE REGEX
         "${vox3dspace_neon_source_file_suffix}$")

    if(neon_sources AND VOX3DSPACE_NEON_INTRINSICS_FLAG)
      unset(neon_flags)
      vox3dspace_get_intrinsics_flag_for_suffix(SUFFIX
                                             ${vox3dspace_neon_source_file_suffix}
                                             VARIABLE neon_flags)
      if(neon_flags)
        vox3dspace_set_compiler_flags_for_sources(SOURCES ${neon_sources} FLAGS
                                               ${neon_flags})
      endif()
    endif()
  endif()
endmacro()
