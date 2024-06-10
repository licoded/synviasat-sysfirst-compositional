set (APP_NAME "ltlfsyn-bin")

#configure directories
set (APP_MODULE_PATH    "${PROJECT_SOURCE_DIR}/app")
set (APP_SRC_PATH       "${APP_MODULE_PATH}/src" )

#set includes
include_directories (${APP_SRC_PATH} ${LIBRARY_INCLUDE_PATH})
include_directories (${LIBRARY_INCLUDE_PATH}/minisat)

#set sources
file (GLOB APP_SOURCE_FILES "${APP_SRC_PATH}/*.cpp")

#set target executable
add_executable (${APP_NAME} ${APP_SOURCE_FILES})

#add the library
target_link_libraries(${APP_NAME}
    ${LIB_CUDD}
    util
    formula
    synutil
    ltlfsyn)

set_target_properties(${APP_NAME} PROPERTIES OUTPUT_NAME ltlfsyn)

# #test
# enable_testing ()
# add_subdirectory (test)