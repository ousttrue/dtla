set(TARGET_NAME gizmesh)
add_library(
  ${TARGET_NAME}
  src/gizmesh.cpp src/geometry_mesh.cpp src/gizmo_translation.cpp
  src/gizmo_rotation.cpp src/gizmo_scale.cpp)

target_include_directories(
  ${TARGET_NAME}
  PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include
  PRIVATE)
target_link_libraries(${TARGET_NAME} PUBLIC falg)
