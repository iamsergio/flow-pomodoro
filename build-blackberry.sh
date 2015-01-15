#!/bin/sh

# Replace with your own script
source /data/qt5-blackberry.source 5.4

mkdir build_dir-blackberry &> /dev/null
cd build_dir-blackberry
qmake CONFIG+=release ../flow.pro
make -j8

echo "Making package..."
cp ../bar-descriptor-blackberry.xml .
blackberry-nativepackager -devMode -package flow-pomodoro.bar bar-descriptor-blackberry.xml

if [ $# = 1 ] ; then
    if [ $1 = "install" ] ; then
        echo "installing..."
        blackberry-deploy -installApp -device z10 -password 12345 flow-pomodoro.bar
    fi
fi
