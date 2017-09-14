# (C) 2017 by T. Burt

# stop if cmake version below 2.8
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

# project name
project(lderive)

enable_language(C)

set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)
set(VERSION_PATCH 0)

include_directories ("${PROJECT_SOURCE_DIR}/include")
add_subdirectory (src) 

# add the executable
add_executable (Test test/main.test.c)
target_link_libraries (Test lderive)
target_link_libraries (Test m)
