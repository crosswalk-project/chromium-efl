#! /bin/bash

# Fast push/pull with Tizen device (or emulator).
# Uses rsync which means faster reupload of libraries after recompilation as only the diff has to be uploaded.

usage() {
    cat << EOF
usage: $0 (push|pull) src dst

EOF
}

die() {
    echo "ERROR at $1:$2"
    usage
    exit 1
}

trap 'die $0 $LINENO' ERR SIGINT SIGTERM SIGQUIT

if echo "$@" | grep -Eqw "\-h|\-\-help"; then
    usage
    exit 0
fi

[ "$#" == "3" ]

src=$2
dst=$3
if [ "$1" == "push" ]; then
    src=$(readlink -f $src)
    [ -f $src ]
elif [ "$1" == "pull" ]; then
    # NOTE: dirname "" is .
    [ -d $(dirname "$dst") ]
else
    false
fi

SCRIPT_PATH=$(readlink -f "$(dirname $0)")

# Hard-coded, live with it!
PORT=9128
USER=root

sdb start-server
sdb root on >/dev/null
sdb shell "change-booting-mode.sh --update" >/dev/null

# Start sshd on device if not running.
function startSSHD() {
sdb shell << EOF
if [ ! -f "/var/run/sshd.pid" ]; then
    mkdir -m 700 /var/run/sshd &>/dev/null
    /usr/sbin/sshd
fi
exit
EOF
}

startSSHD >/dev/null

fileExistsOnDevice() {
    lsout=$(sdb shell "ls $1 2>/dev/null")
    ([ -n "$lsout" ] && echo 1) || echo 0
}

if [ "$(fileExistsOnDevice /root/.ssh/authorized_keys)" -eq 0 ]; then
    echo "setting up /root/.ssh/authorized_keys"
    sdb shell "mkdir /root/.ssh" &>/dev/null
    sdb push "$HOME/.ssh/id_rsa.pub" "/root/.ssh/authorized_keys"
fi
if [ "$(fileExistsOnDevice /usr/local/bin/rsync)" -eq 0 ]; then
    echo "installing rsync on device"
    PREBUILD_PATH=$(readlink -f $SCRIPT_PATH/../build/prebuild)
    sdb push "$PREBUILD_PATH/rsync" /usr/local/bin
fi

# It's ok to do the forwarding multiple times.
sdb forward tcp:$PORT tcp:22 || (\
    >&2 echo "ERROR: sdb port forward failed"; \
    >&2 echo "Hint: if you have /root/.ssh/authorized_keys file on your device make sure it contains your public key."; \
    false)

do_rsync() {
    rsync -axzH -e "ssh -oStrictHostKeyChecking=no -p $PORT" $1 $2
}

if [ "$1" == "push" ]; then
    do_rsync $src $USER@localhost:$dst
else
    do_rsync $USER@localhost:$src $dst
fi
