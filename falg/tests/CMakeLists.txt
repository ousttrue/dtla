set(TARGET_NAME falg_tests)
add_executable(${TARGET_NAME}
    main.cpp
    )
target_include_directories(${TARGET_NAME} PRIVATE
    ${EXTERNAL_DIR}/catch2
    ${CMAKE_CURRENT_LIST_DIR}/../gizmesh
    )
target_link_libraries(${TARGET_NAME} PRIVATE
    gizmesh
    catch2
    )   
