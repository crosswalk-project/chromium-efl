#!/bin/bash

SCRIPTDIR=$( cd $(dirname $0) ; pwd -P )
TOPDIR=$( cd ${SCRIPTDIR}/.. ; pwd -P )

source ${SCRIPTDIR}/common.sh

host_arch=$(getHostArch)
target=desktop

GYP_GENERATOR_OUTPUT=${TOPDIR}/"out.${host_arch}"

if echo "$@" | grep -cq '\-\Dbuilding_for_tizen_mobile=1'; then
  GYP_GENERATOR_OUTPUT=${TOPDIR}/"out.mobile.${host_arch}"
  target=mobile
fi
if echo "$@" | grep -cq '\-\Dbuilding_for_tizen_tv=1'; then
  GYP_GENERATOR_OUTPUT=${TOPDIR}/"out.tv.${host_arch}"
  target=tv
fi

set -e

ORIGINAL_GYP_DEFINES="$GYP_DEFINES"
export GYP_DEFINES=$(echo "$GYP_DEFINES" | sed -e 's/component\s*=\s*shared_library//g')
if [ "$ORIGINAL_GYP_DEFINES" != "$GYP_DEFINES" ]; then
    echo "WARNING: component build is not supported."
    echo "Removing component=shared_library from GYP_DEFINES."
fi

COMMON_GYP_PARAMETERS="--depth=${TOPDIR}/src
                      -I${TOPDIR}/impl/chromium-efl.gypi
                      --generator-output ${GYP_GENERATOR_OUTPUT}
                      --format=ninja
                      --check
                      -Goutput_dir=${GYP_GENERATOR_OUTPUT}
                      -Dchrome_src_dir=${TOPDIR}/src
                      -Defl_impl_dir=${TOPDIR}/impl
                      -Duse_libjpeg_turbo=1
                      -Dproprietary_codecs=1
                      -Dclang=0
                      -Duse_aura=1
                      -Duse_efl=1
                      -Dtizen_multimedia_support=1
                      "

if [ "$target" == "desktop" ]; then
  ADDITIONAL_GYP_PARAMETERS+="-Dbuilding_for_tizen=0
                             "
else
  ADDITIONAL_GYP_PARAMETERS+="-Dbuilding_for_tizen=1
                              -Dwerror=
                              -Dsysroot=
                              --no-parallel
                              -Dpython_ver=$(getPythonVersion)
                              -Dtarget_arch=$host_arch
                              -Dhost_arch=$host_arch
                              -Denable_basic_printing=0
                              -Denable_print_preview=0
                              -Duse_kerberos=0
                              -Duse_gconf=0
                              -Dlinux_use_bundled_gold=0
                              -Dlinux_use_bundled_binutils=0
                              -Dprebuilt_ld_gold_dir=${TOPDIR}/build/prebuild
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
               -Duse_system_libjpeg=0
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

set +e
set -x
${TOPDIR}/build/gyp_chromiumefl \
                                 $COMMON_GYP_PARAMETERS \
                                 $SYSTEM_DEPS \
                                 $ADDITIONAL_GYP_PARAMETERS \
                                 $@ \
                                 ${TOPDIR}/ewk/chromium-ewk.gyp

ret=$?

if [ "$target" != "desktop" ]; then
  # Restore gyp files to their original states not to mess up the tree permanently.
  $TOPDIR/src/build/linux/unbundle/replace_gyp_files.py --undo $SYSTEM_DEPS
fi

exit $ret
