#!/bin/bash

SCRIPTDIR=$(cd $(dirname $0); pwd -P)

# source common functions and vars
. `dirname ${BASH_SOURCE[0]}`/common.sh

${SCRIPTDIR}/apply_patches.sh

PROFILE_NAME=$(echo "$@" | grep -Po "(?<=\-P\s)[^\s]*")

if [ "$PROFILE_NAME" == "" ]; then
  PROFILE_NAME=tztv_v2.2.1_prehawk
  PROFILE_FLAG="-P $PROFILE_NAME"
fi

TIZEN_VERSION="chromium_efl_tizen_version 2.2.1"

if [ "$USE_GLOBAL_GBS_CONF" == "" ]; then
  CONF_FLAG="--conf ${SCRIPTDIR}/gbs.conf"
fi

gbs $CONF_FLAG build $PROFILE_FLAG -A armv7l --incremental \
    --extra-packs python-base-x86-arm,python-x86-arm,python-xml-x86-arm \
    --define "${TIZEN_VERSION}" "$@"
GBS_RET=$?

${SCRIPTDIR}/apply_patches.sh -r
APPLY_PATCHES_RET=$?

[ "$GBS_RET" != "0" ] && exit $GBS_RET
[ "$APPLY_PATCHES_RET" != "0" ] && exit $APPLY_PATCHES_RET
exit 0
