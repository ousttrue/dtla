set(TARGET_NAME gizmesh)
add_library(${TARGET_NAME}
    gizmesh.cpp
    geometry_mesh.cpp
    gizmo_translation.cpp
    gizmo_rotation.cpp
    gizmo_scale.cpp
    )
set_property(TARGET ${TARGET_NAME}
    PROPERTY CXX_STANDARD 20
    )
target_include_directories(${TARGET_NAME} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/../include
    PRIVATE
    )
target_link_libraries(${TARGET_NAME} PUBLIC
    falg
    )
