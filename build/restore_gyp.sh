#!/bin/bash

# changed gyp files (src/build/linux/unbundle/replace_gyp_files.py)
# REPLACEMENTS = {
#   'use_system_expat': 'third_party/expat/expat.gyp',
#   'use_system_ffmpeg': 'third_party/ffmpeg/ffmpeg.gyp',
#   'use_system_flac': 'third_party/flac/flac.gyp',
#   'use_system_harfbuzz': 'third_party/harfbuzz-ng/harfbuzz.gyp',
#   'use_system_icu': 'third_party/icu/icu.gyp',
#   'use_system_jsoncpp': 'third_party/jsoncpp/jsoncpp.gyp',
#   'use_system_libevent': 'third_party/libevent/libevent.gyp',
#   'use_system_libjpeg': 'third_party/libjpeg/libjpeg.gyp',
#   'use_system_libpng': 'third_party/libpng/libpng.gyp',
#   'use_system_libusb': 'third_party/libusb/libusb.gyp',
#   'use_system_libvpx': 'third_party/libvpx/libvpx.gyp',
#   'use_system_libwebp': 'third_party/libwebp/libwebp.gyp',
#   'use_system_libxml': 'third_party/libxml/libxml.gyp',
#   'use_system_libxslt': 'third_party/libxslt/libxslt.gyp',
#   'use_system_openssl': 'third_party/openssl/openssl.gyp',
#   'use_system_opus': 'third_party/opus/opus.gyp',
#   'use_system_re2': 'third_party/re2/re2.gyp',
#   'use_system_snappy': 'third_party/snappy/snappy.gyp',
#   'use_system_speex': 'third_party/speex/speex.gyp',
#   'use_system_sqlite': 'third_party/sqlite/sqlite.gyp',
#   'use_system_v8': 'v8/tools/gyp/v8.gyp',
#   'use_system_zlib': 'third_party/zlib/zlib.gyp',
# }

SCRIPTDIR=$( cd $(dirname $0) ; pwd -P )

# get changed gyp files, above data
CHANGE_GYP_PATH="third_party/expat/expat.gyp third_party/ffmpeg/ffmpeg.gyp third_party/flac/flac.gyp third_party/harfbuzz-ng/harfbuzz.gyp third_party/icu/icu.gyp third_party/jsoncpp/jsoncpp.gyp third_party/libevent/libevent.gyp third_party/libjpeg/libjpeg.gyp third_party/libpng/libpng.gyp third_party/libusb/libusb.gyp third_party/libvpx/libvpx.gyp third_party/libwebp/libwebp.gyp third_party/libxml/libxml.gyp third_party/libxslt/libxslt.gyp third_party/openssl/openssl.gyp third_party/opus/opus.gyp third_party/re2/re2.gyp third_party/snappy/snappy.gyp third_party/speex/speex.gyp third_party/sqlite/sqlite.gyp v8/tools/gyp/v8.gyp third_party/zlib/zlib.gyp"

# move chromium root directory
pushd ${SCRIPTDIR}/../src > /dev/null

#restore old original files
for file in $CHANGE_GYP_PATH; do
  #if *.orig file does not exist we assume that
  #*.gyp files is in its original state
  # If something goes wrong then git clean -fdx on
  # chrome/src will fix the issues
  if [ -e ${file}.orig ]; then
    mv ${file}.orig ${file}
  fi
done

# move origin directory
popd > /dev/null
