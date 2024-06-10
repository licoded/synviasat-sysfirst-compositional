# Set default build type to Debug if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type (Debug or Release)" FORCE)
endif()

# Print the current build type
message(STATUS "Building in '${CMAKE_BUILD_TYPE}' mode.")

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    message("-- In Release mode")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -static -flto -funroll-loops -fprofile-use")
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("-- In Debug mode")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D DEBUG -Wall -g")
    # set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -pg -fsanitize=undefined -fsanitize=address -fno-omit-frame-pointer")
endif()
