set(TARGET_NAME gizmesh_example_gl3)
add_executable(${TARGET_NAME} main.cpp gl-api.cpp renderer_impl_gl3.cpp
                              wgl_context.cpp)
set_property(TARGET ${TARGET_NAME} PROPERTY CXX_STANDARD 20)
target_include_directories(
  ${TARGET_NAME} PRIVATE ${EXTERNAL_DIR}/glew/include
                         ${CMAKE_CURRENT_LIST_DIR}/../common)
target_compile_definitions(${TARGET_NAME} PRIVATE)
target_link_libraries(
  ${TARGET_NAME}
  PRIVATE gizmesh camera_example window_example
          #
          glew OPENGL32 winmm)
