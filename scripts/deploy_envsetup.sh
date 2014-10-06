# NOTE: $0 doesn't work for sourced scripts.
__SCRIPT_PATH=$(readlink -f $(dirname ${BASH_SOURCE[0]}))

unset DEPLOY_DIR
unset OUT_DIR
unset DEPLOY_STRIPPED_LIBS
unset TIZEN_SDK_TOOLCHAIN_PATH
[ -n "$DEPLOY_DIR" ] || DEPLOY_DIR="/opt/usr/dev"
[ -n "$OUT_DIR" ] || OUT_DIR=$(readlink -f "$__SCRIPT_PATH/../out.mobile.arm")
[ -n "$DEPLOY_STRIPPED_LIBS" ] || DEPLOY_STRIPPED_LIBS=1
[ -n "$TIZEN_SDK_TOOLCHAIN_PATH" ] || TIZEN_SDK_TOOLCHAIN_PATH="$HOME/tizen-sdk/tools/arm-linux-gnueabi-gcc-4.5/bin"

fastpush() {
    $__SCRIPT_PATH/tizensync.sh push $1 $2
}

setupdevice() {
  __setupdevice >/dev/null
}

deploylib() {
    lib=$1
    target_dir=$2
    stripped="$1.0"
    if [ "$DEPLOY_STRIPPED_LIBS" != "1" ]; then
        stripped=$lib
    elif [  ! -e "$stripped" -o "$stripped" -ot "$lib" ]; then
        STRIP="$TIZEN_SDK_TOOLCHAIN_PATH/arm-linux-gnueabi-strip"
        $STRIP -s $lib -o $stripped
    fi
    fastpush $stripped $target_dir/$(basename $lib)
}

# Usage: deploy* debug|release [outdir]
# outdir is out.mobile.arm by default.
# sdb root should be on.

deploychromiumlib() {
    __setup_enter
    __setOutDir $1 $2 || return 1
    LIB="$__out_dir/lib/libchromium-efl.so"
    deploylib $LIB $__deploy_dir
    __setup_exit
}

deploywebproc() {
    __setup_enter
    __setOutDir $1 $2 || return 1
    web_proc_path=$__out_dir/efl_webprocess
    fastpush $web_proc_path $__deploy_dir
    __setup_exit
}

deploychromium() {
    __setup_enter
    deploychromiumlib $1 $2 && deploywebproc $1 $2
    __setup_exit
}

deployewklib() {
    __setup_enter
    __setOutDir $1 $2 || return 1
    LIB="$__out_dir/lib/libchromium-ewk.so"
    deploylib $LIB $__deploy_dir
    __setup_exit
}

deployapps() {
    __setup_enter
    __setOutDir $1 $2 || return 1
    [ -f "$__out_dir/efl_webview_app" -o -f "$__out_dir/mini_browser" ] || (>&2 echo "no app binaries exist"; return 1)
    [ -f "$__out_dir/efl_webview_app" ] && sdb push "$__out_dir/efl_webview_app" $DEPLOY_DIR
    [ -f "$__out_dir/mini_browser" ] && sdb push "$__out_dir/mini_browser" $DEPLOY_DIR
    __setup_exit
}

deployewk() {
    __setup_enter
    deployewklib $1 $2 && deployapps $1 $2
    __setup_exit
}

deployall() {
    __setup_enter
    deploychromium $1 $2 && deployewk $1 $2
    __setup_exit
}


# Internal

__deploy_dir=""
__out_dir=""

__setOutDir() {
    if [ "$1" == "release" ]; then
        __deploy_dir="$DEPLOY_DIR/lib/release"
        if  [ -z "$2" ]; then
            __out_dir="$OUT_DIR/Release"
        else
            __out_dir=$(readlink -f $2)/Release
        fi
    elif [ "$1" == "debug" ]; then
        __deploy_dir="$DEPLOY_DIR/lib/debug"
        if  [ -z "$2" ]; then
            __out_dir="$OUT_DIR/Release"
        else
            __out_dir=$(readlink -f $2)/Release
        fi
    else
        >&2 echo "Wrong config. Options: debug or release."
        return 1
    fi
}

__setupdevice() {
    sdb shell << EOF
su
cd "$DEPLOY_DIR"
mkdir -p lib/release &>/dev/null
mkdir -p lib/debug &>/dev/null
[ -f /var/run/sshd.pid ] || /usr/sbin/sshd
exit
exit
EOF

    sdb push "$__SCRIPT_PATH/device_envsetup.sh" $DEPLOY_DIR
}

__setup_enter_counter="0"

__setup_enter() {
    if [ "$__setup_enter_counter" == "0" ]; then
        setupdevice
    fi
    let __setup_enter_counter=__setup_enter_counter+1
}

__setup_exit() {
    let __setup_enter_counter=__setup_enter_counter-1
}
