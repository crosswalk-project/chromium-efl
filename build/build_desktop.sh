#!/bin/bash

SCRIPTDIR=$( cd $(dirname $0) ; pwd -P )
TOPDIR=$( cd ${SCRIPTDIR}/.. ; pwd -P )
SRCDIR=$( cd ${TOPDIR}/.. ; pwd -P )

source ${SCRIPTDIR}/common.sh
trap 'error_report $0 $LINENO' ERR SIGINT SIGTERM SIGQUIT

host_arch=$(getHostArch)

if [ -z "$GYP_GENERATOR_OUTPUT" ]; then
  export GYP_GENERATOR_OUTPUT=${SRCDIR}/"out.${host_arch}"
fi

parseHostBuildScriptParams desktop $@

JHBUILD_STAMPFILE="${GYP_GENERATOR_OUTPUT}/Dependencies/Root/jhbuild.stamp"

shouldForceJHBuild() {
  if [[ $FORCE_JHBUILD == 1 ]]; then
    return 1
  fi

  # Check if anything in jhbuild is more recent than stamp file.
  return $(find $SCRIPTDIR/jhbuild -type f -newer $JHBUILD_STAMPFILE -print | wc -l)
}

if [[ $(shouldForceJHBuild) > 0 ]]; then
  rm -f $JHBUILD_STAMPFILE
fi

JHBUILD_DEPS="${GYP_GENERATOR_OUTPUT}/Dependencies/Root"
if [ "${host_arch}" == "x64" ]; then
  _LIBDIR=lib64
elif [ "${host_arch}" == "ia32" ]; then
  _LIBDIR=lib
fi
export PKG_CONFIG_PATH="${JHBUILD_DEPS}/${_LIBDIR}/pkgconfig"

if [ ! -f "$JHBUILD_STAMPFILE" ]; then
  jhbuild --no-interact -f ${SCRIPTDIR}/jhbuild/jhbuildrc

  if [[ $? == 0 ]]; then
    echo "Yay! jhbuild done!" > $JHBUILD_STAMPFILE
  fi
fi

export __GYP_CHROMIUMEFL_TARGET=desktop

hostGypChromiumEfl

export LD_LIBRARY_PATH="${JHBUILD_DEPS}/${_LIBDIR}:$LD_LIBRARY_PATH"
export PATH="${JHBUILD_DEPS}/bin:$PATH"

hostNinja desktop
