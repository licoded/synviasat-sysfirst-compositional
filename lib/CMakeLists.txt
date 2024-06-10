#configure variables
set (LIB_NAME "myfuncs")

#configure directories
set (LIBRARY_MODULE_PATH "${PROJECT_SOURCE_DIR}/lib")
set (LIBRARY_SRC_PATH  "${LIBRARY_MODULE_PATH}/src" )
set (LIBRARY_INCLUDE_PATH  "${LIBRARY_MODULE_PATH}/include")

#set includes
include_directories (${LIBRARY_INCLUDE_PATH})

#set sources
file (GLOB_RECURSE LIB_HEADER_FILES ${LIBRARY_INCLUDE_PATH}/*.h)
file (GLOB_RECURSE LIB_SOURCE_FILES "${LIBRARY_SRC_PATH}/*.cpp" ${LIBRARY_INCLUDE_PATH}/*.cpp)

#set library
add_library (${LIB_NAME}
        STATIC
        ${LIB_SOURCE_FILES}
        ${LIB_HEADER_FILES})
