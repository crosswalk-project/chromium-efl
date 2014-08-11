#!/bin/bash

SCRIPTDIR=$( cd $(dirname ${BASH_SOURCE[0]}) ; pwd -P )
TOPDIR=$( cd ${SCRIPTDIR}/.. ; pwd -P )

source ${SCRIPTDIR}/common.sh

host_arch=$(getHostArch)

export GYP_GENERATOR_OUTPUT="out.${host_arch}"

jhbuild --no-interact -f ${SCRIPTDIR}/jhbuild/jhbuildrc

export JHBUILD_DEPS="${TOPDIR}/${GYP_GENERATOR_OUTPUT}/Dependencies/Root"

if [ "${host_arch}" == "x64" ]; then
  _LIBDIR=lib64
elif [ "${host_arch}" == "ia32" ]; then
  _LIBDIR=lib
fi

export PKG_CONFIG_PATH="${JHBUILD_DEPS}/${_LIBDIR}/pkgconfig"
export LD_LIBRARY_PATH="${JHBUILD_DEPS}/${_LIBDIR}"
export PATH="$PATH:${JHBUILD_DEPS}/bin"



if [ ! -e ${TOPDIR}/src/build/util/LASTCHANGE.blink ]; then
  ${TOPDIR}/src/build/util/lastchange.py -s ${TOPDIR}/src/third_party/WebKit -o ${TOPDIR}/src/build/util/LASTCHANGE.blink
fi

${TOPDIR}/build/gyp_chromiumefl --depth=${TOPDIR}/src -I${TOPDIR}/impl/chromium-efl.gypi \
                                 --generator-output ${TOPDIR}/${GYP_GENERATOR_OUTPUT} \
                                 --format=ninja \
                                 --check \
                                 -Goutput_dir=${TOPDIR}/${GYP_GENERATOR_OUTPUT} \
                                 -Dchrome_src_dir=${TOPDIR}/src \
                                 -Defl_impl_dir=${TOPDIR}/impl \
                                 -DOS=linux \
                                 -Duse_libjpeg_turbo=0 \
                                 -Dproprietary_codecs=1 \
                                 -Duse_xi2_mt=2 \
                                 -Dtoolkit_uses_gtk=0 \
                                 -Duse_aura=1 \
                                 -Dlinux_use_bundled_binutils=0 \
                                 -Dclang=0 \
                                 -DSK_SUPPORT_LEGACY_BITMAP_CONFIG=1 \
                                 ${TOPDIR}/impl/chromium-efl.gyp
