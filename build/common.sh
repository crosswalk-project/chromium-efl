#!/bin/bash

pushd `dirname $0` > /dev/null
export SCRIPTDIR=`pwd -L`
popd > /dev/null

export TOPDIR=$(readlink -f "${SCRIPTDIR}/..")
if [[ -z "${CHROME_SRC}" ]]; then
  # If $CHROME_SRC was not set, assume src subfolder in project directory is CHROME_SRC.
  export CHROME_SRC="${TOPDIR}/src"
fi

function applyPatch {
  directory=$1
  shift
  patchfile=$1
  shift
  echo Applying $patchfile in $directory
  pushd "$directory" > /dev/null
  patch -p1 $@ < "$patchfile"
  popd > /dev/null
}

function getHostOs() {
  echo $(uname -s | sed -e 's/Linux/linux/;s/Darwin/mac/')
}

function getHostArch() {
  echo $(uname -m | sed -e \
        's/i.86/ia32/;s/x86_64/x64/;s/amd64/x64/;s/arm.*/arm/;s/i86pc/ia32/')
}

function getPythonVersion() {
  echo $(python --version  2>&1 | sed -e 's/Python \([0-9]\+\.[0-9]\+\)\.[0-9]\+/\1/')
}

function setIfUndef() {
    eval original=\$$1
    new=$2
    if [ -n "$original" ]; then
        echo $original
    else
        echo $new
    fi
}


function hostBuldScriptUsage() {
cat << EOF
usage: $1 [OPTIONS]

Build non gbs version of chromium-efl

OPTIONS:
   -h, --help    Show this message
   --skip-gyp    Skip restore_gyp, jhbuild and gyp_chromium steps
   --ccache      configure ccache installed in your system
   --content-shell Build content_shell application
   --skip-ninja  Skip ninja step
   --build-ewk-unittests  build ewk unittests
   --debug       build debug version of chromium-efl (in $GYP_GENERATOR_OUTPUT/Debug instead of default $GYP_GENERATOR_OUTPUT/Release)
   -jN           set number of jobs, just like with make or ninja

examples:
$0 --skip-gyp
$0 --skip-gyp --ccache
$0 --skip-ninja
EOF
  exit
}

function parseHostBuildScriptParams() {

  export SKIP_GYP=0
  export USE_CCACHE=0
  export SKIP_NINJA=0
  export BUILD_SUBDIRECTORY=Release

  if echo "$@" | grep -cq '\(\(\-\-help\)\|\(\-h\)\)'; then
    hostBuldScriptUsage ${0}
  fi

  if echo "$@" | grep -cq '\-\-skip-gyp'; then
    export SKIP_GYP=1
  fi

  if echo "$@" | grep -cq '\-\-skip-ninja'; then
    export SKIP_NINJA=1
  fi

  if echo "$@" | grep -cq '\-\-content_shell'; then
    export BUILD_CONTENT_SHELL=1
  fi

  if echo "$@" | grep -cq '\-\-ccache'; then
    export USE_CCACHE=1
    echo using ccache
    source $TOPDIR/build/ccache_env.sh ${1}
  fi

  if echo "$@" | grep -cq '\-\-build-ewk-unittests'; then
    export BUILD_EWK_UNITTESTS=1
  fi
  if echo "$@" | grep -cq '\-\-debug'; then
    export BUILD_SUBDIRECTORY=Debug
  fi
  # Will be empty string if -j not specified or ill-formatted, otherwise -j and the number argument together.
  # \grep because folks often alias grep but we want the vanilla behavior.
  export JOBS=$(echo "$@" | \grep -Eo '\-j\s*[1-9]([0-9]*)')

}

function hostGypChromiumEfl() {
  if [ "$SKIP_GYP" == "0" ]; then
    ${TOPDIR}/build/gyp_chromiumefl.sh $@
  fi
}

function hostNinja() {
  if [ "$SKIP_NINJA" == "0" ]; then
    TARGETS="chromium-efl efl_webprocess chromium-ewk efl_webview_app"
    if [ "$BUILD_EWK_UNITTESTS" == "1" ]; then
      TARGETS="$TARGETS ewk_unittests"
    fi
    if [ "$BUILD_CONTENT_SHELL" == "1" ]; then
      TARGETS="$TARGETS content_shell_efl"
    fi
    BUILDDIR=${GYP_GENERATOR_OUTPUT}/${BUILD_SUBDIRECTORY}
    ninja -C $BUILDDIR ${JOBS} ${TARGETS}
  fi
}

function error_report() {
  echo "Error: File:$1 Line:$2"
  exit 1
}
