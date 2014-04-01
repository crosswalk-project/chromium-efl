#!/bin/bash

# source common functions and vars
. `dirname ${BASH_SOURCE[0]}`/common.sh

# Unbundle system dependencies to make sure they are not built.
. `dirname ${BASH_SOURCE[0]}`/system_deps_mobile.sh
$CHROME_SRC/build/linux/unbundle/replace_gyp_files.py $(echo $SYSTEM_DEPS | sed -e 's/use_system_/-Duse_system_/g')
