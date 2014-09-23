#!/bin/bash

SCRIPTDIR=$(cd $(dirname $0); pwd -P)

# source common functions and vars
. `dirname ${BASH_SOURCE[0]}`/common.sh

${SCRIPTDIR}/apply_patches.sh

GBS_CONF=$HOME/.gbs.conf
if [ ! -f $GBS_CONF  ]; then
    GBS_CONF=${SCRIPTDIR}/gbs.conf
fi

gbs --conf $GBS_CONF build -P tizentv_product -A armv7l --incremental "$@"

${SCRIPTDIR}/apply_patches.sh -r
