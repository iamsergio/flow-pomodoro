#!/bin/bash

function isTag()
{
    # Checks if HEAD is a tag
    return git describe --exact-match &> /dev/null
}


cp version_template.h version.h
LAST_SHA1=`git rev-parse HEAD`
LAST_DATE=`git log -1 --format=%cd --date=short`
LAST_TAG=`git describe --abbrev=0`
BRANCH=`git symbolic-ref --short HEAD`

if isTag ; then
    ISTAG=true
else
    ISTAG=false
fi


sed -i "s/SHA1_REPLACEME/$LAST_SHA1/" version.h
sed -i "s/DATE_REPLACEME/$LAST_DATE/" version.h

sed -i "s/LASTTAG_REPLACEME/$LAST_TAG/" version.h
sed -i "s/BRANCH_REPLACEME/$BRANCH/" version.h
sed -i "s/ISTAG_REPLACEME/$ISTAG/" version.h
