#!/bin/bash

# source common functions and vars
. `dirname ${BASH_SOURCE[0]}`/common.sh

PATCHED_DIR=$SCRIPTDIR/patched_files

cp -a $PATCHED_DIR/arm-neon-config.h ${CHROME_SRC}/third_party/ffmpeg/chromium/config/Chromium/linux/arm-neon/config.h
cp -a $PATCHED_DIR/arm-config.h ${CHROME_SRC}/third_party/ffmpeg/chromium/config/Chromium/linux/arm/config.h
cp -a $PATCHED_DIR/incoming_video_stream.cc ${CHROME_SRC}/third_party/webrtc/modules/video_render/
cp -a $PATCHED_DIR/rtp_format_h264.cc ${CHROME_SRC}/third_party/webrtc/modules/rtp_rtcp/source/
