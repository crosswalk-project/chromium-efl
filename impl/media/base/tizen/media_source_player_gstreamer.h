// Copyright 2014 Samsung Electronics Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_TIZEN_MEDIA_SOURCE_PLAYER_GSTREAMER_H_
#define MEDIA_BASE_TIZEN_MEDIA_SOURCE_PLAYER_GSTREAMER_H_

#include <map>

#include <Ecore.h>
#include <Ecore_X.h>
#include <gst/gst.h>

#include "base/cancelable_callback.h"
#include "base/memory/scoped_ptr.h"
#include "base/threading/thread.h"
#include "base/time/default_tick_clock.h"
#include "base/timer/timer.h"
#include "media/base/tizen/demuxer_tizen.h"
#include "media/base/tizen/media_player_tizen.h"
#include "media/base/decoder_buffer.h"

#if defined(TIZEN_MULTIMEDIA_PIXMAP_SUPPORT)
#include "ui/gl/efl_pixmap.h"
#endif

#if defined(OS_TIZEN_TV)
#include "media/audio/tizen/audio_session_manager.h"
#endif

namespace {
struct AudioCodecGstElementsMapping {
  media::AudioCodec codec;
  const char **elements;
};

struct VideoCodecGstElementsMapping {
  media::VideoCodec codec;
  const char **elements;
};
}

namespace media {

// This class handles media source extensions for Gstreamer port.
class MEDIA_EXPORT MediaSourcePlayerGstreamer : public MediaPlayerTizen,
    public DemuxerTizenClient {
 public:

  // Constructs a player with the given ID and demuxer. |manager| must outlive
  // the lifetime of this object.
  MediaSourcePlayerGstreamer(
      int player_id,
      scoped_ptr<DemuxerTizen> demuxer,
      MediaPlayerManager* manager);

  virtual ~MediaSourcePlayerGstreamer();

  // MediaPlayerTizen implementation.
  virtual void Play() override;
  virtual void Pause(bool is_media_related_action) override;
  virtual void SetRate(double rate) override;
  virtual void Seek(const double time) override;
  virtual void SetVolume(double volume) override;
  virtual double GetCurrentTime() override;
  virtual void Destroy() override;

  // DemuxerTizenClient implementation.
  virtual void OnDemuxerConfigsAvailable(
      const DemuxerConfigs& params) override;
  virtual void OnDemuxerDataAvailable(
      base::SharedMemoryHandle foreign_memory_handle,
      const media::DemuxedBufferMetaData& meta_data) override;
  virtual void OnBufferMetaDataAvailable(
      const media::DemuxedBufferMetaData& meta_data) override;
  virtual void OnDemuxerSeekDone(
      const base::TimeDelta& actual_browser_seek_time) override;
  virtual void OnDemuxerDurationChanged(base::TimeDelta duration) override;

  // GStreamer Message handler
  void HandleMessage(GstMessage* message);

  // AppSink related
  GstBuffer* PullBuffer();
  void GetFrameDetails();
  void OnNewFrameAvailable(const GstBuffer* buffer);

#if defined(TIZEN_MULTIMEDIA_PIXMAP_SUPPORT)
  void PlatformSurfaceUpdated();
  void OnVideoConfigsChanged();
  int GetSurfaceID() const;
#endif

  void OnReadDemuxedData(media::DemuxerStream::Type type);
  void OnStopDemuxedData(media::DemuxerStream::Type type);

  void UpdateVideoSeekOffset(guint64 video_seek_offset) {
    video_seek_offset_ = video_seek_offset;
  }
  void UpdateAudioSeekOffset(guint64 audio_seek_offset) {
    audio_seek_offset_ = audio_seek_offset;
  }

 protected:
  virtual void Release() override;

 private:
  void PrepareForVideoFrame();

#if defined(TIZEN_MULTIMEDIA_PIXMAP_SUPPORT)
  void PrepareForVideoSink();
  void VideoConfigsChanged();
  void CreatePixmap();
  void SetPixmap();
  bool IsXWindowHadleSet() { return is_xwindow_handle_set_;}
  void XWindowIdPrepared(GstMessage*);
  void UnregisterDamageHandler();
#endif

  void ReadDemuxedData(media::DemuxerStream::Type type);
  void SaveDecoderBuffer(
      base::SharedMemoryHandle foreign_memory_handle,
      const media::DemuxedBufferMetaData& meta_data);
  void ReadFromQueueIfAny(DemuxerStream::Type type);

  bool HasAudio();
  bool HasVideo();

  // Note: Must be invoked only if elements are not added into the pipeline.
  void ReleaseAudioElements();
  void ReleaseVideoElements();

  void SeekInternal(const GstClockTime time);

  // For internal seeks.
  void RequestPlayerSeek(double seekTime);

  // |current_time_update_timer_| related
  void OnCurrentTimeUpdateTimerFired();
  void StartCurrentTimeUpdateTimer();
  void StopCurrentTimeUpdateTimer();
  void OnGetFrameDetails();

  void OnUpdateStates();
  void HandleBufferingMessage();
  void OnPlaybackComplete();
  void UpdateSeekState(bool state);
  void OnTimeChanged();

  // Error handling API
  void HandleError(media::MediaPlayerTizen::NetworkState state);

  scoped_ptr<DemuxerTizen> demuxer_;

  const scoped_refptr<base::MessageLoopProxy> main_loop_;

#if defined(OS_TIZEN_TV)
  scoped_refptr<AudioSessionManager> audio_session_manager_;
#endif

  // Stats about the media.
  bool playing_;

  // Weak pointer passed to media decoder jobs for callbacks.
  base::WeakPtrFactory<MediaSourcePlayerGstreamer> weak_this_;

  // To support Vedio Sink (pixmap).
#if defined(TIZEN_MULTIMEDIA_PIXMAP_SUPPORT)
  int pixmap_id_;
  scoped_refptr<gfx::EflPixmap> efl_pixmap_;
  Ecore_X_Damage m_damage;
  Ecore_Event_Handler* m_damageHandler;
  typedef std::map<std::string, scoped_refptr<gfx::EflPixmap> > EflPixmapMap;
  EflPixmapMap  efl_pixmaps_map_;
#endif

  // Will allow to listen to |prepare-xid| message only once.
  bool is_xwindow_handle_set_;

  // Whenever resolution changes, to change the pixmap
  // if Video Playback is backed by |pixmap|.
  bool is_backed_by_pixmap;

  GstElement* pipeline_;
  GstElement* video_appsrc_;
  GstElement* video_parse_;
  GstElement* video_decoder_;
  GstElement* video_sink_;
  GstElement* audio_appsrc_;
  GstElement* audio_decoder_;
  GstElement* audio_sink_;
  GstElement* video_queue_;
  GstElement* audio_queue_;
  GstElement* audio_parse_;
  GstElement* audio_convert_;
  GstElement* audio_resampler_;
  GstElement* audio_volume_;
  GstPad* video_sink_pad_;
  bool should_feed_audio_;
  bool should_feed_video_;

  int gst_width_;
  int gst_height_;
  guint32 video_format_;
  int media_type;
  int bufsize_sn12_;
  double play_rate_;
  double duration_;
  bool is_paused_due_underflow_;

  base::SharedMemory shared_memory;
  uint32 shared_memory_size;
  base::SharedMemoryHandle foreign_memory_handle;

  int buffered_;
  bool is_paused_;
  bool is_seeking_;
  bool is_demuxer_seeking_;
  int audio_buffered_;
  int video_buffered_;

  bool is_gst_pipeline_constructed_;
  std::deque<scoped_refptr<DecoderBuffer> > audio_buffer_queue_;
  std::deque<scoped_refptr<DecoderBuffer> > video_buffer_queue_;

  bool is_download_finished_;
  bool is_end_reached_;
  bool error_occured_;

  base::RepeatingTimer<MediaSourcePlayerGstreamer> current_time_update_timer_;
  int raw_video_frame_size_;
  guint64 video_seek_offset_;
  guint64 audio_seek_offset_;

  // When seeked to |x| seconds, |ChunkDemuxer| will send data from
  // |x - delta_x| where |x-delta_x| is location of I-Frame. And gstreamer
  // decoder is supposed to play media from seek time i.e. |x| seconds.
  // But in the gstreamer pipeline created here it starts playing from I-Frame
  // i.e. |x-delta_x| but gst-pipeline time starts running from |x|. Hence
  // jerky playback is observed for some time during seek. To avoid this, now
  // gst-pipeline is also seeked to nearest I-Frame i.e. |x-delta_x|.
  bool is_seeking_iframe_;

  DISALLOW_COPY_AND_ASSIGN(MediaSourcePlayerGstreamer);
};

}  // namespace media

#endif  // MEDIA_BASE_TIZEN_MEDIA_SOURCE_PLAYER_GSTREAMER_H_
