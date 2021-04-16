project(depthSR_gpu)
cmake_minimum_required(VERSION 3.5)

# packages
find_package(CUDA)

INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/include")

# nvcc flags -g for debug
#set(CUDA_NVCC_FLAGS -O3;-G;-g)

# set(CUDA_NVCC_FLAGS -gencode arch=compute_20,code=sm_20;-G;-g)
# set(CUDA_NVCC_FLAGS -gencode arch=compute_52,code=sm_52;-G;-g;-rdc=true)

file(GLOB_RECURSE CURRENT_HEADERS  *.h *.hpp *.cuh)
file(GLOB CURRENT_SOURCES  *.cpp *.cu)

source_group("Include" FILES ${CURRENT_HEADERS})
source_group("Source" FILES ${CURRENT_SOURCES})

# cuda_add_library(gpu SHARED ${CURRENT_HEADERS} ${CURRENT_SOURCES})
cuda_add_library(depthSR_gpu STATIC ${CURRENT_HEADERS} ${CURRENT_SOURCES})