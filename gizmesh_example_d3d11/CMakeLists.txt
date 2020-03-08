set(TARGET_NAME gizmesh_example_d3d11)
add_executable(${TARGET_NAME}
    main.cpp
    renderer_impl_d3d11.cpp
    dx11_context.cpp
    Win32Window.cpp
    OrbitCamera.cpp
    )
set_property(TARGET ${TARGET_NAME}
    PROPERTY CXX_STANDARD 20
    )
target_include_directories(${TARGET_NAME} PRIVATE
    )
target_compile_definitions(${TARGET_NAME} PRIVATE
    )
target_link_libraries(${TARGET_NAME} PRIVATE
    gizmesh
    d3d11
    d3dcompiler
    winmm
    )
