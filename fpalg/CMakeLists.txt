set(TARGET_NAME fpalg)
# header only
add_library(${TARGET_NAME} INTERFACE)
target_include_directories(${TARGET_NAME} INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
    )
