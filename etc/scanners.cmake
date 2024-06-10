file (GLOB_RECURSE LTLFSYN_CXX_FILES 
  ${CMAKE_SOURCE_DIR}/lib/**.h
  ${CMAKE_SOURCE_DIR}/lib/**.cpp
  ${CMAKE_SOURCE_DIR}/app/**.h
  ${CMAKE_SOURCE_DIR}/app/**.cpp)

add_custom_target (format "clang-format" -i ${LTLFSYN_CXX_FILES} COMMENT "Formatting source code...")

foreach (tidy_target ${LTLFSYN_CXX_FILES})
  string(REPLACE "${CMAKE_SOURCE_DIR}/" "" relative_path ${tidy_target})
  string(REPLACE "/" "__" tidy_target_name "${relative_path}")
  set (tidy_command clang-tidy --quiet -header-filter=.* -p=${PROJECT_BINARY_DIR} ${tidy_target})
  add_custom_target (tidy_${tidy_target_name} ${tidy_command})
  # message(STATUS "tidy_target_name: ${tidy_target_name}")
  list (APPEND LTLFSYN_TIDY_TARGETS tidy_${tidy_target_name})
endforeach (tidy_target)

# message(STATUS "LTLFSYN_TIDY_TARGETS: ${LTLFSYN_TIDY_TARGETS}")

add_custom_target (tidy DEPENDS ${LTLFSYN_TIDY_TARGETS})