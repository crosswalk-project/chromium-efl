#!/bin/bash

SCRIPTDIR=$( cd $(dirname $0) ; pwd -P )
TOPDIR=$( cd ${SCRIPTDIR}/.. ; pwd -P )

source ${SCRIPTDIR}/common.sh

host_arch=$(getHostArch)

export GYP_GENERATOR_OUTPUT=${TOPDIR}/"out.${host_arch}"

usage() {
cat << EOF
usage: $0 [OPTIONS]

Build desktop version of chromium-efl

OPTIONS:
   -h, --help    Show this message
   --skip-gyp    Skip restore_gyp, jhbuild and gyp_chromium steps
   --ccache      configure ccache installed in your system
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

SKIP_GYP=0
USE_CCACHE=0
SKIP_NINJA=0

BUILD_SUBDIRECTORY=Release

if echo "$@" | grep -cq '\(\(\-\-help\)\|\(\-h\)\)'; then
  usage
fi

if echo "$@" | grep -cq '\-\-skip-gyp'; then
  SKIP_GYP=1
fi

if echo "$@" | grep -cq '\-\-skip-ninja'; then
  SKIP_NINJA=1
fi

if echo "$@" | grep -cq '\-\-ccache'; then
  USE_CCACHE=1
fi

if echo "$@" | grep -cq '\-\-build-ewk-unittests'; then
  BUILD_EWK_UNITTESTS=1
fi

if echo "$@" | grep -cq '\-\-content_shell'; then
  BUILD_CONTENT_SHELL=1
fi

JHBUILD_STAMPFILE="${GYP_GENERATOR_OUTPUT}/Dependencies/Root/jhbuild.stamp"

shouldForceJHBuild() {
  if echo "$@" | grep -cq '\-\-force-jhbuild'; then
    return 1
  fi

  # Check if anything in jhbuild is more recent than stamp file.
  for i in $(find "$SCRIPTDIR/jhbuild"); do
    if [ -f "$i" -a "$JHBUILD_STAMPFILE" -ot "$i" ]; then
      return 1
    fi
  done

  return 0
}

if [ "$(shouldForceJHBuild $@)" == "1" ]; then
  rm -f $JHBUILD_STAMPFILE
fi

if echo "$@" | grep -cq '\-\-debug'; then
  BUILD_SUBDIRECTORY=Debug
fi

# Will be empty string if -j not specified or ill-formatted, otherwise -j and the number argument together.
# \grep because folks often alias grep but we want the vanilla behavior.
JOBS=$(echo "$@" | \grep -Eo '\-j\s*[1-9]([0-9]*)')

set -e

JHBUILD_DEPS="${GYP_GENERATOR_OUTPUT}/Dependencies/Root"
if [ "${host_arch}" == "x64" ]; then
  _LIBDIR=lib64
elif [ "${host_arch}" == "ia32" ]; then
  _LIBDIR=lib
fi
export PKG_CONFIG_PATH="${JHBUILD_DEPS}/${_LIBDIR}/pkgconfig"

if [ ! -f "$JHBUILD_STAMPFILE" ]; then
  jhbuild --no-interact -f ${SCRIPTDIR}/jhbuild/jhbuildrc

  if [ "$?" == "0" ]; then
    echo "Yay! jhbuild done!" > $JHBUILD_STAMPFILE
  fi
fi

if [ "$SKIP_GYP" == "0" ]; then
  ${TOPDIR}/build/gyp_chromiumefl.sh
fi

if [ "$SKIP_NINJA" == "0" ]; then
  if [ "$USE_CCACHE" == "1" ]; then
    echo using ccache
    set +e
    source $TOPDIR/build/ccache_env.sh desktop
    set -e
  fi
  export LD_LIBRARY_PATH="${JHBUILD_DEPS}/${_LIBDIR}:$LD_LIBRARY_PATH"
  export PATH="${JHBUILD_DEPS}/bin:$PATH"

  TARGETS="chromium-efl efl_webprocess chromium-ewk efl_webview_app"
  if [ "$BUILD_EWK_UNITTESTS" == "1" ]; then
    TARGETS="$TARGETS ewk_unittests"
  fi

  if [ "$BUILD_CONTENT_SHELL" == "1" ]; then
    TARGETS="$TARGETS content_shell_efl"
  fi

  ninja -C ${GYP_GENERATOR_OUTPUT}/${BUILD_SUBDIRECTORY} ${JOBS} ${TARGETS}
fi
