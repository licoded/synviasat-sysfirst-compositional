#configure variables
set (LIB_NAME "util")

#configure directories
set (LIBRARY_MODULE_PATH "${PROJECT_SOURCE_DIR}/lib")
set (LIBRARY_SRC_PATH  "${LIBRARY_MODULE_PATH}/src" )
set (LIBRARY_INCLUDE_PATH  "${LIBRARY_MODULE_PATH}/include")

#set includes
include_directories (${LIBRARY_INCLUDE_PATH})

#set sources
file (GLOB LIB_HEADER_FILES 
        ${LIBRARY_INCLUDE_PATH}/util/*.h
        ${LIBRARY_INCLUDE_PATH}/partvar/*.h
        ${LIBRARY_INCLUDE_PATH}/*.h)
file (GLOB LIB_SOURCE_FILES 
        ${LIBRARY_SRC_PATH}/partvar/*.cpp
        ${LIBRARY_SRC_PATH}/util/*.cpp
        ${LIBRARY_SRC_PATH}/*.cpp
        ${LIBRARY_INCLUDE_PATH}/partvar/*.cpp
        ${LIBRARY_INCLUDE_PATH}/util/*.cpp
        ${LIBRARY_INCLUDE_PATH}/*.cpp)

#set library
add_library (${LIB_NAME}
        STATIC
        ${LIB_SOURCE_FILES}
        ${LIB_HEADER_FILES})

target_link_libraries(${LIB_NAME} formula)

add_subdirectory(include/minisat)
add_subdirectory(include/ltlparser)
add_subdirectory(include/formula)
add_subdirectory(include/ltlfsat)
add_subdirectory(include/synutil)
add_subdirectory(include/ltlfsyn)
# add_subdirectory(unit-test)

add_dependencies(formula ltlparser)
add_dependencies(ltlfsat minisat)
add_dependencies(ltlfsat formula)
add_dependencies(synutil ltlfsat)
add_dependencies(ltlfsyn synutil)
