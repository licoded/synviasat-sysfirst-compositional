#configure variables
set (LIB_NAME "formula")

#configure directories
set (LIBRARY_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")

#set includes
include_directories (${LIBRARY_MODULE_PATH})
include_directories (${LIBRARY_INCLUDE_PATH})
include_directories ("${LIBRARY_INCLUDE_PATH}/minisat")

#set sources
file (GLOB_RECURSE LIB_HEADER_FILES ${LIBRARY_MODULE_PATH}/*.h)
file (GLOB LIB_SOURCE_FILES
        "${LIBRARY_MODULE_PATH}/*.cpp"
        "${LIBRARY_MODULE_PATH}/*.c")

#set library
add_library (${LIB_NAME}
        STATIC
        ${LIB_SOURCE_FILES}
        ${LIB_HEADER_FILES})

target_link_libraries(${LIB_NAME} ltlparser util minisat z)