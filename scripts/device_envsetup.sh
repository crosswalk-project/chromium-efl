#! /bin/sh

# Use this on the device after deploying with deploy_envsetup.sh.
# Run it in the root of deployment (/opt/usr/dev by default).
# Usage: . ./device_envsetup debug|release

libdir=""
if [ "$1" == "release" ]; then libdir=lib/release;
elif [ "$1" == "debug" ]; then libdir=lib/debug;
else >&2 echo "no valid config (debug/release)"; return;
fi

if [ ! -f $PWD/$libdir/efl_webprocess -o ! -f $PWD/$libdir/libchromium-efl.so -o ! -f $PWD/$libdir/libchromium-ewk.so ]; then
    >&2 echo "no complete build found in $PWD (need to have $libdir/efl_webprocess, $libdir/libchromium-efl.so, $libdir/libchromium-ewk.so)"
    return 1
fi

export EFL_WEBPROCESS_PATH=$PWD/$libdir/efl_webprocess
export LD_LIBRARY_PATH=$PWD/$libdir:$PWD
