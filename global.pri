# qtHaveModule() doesn't work with qml only modules
!exists($$[QT_INSTALL_QML]/QtQuick/Controls/TabView.qml) {
    error("QtQuickControls module was not found")
}

lessThan(QT_MAJOR_VERSION, 5) | lessThan(QT_MINOR_VERSION, 4) {
    error("You need at least Qt 5.4.")
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

# Features:
DEFINES += NO_WEBDAV # Will uncomment when UI is ready
# DEFINES += DEVELOPER_MODE
# DEFINES += FLOW_DEBUG_TIMESTAMPS

contains(DEFINES, DEVELOPER_MODE) {
    CONFIG += debug
    *-g++*|*clang* {
        QMAKE_CXXFLAGS += -Werror -Wall -Wextra
    }
    *clang* {
        QMAKE_CXXFLAGS += -Wno-error=inconsistent-missing-override
    }
}
