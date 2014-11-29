#!/bin/bash

function getHostArch() {
  echo $(uname -m | sed -e \
        's/i.86/ia32/;s/x86_64/x64/;s/amd64/x64/;s/arm.*/arm/;s/i86pc/ia32/')
}

_LIBDIR=lib
if [ "$(getHostArch)" == "x64" ]; then
  _LIBDIR=lib64
fi

SCRIPTDIR=$(cd $(dirname $0); pwd -P)

OUT_DIR=$(echo $SCRIPTDIR | grep -Po "(?<=/)out\..*?(?=/)")

BUILD_MODE=Release

if echo $SCRIPTDIR | grep -q "Debug/efl_webview_app"; then
  BUILD_MODE=Debug
fi

CHROMIUM_EFL_LIBDIR=$(cd $SCRIPTDIR/../../../chromium/${OUT_DIR}/$BUILD_MODE/lib; pwd -P)
CHROMIUM_EFL_DEPENDENCIES_LIBDIR=$(cd $SCRIPTDIR/../../../chromium/$OUT_DIR/Dependencies/Root/$_LIBDIR; pwd -P)

export LD_LIBRARY_PATH=$CHROMIUM_EFL_DEPENDENCIES_LIBDIR:$CHROMIUM_EFL_LIBDIR:${LD_LIBRARY_PATH}
echo "LD_LIBRARY_PATH=${LD_LIBRARY_PATH}"
${SCRIPTDIR}/efl_webview_app $@

