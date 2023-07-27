cmake_minimum_required(VERSION 3.21)
# project (cmakeSubTest C CXX)


function(scan_dirs)
    set(options)
    set(oneValueArgs LIST DIR)
    set(multiValueArgs)

    cmake_parse_arguments(TARGET "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT EXISTS ${TARGET_DIR})
        message(FATAL_ERROR "Directory not found `${TARGET_DIR}`")
    endif()

    file(GLOB DIR_FILES RELATIVE ${TARGET_DIR} ${TARGET_DIR}/*)

    set(DIR_LIST)

    foreach(FILE_NAME ${DIR_FILES})
        if(IS_DIRECTORY ${TARGET_DIR}/${FILE_NAME})
            list(APPEND DIR_LIST ${FILE_NAME})
        endif()
    endforeach()

    set(${TARGET_LIST} ${DIR_LIST} PARENT_SCOPE)
endfunction()
scan_dirs(
    DIR ${CMAKE_CURRENT_SOURCE_DIR}
    LIST SUB_DIRS)

# List of all samples
set(TOTAL_SAMPLE_ID_LIST)

# For each directory, add all the samples that exist within
function(add_sub_dirs DIRECTORY)
    scan_dirs(
        DIR ${DIRECTORY}
        LIST DIR_LIST)

    foreach(CURR_DIR ${DIR_LIST})
        if(EXISTS "${DIRECTORY}/${CURR_DIR}/CMakeLists.txt")
            add_subdirectory(${DIRECTORY}/${CURR_DIR})
            list(APPEND TOTAL_SAMPLE_ID_LIST ${CURR_DIR})
        else()
            add_sub_dirs(${DIRECTORY}/${CURR_DIR})
        endif()
        set(TOTAL_SAMPLE_ID_LIST ${TOTAL_SAMPLE_ID_LIST} PARENT_SCOPE)
    endforeach()
endfunction(add_sub_dirs)

foreach(SUB_DIR ${SUB_DIRS})
    add_sub_dirs(${CMAKE_CURRENT_SOURCE_DIR}/${SUB_DIR})
endforeach()

message("CMAKE_BUILD_TYPE=" ${CMAKE_BUILD_TYPE})
set(CMAKE_BUILD_TYPE DEBUG)
if(CMAKE_BUILD_TYPE STREQUAL "DEBUG")
  set(DEBUG_OFF OFF)
else()
  set(DEBUG_OFF ON)
endif()

message("DEBUG_OFF=" ${DEBUG_OFF})

add_definitions(-DUSE_LOG0 -DUSE_LOG1)
add_definitions(-DLIB_VERSION_NUMBER=0x02010800)

set(src_lsit src/main.cc)
set(geom_src_lsit geomsrc/main.cc)
message(STATUS "BINARY dir: " ${cmakeSubTest_BINARY_DIR})
message(STATUS "SOURCE dir: " ${cmakeSubTest_SOURCE_DIR})
# set_property(GLOBAL PROPERTY USE_FOLDERS ON)
add_executable(cmakeSubTest_src ${src_lsit})
set_target_properties(cmakeSubTest_src PROPERTIES FOLDER "CmakeSubTest")
# set_property(TARGET cmakeSubTest_src PROPERTY FOLDER "CmakeSubTest")
add_executable(cmakeSubTest_geom_src ${geom_src_lsit})
set_target_properties(cmakeSubTest_geom_src PROPERTIES FOLDER "CmakeSubTest")
# set_property(TARGET cmakeSubTest_geom_src PROPERTY FOLDER "CmakeSubTest")