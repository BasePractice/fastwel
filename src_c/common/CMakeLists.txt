cmake_minimum_required(VERSION 3.4.1)
project(common)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(.)

add_library(${PROJECT_NAME}_resource resource.c resource.h msdos_emul.c)
