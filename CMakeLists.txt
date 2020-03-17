project(gizmesh)
cmake_minimum_required(VERSION 3.0.0)
set(EXTERNAL_DIR ${CMAKE_CURRENT_LIST_DIR}/external)
subdirs(
    falg
    falg_tests
    example/window_example
    example/camera_example
    gizmesh
    example/gizmesh_example_d3d11
    external/glew-2.1.0
    example/gizmesh_example_gl3
    )
