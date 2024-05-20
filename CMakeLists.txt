cmake_minimum_required(VERSION 3.5)
project(GEMS)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})


set(SOURCES
    Source.cpp
)

add_executable(GEMS ${SOURCES})

target_link_libraries(GEMS PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/GLFW/libglfw3.a)
