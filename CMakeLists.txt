cmake_minimum_required(VERSION 3.8)
PROJECT(ltlfsyn)

# set the project name
set(PROJECT_NAME ltlfsyn)

if(${CMAKE_GENERATOR} STREQUAL "Unix Makefiles")
  set(CMAKE_MAKE_PROGRAM "${PROJECT_SOURCE_DIR}/scripts/make-parallel.sh" CACHE STRING "" FORCE)
endif()

set(LIBRARY_APPLICATION_PATH ${PROJECT_SOURCE_DIR}/app)
set(LIBRARY_DEPENDS_PATH ${PROJECT_SOURCE_DIR}/lib/deps)
set(LIBRARY_INCLUDE_PATH ${PROJECT_SOURCE_DIR}/lib/include)
set(LIB_SYFT ${LIBRARY_DEPENDS_PATH}/syft/libsynthesis.a)
set(LIB_MONA
  ${LIBRARY_DEPENDS_PATH}/mona/libmonadfa.a
  ${LIBRARY_DEPENDS_PATH}/mona/libmonamem.a
  ${LIBRARY_DEPENDS_PATH}/mona/libmonabdd.a
)
set(LIB_CUDD ${LIBRARY_DEPENDS_PATH}/cudd/libcudd.a)
# set(EXTERNAL_LIBS ${LIB_SYFT} ${LIB_MONA} ${LIB_CUDD} z)

include(etc/build_type.cmake)
include(etc/cflags.cmake)
include(etc/scanners.cmake)

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output/bin )   # control add_executable
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output/lib )   # control add_library SHARED
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output/lib )   # control add_library STATIC

add_subdirectory(lib)
add_subdirectory(app)