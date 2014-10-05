#!/bin/sh

# Replace with your own script
source /data/qt5-blackberry-5.3.source

mkdir build_dir-blackberry &> /dev/null
cd build_dir-blackberry
qmake CONFIG+=release ../flow.pro
make -j8

echo "Making package..."
cp ../bar-descriptor-blackberry.xml .
blackberry-nativepackager -devMode -package flow-pomodoro.bar bar-descriptor-blackberry.xml

echo "installing..."
blackberry-deploy -installApp -device z10 -password 12345 flow-pomodoro.bar
