cmake_minimum_required(VERSION 3.18)

PROJECT(ImGUI)

#设置库文件的输出目录
SET(CMAKE_DEBUG_POSTFIX "_Debug")
SET(CMAKE_RELEASE_POSTFIX "_Release") 
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG  ${PROJECT_SOURCE_DIR}/../../lib/ImGUI/Debug)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE  ${PROJECT_SOURCE_DIR}/../../lib/ImGUI/Release)

set(ImGUI_SOURCE_FILE 
imgui_demo.cpp
imgui_draw.cpp
imgui_impl_dx12.cpp
imgui_impl_win32.cpp
imgui_tables.cpp
imgui_widgets.cpp
imgui.cpp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(ImGUI ${ImGUI_SOURCE_FILE})