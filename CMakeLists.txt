cmake_minimum_required(VERSION 2.8.8)

project(flow)

find_package(Qt5Gui)
find_package(Qt5Qml)
find_package(Qt5Quick)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(${Qt5Core_INCLUDES})
include_directories(${Qt5Gui_INCLUDES})

set(CMAKE_CXX_FLAGS "${Qt5Gui_EXECUTABLE_COMPILE_FLAGS}")


add_subdirectory(src)
add_subdirectory(plugins)
