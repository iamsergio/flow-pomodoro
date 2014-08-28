#!/bin/sh

mkdir build_dir-playbook &> /dev/null
cd build_dir-playbook
qmake ../flow.pro
make -j8

# blackberry-nativepackager -devMode -package flow-pomodoro.bar bar-descriptor.xml
# blackberry-deploy -installApp -device 192.168.1.143 -password kdab flow-pomodoro.bar
