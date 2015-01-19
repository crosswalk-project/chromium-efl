#!/bin/bash

source $(dirname $0)/common.sh
trap 'error_report $0 $LINENO' ERR SIGINT SIGTERM SIGQUIT
host_arch=$(getHostArch)

if [ -z "$GYP_GENERATOR_OUTPUT" ]; then
  GYP_GENERATOR_OUTPUT=${TOPDIR}/"out.${host_arch}"
fi

EXTRA_GYP_ARGS="$@"

# __GYP_CHROMIUMEFL_TARGET is defined when we are called from build_desktop.sh.
# Otherwise we are called by gbs whch already passes the correct building_for_* flag.
if [ "$__GYP_CHROMIUMEFL_TARGET" == "desktop" ]; then
  target=desktop
elif echo "$@" | grep -cq '\-\Dbuilding_for_tizen_mobile=1'; then
  target=mobile
  buildType=gbs
elif echo "$@" | grep -cq '\-\Dbuilding_for_tizen_tv=1'; then
  target=tv
  buildType=gbs
fi

if [ "$__GYP_CHROMIUMEFL_TARGET" == "crosscompile" ]; then
  buildType=crosscompile
  EXTRA_GYP_ARGS+=" -Dedje_compiler=${TOPDIR}/out.${host_arch}/Dependencies/Root/bin/edje_cc"
  if [ "$target" == "mobile" ]; then
    EXTRA_GYP_ARGS+=" -Dcustom_libc_dir=/opt/usr/eglibc-2.18/lib"
  fi
fi

ORIGINAL_GYP_DEFINES="$GYP_DEFINES"
export GYP_DEFINES=$(echo "$GYP_DEFINES" | sed -e 's/component\s*=\s*shared_library//g')
if [ "$ORIGINAL_GYP_DEFINES" != "$GYP_DEFINES" ]; then
    echo "WARNING: component build is not supported."
    echo "Removing component=shared_library from GYP_DEFINES."
fi

COMMON_GYP_PARAMETERS="
                      -Duse_libjpeg_turbo=1
                      -Dproprietary_codecs=1
                      -Dtizen_multimedia_support=1
                      -Duse_aura=0
                      -Duse_efl=1
                      -Duse_gnome_keyring=0
                      -Duse_openssl=1
                      -Duse_openssl_certs=1
                      -Dtoolkit_uses_gtk=0
                      "
if [[ $BUILD_CONTENT_SHELL == 1 ]]; then
  COMMON_GYP_PARAMETERS+="-Dchromium_efl_library_type=static_library"
fi

add_desktop_flags() {
  ADDITIONAL_GYP_PARAMETERS+="-Dbuilding_for_tizen=0
                              -Dclang=${USE_CLANG}
                              -Dmake_clang_dir=${CLANG_DIR}
                              -Dtizen_multimedia_use_capi_for_me=0
                             "
}

add_arm_flags() {
  ADDITIONAL_GYP_PARAMETERS+="-Darmv7=1
                              -Darm_thumb=1
                              -Darm_neon=1
                             "
}

add_tizen_flags() {
  ADDITIONAL_GYP_PARAMETERS+="-Dbuilding_for_tizen=1
                              -Dwerror=
                              -Dpython_ver=$(getPythonVersion)
                              -Denable_basic_printing=1
                              -Denable_print_preview=1
                              -Duse_kerberos=0
                              -Duse_gconf=0
                              -Duse_alsa=0
                              -Dclang=0
                              -Dlinux_use_bundled_gold=0
                              -Dlinux_use_bundled_binutils=0
                              -Dprebuilt_ld_gold_dir=${TOPDIR}/build/prebuild
                             "
  if [ "$target" == "mobile" ]; then
    ADDITIONAL_GYP_PARAMETERS+="-Dtizen_multimedia_use_capi_for_me=0
                               "
  elif [ "$target" == "tv" ]; then
    ADDITIONAL_GYP_PARAMETERS+="-Dtizen_multimedia_use_capi_for_me=1
                               "
  fi
}

add_gbs_flags() {
  ADDITIONAL_GYP_PARAMETERS+="-Dtarget_arch=$host_arch
                              -Dhost_arch=$host_arch
                              -Dsysroot=
                              --no-parallel
                             "
  # TODO(b.kelemen): ideally crosscompile should also support system libs.
  # Unfortunately the gbs root doesn't contain everything chromium needs.
  SYSTEM_DEPS="-Duse_system_expat=1
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
    SYSTEM_DEPS+="-Duse_system_bzip2=1
                  -Duse_system_libexif=1
                  -Duse_system_nspr=1
                 "
  fi
}

add_cross_flags() {
  ADDITIONAL_GYP_PARAMETERS+="-Dtarget_arch=arm
                              -Dhost_arch=$host_arch
                              -Dsysroot=$SYSROOTDIR
                              -Darm_tune=arm7
                              -Dhost_clang=0
                             "

  # Compiling yasm with crosscompile + icecc leads to some strange errors (one file is built for target instead of host).
  # Yasm is an assembler used only by the build (not at runtime) and it is generally available in Linux distros so let's just
  # use it from the system.
  ADDITIONAL_GYP_PARAMETERS+="-Duse_system_yasm=1"
}

if [ "$target" == "desktop" ]; then
  add_desktop_flags
else
  add_tizen_flags
  if [ "$target" == "mobile" -o "$target" == "tv" ]; then
    if [ "$target_arch" == "arm" ]; then
      add_arm_flags
    fi
    if [ "$buildType" == "gbs" ]; then
      add_gbs_flags
    elif [ "$buildType" == "crosscompile" ]; then
      add_cross_flags
    fi
  fi
  if [ "$SYSTEM_DEPS" != "" ]; then
    #replacing original files with correct ones according to $SYSTEM_DEPS
    $TOPDIR/src/build/linux/unbundle/replace_gyp_files.py $SYSTEM_DEPS
  fi
fi

set -x
${TOPDIR}/build/gyp_chromiumefl \
                                 $COMMON_GYP_PARAMETERS \
                                 $SYSTEM_DEPS \
                                 $ADDITIONAL_GYP_PARAMETERS \
                                 $EXTRA_GYP_ARGS \
                                 ${TOPDIR}/ewk/chromium-ewk.gyp

ret=$?

if [ "$SYSTEM_DEPS" != "" ]; then
  # Restore gyp files to their original states not to mess up the tree permanently.
  $TOPDIR/src/build/linux/unbundle/replace_gyp_files.py --undo $SYSTEM_DEPS
fi

exit $ret
