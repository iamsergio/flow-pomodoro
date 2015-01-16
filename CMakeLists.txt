cmake_minimum_required(VERSION 2.8.8)

project(flow)

find_package(Qt5Gui)
find_package(Qt5Qml)
find_package(Qt5Quick)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

execute_process(COMMAND "qmake" "-query" "QT_INSTALL_PLUGINS" OUTPUT_VARIABLE QT_INSTALL_PLUGINS)
string(STRIP ${QT_INSTALL_PLUGINS} QT_INSTALL_PLUGINS)
set(QT_INSTALL_PLUGINS ${QT_INSTALL_PLUGINS}/flow/)

include_directories(${Qt5Core_INCLUDES})
include_directories(${Qt5Gui_INCLUDES})

set(CMAKE_CXX_FLAGS "${Qt5Gui_EXECUTABLE_COMPILE_FLAGS}")


add_subdirectory(src)
add_subdirectory(plugins)

set(CPACK_PACKAGE_VERSION "0.9.5")
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "9")
set(CPACK_PACKAGE_VERSION_PATCH "5")
set(CPACK_PACKAGE_CONTACT "Sergio Martins <iamsergio@gmail.com>") # deb
set(CPACK_GENERATOR "DEB;")

# Deb
set(CPACK_PACKAGE_NAME "flow-pomodoro")
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "A sticky task manager and pomodoro that blocks distractions.")
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")

# NSIS
set(CPACK_PACKAGE_EXECUTABLES "flow.exe" "${CMAKE_CURRENT_SOURCE_DIR}/src/img/icon.ico") # nsis

include(CPack)
