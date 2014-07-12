#!/bin/sh

command -v qmllint &> /dev/null || exit 0;
find . -name "*.qml" | xargs qmllint
