cmake_minimum_required(VERSION 3.8)
PROJECT(ltlfsyn)

# set the project name
set(PROJECT_NAME ltlfsyn)

include(etc/build_type.cmake)
include(etc/cflags.cmake)
# include(etc/scanners.cmake)

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output/bin )   # control add_executable
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output/lib )   # control add_library SHARED
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output/lib )   # control add_library STATIC

set(LIBRARY_INCLUDE_PATH ${PROJECT_SOURCE_DIR}/lib/include)

add_subdirectory(app)
add_subdirectory(lib)
add_subdirectory(lib/unit-test)
