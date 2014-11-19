// Copyright 2014 Samsung Electronics Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_RENDERER_MEDIA_TIZEN_MEDIA_SOURCE_DELEGATE_TIZEN_H_
#define CONTENT_RENDERER_MEDIA_TIZEN_MEDIA_SOURCE_DELEGATE_TIZEN_H_

#include "content/renderer/media/tizen/renderer_demuxer_tizen.h"
#include "media/base/decoder_buffer.h"
#include "media/base/demuxer_stream.h"
#include "media/base/media_keys.h"
#include "media/filters/chunk_demuxer.h"
#include "third_party/WebKit/public/platform/WebMediaPlayer.h"

namespace content {

class MediaSourceDelegateTizen
    : public media::DemuxerHost {
public:
  typedef base::Callback<void(blink::WebMediaSource*)>
      MediaSourceOpenedCB;
  typedef base::Callback<void(blink::WebMediaPlayer::NetworkState)>
      UpdateNetworkStateCB;
  typedef base::Callback<void(double)> DurationChangeCB;

  class Destroyer {
   public:
    inline void operator()(void* media_source_delegate) const {
      static_cast<MediaSourceDelegateTizen*>(
          media_source_delegate)->Destroy();
    }
  };

  MediaSourceDelegateTizen(
      RendererDemuxerTizen* demuxer_client,
      int demuxer_client_id,
      const scoped_refptr<base::SingleThreadTaskRunner>& media_task_runner,
      media::MediaLog* media_log);
  ~MediaSourceDelegateTizen();

  //DemuxerHost implementation.
  virtual void AddBufferedTimeRange(
      base::TimeDelta start,
      base::TimeDelta end) override {};

  // Sets the duration of the media in microseconds.
  // Duration may be kInfiniteDuration() if the duration is not known.
  virtual void SetDuration(base::TimeDelta duration) override;

  // Stops execution of the pipeline due to a fatal error.  Do not call this
  // method with PIPELINE_OK.
  virtual void OnDemuxerError(media::PipelineStatus error) override;

  // Add |text_stream| to the collection managed by the text renderer.
  virtual void AddTextStream(
      media::DemuxerStream* text_stream,
      const media::TextTrackConfig& config) override {};

  // Remove |text_stream| from the presentation.
  virtual void RemoveTextStream(media::DemuxerStream* text_stream) override {};

  void InitializeMediaSource(
      const MediaSourceOpenedCB& media_source_opened_cb,
      const UpdateNetworkStateCB& update_network_state_cb,
      const DurationChangeCB& duration_change_cb);

  // Called when DemuxerStreamPlayer needs to read data from ChunkDemuxer.
  void OnReadFromDemuxer(media::DemuxerStream::Type type);

  // Called by the Destroyer to destroy an instance of this object.
  void Destroy();

  // In MSE case, calls ChunkDemuxer::StartWaitingForSeek(), sets the
  // expectation that a regular seek will be arriving.
  void StartWaitingForSeek(const base::TimeDelta& seek_time);

  // Calls ChunkDemuxer::CancelPendingSeek(). Also sets the
  // expectation that a regular seek will be arriving.
  void CancelPendingSeek(const base::TimeDelta& seek_time);

  // Sets the expectation that a regular seek will be arriving.
  void StartSeek(
      const base::TimeDelta& seek_time,
      bool is_seeking_pending_seek);

  // Callback for ChunkDemuxer::Seek() and callback chain for resetting
  // decrypted audio/video streams if present.
  //
  // Runs on the media thread.
  void OnDemuxerSeekDone(media::PipelineStatus status);

  // Called when the player needs the new config data from ChunkDemuxer.
  void OnMediaConfigRequest();

 private:
  void OnNeedKey(const std::string& type, const std::vector<uint8>& init_data);
  void OnDemuxerOpened();
  void InitializeDemuxer();
  void OnDemuxerInitDone(media::PipelineStatus status);
  void StopDemuxer();
  void DeleteSelf();

  bool CanNotifyDemuxerReady();
  void NotifyDemuxerReady();
  void OnDurationChanged(const base::TimeDelta& duration);
  void OnBufferReady(
      media::DemuxerStream::Type type,
      media::DemuxerStream::Status status,
      const scoped_refptr<media::DecoderBuffer>& buffer);
  void SeekInternal(const base::TimeDelta& seek_time);

  // Message loop for main renderer thread and corresponding weak pointer.
  const scoped_refptr<base::MessageLoopProxy> main_loop_;
  base::WeakPtrFactory<MediaSourceDelegateTizen> main_weak_factory_;
  base::WeakPtr<MediaSourceDelegateTizen> main_weak_this_;

  // Message loop for media thread and corresponding weak pointer.
  const scoped_refptr<base::SingleThreadTaskRunner> media_task_runner_;
  base::WeakPtrFactory<MediaSourceDelegateTizen> media_weak_factory_;
  RendererDemuxerTizen* demuxer_client_;
  int demuxer_client_id_;
  scoped_refptr<media::MediaLog> media_log_;

  MediaSourceOpenedCB media_source_opened_cb_;
  UpdateNetworkStateCB update_network_state_cb_;
  DurationChangeCB duration_change_cb_;

  scoped_ptr<media::ChunkDemuxer> chunk_demuxer_;
  media::DemuxerStream* audio_stream_;
  media::DemuxerStream* video_stream_;

  base::TimeDelta seek_time_;
  bool pending_seek_;
  bool is_seeking_;

  // Will handle internal seek coming from |MediaSourcePlayerGstreamer|
  // if new seek has been fired by |HTMLMediaElement|.
  // Always one should seek to latest time and ignore previous seeks.
  bool seeking_pending_seek_;

  // Will handle |seek| request coming after |chunk_demuxer|
  // has requested |gstreamer| to seek.
  bool is_demuxer_seek_done_;
  base::TimeDelta pending_seek_time_;

  bool is_audio_read_fired_;
  bool is_video_read_fired_;

  bool is_demuxer_ready_;

  uint32 shared_memory_size_;
  base::SharedMemory shared_memory_;
  base::SharedMemoryHandle foreign_memory_handle_;
};

}  // namespace content

#endif  // CONTENT_RENDERER_MEDIA_TIZEN_MEDIA_SOURCE_DELEGATE_TIZEN_H_
