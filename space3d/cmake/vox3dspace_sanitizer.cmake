if(VOX3DSPACE_CMAKE_VOX3DSPACE_SANITIZER_CMAKE_)
  return()
endif() # VOX3DSPACE_CMAKE_VOX3DSPACE_SANITIZER_CMAKE_
set(VOX3DSPACE_CMAKE_VOX3DSPACE_SANITIZER_CMAKE_ 1)

# Handles the details of enabling sanitizers.
macro(vox3dspace_configure_sanitizer)
  if(VOX3DSPACE_SANITIZE AND NOT EMSCRIPTEN AND NOT MSVC)
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
      if(VOX3DSPACE_SANITIZE MATCHES "cfi")
        list(APPEND SAN_CXX_FLAGS "-flto" "-fno-sanitize-trap=cfi")
        list(APPEND SAN_LINKER_FLAGS "-flto" "-fno-sanitize-trap=cfi"
                    "-fuse-ld=gold")
      endif()

      if(${CMAKE_SIZEOF_VOID_P} EQUAL 4
         AND VOX3DSPACE_SANITIZE MATCHES "integer|undefined")
        list(APPEND SAN_LINKER_FLAGS "--rtlib=compiler-rt" "-lgcc_s")
      endif()
    endif()

    list(APPEND SAN_CXX_FLAGS "-fsanitize=${VOX3DSPACE_SANITIZE}")
    list(APPEND SAN_LINKER_FLAGS "-fsanitize=${VOX3DSPACE_SANITIZE}")

    # Make sanitizer callstacks accurate.
    list(APPEND SAN_CXX_FLAGS "-fno-omit-frame-pointer")
    list(APPEND SAN_CXX_FLAGS "-fno-optimize-sibling-calls")

    vox3dspace_test_cxx_flag(FLAG_LIST_VAR_NAMES SAN_CXX_FLAGS FLAG_REQUIRED)
    vox3dspace_test_exe_linker_flag(FLAG_LIST_VAR_NAME SAN_LINKER_FLAGS)
  endif()
endmacro()
