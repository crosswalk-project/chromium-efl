#!/bin/bash

# Dependencies we want to use from the system.
# no system ffmpeg
# no system jsoncpp
# no system libevent

# can't use use_system_sqlite=1 because of the bug:
# https://code.google.com/p/chromium/issues/detail?id=22208

# can't use use_system_protobuf=1 because 11
# symbol lookup error: /usr/lib/libchromium-efl.so: undefined symbol: _ZNK6google8protobuf7Message11GetTypeNameEv
SYSTEM_DEPS="use_alsa=0
             use_system_libjpeg=1
             use_system_flac=1
             use_system_expat=1
             use_system_harfbuzz=1
             use_system_libpng=1
             use_system_libusb=1
             use_system_libxml=1
             use_system_libxslt=1
             use_system_speex=1
             use_system_zlib=1
             use_system_minizip=1
             use_system_icu=1
             use_system_libexif=1
             use_system_bzip2=1
             use_system_nspr=1
             use_system_vpx=1
             "
