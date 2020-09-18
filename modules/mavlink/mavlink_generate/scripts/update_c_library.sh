#!/bin/bash

# c_library repository update script
# Author: Thomas Gubler <thomasgubler@gmail.com>
#
# This script can be used together with a github webhook to automatically
# generate new c header files and push to the c_library repository whenever
# the message specifications are updated.
# The script assumes that the git repositories in MAVLINK_GIT_PATH and
# CLIBRARY_GIT_PATH are set up prior to invoking the script.
#
# Usage, for example:
# cd ~/src
# git clone git@github.com:mavlink/mavlink.git
# cd mavlink
# git remote rename origin upstream
# mkdir -p include/mavlink/v1.0
# cd include/mavlink/v1.0
# git clone git@github.com:mavlink/c_library_v1.git
# cd ~/src/mavlink
# ./scripts/update_c_library.sh 1
#
# A one-liner for the TMP directory (e.g. for crontab)
# cd /tmp; git clone git@github.com:mavlink/mavlink.git &> /dev/null; \
# cd /tmp/mavlink && git remote rename origin upstream &> /dev/null; \
# mkdir -p include/mavlink/v1.0 && cd include/mavlink/v1.0 && git clone git@github.com:mavlink/c_library_v1.git &> /dev/null; \
# cd /tmp/mavlink && ./scripts/update_c_library.sh &> /dev/null

function generate_headers() {
python pymavlink/tools/mavgen.py \
    --output $CLIBRARY_PATH \
    --lang C \
    --wire-protocol $2.0 \
    message_definitions/v1.0/$1.xml
}

# settings
MAVLINK_PATH=$PWD
MAVLINK_GIT_REMOTENAME=upstream
MAVLINK_GIT_BRANCHNAME=master
CLIBRARY_PATH=$MAVLINK_PATH/include/mavlink/v$1.0/c_library_v$1
CLIBRARY_GIT_REMOTENAME=origin
CLIBRARY_GIT_BRANCHNAME=master

# fetch latest message specifications
#cd $MAVLINK_PATH
#git fetch $MAVLINK_GIT_REMOTENAME
#git diff $MAVLINK_GIT_REMOTENAME/$MAVLINK_GIT_BRANCHNAME --exit-code
#RETVAL=$?
# if the diff value is zero nothing changed - abort
#[ $RETVAL -eq 0 ] && exit 0
#echo -e "\0033[34mFetching latest protocol specifications\0033[0m\n"
#git pull $MAVLINK_GIT_REMOTENAME $MAVLINK_GIT_BRANCHNAME || exit 1

# save git hash
MAVLINK_GITHASH=$(git rev-parse HEAD)

# delete old c headers
rm -rf $CLIBRARY_PATH/*

# generate new c headers
echo -e "\0033[34mStarting to generate c headers\0033[0m\n"
generate_headers ardupilotmega $1
generate_headers autoquad $1
generate_headers matrixpilot $1
generate_headers minimal $1
generate_headers slugs $1
generate_headers test $1
generate_headers ASLUAV $1
generate_headers standard $1
mkdir -p $CLIBRARY_PATH/message_definitions
cp message_definitions/v1.0/* $CLIBRARY_PATH/message_definitions/.
echo -e "\0033[34mFinished generating c headers\0033[0m\n"

# git add and git commit in local c_library repository
cd $CLIBRARY_PATH
git add --all :/ || exit 1
COMMIT_MESSAGE="autogenerated headers for rev https://github.com/mavlink/mavlink/tree/"$MAVLINK_GITHASH
git commit -m "$COMMIT_MESSAGE" || exit 1

# push to c_library repository
git push $CLIBRARY_GIT_REMOTENAME $CLIBRARY_GIT_BRANCHNAME || exit 1
echo -e "\0033[34mHeaders updated and pushed successfully\0033[0m"
