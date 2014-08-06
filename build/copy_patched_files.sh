#!/bin/bash

# source common functions and vars
. `dirname ${BASH_SOURCE[0]}`/common.sh

cp $EFL_WEBVIEW_DIR/build/patched_files/arm-neon-config.h ${CHROME_SRC}/third_party/ffmpeg/chromium/config/Chromium/linux/arm-neon/config.h
cp $EFL_WEBVIEW_DIR/build/patched_files/arm-config.h ${CHROME_SRC}/third_party/ffmpeg/chromium/config/Chromium/linux/arm/config.h
cp $EFL_WEBVIEW_DIR/build/patched_files/incoming_video_stream.cc ${CHROME_SRC}/third_party/webrtc/modules/video_render/
