set(TARGET_NAME camera_example)
add_library(${TARGET_NAME} OrbitCamera.cpp)
set_property(TARGET ${TARGET_NAME} PROPERTY CXX_STANDARD 20)
target_include_directories(
  ${TARGET_NAME}
  PUBLIC ${CMAKE_CURRENT_LIST_DIR}
  PRIVATE)
target_link_libraries(${TARGET_NAME} PUBLIC falg window_example)
