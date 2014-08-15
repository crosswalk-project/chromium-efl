#!/bin/bash

if [ ! -z ${BASH_SOURCE[0]} ]; then
  _ARGV0=${BASH_SOURCE[0]}
else
  _ARGV0=$0
fi

if [ "${BUILDING_WITH_GBS}" = 1 ]; then
  TOP_DIR=$(pwd -P)
else
  _SCRIPT=$(readlink -f $_ARGV0)
  _SCRIPT_DIR=$(dirname $_SCRIPT)
  TOP_DIR="${_SCRIPT_DIR}/../"
fi

setup_jhbuild_deps() {
  export JHBUILD_DEPS="${TOP_DIR}/${GYP_GENERATOR_OUTPUT}/Dependencies/Root"
  if [ -d "$JHBUILD_DEPS" ] ; then
    if [ ${host_arch} = 'x64' ]; then
      _LIBDIR=lib64
    elif [ ${host_arch} = 'ia32' ]; then
      _LIBDIR=lib
    fi
    export PKG_CONFIG_PATH="${JHBUILD_DEPS}/${_LIBDIR}/pkgconfig"
    export LD_LIBRARY_PATH="${JHBUILD_DEPS}/${_LIBDIR}"
    export PATH="$PATH:${JHBUILD_DEPS}/bin"
  fi
}

if [[ -z "${CHROME_SRC}" ]]; then
  # If $CHROME_SRC was not set, assume src subfolder in current directory is CHROME_SRC.
  export CHROME_SRC="${TOP_DIR}/src"
fi

if [ "$CHROMIUM_TIZEN_CROSSCOMPILE" = 1 ] ; then
  if [ "x"$TIZEN_SDK_PATH = "x" ]; then
    TIZEN_SDK_PATH="$HOME/tizen-sdk"
  fi
  if [ "x"$GBS_ROOT_PATH = "x" ]; then
    GBS_ROOT_PATH="$HOME/GBS-ROOT"
  fi

  export CROSSTOOLROOT="${TIZEN_SDK_PATH}"/tools/arm-linux-gnueabi-gcc-4.5
  export CROSSTOOLLIBEXEC=$CROSSTOOLROOT/libexec/gcc/arm-linux-gnueabi/4.5.4
  export CROSSTOOLBIN=$CROSSTOOLROOT/bin
  export CROSSTOOL=$CROSSTOOLBIN/arm-linux-gnueabi
  export CXX_target=$CROSSTOOL-g++
  export CC_target=$CROSSTOOL-gcc
  export AR_target=$CROSSTOOL-ar
  export AS_target=$CROSSTOOL-as
  export RANLIB_target=$CROSSTOOL-ranlib
  export CXX_host=g++
  export CC_host=gcc
  export AR_host=ar
  export AS_host=as
  export RANLIB_host=ranlib
  export PATH=$CROSSTOOLBIN:$PATH

  export SYSROOTDIR="${GBS_ROOT_PATH}"/local/BUILD-ROOTS/scratch.armv7l.0
  export GYP_CROSSCOMPILE=1
  toolchain_target=arm-linux-gnueabi-gcc-4.5
  toolchain_path="${TIZEN_SDK_PATH}"/tools/arm-linux-gnueabi-gcc-4.5/bin
  export PKG_CONFIG_PATH=/usr/lib
else
  export GYP_CROSSCOMPILE=0
  export SYSROOTDIR="\"\""
fi

export GYP_GENERATORS=ninja
host_os=$(uname -s | sed -e 's/Linux/linux/;s/Darwin/mac/')
host_arch=$(uname -m | sed -e \
        's/i.86/ia32/;s/x86_64/x64/;s/amd64/x64/;s/arm.*/arm/;s/i86pc/ia32/')
python_ver=$(python --version  2>&1 | sed -e 's/Python \([0-9]\+\.[0-9]\+\)\.[0-9]\+/\1/')

DEFINES="OS=linux"
DEFINES+=" host_os=${host_os} host_arch=${host_arch} python_ver=${python_ver}"
DEFINES+=" use_libjpeg_turbo=0 proprietary_codecs=1"
DEFINES+=" use_xi2_mt=2 toolkit_uses_gtk=0 use_aura=1 toolkit_efl=1"
DEFINES+=" use_gstreamer=1 clang=0 host_clang=0"

echo "[M37] TODO: Fix and re-enable notifications (edit build/envsetup.sh)"
DEFINES+=" notifications=0"

# System dependencies
if [ -e "${TOP_DIR}/build/TARGET" ]; then
  if [ $(cat "${TOP_DIR}/build/TARGET") = "mobile" ]; then
    . build/system_deps_mobile.sh
  else # tv
    . build/system_deps_tv.sh
  fi
  DEFINES+=" $SYSTEM_DEPS"
fi

if [ $host_arch = "arm" -o "$CHROMIUM_TIZEN_CROSSCOMPILE" = 1 ] ; then
  target_arch="arm"
  DEFINES+=" sysroot=${SYSROOTDIR}"
  DEFINES+=" target_arch=arm armv7=1 arm_thumb=1 arm_neon=1"
fi

# Add prebuild folder with prebuild build tools for arm for GBS build.
if [ $host_arch = "arm" ] ; then
  export PATH=${TOP_DIR}/build/prebuild:$PATH
fi

#  toolkit_views=1

# component=shared results in strange link errors. Probably something to fix upstream.
GYP_DEFINES_NO_SHARED=$(echo $GYP_DEFINES | sed 's|component=shared_library||g')
export GYP_DEFINES="${GYP_DEFINES_NO_SHARED}"
export GYP_GENERATOR_OUTPUT="out.${host_arch}"

# Use jhbuild dependencies.
setup_jhbuild_deps

if [ "${BUILDING_WITH_GBS}" = 1 -o "${CHROMIUM_TIZEN_CROSSCOMPILE}" = 1 ]; then
  DEFINES+=" building_for_tizen=1"
  # get Target Name and add Flag to GYP_DEFINES
  if [ -e "${TOP_DIR}/build/TARGET" ]; then
    if [ $(cat "${TOP_DIR}/build/TARGET") = "mobile" ]; then
      DEFINES+=" building_for_tizen_mobile=1"
      DEFINES+=" building_for_tizen_tv=0"
    else # tv
      DEFINES+=" building_for_tizen_tv=1"
      DEFINES+=" building_for_tizen_mobile=0"
    fi
  else
    DEFINES+=" building_for_tizen_mobile=0"
    DEFINES+=" building_for_tizen_tv=0"
  fi
else
  DEFINES+=" building_for_tizen=0"
  DEFINES+=" building_for_tizen_mobile=0"
  DEFINES+=" building_for_tizen_tv=0"
fi

export GYP_DEFINES="${DEFINES} ${GYP_DEFINES}"

# Fetches EFL build dependencies using jhbuild.
efl_jhbuild() {
  jhbuild --no-interact -f ${TOP_DIR}/build/jhbuild/jhbuildrc
  if [ $? -eq 0 ]; then
    setup_jhbuild_deps
  fi
}

# Performs a gyp_chromium run to convert gyp->Makefile for android code.
gyp_chromiumefl() {
  # This is just a simple wrapper of gyp_chromium, please don't add anything
  # in this function.
  echo "GYP_GENERATORS set to '$GYP_GENERATORS'"
  (
    "${TOP_DIR}/build/gyp_chromiumefl" "$@"
  )
}

if [ "$target_arch" = "arm" ] ; then
  export EFLWV_APP_PATH=/usr/bin
  export EFLWV_LIB_PATH=/usr/lib
  export CHROMIUM_DIR=/usr/share/org.tizen.chromium-efl/
  export STRIP_BIN=${TIZEN_SDK_PATH}/tools/arm-linux-gnueabi-gcc-4.5/arm-linux-gnueabi/bin/strip
  export SDB_BIN=${TIZEN_SDK_PATH}/tools/sdb

  tizen_deploy_file() {
    echo "Processing $1"
    ${STRIP_BIN} -o /tmp/$1 $1
    ${SDB_BIN} push /tmp/$1 $2/$1
    rm -f /tmp/$1
  }

  determine_output_folder() {
    if [ $# -eq 0 ]; then
      return
    fi
    if [ "$1" = "release" -o "$1" = "r" ]; then
      OUTPUT_FOLDER=${TOP_DIR}/${GYP_GENERATOR_OUTPUT}/Release
    elif [ "$1" = "debug" -o "$1" = "d" ]; then
      OUTPUT_FOLDER=${TOP_DIR}/${GYP_GENERATOR_OUTPUT}/Debug
    fi
  }

  tizen_deploy() {
    determine_output_folder $@
    if [ ! -d "$OUTPUT_FOLDER" ]; then
      echo "Usage: tizen_deploy r[elease] | d[ebug]"
      return
    fi

    ${SDB_BIN} shell mount -o remount,rw /
    pushd ${OUTPUT_FOLDER}/lib
    SHARED=`ls -1 *.so`
    for K in ${SHARED} ; do
      tizen_deploy_file ${K} ${EFLWV_LIB_PATH}
    done
    popd
    pushd ${OUTPUT_FOLDER}
    ${SDB_BIN} push content_shell.pak ${CHROMIUM_DIR}/
    popd
  }
fi
