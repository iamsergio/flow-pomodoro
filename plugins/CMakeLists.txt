include_directories("${PROJECT_SOURCE_DIR}/src/")

macro(add_flow_plugin _name)
    add_definitions(-DQT_PLUGIN)
    add_definitions(-DQT_SHARED)
    add_library(${_name} SHARED ${_name}.cpp)
    qt5_use_modules(${_name} Gui Quick DBus)

    if (QT_INSTALL_PLUGINS)
        install(TARGETS ${_name} DESTINATION ${QT_INSTALL_PLUGINS})
    else()
        install(TARGETS ${_name} DESTINATION lib/kde4/plugins/flow/)
    endif()

endmacro()

add_subdirectory(kmail)
add_subdirectory(pidgin)
add_subdirectory(hosts)
add_subdirectory(shellscript)
