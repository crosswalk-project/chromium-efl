#!/bin/bash

pushd `dirname $0` > /dev/null
export SCRIPTDIR=`pwd -L`
popd > /dev/null

export TOPDIR=$(readlink -f "${SCRIPTDIR}/..")
if [[ -z "${CHROME_SRC}" ]]; then
  # If $CHROME_SRC was not set, assume src subfolder in project directory is CHROME_SRC.
  export CHROME_SRC="${SRCDIR}"
fi

export CLANG_DIR="${TOPDIR}/src/third_party/llvm-build/Release+Asserts/"

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
   -h, --help            Show this message
   --build-ewk-unittests Build ewk unittests
   --ccache              Configure ccache installed in your system
   --clang               Use chromium's clang compiler to build the sources
   --content-shell       Build content_shell application
   --debug               Build debug version of chromium-efl (out.${host_arch}/Debug instead of out.${host_arch}/Release)
   -jN                   Set number of jobs, just like with make or ninja
   --skip-gyp            Skip restore_gyp, jhbuild and gyp_chromium steps
   --skip-ninja          Skip ninja step

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
  export USE_CLANG=0
  export FORCE_JHBUILD=0
  export SKIP_NINJA=0
  export BUILD_EWK_UNITTESTS=0
  export BUILD_CONTENT_SHELL=0
  export BUILD_SUBDIRECTORY=Release

  local platform="$1"
  shift

  while [[ $# > 0 ]]; do
    case "$1" in
      -h|--help)
        hostBuldScriptUsage ${0}
        ;;
      --skip-gyp)
        export SKIP_GYP=1
        ;;
      --ccache)
        echo using ccache
        export USE_CCACHE=1
        source $TOPDIR/build/ccache_env.sh ${platform}
        ;;
      --clang)
        export USE_CLANG=1
        ;;
      --content-shell)
        export BUILD_CONTENT_SHELL=1
        ;;
      --force-jhbuild)
        export FORCE_JHBUILD=1
        ;;
      --skip-ninja)
        export SKIP_NINJA=1
        ;;
      --build-ewk-unittests)
        export BUILD_EWK_UNITTESTS=1
        ;;
      --debug)
        export BUILD_SUBDIRECTORY="Debug"
        ;;
      -j*)
        export JOBS="$1"
        ;;
      *)
        echo "Unknown argument: $1"
        exit 1
        ;;
    esac
    shift;
  done
}

function hostGypChromiumEfl() {
  if [[ $SKIP_GYP == 0 ]]; then
    ${TOPDIR}/build/gyp_chromiumefl.sh $@
  fi
}

function hostNinja() {
  if [[ $SKIP_NINJA == 0 ]]; then
    TARGETS="chromium-efl efl_webprocess chromium-ewk efl_webview_app"
    if [[ $BUILD_EWK_UNITTESTS == 1 ]]; then
      TARGETS="$TARGETS ewk_unittests"
    fi
    if [[ $BUILD_CONTENT_SHELL == 1 ]]; then
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
