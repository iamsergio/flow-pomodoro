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

android {
    QT += androidextras
}

win32:!mingw {
	# Some Qt 5.4 madness going on here, link explicitly
	LIBS += Shell32.lib
}

INCLUDEPATH += src

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

CONFIG += c++11

DEFINES += FLOW_VERSION="v0.9.2"
DEFINES += FLOW_VERSION_DATE="03-12-2014"

# Features:
DEFINES += NO_WEBDAV # Will uncomment when UI is ready
# DEFINES += DEVELOPER_MODE
# DEFINES += FLOW_DEBUG_TIMESTAMPS

contains(DEFINES, DEVELOPER_MODE) {
    CONFIG += debug
}
