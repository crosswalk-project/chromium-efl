#! /bin/bash

# Fast push/pull with Tizen device (or emulator).
# Uses rsync which means faster reupload of libraries after recompilation as only the diff has to be uploaded.

usage() {
    echo "ERROR at $(caller)"

    cat << EOF
usage: $0 (push|pull) src dst

Preconditions:
    * sdb root should be on
    * /root/.ssh/authorized_keys should contain your public key
EOF
    exit
}

[ "$#" == "3" ] || usage

src=$2
dst=$3
if [ "$1" == "push" ]; then
    src=$(readlink -f $src)
    [ -f $src ] || usage
elif [ "$1" == "pull" ]; then
    # NOTE: dirname "" is .
    [ -d $(dirname "$dst") ] || usage
else
    usage
fi

SCRIPT_PATH=$(readlink -f "$(dirname $0)")

# Hard-coded, live with it!
PORT=9128
USER=root

sdb start-server

# Start sshd on device if not running
function startSSHD() {
sdb shell << EOF
su
if [ ! -f "/var/run/sshd.pid" ]; then
  mkdir -m 700 /var/run/sshd &>/dev/null
  /usr/sbin/sshd
fi
exit
exit
EOF
}

startSSHD >/dev/null

# Copy bundled rsync if not there yet.
lscmd="ls /usr/local/bin/rsync 2>/dev/null"
output=$(sdb shell "$lscmd")
if [ -z "$output" ]; then
  PREBUILD_PATH=$(readlink -f $SCRIPT_PATH/../build/prebuild)
  sdb push "$PREBUILD_PATH/rsync" /usr/local/bin
fi

# It looks like it's ok to do the forward multiple times.
sdb forward tcp:$PORT tcp:22
[ "$?" == "0" ] || (>&2 echo "error sdb port forward failed"; usage)

do_rsync() {
  rsync -axzH -e "ssh -oStrictHostKeyChecking=no -p $PORT" $1 $2
}

if [ "$1" == "push" ]; then
    do_rsync $src $USER@localhost:$dst
else
    do_rsync $USER@localhost:$src $dst
fi
