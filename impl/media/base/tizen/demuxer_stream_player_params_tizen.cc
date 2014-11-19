// Copyright 2014 Samsung Electronics Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/base/tizen/demuxer_stream_player_params_tizen.h"

namespace media {

DemuxerConfigs::DemuxerConfigs()
    : audio_codec(kUnknownAudioCodec),
      audio_channels(0),
      audio_sampling_rate(0),
      is_audio_encrypted(false),
      video_codec(kUnknownVideoCodec),
      is_video_encrypted(false),
      duration_ms(0) {}

DemuxerConfigs::~DemuxerConfigs() {}

DemuxedBufferMetaData::DemuxedBufferMetaData()
    : size(0),
      end_of_stream(false),
      type(DemuxerStream::UNKNOWN),
      status(DemuxerStream::kAborted) {}

DemuxedBufferMetaData::~DemuxedBufferMetaData() {}

}  // namespace media
