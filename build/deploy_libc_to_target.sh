#!/bin/bash

SCRIPTDIR=$( cd $(dirname $0) ; pwd -P )
TOPDIR=$( cd ${SCRIPTDIR}/.. ; pwd -P )
LIBC_DIR=$( cd ${TOPDIR}/ewk/thirdparty/eglibc-2.18-rpm ; pwd -P )

source ${SCRIPTDIR}/common.sh

# 1/ nuke any previous custom libc deployment.
# 2/ create our custom libc folder
# 3/ copy libc rpm to target
# 4/ extract it and remove unneeded left over.
sdb root on
sdb shell "rm -fr /opt/usr/eglibc-2.18"
sdb shell "mkdir /opt/usr/eglibc-2.18"
sdb push $LIBC_DIR/eglibc-2.18-5.1.armv7l.rpm /opt/usr/eglibc-2.18
sdb shell "cd /opt/usr/eglibc-2.18/ ; rpm2cpio eglibc-2.18-5.1.armv7l.rpm | cpio -imdv"
sdb shell "cd /opt/usr/eglibc-2.18/ ; rm -fr eglibc-2.18-5.1.armv7l.rpm etc sbin usr var"
