set(TARGET_NAME glew)
add_library(${TARGET_NAME} STATIC src/glew.c)
target_compile_definitions(${TARGET_NAME} PUBLIC GLEW_STATIC)
target_include_directories(
  ${TARGET_NAME}
  PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include
  PRIVATE)
target_link_libraries(${TARGET_NAME} PUBLIC)
