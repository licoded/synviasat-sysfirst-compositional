#configure variables
set (LIB_NAME "minisat")

#configure directories
set (LIBRARY_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")

#set includes
include_directories (${LIBRARY_MODULE_PATH})

#set sources
file (GLOB_RECURSE LIB_HEADER_FILES ${LIBRARY_MODULE_PATH}/*.h)
set (LIB_SOURCE_FILES ${LIBRARY_MODULE_PATH}/core/Solver.cc)

#set library
add_library (${LIB_NAME}
        STATIC
        ${LIB_SOURCE_FILES}
        ${LIB_HEADER_FILES})
