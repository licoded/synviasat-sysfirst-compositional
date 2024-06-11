#configure variables
set (LIB_NAME "syn")

#configure directories

#set includes
include_directories (${LIBRARY_MODULE_PATH})

#set sources
file (GLOB_RECURSE LIB_HEADER_FILES ${LIBRARY_MODULE_PATH}/*.h)
file (GLOB_RECURSE LIB_SOURCE_FILES
        "${LIBRARY_SOURCES_PATH}/*.cpp"
        "${LIBRARY_SOURCES_PATH}/*.c"
        "${LIBRARY_SOURCES_PATH}/*.cc"
        "${LIBRARY_MODULE_PATH}/*.cpp"
        "${LIBRARY_MODULE_PATH}/*.c"
        "${LIBRARY_MODULE_PATH}/*.cc")

MESSAGE(STATUS "LIB_SOURCE_FILES: " ${LIB_SOURCE_FILES})

#set library
add_library (${LIB_NAME}
        STATIC
        -lsynutil
        ${LIB_SOURCE_FILES})

# target_link_libraries(${LIB_NAME} synutil)
