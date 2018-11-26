cmake_minimum_required(VERSION 3.4.1)
project(dummy)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(.)

add_executable(${PROJECT_NAME} dummy.c)
target_link_libraries(${PROJECT_NAME} common_resource)
