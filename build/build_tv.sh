#!/bin/bash

SCRIPTDIR=$(cd $(dirname $0); pwd -P)

# source common functions and vars
. `dirname ${BASH_SOURCE[0]}`/common.sh

# copy patched files (for GCC 4.5)
. ${SCRIPTDIR}/copy_patched_files.sh

gbs build  -A armv7l --incremental "$@"
