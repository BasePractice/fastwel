cmake_minimum_required(VERSION 3.4.1)
project(p55)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(.)
include_directories(${MSDOS_INCLUDE})

if (WIN32)
    add_definitions(-Dinterrupt=__stdcall -Dfar=_FAR)
endif ()

add_executable(${PROJECT_NAME}i p55i.c)
target_link_libraries(${PROJECT_NAME}i common_resource)
