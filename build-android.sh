#!/bin/sh

# Source android env, change this to your script:
# use qt-android 5.6

mkdir build_dir-android &> /dev/null
cd build_dir-android
qmake CONFIG+=release ../flow.pro
make -j8
make install INSTALL_ROOT=android-build

if [ $# = 1 ] ; then
    if [ $1 = "install" ] ; then
        androiddeployqt --input src/android-libflow.so-deployment-settings.json --output src/android-build/ --deployment bundled --ant /usr/bin/ant --android-platform android-18 --jdk /usr/lib/jvm/java-8-openjdk --reinstall # --device xxxx
    fi
fi
