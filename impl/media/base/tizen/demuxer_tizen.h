// Copyright 2014 Samsung Electronics Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_TIZEN_DEMUXER_TIZEN_H_
#define MEDIA_BASE_TIZEN_DEMUXER_TIZEN_H_

#include "media/base/tizen/demuxer_stream_player_params_tizen.h"

namespace media {

// Defines the client callback interface.
class MEDIA_EXPORT DemuxerTizenClient {
 public:

  // Called in response to RequestDemuxerConfigs() and also when the demuxer
  // has initialized.
  virtual void OnDemuxerConfigsAvailable(const DemuxerConfigs& params) = 0;

  // Called in response to RequestDemuxerData().
  virtual void OnDemuxerDataAvailable(
      base::SharedMemoryHandle foreign_memory_handle,
      const media::DemuxedBufferMetaData& meta_data) = 0;

  // Called in response to RequestDemuxerData() when no data is available.
  virtual void OnBufferMetaDataAvailable(
      const media::DemuxedBufferMetaData& meta_data) = 0;

  // Called to inform demuxer seek completion.
  virtual void OnDemuxerSeekDone(
      const base::TimeDelta& actual_browser_seek_time) = 0;

  // Called whenever the demuxer has detected a duration change.
  virtual void OnDemuxerDurationChanged(base::TimeDelta duration) = 0;

 protected:
  virtual ~DemuxerTizenClient() {}
};

// Defines a demuxer with asynchronous operations.
class MEDIA_EXPORT DemuxerTizen {
 public:
  virtual ~DemuxerTizen() {}

  // Initializes this demuxer with |client| as the callback handler.
  // Must be called prior to calling any other methods.
  virtual void Initialize(DemuxerTizenClient* client) = 0;

  // Called to request the current audio/video decoder configurations.
  virtual void RequestDemuxerConfigs() = 0;

  // Called to request demuxer seek.
  virtual void RequestDemuxerSeek(const base::TimeDelta& time_to_seek) = 0;

  // Called to request additional data from the demuxer.
  virtual void RequestDemuxerData(media::DemuxerStream::Type type) = 0;
};

}  // namespace media

#endif  // MEDIA_BASE_TIZEN_DEMUXER_TIZEN_H_