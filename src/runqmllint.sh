#!/bin/sh

command -v qmllint &> /dev/null || exit 0;
find $1 -name "*.qml" | xargs qmllint
