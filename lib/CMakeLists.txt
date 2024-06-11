#configure variables
set (LIB_NAME "syn")

#set includes
include_directories (${LIBRARY_INCLUDE_PATH})

#set sources
file (GLOB_RECURSE LIB_HEADER_FILES ${LIBRARY_INCLUDE_PATH}/*.h)
file (GLOB_RECURSE LIB_SOURCE_FILES
        "${LIBRARY_INCLUDE_PATH}/*.cpp"
        "${LIBRARY_INCLUDE_PATH}/*.c"
        "${LIBRARY_INCLUDE_PATH}/*.cc")

MESSAGE(STATUS "LIB_SOURCE_FILES: " ${LIB_SOURCE_FILES})

#set library
add_library (${LIB_NAME}
        STATIC
        ${LIB_SOURCE_FILES})
target_link_libraries(${LIB_NAME} ${LIB_CUDD})
target_link_libraries(${LIB_NAME} z)

# add_subdirectory(include/minisat)
# add_subdirectory(include/ltlparser)
# add_subdirectory(include/formula)
# add_subdirectory(include/ltlfsat)
# add_subdirectory(unit-test)

# add_dependencies(formula ltlparser)
# add_dependencies(ltlfsat minisat)
# add_dependencies(ltlfsat formula)
