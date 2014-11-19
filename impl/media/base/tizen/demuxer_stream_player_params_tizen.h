// Copyright 2014 Samsung Electronics Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_TIZEN_DEMUXER_STREAM_PLAYER_PARAMS_TIZEN_H_
#define MEDIA_BASE_TIZEN_DEMUXER_STREAM_PLAYER_PARAMS_TIZEN_H_

#include <vector>

#include "media/base/audio_decoder_config.h"
#include "media/base/decrypt_config.h"
#include "media/base/demuxer_stream.h"
#include "media/base/media_export.h"
#include "media/base/video_decoder_config.h"
#include "ui/gfx/size.h"

namespace media {

struct MEDIA_EXPORT DemuxerConfigs {
  DemuxerConfigs();
  ~DemuxerConfigs();

  AudioCodec audio_codec;
  int audio_channels;
  int audio_sampling_rate;
  bool is_audio_encrypted;
  std::vector<uint8> audio_extra_data;

  VideoCodec video_codec;
  gfx::Size video_size;
  bool is_video_encrypted;
  std::vector<uint8> video_extra_data;

  int duration_ms;
};

struct MEDIA_EXPORT DemuxedBufferMetaData {
  DemuxedBufferMetaData();
  ~DemuxedBufferMetaData();

  int size;
  bool end_of_stream;
  base::TimeDelta timestamp;
  base::TimeDelta time_duration;
  DemuxerStream::Type type;
  DemuxerStream::Status status;
};

};  // namespace media

#endif  // MEDIA_BASE_TIZEN_DEMUXER_STREAM_PLAYER_PARAMS_TIZEN_H_
