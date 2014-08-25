
SCRIPTDIR=$(cd $(dirname ${BASH_SOURCE[0]}); pwd -P)
TOPDIR=$(cd ${SCRIPTDIR}/.. ; pwd -P)
source ${SCRIPTDIR}/common.sh

function usage() {
  echo "Usage:"
  echo "source build/ccache_env.sh desktop|mobile|tv"
}


if [ "$1" == "desktop" -o  "$1" == "mobile" -o "$1" == "tv" ]; then
  OUTPUT_BASE_FOLDER=out.${1}.$(getHostArch)
  export CCACHE_CPP2=yes
  export CCACHE_SLOPPINESS=time_macros
  export CCACHE_BASEDIR=${TOPDIR}/src
  export CCACHE_DIR=${TOPDIR}/${OUTPUT_BASE_FOLDER}.ccache
  export CCACHE_COMPILERCHECK=content
  CCACHESIZE=$(ccache -s | grep max | grep -oE "[0-9]+" | head -1)
  if [ $CCACHESIZE == "1" ]; then
     ccache -M 10
  fi

  if [ "$1" == "desktop" ]; then
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
