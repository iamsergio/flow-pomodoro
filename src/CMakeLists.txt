
set(flow_SRC
    calendarcontroller.cpp
    checkabletagmodel.cpp
    circularprogressindicator.cpp
    checkbox.cpp
    controller.cpp
    duedate.cpp
    extendedtagsmodel.cpp
    genericlistmodel.h
    gitupdater.cpp
    gitutils.cpp
    jsonstorage.cpp
    kernel.cpp
    loadmanager.cpp
    pluginmodel.cpp
    quickview.cpp
    main.cpp
    nonemptytagfilterproxy.cpp
    runtimeconfiguration.cpp
    settings.cpp
    sortedtagsmodel.cpp
    sortedtaskcontextmenumodel.cpp
    storage.cpp
    syncable.cpp
    tag.cpp
    tagref.cpp
    task.cpp
    taskcontextmenumodel.cpp
    taskfilterproxymodel.cpp
    tooltipcontroller.cpp
    utils.cpp
)

qt5_add_resources(RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/resources.qrc)

add_executable(flow ${flow_SRC} ${RESOURCES})

qt5_use_modules(flow Gui Quick DBus Widgets)

install (TARGETS flow DESTINATION bin)
