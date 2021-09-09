set(TARGET_NAME gizmesh_example_d3d11)
add_executable(${TARGET_NAME} main.cpp renderer_impl_d3d11.cpp dx11_context.cpp)

target_include_directories(${TARGET_NAME}
                           PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../common)
target_compile_definitions(${TARGET_NAME} PRIVATE)
target_link_libraries(
  ${TARGET_NAME}
  PRIVATE gizmesh
          camera_example
          window_example
          #
          d3d11
          d3dcompiler
          winmm
          imgui)
