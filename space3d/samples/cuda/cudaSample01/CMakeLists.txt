cmake_minimum_required(VERSION 3.21)
# project(cudaApp)
project(cudaApp LANGUAGES CUDA)

set(CMAKE_CXX_STANDARD 17)

find_package(CUDAToolkit REQUIRED)

# add_executable(${PROJECT_NAME} main.cc)
add_executable(${PROJECT_NAME} main.cu)

target_link_libraries(${PROJECT_NAME} CUDA::cudart)