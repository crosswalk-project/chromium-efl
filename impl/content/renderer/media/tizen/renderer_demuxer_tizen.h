// Copyright 2014 Samsung Electronics Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_RENDERER_MEDIA_TIZEN_RENDERER_DEMUXER_TIZEN_H_
#define CONTENT_RENDERER_MEDIA_TIZEN_RENDERER_DEMUXER_TIZEN_H_

#include "base/atomic_sequence_num.h"
#include "base/id_map.h"
#include "ipc/message_filter.h"
#include "media/base/tizen/demuxer_stream_player_params_tizen.h"

namespace base {
class SingleThreadTaskRunner;
}

namespace content {

class MediaSourceDelegateTizen;
class ThreadSafeSender;

// Represents the renderer process half of an IPC-based implementation of
// media::DemuxerTizen.
//
// Refer to BrowserDemuxerTizen for the browser process half.
class RendererDemuxerTizen : public IPC::MessageFilter {
 public:
  RendererDemuxerTizen();

  // Returns the next available demuxer client ID for use in IPC messages.
  //
  // Safe to call on any thread.
  int GetNextDemuxerClientID();

  // Associates |delegate| with |demuxer_client_id| for handling incoming IPC
  // messages.
  //
  // Must be called on media thread.
  void AddDelegate(
      int demuxer_client_id,
      MediaSourceDelegateTizen* delegate);

  // Removes the association created by AddDelegate().
  //
  // Must be called on media thread.
  void RemoveDelegate(int demuxer_client_id);

  // IPC::ChannelProxy::MessageFilter overrides.
  virtual bool OnMessageReceived(const IPC::Message& message) override;

  // media::DemuxerTizenClient "implementation".
  void DemuxerReady(
      int demuxer_client_id,
      const media::DemuxerConfigs& configs);
  void ReadFromDemuxerAck(
      int demuxer_client_id,
      base::SharedMemoryHandle foreign_memory_handle,
      const media::DemuxedBufferMetaData& meta_data);
  void BufferMetaDataAck(
      int demuxer_client_id,
      const media::DemuxedBufferMetaData& meta_data);
  void DemuxerSeekDone(
      int demuxer_client_id,
      const base::TimeDelta& actual_browser_seek_time);
  void DurationChanged(int demuxer_client_id, const base::TimeDelta& duration);

 protected:
  friend class base::RefCountedThreadSafe<RendererDemuxerTizen>;
  virtual ~RendererDemuxerTizen();

 private:
  void DispatchMessage(const IPC::Message& message);
  void OnReadFromDemuxer(int demuxer_client_id,
      media::DemuxerStream::Type type);
  void OnDemuxerSeekRequest(int demuxer_client_id,
      const base::TimeDelta& time_to_seek);
  void OnMediaConfigRequest(int demuxer_client_id);

  base::AtomicSequenceNumber next_demuxer_client_id_;

  IDMap<MediaSourceDelegateTizen> delegates_;
  scoped_refptr<ThreadSafeSender> thread_safe_sender_;
  scoped_refptr<base::SingleThreadTaskRunner> media_task_runner_;

  DISALLOW_COPY_AND_ASSIGN(RendererDemuxerTizen);
};

} // namespace content

#endif // CONTENT_RENDERER_MEDIA_TIZEN_RENDERER_DEMUXER_TIZEN_H_