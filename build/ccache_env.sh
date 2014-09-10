
SCRIPTDIR=$(cd $(dirname ${BASH_SOURCE[0]}); pwd -P)
TOPDIR=$(cd ${SCRIPTDIR}/.. ; pwd -P)
source ${SCRIPTDIR}/common.sh

function usage() {
  echo "Usage:"
  echo "source build/ccache_env.sh desktop|mobile|tv"
}

if ! type ccache &> /dev/null; then
  echo "WARNING: ccache option used but ccache is not installed"
  return 0
fi

if [ "$1" == "desktop" -o  "$1" == "mobile" -o "$1" == "tv" -o "$1" == "crosscompile" ]; then
  OUTPUT_BASE_FOLDER=out.${1}.$(getHostArch)
  export CCACHE_CPP2=yes
  export CCACHE_SLOPPINESS=time_macros
  export CCACHE_BASEDIR=${TOPDIR}

  if [ "$USER" == "abuild" ]; then
    #/.ccache directory in gbs sysroot is not removed by -C option
    #unlike /var/tmp and /tmp directories
    #so having /.ccache as out ccache dir will make possible to use
    #ccache also with bot incremental and non-incremental builds
    export CCACHE_DIR=/.ccache/${OUTPUT_BASE_FOLDER}
  fi

  export CCACHE_COMPILERCHECK=content
  CCACHESIZE=$(ccache -s | grep max | grep -oE "[0-9]+" | head -1)
  if [ $CCACHESIZE == "1" ]; then
     ccache -M 10
  fi

  if [ "$1" == "desktop" -o "$1" == "crosscompile" ]; then
    ccacheInPath=$(echo $PATH | grep -c /usr/lib/ccache)
    if [ "$ccacheInPath" == "0" ]; then
      export PATH=/usr/lib/ccache:$PATH
    fi
  fi
else
  echo "Warning! You need to pick correct profile when you include ccache_env.sh"
  echo "ccache will not be configured"
  usage
fi
