#!/bin/bash

SCRIPTDIR=$(cd $(dirname $0); pwd -P)

# source common functions and vars
. `dirname ${BASH_SOURCE[0]}`/common.sh

${SCRIPTDIR}/apply_patches.sh

TIZEN_VERSION="chromium_efl_tizen_version 2.2.1"

if [ ! -f "$HOME/.gbs.conf" ]; then
    CONF_FLAG="--conf ${SCRIPTDIR}/gbs.conf"
    PROFILE_FLAG="-P tizentv_product"
fi

gbs $CONF_FLAG build $PROFILE_FLAG -A armv7l --incremental \
    --define "${TIZEN_VERSION}" "$@"

${SCRIPTDIR}/apply_patches.sh -r
