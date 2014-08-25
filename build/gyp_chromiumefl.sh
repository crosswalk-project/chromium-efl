#!/bin/bash

SCRIPTDIR=$( cd $(dirname $0) ; pwd -P )
TOPDIR=$( cd ${SCRIPTDIR}/.. ; pwd -P )

source ${SCRIPTDIR}/common.sh

host_arch=$(getHostArch)
target=desktop

#need to be exported because it is used by jhbuild
export GYP_GENERATOR_OUTPUT=${TOPDIR}/"out.${host_arch}"

if echo "$@" | grep -cq '\-\Dbuilding_for_tizen_mobile=1'; then
  GYP_GENERATOR_OUTPUT=${TOPDIR}/"out.mobile.${host_arch}"
  target=mobile
fi
if echo "$@" | grep -cq '\-\Dbuilding_for_tizen_tv=1'; then
  GYP_GENERATOR_OUTPUT=${TOPDIR}/"out.tv.${host_arch}"
  target=tv
fi

if [ "$target" == "desktop" ]; then
  JHBUILD_DEPS="${GYP_GENERATOR_OUTPUT}/Dependencies/Root"

  if [ "${host_arch}" == "x64" ]; then
    _LIBDIR=lib64
  elif [ "${host_arch}" == "ia32" ]; then
    _LIBDIR=lib
  fi

  export PKG_CONFIG_PATH="${JHBUILD_DEPS}/${_LIBDIR}/pkgconfig"
fi

set -e

if [ ! -e ${TOPDIR}/src/build/util/LASTCHANGE.blink ]; then
  ${TOPDIR}/src/build/util/lastchange.py -s ${TOPDIR}/src/third_party/WebKit -o ${TOPDIR}/src/build/util/LASTCHANGE.blink
fi

COMMON_GYP_PARAMETERS="--depth=${TOPDIR}/src
                      -I${TOPDIR}/impl/chromium-efl.gypi
                      --generator-output ${GYP_GENERATOR_OUTPUT}
                      --format=ninja
                      --check
                      -Goutput_dir=${GYP_GENERATOR_OUTPUT}
                      -Dchrome_src_dir=${TOPDIR}/src
                      -Defl_impl_dir=${TOPDIR}/impl
                      -Duse_libjpeg_turbo=0
                      -Dproprietary_codecs=1
                      -Dclang=0
                      -Duse_aura=1
                      "

#reverting back origin files
${SCRIPTDIR}/restore_gyp.sh

if [ "$target" == "desktop" ]; then
  ADDITIONAL_GYP_PARAMETERS+="-Dbuilding_for_tizen=0
                             "
else
  ADDITIONAL_GYP_PARAMETERS+="-Dbuilding_for_tizen=1
                              -Duse_gstreamer=1
                              -Dwerror=
                              -Dsysroot=
                              --no-parallel
                              -Dpython_ver=$(getPythonVersion)
                              -Dtarget_arch=$host_arch
                              -Dhost_arch=$host_arch
                             "

  if [ "${GBS_ARCH}" == "arm" ]; then
    ADDITIONAL_GYP_PARAMETERS+="-Darmv7=1
                                -Darm_thumb=1
                                -Darm_neon=1
                               "
  fi

  SYSTEM_DEPS="-Duse_alsa=0
               -Duse_system_expat=1
               -Duse_system_flac=1
               -Duse_system_icu=1
               -Duse_system_libjpeg=1
               -Duse_system_libpng=1
               -Duse_system_libusb=1
               -Duse_system_libxml=1
               -Duse_system_libxslt=1
               -Duse_system_minizip=1
               -Duse_system_speex=1
               -Duse_system_zlib=1
              "
  if [ "$target" == "mobile" ]; then
      SYSTEM_DEPS+=" -Duse_system_bzip2=1
                     -Duse_system_libexif=1
                     -Duse_system_nspr=1
                     -Duse_system_vpx=1"
  fi

  #and replacing original files with correct ones according to $SYSTEM_DEPS
  $TOPDIR/src/build/linux/unbundle/replace_gyp_files.py $SYSTEM_DEPS

fi


if [ "$target" == "desktop" ]; then
  jhbuild --no-interact -f ${SCRIPTDIR}/jhbuild/jhbuildrc
fi

set -x
${TOPDIR}/build/gyp_chromiumefl \
                                 $COMMON_GYP_PARAMETERS \
                                 $SYSTEM_DEPS \
                                 $ADDITIONAL_GYP_PARAMETERS \
                                 $@ \
                                 ${TOPDIR}/impl/chromium-efl.gyp
