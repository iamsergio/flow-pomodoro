#!/bin/sh

# Source android env, change this to your script:
# source /data/qt5-android-armv5.source 5.6
# source /data/qt5-android.source 5.6

mkdir build_dir-android &> /dev/null
cd build_dir-android
qmake CONFIG+=release ../flow.pro
make -j8
make install INSTALL_ROOT=android-build

if [ $# = 1 ] ; then
    if [ $1 = "install" ] ; then
        androiddeployqt --input src/android-libflow.so-deployment-settings.json --output src/android-build/ --deployment bundled --ant /usr/bin/ant --android-platform android-18 --jdk /usr/lib/jvm/java-8-openjdk --reinstall --device 058970b2f0df5762
    fi
fi
