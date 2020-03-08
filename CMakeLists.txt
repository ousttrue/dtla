project(gizmesh)
cmake_minimum_required(VERSION 3.0.0)
set(EXTERNAL_DIR ${CMAKE_CURRENT_LIST_DIR}/external)
subdirs(gizmesh gizmesh_example_d3d11 fpalg_tests)
