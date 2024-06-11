#configure variables
set (LIB_NAME "deps")

#set includes
include_directories (${CMAKE_CURRENT_SOURCE_DIR})
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/minisat)

set (FORMULA_DIR        ${CMAKE_CURRENT_SOURCE_DIR}/formula)
set (LTLPARSER_DIR      ${CMAKE_CURRENT_SOURCE_DIR}/ltlparser)
set (MINISAT_DIR        ${CMAKE_CURRENT_SOURCE_DIR}/minisat)
set (UTIL_DIR           ${CMAKE_CURRENT_SOURCE_DIR}/util)

#set sources
file (GLOB LIB_SOURCE_FILES
        ${MINISAT_DIR}/core/Solver.cc
        ${LTLPARSER_DIR}/*.c
        ${FORMULA_DIR}/*.cpp
        ${UTIL_DIR}/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)
# list(REMOVE_ITEM LIB_SOURCE_FILES "${LTLPARSER_DIR}/utility.cpp") # don't need it, as others are .c files

#set library
add_library (${LIB_NAME} STATIC ${LIB_SOURCE_FILES})
target_link_libraries(${LIB_NAME} z)
