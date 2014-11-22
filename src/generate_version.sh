#!/bin/bash

function isTag()
{
# Chec
}


cp version_template.h version.h
LAST_SHA1=`git rev-parse HEAD`
LAST_DATE=`git log -1 --format=%cd --date=short`
LAST_TAG='git describe --abbrev=0'
BRANCH=`git symbolic-ref --short HEAD`

if ! git describe --exact-match &> /dev/null; then
    echo exists
fi


sed -i "s/SHA1_REPLACEME/$LAST_SHA1/" version.h
sed -i "s/DATE_REPLACEME/$LAST_DATE/" version.h
