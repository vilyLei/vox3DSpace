cmake_minimum_required(VERSION 3.21)
# project(cudaApp)
project(cudaApp LANGUAGES CUDA)

set(CMAKE_CXX_STANDARD 17)

find_package(CUDA)
# find_package(CUDAToolkit REQUIRED)

# add_executable(${PROJECT_NAME} main.cc)
add_executable(${PROJECT_NAME} 
"exception.h"
"helper_cuda.h"
"helper_functions.h"
"helper_image.h"
"helper_string.h"
"asyncAPI.cu"
)

# target_link_libraries(${PROJECT_NAME} CUDA::cudart)
target_link_libraries(${PROJECT_NAME})