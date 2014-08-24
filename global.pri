*-g++*|*clang* {
    QMAKE_CXXFLAGS += -Werror -Wall -Wextra
}

# qtHaveModule() does work with qml only modules
!exists($$[QT_INSTALL_QML]/QtGraphicalEffects/Desaturate.qml) {
    error("QtGraphicalEffects module was not found")
}

!exists($$[QT_INSTALL_QML]/QtQuick/Controls/TabView.qml) {
    error("QtQuickControls module was not found")
}

INCLUDEPATH += src

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

CONFIG += debug c++11

# Features:
DEFINES += NO_WEBDAV # Will uncomment when UI is ready
# DEFINES += DEVELOPER_MODE
