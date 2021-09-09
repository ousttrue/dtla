set(TARGET_NAME imgui)
add_library(${TARGET_NAME})
target_sources(${TARGET_NAME} PRIVATE imgui/imgui.cpp)
target_include_directories(${TARGET_NAME}
                           PUBLIC ${CMAKE_CURRENT_LISTS_DIR}/imgui)
