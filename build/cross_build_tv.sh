#!/bin/bash

SCRIPTDIR=$( cd $(dirname $0) ; pwd -P )
TOPDIR=$( cd ${SCRIPTDIR}/.. ; pwd -P )

source ${SCRIPTDIR}/common.sh
trap 'postBuild ${SYSROOTDIR};\
      error_report $0 $LINENO' ERR SIGINT SIGTERM SIGQUIT

host_arch=$(getHostArch)


__SUFFIX=crosscompile.orig

function adaptGbsSysrootToCrossCompilation() {
  echo "cd /etc/; [ -e ld.so.conf.d ] && mv ld.so.conf.d ld.so.conf.d.$__SUFFIX;" | gbs chroot --root ${1}
}
function rollbackGbsSysrootChanges() {
  echo "cd /etc/; [ -e ld.so.conf.d.$__SUFFIX ] && mv ld.so.conf.d.$__SUFFIX ld.so.conf.d;" | gbs chroot --root ${1}
}

function preBuild() {
  adaptGbsSysrootToCrossCompilation $1
  ${SCRIPTDIR}/apply_patches.sh ${SCRIPTDIR}/patches
}

function postBuild() {
  rollbackGbsSysrootChanges $1
  ${SCRIPTDIR}/apply_patches.sh -r ${SCRIPTDIR}/patches
}

if [ -z "$GYP_GENERATOR_OUTPUT" ]; then
  export GYP_GENERATOR_OUTPUT=${TOPDIR}/"out.cross.tv.arm"
fi

parseHostBuildScriptParams crosscompile $@

if [ -z "$GBS_ROOT_PATH" ]; then
  echo "Set GBS_ROOT_PATH for cross compilation"
  exit 1
fi

if [ -z "$CROSS_COMPILE" ]; then
  echo "Set CROSS_COMPILE for cross compilation like: /home/user/toolchains/bin/armv7l-tizen-linux-gnueabi-"
  exit 1
fi

# Only override if not set to enable customization for distributed compiling.
export CC_target=$(setIfUndef CC_target ${CROSS_COMPILE}gcc)
export CXX_target=$(setIfUndef CXX_target ${CROSS_COMPILE}g++)
export AR_target=${CROSS_COMPILE}ar
export AS_target=${CROSS_COMPILE}as
export RANLIB_target=${CROSS_COMPILE}ranlib

export SYSROOTDIR="${GBS_ROOT_PATH}"/local/BUILD-ROOTS/scratch.armv7l.0
export PKG_CONFIG_SYSROOT_DIR="${SYSROOTDIR}"
export PKG_CONFIG_PATH="${SYSROOTDIR}/usr/lib/pkgconfig:${SYSROOTDIR}/usr/share/pkgconfig"
export GYP_CROSSCOMPILE=1

export PATH="${TOPDIR}/build/cross-shim:$PATH"

preBuild ${SYSROOTDIR}

export __GYP_CHROMIUMEFL_TARGET=crosscompile
hostGypChromiumEfl -Dbuilding_for_tizen_tv=1 -Dchromium_efl_tizen_version="2.2.1"

hostNinja crosscompile
RET=$?

postBuild ${SYSROOTDIR}

exit $RET
