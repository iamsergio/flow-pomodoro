TEMPLATE = subdirs
SUBDIRS += src plugins

include ("global.pri")

OTHER_FILES += android/AndroidManifest.xml
ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
