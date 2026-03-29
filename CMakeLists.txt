cmake_minimum_required(VERSION 3.20)

project(lib_darkui LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(darkui_combobox STATIC
    src/combobox.cpp
    src/table.cpp
)

target_include_directories(darkui_combobox PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_definitions(darkui_combobox PUBLIC
    UNICODE
    _UNICODE
    NOMINMAX
    WIN32_LEAN_AND_MEAN
)

target_link_libraries(darkui_combobox PUBLIC
    comctl32
    dwmapi
    uxtheme
)

add_executable(darkui_combobox_demo WIN32
    demo/demo_combobox.cpp
)

target_link_libraries(darkui_combobox_demo PRIVATE
    darkui_combobox
)

add_executable(darkui_table_demo WIN32
    demo/demo_table.cpp
)

target_link_libraries(darkui_table_demo PRIVATE
    darkui_combobox
)

if (MINGW)
    target_link_options(darkui_combobox_demo PRIVATE -municode)
    target_link_options(darkui_table_demo PRIVATE -municode)
endif()
