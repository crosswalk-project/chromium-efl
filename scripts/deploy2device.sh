#! /bin/bash

SCRIPT_PATH=$(readlink -f $(dirname $0))

[ -n "$DEPLOY_DIR" ] || DEPLOY_DIR="/opt/usr/dev"
[ -n "$OUT_DIR" ] || OUT_DIR=$(readlink -f "$SCRIPT_PATH/../out.mobile.arm")
[ -n "$DEPLOY_STRIPPED_LIBS" ] || DEPLOY_STRIPPED_LIBS=1
[ -n "$TIZEN_SDK_PATH" ] || TIZEN_SDK_PATH="$HOME/tizen-sdk"

# Globals.
out_dir=$OUT_DIR
out_subdir=""
deploy_subdir=""
arm_strip=""

usage() {
    cat << EOF
Usage: $0 debug|release [-o outdir] [lib|ewk]

    default outdir: out.mobile.arm

ENVIRONMENT
    DEPLOY_DIR
        default: /opt/usr/dev
    OUT_DIR
        default: out.mobile.arm
    DEPLOY_STRIPPED_LIBS
        default: 1
    TIZEN_SDK_PATH
        default: ~/tizen-sdk

EOF
}

dieTrap() {
    echo "ERROR at $0:$1"
    usage
    exit 1
}

trap 'die $0 $LINENO' ERR SIGINT SIGTERM SIGQUIT

fastpush() {
    src=$1
    dst=$2
    size=$(ls -1s --block-size=1 $src | cut -f 1 -d ' ')
    trashold="524288" # 512K
    # Avoid setup overhead for small files.
    if [ "$size" -le "$trashold" ]; then
        sdb push $src $dst
    else
        $SCRIPT_PATH/tizensync.sh push $src $dst
        # Print a report that is somewhat consistent with sdb push.
        echo "fast-pushed           $src"
    fi
}

_setupDevice() {
    # Do a quick heuristic check that it was already installed via package.
    # We don't handle everything so the package has to be installed once.
    lsout=$(sdb shell "ls /usr/lib/chromium-efl/icudtl.dat 2>/dev/null")
    if [ -z "$lsout" ]; then
        >&2 echo "It seems like you have never installed chromium-efl via package."
        >&2 echo "Sorry but it won't work, exiting..."
        exit 1
    fi

    sdb start-server
    sdb root on
    sdb shell << EOF
mkdir -p "$DEPLOY_DIR/lib/release"
mkdir -p "$DEPLOY_DIR/lib/debug"
[ -f /var/run/sshd.pid ] || /usr/sbin/sshd
exit
EOF
    sdb push "$SCRIPT_PATH/device_envsetup.sh" $DEPLOY_DIR
}

setupDevice() {
    _setupDevice > /dev/null
}

get_arm_strip() {
    if [ -n "$CROSS_COMPILE" ] && type "${CROSS_COMPILE}strip" &> /dev/null; then
        echo "${CROSS_COMPILE}strip"
        return 0
    fi

    # Try to use the one from tizen sdk.
    for _dummy in $(seq 1 1); do # dummy loop for bailing on error to print message.
        # Glob (*) to handle different versions.
        pushd $TIZEN_SDK_PATH/tools/arm-linux-gnueabi-gcc*/bin > /dev/null || break
        path="$(pwd)/arm-linux-gnueabi-strip"
        [ -x "$path" ] || break
        popd > /dev/null || break
        return 0
    done

    >&2 echo "Cannot find usable strip binary. Hint: try setting TIZEN_SDK_PATH to your installation."
    return 1
}

deploylib() {
    lib=$1
    target_dir=$2
    [ -f "$lib" ]
    stripped="$1.0"
    if [ "$DEPLOY_STRIPPED_LIBS" != "1" ]; then
        stripped=$lib
    elif [  ! -e "$stripped" -o "$stripped" -ot "$lib" ]; then
        echo "stripping $lib to $stripped..."
        $arm_strip -s $lib -o $stripped
    fi

    fastpush $stripped $target_dir/$(basename $lib)
}

deploychromiumlib() {
    lib="$out_subdir/lib/libchromium-efl.so"
    echo "deploying $lib..."
    deploylib $lib $deploy_subdir

    web_proc=$out_subdir/efl_webprocess
    echo "deploying $web_proc..."
    fastpush $web_proc $deploy_subdir
}

deployewk() {
    lib="$out_subdir/lib/libchromium-ewk.so"
    echo "deploying $lib..."
    deploylib $lib $deploy_subdir

    webview_app="$out_subdir/efl_webview_app"
    if [ -f "$webview_app" ]; then
        echo "deploying $webview_app..."
        fastpush "$webview_app" $DEPLOY_DIR
    fi
    minibrowser="$out_subdir/mini_browser"
    if [ -f "$minibrowser" ]; then
        echo "deploying $minibrowser..."
        fastpush "$minibrowser" $DEPLOY_DIR
    fi
}

# Off we go...

if echo "$@" | grep -Eqw "\-h|\-\-help"; then
    usage
    exit 0
fi

buildtype=$1
[ "$buildtype" == "release" -o "$buildtype" == "debug" ]
shift

deploy_part="all"

while [ "$#" -gt "0" ]; do
    if [ "$1" == "-o" ]; then
        out_dir=$2
        shift
    else
        [ -z "$deploy_part" ]
        deploy_part=$1
    fi
    shift
done

[ -d "$out_dir" ]
[ "$deploy_part" == "all" -o "$deploy_part" == "lib" -o "$deploy_part" == "ewk" ]

if [ "$buildtype" == "release" ]; then
    deploy_subdir="$DEPLOY_DIR/lib/release"
    out_subdir="${out_dir}/Release"
elif [ "$buildtype" == "debug" ]; then
    deploy_subdir="$DEPLOY_DIR/lib/debug"
    out_subdir="${out_dir}/Debug"
fi

[ -d "$out_subdir" ]

if [ "$DEPLOY_STRIPPED_LIBS" == "1" ]; then
    arm_strip="$(get_arm_strip)"
fi

setupDevice

case "$deploy_part" in
    lib)
        deploychromiumlib
        ;;
    ewk)
        deployewk
        ;;
    all)
        deploychromiumlib && deployewk
        ;;
esac

echo ""
echo "SUCCESS!"
echo "Build has been deployed to device."
echo "Source in device_envsetup.sh in $DEPLOY_DIR before starting application."
