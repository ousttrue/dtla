set(TARGET_NAME imgui)
add_library(${TARGET_NAME})
target_sources(
  ${TARGET_NAME}
  PRIVATE imgui/imgui.cpp
          imgui/imgui_draw.cpp
          imgui/imgui_demo.cpp
          imgui/imgui_tables.cpp
          imgui/imgui_widgets.cpp
          imgui/backends/imgui_impl_dx11.cpp
          imgui/backends/imgui_impl_win32.cpp)
target_include_directories(
  ${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/imgui
                        ${CMAKE_CURRENT_LIST_DIR}/imgui/backends)

set(TARGET_NAME example_win32_directx11)
add_executable(${TARGET_NAME} imgui/examples/example_win32_directx11/main.cpp)
target_link_libraries(${TARGET_NAME} PRIVATE imgui d3d11.lib)
