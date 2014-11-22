#!/bin/bash

cp version_template.h version.h
LAST_SHA1=`git rev-parse HEAD`
LAST_DATE=`git log -1 --format=%cd --date=short`
BRANCH=`git symbolic-ref --short HEAD`

# Check if HEAD is tagged
if git describe --exact-match &> /dev/null ; then
    ISTAG=true
else
    ISTAG=false
fi

if git status &> /dev/null ; then
    HASGIT=true
else
    HASGIT=false
fi


sed -i "s/SHA1_REPLACEME/$LAST_SHA1/" version.h
sed -i "s/DATE_REPLACEME/$LAST_DATE/" version.h
sed -i "s/BRANCH_REPLACEME/$BRANCH/" version.h
sed -i "s/ISTAG_REPLACEME/$ISTAG/" version.h
sed -i "s/HASGIT_REPLACEME/$HASGIT/" version.h
