#!/bin/bash

# source common functions and vars
. `dirname $0`/common.sh
trap '${SCRIPTDIR}/apply_patches.sh -r ${SCRIPTDIR}/patches;\
      error_report $0 $LINENO' ERR SIGINT SIGTERM SIGQUIT

${SCRIPTDIR}/apply_patches.sh ${SCRIPTDIR}/patches

# "|| :" means "or always succeeding built-in command"
PROFILE_NAME=$(echo "$@" | grep -Po "(?<=\-P\s)[^\s]*" || :)

if [ "$PROFILE_NAME" == "" ]; then
  PROFILE_NAME=tizenmb_v2.4
  PROFILE_FLAG="-P $PROFILE_NAME"
fi

TIZEN_VERSION="chromium_efl_tizen_version 2.4"

if [ "$USE_GLOBAL_GBS_CONF" == "" ]; then
  CONF_FLAG="--conf ${SCRIPTDIR}/gbs.conf"
fi

gbs $CONF_FLAG build $PROFILE_FLAG -A armv7l --incremental \
    --define "${TIZEN_VERSION}" "$@"

${SCRIPTDIR}/apply_patches.sh -r ${SCRIPTDIR}/patches

