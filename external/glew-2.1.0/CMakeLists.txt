set(TARGET_NAME glew)
add_library(${TARGET_NAME}
    src/glew.c
    )
target_include_directories(${TARGET_NAME} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
    PRIVATE
    )
target_link_libraries(${TARGET_NAME} PUBLIC
    )
