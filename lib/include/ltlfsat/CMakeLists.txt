#configure variables
set (LIB_NAME "ltlfsat")

#set includes
include_directories (${LIBRARY_INCLUDE_PATH})
include_directories (${LIBRARY_DEPENDS_PATH})
include_directories (${LIBRARY_DEPENDS_PATH}/minisat)

#set sources
file (GLOB LIB_SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

#set library
add_library (${LIB_NAME}
        STATIC
        ${LIB_SOURCE_FILES}
)
target_link_libraries(${LIB_NAME} deps)
