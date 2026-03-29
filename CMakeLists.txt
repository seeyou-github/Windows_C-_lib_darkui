cmake_minimum_required(VERSION 3.20)

project(lib_darkui LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(darkui_combobox STATIC
    src/button.cpp
    src/combobox.cpp
    src/edit.cpp
    src/progress.cpp
    src/scrollbar.cpp
    src/slider.cpp
    src/tab.cpp
    src/table.cpp
    src/toolbar.cpp
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

add_executable(darkui_button_demo WIN32
    demo/demo_button.cpp
)

target_link_libraries(darkui_button_demo PRIVATE
    darkui_combobox
)

add_executable(darkui_slider_demo WIN32
    demo/demo_slider.cpp
)

target_link_libraries(darkui_slider_demo PRIVATE
    darkui_combobox
)

add_executable(darkui_progress_demo WIN32
    demo/demo_progress.cpp
)

target_link_libraries(darkui_progress_demo PRIVATE
    darkui_combobox
)

add_executable(darkui_scrollbar_demo WIN32
    demo/demo_scrollbar.cpp
)

target_link_libraries(darkui_scrollbar_demo PRIVATE
    darkui_combobox
)

add_executable(darkui_tab_demo WIN32
    demo/demo_tab.cpp
)

target_link_libraries(darkui_tab_demo PRIVATE
    darkui_combobox
)

add_executable(darkui_showcase_demo WIN32
    demo/demo_showcase.cpp
)

target_link_libraries(darkui_showcase_demo PRIVATE
    darkui_combobox
)

add_executable(darkui_toolbar_demo WIN32
    demo/demo_toolbar.cpp
)

target_link_libraries(darkui_toolbar_demo PRIVATE
    darkui_combobox
)

add_executable(darkui_toolbar_menubar_demo WIN32
    demo/demo_toolbar_menubar.cpp
)

target_link_libraries(darkui_toolbar_menubar_demo PRIVATE
    darkui_combobox
)

add_executable(darkui_edit_demo WIN32
    demo/demo_edit.cpp
)

target_link_libraries(darkui_edit_demo PRIVATE
    darkui_combobox
)

if (MINGW)
    target_link_options(darkui_combobox_demo PRIVATE -municode)
    target_link_options(darkui_table_demo PRIVATE -municode)
    target_link_options(darkui_button_demo PRIVATE -municode)
    target_link_options(darkui_slider_demo PRIVATE -municode)
    target_link_options(darkui_progress_demo PRIVATE -municode)
    target_link_options(darkui_scrollbar_demo PRIVATE -municode)
    target_link_options(darkui_tab_demo PRIVATE -municode)
    target_link_options(darkui_showcase_demo PRIVATE -municode)
    target_link_options(darkui_toolbar_demo PRIVATE -municode)
    target_link_options(darkui_toolbar_menubar_demo PRIVATE -municode)
    target_link_options(darkui_edit_demo PRIVATE -municode)
endif()
