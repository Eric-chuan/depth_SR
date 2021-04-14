cmake_minimum_required(VERSION 3.5)

project(depthSR LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/include")

file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
FIND_PACKAGE(OpenCV REQUIRED HINTS "$ENV{HOME}/lib/opencv3_4_11/share/OpenCV")

add_executable(depthSR ${SOURCES})

TARGET_LINK_LIBRARIES(depthSR dl)
TARGET_LINK_LIBRARIES(depthSR m)
target_link_libraries(depthSR Threads::Threads)
target_link_libraries(depthSR ${OpenCV_LIBS})
target_link_libraries(depthSR ${EXTRA_LIBS})