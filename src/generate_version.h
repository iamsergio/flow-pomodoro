#!/bin/bash

cp version_template.h version.h
LAST_SHA1=`git rev-parse HEAD`
LAST_DATE=`git log -1 --format=%cd --date=short`
sed -i "s/SHA1_REPLACEME/$LAST_SHA1/" version.h
sed -i "s/DATE_REPLACEME/$LAST_DATE/" version.h
