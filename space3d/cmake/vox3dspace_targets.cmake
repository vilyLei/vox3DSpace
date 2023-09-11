
if(VOX3DSPACE_CMAKE_VOX3DSPACE_TARGETS_CMAKE_)
  return()
endif() # VOX3DSPACE_CMAKE_VOX3DSPACE_TARGETS_CMAKE_
set(VOX3DSPACE_CMAKE_VOX3DSPACE_TARGETS_CMAKE_ 1)

# 定义延迟加载的宏
macro(vox3dspace_add_delayload_flags flagsVar)
  set(dlls "${ARGN}")
  foreach(dll ${dlls})
    set(${flagsVar} "${${flagsVar}} /DELAYLOAD:${dll}.dll")
  endforeach()
endmacro()

# some cases
# vox3dspace_add_delayload_flags(CMAKE_EXE_LINKER_FLAGS dll1 dll2)

# Resets list variables used to track vox3dspace targets.
macro(vox3dspace_reset_target_lists)
  unset(vox3dspace_targets)
  unset(vox3dspace_exe_targets)
  unset(vox3dspace_lib_targets)
  unset(vox3dspace_objlib_targets)
  unset(vox3dspace_module_targets)
  unset(vox3dspace_sources)
  unset(vox3dspace_test_targets)
endmacro()

# Creates an executable target. The target name is passed as a parameter to the
# NAME argument, and the sources passed as a parameter to the SOURCES argument:
# vox3dspace_add_executable(NAME <name> SOURCES <sources> [optional args])
#
# Optional args:
# cmake-format: off
#   - OUTPUT_NAME: Override output file basename. Target basename defaults to
#     NAME.
#   - TEST: Flag. Presence means treat executable as a test.
#   - DEFINES: List of preprocessor macro definitions.
#   - INCLUDES: list of include directories for the target.
#   - COMPILE_FLAGS: list of compiler flags for the target.
#   - LINK_FLAGS: List of linker flags for the target.
#   - OBJLIB_DEPS: List of CMake object library target dependencies.
#   - LIB_DEPS: List of CMake library dependencies.
# cmake-format: on
#
# Sources passed to this macro are added to $vox3dspace_test_sources when TEST is
# specified. Otherwise sources are added to $vox3dspace_sources.
#
# Targets passed to this macro are always added to the $vox3dspace_targets list. When
# TEST is specified targets are also added to the $vox3dspace_test_targets list.
# Otherwise targets are added to $vox3dspace_exe_targets.
macro(vox3dspace_add_executable)
  unset(exe_TEST)
  unset(exe_TEST_DEFINES_MAIN)
  unset(exe_NAME)
  unset(exe_OUTPUT_NAME)
  unset(exe_SOURCES)
  unset(exe_DEFINES)
  unset(exe_INCLUDES)
  unset(exe_LIB_INCLUDES)
  unset(exe_EXEC_SYS)
  unset(exe_COMPILE_FLAGS)
  unset(exe_RPOJECT_FLAGS)
  unset(exe_LINK_FLAGS)
  unset(exe_OBJLIB_DEPS)
  unset(exe_LIB_DEPS)
  unset(exe_FOLDER)
  set(optional_args TEST)
  set(single_value_args NAME OUTPUT_NAME)
  set(multi_value_args SOURCES DEFINES INCLUDES LIB_INCLUDES EXEC_SYS COMPILE_FLAGS RPOJECT_FLAGS LINK_FLAGS
                       OBJLIB_DEPS LIB_DEPS FOLDER)

  cmake_parse_arguments(exe "${optional_args}" "${single_value_args}"
                        "${multi_value_args}" ${ARGN})

  if(VOX3DSPACE_VERBOSE GREATER 1)
    message("--------- vox3dspace_add_executable ---------\n"
            "exe_TEST=${exe_TEST}\n"
            "exe_TEST_DEFINES_MAIN=${exe_TEST_DEFINES_MAIN}\n"
            "exe_NAME=${exe_NAME}\n"
            "exe_OUTPUT_NAME=${exe_OUTPUT_NAME}\n"
            "exe_SOURCES=${exe_SOURCES}\n"
            "exe_DEFINES=${exe_DEFINES}\n"
            "exe_INCLUDES=${exe_INCLUDES}\n"
            "exe_LIB_INCLUDES=${exe_LIB_INCLUDES}\n"
            "exe_EXEC_SYS=${exe_EXEC_SYS}\n"
            "exe_COMPILE_FLAGS=${exe_COMPILE_FLAGS}\n"
            "exe_RPOJECT_FLAGS=${exe_RPOJECT_FLAGS}\n"
            "exe_LINK_FLAGS=${exe_LINK_FLAGS}\n"
            "exe_OBJLIB_DEPS=${exe_OBJLIB_DEPS}\n"
            "exe_LIB_DEPS=${exe_LIB_DEPS}\n"
            "exe_FOLDER=${exe_FOLDER}\n"
            "------------------------------------------\n")
  endif()

  if(NOT (exe_NAME AND exe_SOURCES))
    message(FATAL_ERROR "vox3dspace_add_executable: NAME and SOURCES required.")
  endif()

  list(APPEND vox3dspace_targets ${exe_NAME})
  if(exe_TEST)
    list(APPEND vox3dspace_test_targets ${exe_NAME})
    list(APPEND vox3dspace_test_sources ${exe_SOURCES})
  else()
    list(APPEND vox3dspace_exe_targets ${exe_NAME})
    list(APPEND vox3dspace_sources ${exe_SOURCES})
  endif()

  if(exe_LIB_INCLUDES)
    link_directories(${exe_LIB_INCLUDES})
  endif()

  add_executable(${exe_NAME} ${exe_EXEC_SYS} ${exe_SOURCES})
  # thanks: https://cmake.org/cmake/help/latest/manual/cmake-compile-features.7.html#requiring-language-standards
  # thanks: https://cmake.org/cmake/help/latest/prop_gbl/CMAKE_CXX_KNOWN_FEATURES.html#prop_gbl:CMAKE_CXX_KNOWN_FEATURES
  # target_compile_features(${exe_NAME} PRIVATE cxx_std_17)
  target_compile_features(${exe_NAME} PRIVATE ${CXX_STD_VERSION})

  if(NOT EMSCRIPTEN)
    set_target_properties(${exe_NAME} PROPERTIES VERSION ${VOX3DSPACE_VERSION})
  endif()

  if(exe_OUTPUT_NAME)
    set_target_properties(${exe_NAME} PROPERTIES OUTPUT_NAME ${exe_OUTPUT_NAME})
  endif()

  vox3dspace_process_intrinsics_sources(TARGET ${exe_NAME} SOURCES ${exe_SOURCES})

  if(exe_DEFINES)
    target_compile_definitions(${exe_NAME} PRIVATE ${exe_DEFINES})
  endif()
  
  if(exe_FOLDER)
    set_target_properties(${exe_NAME} PROPERTIES FOLDER ${exe_FOLDER})
  endif()

  message(STATUS "--- info exe_INCLUDES: ${exe_INCLUDES}")
  if(exe_INCLUDES)
    target_include_directories(${exe_NAME} PRIVATE ${exe_INCLUDES})
  endif()

  if(exe_COMPILE_FLAGS OR exe_RPOJECT_FLAGS)
    target_compile_options(${exe_NAME}
                           PRIVATE ${exe_COMPILE_FLAGS} ${exe_RPOJECT_FLAGS})
  endif()

  if(exe_LINK_FLAGS OR VOX3DSPACE_EXE_LINKER_FLAGS)
    if(${CMAKE_VERSION} VERSION_LESS "3.13")
      list(APPEND exe_LINK_FLAGS "${VOX3DSPACE_EXE_LINKER_FLAGS}")
      # LINK_FLAGS is managed as a string.
      vox3dspace_set_and_stringify(SOURCE "${exe_LINK_FLAGS}" DEST exe_LINK_FLAGS)
      set_target_properties(${exe_NAME}
                            PROPERTIES LINK_FLAGS "${exe_LINK_FLAGS}")
    else()
      target_link_options(${exe_NAME} PRIVATE ${exe_LINK_FLAGS}
                          ${VOX3DSPACE_EXE_LINKER_FLAGS})
    endif()
  endif()

  if(exe_OBJLIB_DEPS)
    foreach(objlib_dep ${exe_OBJLIB_DEPS})
      target_sources(${exe_NAME} PRIVATE $<TARGET_OBJECTS:${objlib_dep}>)
    endforeach()
  endif()

  if(CMAKE_THREAD_LIBS_INIT)
    list(APPEND exe_LIB_DEPS ${CMAKE_THREAD_LIBS_INIT})
  endif()

  if(BUILD_SHARED_LIBS AND (MSVC OR WIN32))
    target_compile_definitions(${exe_NAME} PRIVATE "VOX3DSPACE_BUILDING_DLL=0")
  endif()

  if(exe_LIB_DEPS)
    unset(exe_static)
    if("${CMAKE_EXE_LINKER_FLAGS} ${VOX3DSPACE_EXE_LINKER_FLAGS}" MATCHES "static")
      set(exe_static ON)
    endif()

    if(exe_static AND CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
      # Third party dependencies can introduce dependencies on system and test
      # libraries. Since the target created here is an executable, and CMake
      # does not provide a method of controlling order of link dependencies,
      # wrap all of the dependencies of this target in start/end group flags to
      # ensure that dependencies of third party targets can be resolved when
      # those dependencies happen to be resolved by dependencies of the current
      # target.
      list(INSERT exe_LIB_DEPS 0 -Wl,--start-group)
      list(APPEND exe_LIB_DEPS -Wl,--end-group)
    endif()
    # message("XXXXXX 4 exe_NAME: ${exe_NAME}, exe_LIB_DEPS: ${exe_LIB_DEPS}")
    target_link_libraries(${exe_NAME} PRIVATE ${exe_LIB_DEPS})
  endif()
endmacro()

# Creates a library target of the specified type. The target name is passed as a
# parameter to the NAME argument, the type as a parameter to the TYPE argument,
# and the sources passed as a parameter to the SOURCES argument:
# vox3dspace_add_library(NAME <name> TYPE <type> SOURCES <sources> [optional args])
#
# Optional args:
# cmake-format: off
#   - OUTPUT_NAME: Override output file basename. Target basename defaults to
#     NAME. OUTPUT_NAME is ignored when BUILD_SHARED_LIBS is enabled and CMake
#     is generating a build for which MSVC is true. This is to avoid output
#     basename collisions with DLL import libraries.
#   - TEST: Flag. Presence means treat library as a test.
#   - DEFINES: List of preprocessor macro definitions.
#   - INCLUDES: list of include directories for the target.
#   - COMPILE_FLAGS: list of compiler flags for the target.
#   - LINK_FLAGS: List of linker flags for the target.
#   - OBJLIB_DEPS: List of CMake object library target dependencies.
#   - LIB_DEPS: List of CMake library dependencies.
#   - PUBLIC_INCLUDES: List of include paths to export to dependents.
# cmake-format: on
#
# Sources passed to the macro are added to the lists tracking vox3dspace sources:
# cmake-format: off
#   - When TEST is specified sources are added to $vox3dspace_test_sources.
#   - Otherwise sources are added to $vox3dspace_sources.
# cmake-format: on
#
# Targets passed to this macro are added to the lists tracking vox3dspace targets:
# cmake-format: off
#   - Targets are always added to $vox3dspace_targets.
#   - When the TEST flag is specified, targets are added to
#     $vox3dspace_test_targets.
#   - When TEST is not specified:
#     - Libraries of type SHARED are added to $vox3dspace_dylib_targets.
#     - Libraries of type OBJECT are added to $vox3dspace_objlib_targets.
#     - Libraries of type STATIC are added to $vox3dspace_lib_targets.
# cmake-format: on
macro(vox3dspace_add_library)
  unset(lib_TEST)
  unset(lib_NAME)
  unset(lib_OUTPUT_NAME)
  unset(lib_TYPE)
  unset(lib_SOURCES)
  unset(lib_DEFINES)
  unset(lib_INCLUDES)
  unset(lib_COMPILE_FLAGS)
  unset(lib_LINK_FLAGS)
  unset(lib_OBJLIB_DEPS)
  unset(lib_LIB_DEPS)
  unset(lib_PUBLIC_INCLUDES)
  unset(lib_TARGET_PROPERTIES)
  set(optional_args TEST)
  set(single_value_args NAME OUTPUT_NAME TYPE)
  set(multi_value_args SOURCES DEFINES INCLUDES COMPILE_FLAGS LINK_FLAGS
                       OBJLIB_DEPS LIB_DEPS PUBLIC_INCLUDES TARGET_PROPERTIES)

  cmake_parse_arguments(lib "${optional_args}" "${single_value_args}"
                        "${multi_value_args}" ${ARGN})

  if(VOX3DSPACE_VERBOSE GREATER 1)
    message("--------- vox3dspace_add_library ---------\n"
            "lib_TEST=${lib_TEST}\n"
            "lib_NAME=${lib_NAME}\n"
            "lib_OUTPUT_NAME=${lib_OUTPUT_NAME}\n"
            "lib_TYPE=${lib_TYPE}\n"
            "lib_SOURCES=${lib_SOURCES}\n"
            "lib_DEFINES=${lib_DEFINES}\n"
            "lib_INCLUDES=${lib_INCLUDES}\n"
            "lib_COMPILE_FLAGS=${lib_COMPILE_FLAGS}\n"
            "lib_LINK_FLAGS=${lib_LINK_FLAGS}\n"
            "lib_OBJLIB_DEPS=${lib_OBJLIB_DEPS}\n"
            "lib_LIB_DEPS=${lib_LIB_DEPS}\n"
            "lib_PUBLIC_INCLUDES=${lib_PUBLIC_INCLUDES}\n"
            "---------------------------------------\n")
  endif()

  if(NOT (lib_NAME AND lib_TYPE))
    message(FATAL_ERROR "vox3dspace_add_library: NAME and TYPE required.")
  endif()

  list(APPEND vox3dspace_targets ${lib_NAME})
  if(lib_TEST)
    list(APPEND vox3dspace_test_targets ${lib_NAME})
    list(APPEND vox3dspace_test_sources ${lib_SOURCES})
  else()
    list(APPEND vox3dspace_sources ${lib_SOURCES})
    if(lib_TYPE STREQUAL MODULE)
      list(APPEND vox3dspace_module_targets ${lib_NAME})
    elseif(lib_TYPE STREQUAL OBJECT)
      list(APPEND vox3dspace_objlib_targets ${lib_NAME})
    elseif(lib_TYPE STREQUAL SHARED)
      list(APPEND vox3dspace_dylib_targets ${lib_NAME})
    elseif(lib_TYPE STREQUAL STATIC)
      list(APPEND vox3dspace_lib_targets ${lib_NAME})
    else()
      message(WARNING "vox3dspace_add_library: Unhandled type: ${lib_TYPE}")
    endif()
  endif()

  # cuda_add_library(${lib_NAME} ${lib_TYPE} ${lib_SOURCES})

  add_library(${lib_NAME} ${lib_TYPE} ${lib_SOURCES})
  target_compile_features(${lib_NAME} PUBLIC cxx_std_11)
  target_include_directories(${lib_NAME} PUBLIC $<INSTALL_INTERFACE:include>)
  if(lib_SOURCES)
    vox3dspace_process_intrinsics_sources(TARGET ${lib_NAME} SOURCES ${lib_SOURCES})
  endif()

  if(lib_OUTPUT_NAME)
    if(NOT (BUILD_SHARED_LIBS AND MSVC))
      set_target_properties(${lib_NAME}
                            PROPERTIES OUTPUT_NAME ${lib_OUTPUT_NAME})
    endif()
  endif()

  target_compile_features(${lib_NAME} PRIVATE ${CXX_STD_VERSION})

  if(lib_DEFINES)
    target_compile_definitions(${lib_NAME} PRIVATE ${lib_DEFINES})
  endif()

  if(lib_INCLUDES)
    target_include_directories(${lib_NAME} PRIVATE ${lib_INCLUDES})
  endif()

  if(lib_PUBLIC_INCLUDES)
    target_include_directories(${lib_NAME} PUBLIC ${lib_PUBLIC_INCLUDES})
  endif()

  if(lib_COMPILE_FLAGS OR VOX3DSPACE_CXX_FLAGS)
    target_compile_options(${lib_NAME}
                           PRIVATE ${lib_COMPILE_FLAGS} ${VOX3DSPACE_CXX_FLAGS})
  endif()

  if(lib_LINK_FLAGS)
    set_target_properties(${lib_NAME} PROPERTIES LINK_FLAGS ${lib_LINK_FLAGS})
  endif()

  if(lib_OBJLIB_DEPS)
    foreach(objlib_dep ${lib_OBJLIB_DEPS})
      target_sources(${lib_NAME} PRIVATE $<TARGET_OBJECTS:${objlib_dep}>)
    endforeach()
  endif()

  if(lib_LIB_DEPS)
    if(lib_TYPE STREQUAL STATIC)
      set(link_type PUBLIC)
    else()
      set(link_type PRIVATE)
      if(lib_TYPE STREQUAL SHARED AND CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        # The vox3dspace shared object uses the static vox3dspace as input to turn it into
        # a shared object. Include everything from the static library in the
        # shared object.
        if(APPLE)
          list(INSERT lib_LIB_DEPS 0 -Wl,-force_load)
        else()
          list(INSERT lib_LIB_DEPS 0 -Wl,--whole-archive)
          list(APPEND lib_LIB_DEPS -Wl,--no-whole-archive)
        endif()
      endif()
    endif()
    target_link_libraries(${lib_NAME} ${link_type} ${lib_LIB_DEPS})
  endif()

  if(NOT MSVC AND lib_NAME MATCHES "^lib")
    # Non-MSVC generators prepend lib to static lib target file names. Libvox3dspace
    # already includes lib in its name. Avoid naming output files liblib*.
    set_target_properties(${lib_NAME} PROPERTIES PREFIX "")
  endif()

  if(NOT EMSCRIPTEN)
    # VERSION and SOVERSION as necessary
    if((lib_TYPE STREQUAL BUNDLE OR lib_TYPE STREQUAL SHARED) AND NOT MSVC)
      set_target_properties(${lib_NAME}
                            PROPERTIES VERSION ${VOX3DSPACE_SOVERSION} SOVERSION
                                       ${VOX3DSPACE_SOVERSION_MAJOR})
    endif()
  endif()

  if(BUILD_SHARED_LIBS AND (MSVC OR WIN32))
    if(lib_TYPE STREQUAL SHARED)
      target_compile_definitions(${lib_NAME} PRIVATE "VOX3DSPACE_BUILDING_DLL=1")
    else()
      target_compile_definitions(${lib_NAME} PRIVATE "VOX3DSPACE_BUILDING_DLL=0")
    endif()
  endif()

  # Determine if $lib_NAME is a header only target.
  unset(sources_list)
  if(lib_SOURCES)
    set(sources_list ${lib_SOURCES})
    list(FILTER sources_list INCLUDE REGEX cc$)
  endif()

  if(NOT sources_list)
    if(NOT XCODE)
      # This is a header only target. Tell CMake the link language.
      set_target_properties(${lib_NAME} PROPERTIES LINKER_LANGUAGE CXX)
    else()
      # The Xcode generator ignores LINKER_LANGUAGE. Add a dummy cc file.
      vox3dspace_create_dummy_source_file(TARGET ${lib_NAME} BASENAME ${lib_NAME})
    endif()
  endif()
endmacro()
