cmake_minimum_required(VERSION 3.8)
PROJECT(cmaketest)

# set the project name
set(PROJECT_NAME cmaketest)

# specify the C++ standard 
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D __STDC_LIMIT_MACROS -D __STDC_FORMAT_MACROS -w")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive")
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    message("-- In Release mode")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -static -flto -funroll-loops -fprofile-use")
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("-- In Debug mode")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D DEBUG -Wall -g")
    # set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -pg -fsanitize=undefined -fsanitize=address -fno-omit-frame-pointer")
endif()

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output/bin )   # control add_executable
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output/lib )   # control add_library SHARED
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output/lib )   # control add_library STATIC

set(LIBRARY_INCLUDE_PATH ${PROJECT_SOURCE_DIR}/lib/include)

add_subdirectory(app)
add_subdirectory(lib)
add_subdirectory(lib/unit-test)
