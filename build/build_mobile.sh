#!/bin/bash

export ICECC=no
SCRIPTDIR=$(cd $(dirname $0); pwd -P)

# source common functions and vars
. `dirname ${BASH_SOURCE[0]}`/common.sh

${SCRIPTDIR}/apply_patches.sh

# "|| :" means "or always succeeding built-in command"
PROFILE_NAME=$(echo "$@" | grep -Po "(?<=\-P\s)[^\s]*" || :)

if [ "$PROFILE_NAME" == "" ]; then
  PROFILE_NAME=tizenmb_v2.3
  PROFILE_FLAG="-P $PROFILE_NAME"
fi

TIZEN_VERSION="chromium_efl_tizen_version 2.3"

if [ "$USE_GLOBAL_GBS_CONF" == "" ]; then
  CONF_FLAG="--conf ${SCRIPTDIR}/gbs.conf"
fi

CUSTOM_LIBC_DIR="custom_libc_dir /opt/usr/eglibc-2.18/lib"

gbs $CONF_FLAG build $PROFILE_FLAG -A armv7l --incremental \
    --define "${CUSTOM_LIBC_DIR}" \
    --define "${TIZEN_VERSION}" "$@"
GBS_RET=$?

${SCRIPTDIR}/apply_patches.sh -r
APPLY_PATCHES_RET=$?

[ "$GBS_RET" != "0" ] && exit $GBS_RET
[ "$APPLY_PATCHES_RET" != "0" ] && exit $APPLY_PATCHES_RET
exit 0
