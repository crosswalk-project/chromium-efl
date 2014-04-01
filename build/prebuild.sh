#!/bin/bash

# set build configuration for desktop|mobile|tv
export SCRIPT_PATH=$(readlink -f ${BASH_SOURCE[0]})
export SCRIPT_DIR=$(dirname $SCRIPT_PATH)
export EFL_WEBVIEW_DIR="${SCRIPT_DIR}/../"
export CHROME_SRC="${EFL_WEBVIEW_DIR}/src"

# set desktop build configuration
prepare_desktop_build () {
  echo "set 'desktop' build configuration..."
  echo "desktop" > "${EFL_WEBVIEW_DIR}/build/TARGET"

  # restore gyp files
  . ./build/restore_gyp.sh

  # excute prebuild for desktop
  . ./build/prebuild_desktop.sh

  # usage ninja build for desktop
  echo "### Usage ninja build for desktop"
  echo "    $ ninja -C <outdir>"
  echo "      where <outdir> : ${GYP_GENERATOR_OUTPUT}/Release (if Release mode) "
  echo "                     : ${GYP_GENERATOR_OUTPUT}/Debug (if Debug mode)"
}

# set mobile build configuration
prepare_mobile_build () {
  echo "set 'mobile' build configuration..."
  echo "mobile" > "${EFL_WEBVIEW_DIR}/build/TARGET"

  # restore gyp files
  . ./build/restore_gyp.sh

  # excute prebuild for tizen mobile
  . ./build/prebuild_mobile.sh

  # usage gbs build for tizen mobile
  echo "### Usage GBS build for mobile (using .gbs.conf for mobile)"
  echo "    $ gbs build -A armv7l --incremental --include-all [--clean]"
  echo "      [--define '_debug_mode 1']   excute debug build (default : release)"
  echo "      [--define '_skip_gyp 1']     skip gyp generate  (default : gyp generate)"
}

# set tv build configuration
prepare_tv_build () {
  echo "set 'tv' build configuration..."
  echo "tv" > "${EFL_WEBVIEW_DIR}/build/TARGET"

  # restore gyp files
  . ./build/restore_gyp.sh

  # excute prebuild for tizen tv
  . ./build/prebuild_tv.sh

  # usage gbs build for tizen tv
  echo "### Usage GBS build for tv (using .gbs.conf for tv)"
  echo "    $ gbs build -A armv7l --incremental --include-all [--clean]"
  echo "      [--define '_debug_mode 1']   excute debug build (default : release)"
  echo "      [--define '_skip_gyp 1']     skip gyp generate  (default : gyp generate)"
}

# print out usage
usage_prebuild () {
  echo "### Unknown target name. (Only use desktop | mobile | tv)"
  echo "    $ ./build/prebuild.sh [desktop | mobile | tv]"
}

# excute build configuration from get options
while true
do
  case $1 in
    desktop)
      prepare_desktop_build
      break
      ;;
    mobile)
      prepare_mobile_build
      break
      ;;
    tv)
      prepare_tv_build
      break
      ;;
    *)
      if [ -z $1 ]; then
        prepare_mobile_build
      else
        usage_prebuild
      fi
      break
      ;;
  esac
done
