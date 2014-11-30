TEMPLATE = subdirs
SUBDIRS += plugins src

static {
    CONFIG += ordered
}

include ("global.pri")

android {
    OTHER_FILES += android/AndroidManifest.xml
    DISTFILES += android/src/com/kdab/flowpomodoro/MainActivity.java
}
