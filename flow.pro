TEMPLATE = subdirs

!ios:!android {
    SUBDIRS += plugins
}

SUBDIRS += src

static {
    CONFIG += ordered
}

include ("global.pri")

android {
    OTHER_FILES += android/AndroidManifest.xml
    DISTFILES += android/src/com/kdab/flowpomodoro/MainActivity.java
}
