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

set(CPACK_PACKAGE_VERSION "1.1.0")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "1")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_CONTACT "Sergio Martins <iamsergio@gmail.com>") # deb
set(CPACK_GENERATOR "DEB;")

# Deb
set(CPACK_PACKAGE_NAME "flow-pomodoro")
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "A sticky task manager and pomodoro that blocks distractions.")
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")

# NSIS
set(CPACK_PACKAGE_EXECUTABLES "flow.exe" "${CMAKE_CURRENT_SOURCE_DIR}/src/img/icon.ico") # nsis

include(CPack)
