// Copyright 2014 Samsung Electronics Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/base/tizen/media_source_player_gstreamer.h"

#include <gst/app/gstappsink.h>
#include <gst/app/gstappsrc.h>
#include <gst/interfaces/xoverlay.h>
#include <gst/video/video.h>

#include "base/process/process.h"
#include "media/base/tizen/media_player_manager_tizen.h"

#if defined(OS_TIZEN_MOBILE) && (defined(TIZEN_V_2_3) || defined(TIZEN_V_2_4))
#include <device/power.h>
#endif

namespace {

const uint32 GST_VIDEO_SN12 = GST_MAKE_FOURCC('S','N','1','2');

const uint SN12_TILE_WIDTH = 64;
const uint SN12_TILE_HEIGHT = 32;
const uint SN12_TILE_SIZE = SN12_TILE_WIDTH * SN12_TILE_HEIGHT;

// Pipeline element name
const char* kPipelineName = "gst_pipeline";

// Update duration every 100ms.
const int kDurationUpdateInterval = 100;

// For smooth playback, seeking will be done to I-Frame + kSixteenMilliSeconds
// Reason to choose kSixteenMilliSeconds is duration of each video frame at
// 60 fps video will be ~16 milliseconds.
const int64 kSixteenMilliSeconds = 16000000;

const char *h264elements[] = {
    "h264parse"
#if defined(OS_TIZEN)
    , "omx_h264dec"
#else
    , "ffdec_h264"
#endif
};

const char *aacelements[] = {
    "aacparse"
#if defined(OS_TIZEN_MOBILE)
    , "savsdec_aac", "autoaudiosink"
#elif defined(OS_TIZEN_TV)
    , "omx_aacdec", "alsasink"
#else
    , "faad", "autoaudiosink"
#endif
};

// FIXME: This is derived from command-line pipeline on desktop and mobile.
// Need to find the pipeline on TV.
// Also the connection might be different when converting it to code.
const char *mp3elements[] = {
    "mpegaudioparse"
#if defined(OS_TIZEN_MOBILE)
    , "savsdec_mp3"
#else
    , "ffdec_mp3"
#endif
    ,"autoaudiosink"
};

const AudioCodecGstElementsMapping AudioMapping[] = {
    {media::kCodecAAC, aacelements},
    {media::kCodecMP3, mp3elements},
    {media::kUnknownAudioCodec, NULL}
};

const VideoCodecGstElementsMapping VideoMapping[] = {
    {media::kCodecH264, h264elements},
    {media::kUnknownVideoCodec, NULL}
};

GstClockTime ConvertToGstClockTime(double time) {
  if (time < 0) {
    LOG(ERROR) << "Invalid time:" << time << " Reset to 0";
    time = 0;
  }

  // Extract the integer part of the time (seconds) and the fractional part
  // (microseconds). Attempt to round the microseconds so no floating point
  // precision is lost and we can perform an accurate seek.
  double seconds;
  double microSeconds = std::modf(time, &seconds) * 1000000;
  GTimeVal timeValue;
  timeValue.tv_sec = static_cast<glong>(seconds);
  timeValue.tv_usec = static_cast<glong>(lround(microSeconds / 10) * 10);
  return GST_TIMEVAL_TO_TIME(timeValue);
}

double ConvertNanoSecondsToSeconds(int64 time) {
  double seconds = static_cast<double>(time) /
      (base::Time::kNanosecondsPerSecond);
  return seconds;
}

}  // namespace

namespace media {

static GstBusSyncReply gst_pipeline_message_cb(
    GstBus* bus,
    GstMessage* message,
    gpointer user_data) {
  MediaSourcePlayerGstreamer* player =
      static_cast<MediaSourcePlayerGstreamer*>(user_data);
  if (!player || player->IsPlayerDestructing())
    return GST_BUS_PASS;

  player->HandleMessage(message);
  gst_message_unref(message);
  return GST_BUS_DROP;
}

#if defined(TIZEN_MULTIMEDIA_PIXMAP_SUPPORT)
static Eina_Bool notify_damage_updated_cb(
    void* user_data,
    int type,
    void* event) {
  MediaSourcePlayerGstreamer* player =
      static_cast<MediaSourcePlayerGstreamer*>(user_data);
  if (!player || player->IsPlayerDestructing())
    return ECORE_CALLBACK_PASS_ON;
  player->PlatformSurfaceUpdated();
  return ECORE_CALLBACK_PASS_ON;
}
#endif

#if defined(OS_TIZEN_TV)
#if defined(TIZEN_MULTIMEDIA_PIXMAP_SUPPORT)
static int get_pixmap_id_cb(void* user_data) {
  MediaSourcePlayerGstreamer* player =
      static_cast<MediaSourcePlayerGstreamer*>(user_data);
  return player->GetSurfaceID();
}
#endif

static ASM_cb_result_t media_player_audio_session_event_source_pause(
    ASM_event_sources_t event_source,
    void* user_data) {
  MediaPlayerTizen* player =
      static_cast<MediaPlayerTizen*>(user_data);
  if (!player) {
    return ASM_CB_RES_IGNORE;
  }

  switch (event_source) {
    case ASM_EVENT_SOURCE_CALL_START:
    case ASM_EVENT_SOURCE_ALARM_START:
    case ASM_EVENT_SOURCE_MEDIA:
    case ASM_EVENT_SOURCE_EMERGENCY_START:
    case ASM_EVENT_SOURCE_OTHER_PLAYER_APP:
    case ASM_EVENT_SOURCE_RESOURCE_CONFLICT:
    case ASM_EVENT_SOURCE_EARJACK_UNPLUG:
      player->Pause(true);
      return ASM_CB_RES_PAUSE;
    default:
      return ASM_CB_RES_NONE;
  }
}

static ASM_cb_result_t media_player_audio_session_event_source_play(
    ASM_event_sources_t event_source,
    void* user_data) {
  MediaPlayerTizen* player =
      static_cast<MediaPlayerTizen*>(user_data);
  if (!player) {
    return ASM_CB_RES_IGNORE;
  }

  switch (event_source) {
    case ASM_EVENT_SOURCE_ALARM_END:
      player->Play();
      return ASM_CB_RES_PLAYING;
    default:
      return ASM_CB_RES_NONE;
  }
}

static ASM_cb_result_t media_player_private_audio_session_notify_cb(
    int,
    ASM_event_sources_t event_source,
    ASM_sound_commands_t command,
    unsigned int,
    void* user_data) {
  if (command == ASM_COMMAND_STOP || command == ASM_COMMAND_PAUSE)
    return media_player_audio_session_event_source_pause(
        event_source, user_data);
  if (command == ASM_COMMAND_PLAY || command == ASM_COMMAND_RESUME)
    return media_player_audio_session_event_source_play(
        event_source, user_data);
  return ASM_CB_RES_NONE;
}
#endif

static void  on_gst_start_video_feed_cb(
    GstAppSrc* pipeline,
    guint size,
    void *user_data) {
  MediaSourcePlayerGstreamer* player =
      static_cast<MediaSourcePlayerGstreamer*>(user_data);
  if (!player || player->IsPlayerDestructing())
    return;
  player->OnReadDemuxedData(media::DemuxerStream::VIDEO);
  return ;
}

static void  on_gst_stop_video_feed_cb(GstAppSrc * pipeline,void *user_data) {
  MediaSourcePlayerGstreamer* player =
      static_cast<MediaSourcePlayerGstreamer*>(user_data);
  if (!player || player->IsPlayerDestructing())
    return;
  player->OnStopDemuxedData(media::DemuxerStream::VIDEO);
}

static gboolean on_gst_seek_video_feed_cb(
    GstAppSrc* pipeline,
    guint64 offset,
    void *user_data) {

  MediaSourcePlayerGstreamer* player =
      static_cast<MediaSourcePlayerGstreamer*>(user_data);
  if (!player || player->IsPlayerDestructing())
    return FALSE;
  player->UpdateVideoSeekOffset(offset);
  return TRUE;
}

static GstFlowReturn on_gst_appsink_preroll(
    GstAppSink* sink,
    gpointer user_data) {
  MediaSourcePlayerGstreamer* player =
      static_cast<MediaSourcePlayerGstreamer*>(user_data);
  if (!player || player->IsPlayerDestructing())
    return GST_FLOW_ERROR;
  player->GetFrameDetails();
  return GST_FLOW_OK;
}

static GstFlowReturn on_gst_appsink_buffer(
    GstAppSink* sink,
    gpointer user_data) {
  MediaSourcePlayerGstreamer* player =
      static_cast<MediaSourcePlayerGstreamer*>(user_data);
  if (!player || player->IsPlayerDestructing())
    return GST_FLOW_ERROR;
  player->OnNewFrameAvailable(player->PullBuffer());
  return GST_FLOW_OK;
}

gboolean on_gst_seek_audio_feed_cb(
    GstAppSrc *pipeline,
    guint64 offset,
    void* user_data) {
  MediaSourcePlayerGstreamer* player =
      static_cast<MediaSourcePlayerGstreamer*>(user_data);
  if (!player || player->IsPlayerDestructing())
    return FALSE;
  player->UpdateAudioSeekOffset(offset);
  return TRUE;
}

static void on_gst_start_audio_feed_cb (
    GstAppSrc * pipeline,
    guint size,
    void* user_data) {
  MediaSourcePlayerGstreamer* player =
      static_cast<MediaSourcePlayerGstreamer*>(user_data);
  if (!player || player->IsPlayerDestructing())
    return;
  player->OnReadDemuxedData(media::DemuxerStream::AUDIO);
}

static void on_gst_stop_audio_feed_cb(GstAppSrc* pipeline, void* user_data) {
  MediaSourcePlayerGstreamer* player =
      static_cast<MediaSourcePlayerGstreamer*>(user_data);
  if (!player || player->IsPlayerDestructing())
    return;
  player->OnStopDemuxedData(media::DemuxerStream::AUDIO);
}

#if defined(TIZEN_MULTIMEDIA_PIXMAP_SUPPORT)
static void on_video_sink_caps_changed_cb(
    GObject* gobject,
    GParamSpec* gparamspec,
    void* user_data) {
  MediaSourcePlayerGstreamer* player =
      static_cast<MediaSourcePlayerGstreamer*>(user_data);
  if (!player || player->IsPlayerDestructing())
    return;
  player->OnVideoConfigsChanged();
}
#endif

// Generating Unique string from given width and height.
std::string ConvertWidthAndHeightToString(int width, int height) {
  std::ostringstream width_stream, height_stream;
  width_stream << width;
  height_stream << height;
  return width_stream.str() + "X" + height_stream.str();
}

MediaSourcePlayerGstreamer::MediaSourcePlayerGstreamer(
    int player_id,
    scoped_ptr<DemuxerTizen> demuxer,
    MediaPlayerManager* manager)
    : MediaPlayerTizen(player_id, manager),
      demuxer_(demuxer.Pass()),
      main_loop_(base::MessageLoopProxy::current()),
#if defined(OS_TIZEN_TV)
      audio_session_manager_(AudioSessionManager::CreateAudioSessionManager()),
#endif
      playing_(false),
      weak_this_(this),
#if defined(TIZEN_MULTIMEDIA_PIXMAP_SUPPORT)
      pixmap_id_(0),
      efl_pixmap_(NULL),
      m_damage(0),
      m_damageHandler(0),
#endif
      is_xwindow_handle_set_(false),
      is_backed_by_pixmap(false),
      pipeline_(NULL),
      video_appsrc_(NULL),
      video_parse_(NULL),
      video_decoder_(NULL),
      video_sink_(NULL),
      audio_appsrc_(NULL),
      audio_decoder_(NULL),
      audio_sink_(NULL),
      video_queue_(NULL),
      audio_queue_(NULL),
      audio_parse_(NULL),
      audio_convert_(NULL),
      audio_resampler_(NULL),
      audio_volume_(NULL),
      video_sink_pad_(NULL),
      should_feed_audio_(true),
      should_feed_video_(false),
      gst_width_(0),
      gst_height_(0),
      video_format_(0),
      media_type(0),
      play_rate_(1.0f),
      duration_(0),
      is_paused_due_underflow_(false),
      shared_memory_size(0),
      buffered_(0),
      is_paused_(false),
      is_seeking_(false),
      is_demuxer_seeking_(false),
      audio_buffered_(0),
      video_buffered_(0),
      is_gst_pipeline_constructed_(false),
      is_download_finished_(false),
      is_end_reached_(false),
      error_occured_(false),
      raw_video_frame_size_(0),
      video_seek_offset_(0),
      audio_seek_offset_(0),
      is_seeking_iframe_(false) {
  demuxer_->Initialize(this);
  audio_buffer_queue_.clear();
  video_buffer_queue_.clear();
#if defined(TIZEN_MULTIMEDIA_PIXMAP_SUPPORT)
  efl_pixmaps_map_.clear();
#endif
#if defined(OS_TIZEN_TV)
  if (!audio_session_manager_->RegisterAudioSessionManager(
      MM_SESSION_TYPE_SHARE,
      media_player_private_audio_session_notify_cb, this))
    return;
#endif
}

MediaSourcePlayerGstreamer::~MediaSourcePlayerGstreamer() {
  VLOG(1) << "MediaSourcePlayerGstreamer::" << __FUNCTION__
          << "Player ID:" << GetPlayerId();
}

void MediaSourcePlayerGstreamer::Destroy() {
  if (IsPlayerDestructing())
    return;

  VLOG(1) << "MediaSourcePlayerGstreamer::" << __FUNCTION__
          << "Player ID:" << GetPlayerId();
  destructing_ = true;
  Release();
  main_loop_->DeleteSoon(FROM_HERE, this);
}

void MediaSourcePlayerGstreamer::Play() {
  if (!pipeline_ || error_occured_)
    return;
  if (play_rate_ == 0.0) {
    playing_ = true;
    return;
  }
#if defined(OS_TIZEN_TV)
  if (!audio_session_manager_->SetSoundState(ASM_STATE_PLAYING))
    return;
#endif
  VLOG(1) << "MediaSourcePlayerGstreamer::" << __FUNCTION__
          << "Player ID:" << GetPlayerId();

#if defined(OS_TIZEN_MOBILE) && (defined(TIZEN_V_2_3) || defined(TIZEN_V_2_4))
  if (device_power_wakeup(false) != DEVICE_ERROR_NONE)
    LOG(ERROR) << "|device_power_wakeup| request failed";

  if (device_power_request_lock(POWER_LOCK_DISPLAY, 0) != DEVICE_ERROR_NONE)
    LOG(ERROR) << "|device_power_request_lock| request failed";
#endif

  gst_element_set_state(pipeline_, GST_STATE_PLAYING);
  StartCurrentTimeUpdateTimer();
  playing_ = true;
  is_paused_due_underflow_ = false;
}

void MediaSourcePlayerGstreamer::Pause(bool is_media_related_action) {
  if (!pipeline_ || error_occured_)
    return;

#if defined(OS_TIZEN_TV)
  if (!audio_session_manager_->SetSoundState(ASM_STATE_PAUSE))
    return;
#endif

  VLOG(1) << "MediaSourcePlayerGstreamer::" << __FUNCTION__;
  gst_element_set_state(pipeline_, GST_STATE_PAUSED);

  StopCurrentTimeUpdateTimer();
  if (!is_media_related_action) {

#if defined(OS_TIZEN_MOBILE) && (defined(TIZEN_V_2_3) || defined(TIZEN_V_2_4))
    if (device_power_release_lock(POWER_LOCK_DISPLAY) != DEVICE_ERROR_NONE)
      LOG(ERROR) << "|device_power_release_lock| request failed";
#endif

    is_paused_due_underflow_ = false;
    playing_ = false;
  }
}

void MediaSourcePlayerGstreamer::SetRate(double rate) {
  VLOG(1) << "MediaSourcePlayerGstreamer::"
          << __FUNCTION__ << " : " << rate;

  if (play_rate_ == rate)
    return;

  if (rate == 0.0) {
    play_rate_ = rate;
    Pause(true);
    return;
  }

  // If rate was zero and requested rate is non-zero, change the paused state
  if(play_rate_ == 0.0 && rate != 0.0) {
    Play();
    StartCurrentTimeUpdateTimer();
  }

  play_rate_ = rate;

  RequestPlayerSeek(GetCurrentTime());

#if defined(OS_TIZEN_TV)
  if (!audio_session_manager_->SetSoundState(ASM_STATE_PAUSE))
    return;
#endif
}

void MediaSourcePlayerGstreamer::RequestPlayerSeek(double seekTime) {
  VLOG(1) << "MediaSourcePlayerGstreamer::" << __FUNCTION__
          << " : " << seekTime;
  if (is_demuxer_seeking_)
    return;
  GstState state;
  gst_element_get_state(pipeline_, &state, NULL, 250 * GST_NSECOND);
  is_demuxer_seeking_ = true;
  if (state == GST_STATE_PLAYING)
    Pause(true);
  manager()->OnRequestSeek(GetPlayerId(), seekTime);
}

void MediaSourcePlayerGstreamer::Seek(const double time) {
  GstState state;
  gst_element_get_state(pipeline_, &state, NULL, 250 * GST_NSECOND);

  is_seeking_iframe_ = false;
  VLOG(1) << "MediaSourcePlayerGstreamer::" << __FUNCTION__
          << " : " << time << " state : " << gst_element_state_get_name(state);

  is_demuxer_seeking_ = true;
  if (state == GST_STATE_PLAYING)
    Pause(true);

  // Input to |FromMicroseconds| is |int64|. Additional multiplication
  // is done to avoid data loss.
  base::TimeDelta seek_time = base::TimeDelta::FromMicroseconds(
      static_cast<int64>(time * base::Time::kMicrosecondsPerSecond));
  demuxer_->RequestDemuxerSeek(seek_time);
}

void MediaSourcePlayerGstreamer::SeekInternal(const GstClockTime position) {
  if (!pipeline_ || error_occured_)
    return;

  GstClockTime startTime, endTime;

  is_demuxer_seeking_ = false;

  if (play_rate_ > 0) {
    startTime = position;
    endTime = GST_CLOCK_TIME_NONE;
  } else {
    startTime = 0;
    endTime = position;
  }

  VLOG(1) << "MediaSourcePlayerGstreamer::" << __FUNCTION__
          << " : " << position;

  UpdateSeekState(true);
  audio_buffer_queue_.clear();
  video_buffer_queue_.clear();
  if(!gst_element_seek(pipeline_, play_rate_, GST_FORMAT_TIME,
      static_cast<GstSeekFlags>(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_ACCURATE),
      GST_SEEK_TYPE_SET, startTime, GST_SEEK_TYPE_SET, endTime)) {
    LOG(ERROR) << "Seek to " << position << " failed";
    HandleError(MediaPlayerTizen::NetworkStateDecodeError);
  }
}

double MediaSourcePlayerGstreamer::GetCurrentTime() {
  if (!pipeline_ || error_occured_)
    return 0.0;

  gint64 current_time = 0;
  GstFormat format = GST_FORMAT_TIME;
  gst_element_query_position(pipeline_, &format, &current_time);
  return ConvertNanoSecondsToSeconds(current_time);
}

void MediaSourcePlayerGstreamer::Release() {
  VLOG(1) << "MediaSourcePlayerGstreamer::" << __FUNCTION__;
  DCHECK(IsPlayerDestructing());
  playing_ = false;
  StopCurrentTimeUpdateTimer();
  audio_buffer_queue_.clear();
  video_buffer_queue_.clear();

  if (pipeline_) {
    GstBus* bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline_));
    if (bus) {
      g_signal_handlers_disconnect_by_func(
          bus, reinterpret_cast<gpointer>(gst_pipeline_message_cb), this);
      gst_bus_set_sync_handler(bus, NULL, NULL);
      gst_object_unref(bus);
    }

    gst_element_set_state(pipeline_, GST_STATE_NULL);
    gst_object_unref(pipeline_);
    pipeline_ = NULL;
  }

  if (video_sink_pad_)
    gst_object_unref(video_sink_pad_);

#if defined(OS_TIZEN_TV)
  if (!audio_session_manager_->SetSoundState(ASM_STATE_STOP))
    return;
  if (!audio_session_manager_->DeallocateResources())
    return;
#endif
#if defined(TIZEN_MULTIMEDIA_PIXMAP_SUPPORT)
  UnregisterDamageHandler();
  EflPixmapMap::iterator it = efl_pixmaps_map_.begin();
  while (it != efl_pixmaps_map_.end()) {
    efl_pixmap_ = it->second;
    if (efl_pixmap_.get())
      efl_pixmap_ = NULL;
    it++;
  }
  efl_pixmaps_map_.clear();
#endif
}

#if defined(TIZEN_MULTIMEDIA_PIXMAP_SUPPORT)
void MediaSourcePlayerGstreamer::UnregisterDamageHandler() {
  if (m_damage) {
    ecore_x_damage_free(m_damage);
    m_damage = 0;
  }
  if (m_damageHandler) {
    ecore_event_handler_del(m_damageHandler);
    m_damageHandler = 0;
  }
}
#endif

void MediaSourcePlayerGstreamer::SetVolume(double volume) {
  if (audio_volume_)
    g_object_set(G_OBJECT(audio_volume_), "volume", volume, NULL);
}

void MediaSourcePlayerGstreamer::OnDemuxerConfigsAvailable(
    const DemuxerConfigs& configs) {
  if (IsPlayerDestructing())
    return;

  if ((configs.video_codec == kUnknownVideoCodec ||
      configs.video_codec != kCodecH264) &&
      (configs.audio_codec == kUnknownAudioCodec ||
      (configs.audio_codec != kCodecAAC &&
      configs.audio_codec != kCodecMP3))) {
    LOG(ERROR) << "Audio and Video codecs not supported for MediaSource";
    HandleError(MediaPlayerTizen::NetworkStateFormatError);
    return;
  }

  gst_width_ = configs.video_size.width();
  gst_height_ = configs.video_size.height();

  if (is_gst_pipeline_constructed_) {
    return;
  }
  is_gst_pipeline_constructed_ = true;

  GError* err = NULL;
  if (!gst_is_initialized()) {
    gst_init_check(NULL, NULL, &err);
  }

  if (gst_is_initialized() && !err) {
    pipeline_ = gst_pipeline_new(kPipelineName);
    if (!pipeline_) {
      LOG(ERROR) << "Unable to Create |Pipeline|";
      HandleError(MediaPlayerTizen::NetworkStateDecodeError);
      return;
    }

    int i = 0;
    while (VideoMapping[i].codec != kUnknownVideoCodec) {
      if (configs.video_codec == VideoMapping[i].codec) {
        media_type |= MEDIA_VIDEO_MASK;
        video_appsrc_ = gst_element_factory_make("appsrc", "video-source");
        video_parse_ = gst_element_factory_make(VideoMapping[i].elements[0],
            "video-parse");
        video_queue_ = gst_element_factory_make("queue2", "video-queue");
        video_decoder_ = gst_element_factory_make(VideoMapping[i].elements[1],
            "video-decoder");

#if defined(TIZEN_MULTIMEDIA_PIXMAP_SUPPORT)
        PrepareForVideoSink();
#else
        PrepareForVideoFrame();
#endif

#if defined(TIZEN_MULTIMEDIA_PIXMAP_SUPPORT)
        if (is_backed_by_pixmap)
          CreatePixmap();
#endif
        if (!video_appsrc_ || !video_parse_ || !video_queue_ ||
            !video_decoder_ || !video_sink_) {
          LOG(ERROR) << "Not  all elements of video pipeline could be created";
          ReleaseVideoElements();
          HandleError(MediaPlayerTizen::NetworkStateDecodeError);
          return;
        }

        g_object_set(GST_OBJECT(video_appsrc_), "format", GST_FORMAT_TIME, NULL);
        g_object_set(GST_OBJECT(video_appsrc_), "stream-type",
            GST_APP_STREAM_TYPE_SEEKABLE, NULL);
        g_object_set(GST_OBJECT(video_appsrc_), "do-timestamp", false, NULL);

        // Will make the queue to send GST_MESSAGE_BUFFERING
        g_object_set(G_OBJECT(video_queue_), "use-buffering", true, NULL);

        // Will trigger need-data callback if buffer goes below 30%,
        // default is 10%. Data type of property "low-percent" differs
        // on desktop and TIZEN platform.
#if defined(OS_TIZEN)
        g_object_set(G_OBJECT(video_queue_), "low-percent", (double)30.0, NULL);
#else
        g_object_set(G_OBJECT(video_queue_), "low-percent", 30, NULL);
#endif

        gst_bin_add_many(GST_BIN(pipeline_), video_appsrc_, video_queue_,
            video_parse_, video_decoder_, video_sink_, NULL);

        // Why |video_queue_| is placed after |video_appsrc_|?
        // For understanding puprose consider http://tinyurl.com/qos-iron url.
        // For 1080p resolution of the video in above url, each decoded frame
        // is of size 2304000 bytes ~ 2.19 MB. If video_queue_ is placed before
        // |video_sink_| then queue will buffer decoded frames, so to buffer
        // two second worth of data queue will require 2304000*24(fps)*2 ~ 96MB
        // of queue size. This property can't be set for pixmap backed playback
        // as frame size won't be available for pixmap backed | video_sink|.
        // And this size varies from video to video.
        //
        // But if |video_queue_| is placed after |video_appsrc_|, queue will
        // buffer encoded data. For the same video of 1080p, maximum encoded
        // frame is of 115398byte ~ 0.110052109 MB. So for 2 sec data, queue
        // need to buffer 5308308bytes in queue ~ 5MB, this can be set
        // dynamically. Refer |OnDemuxerDataAvailable| for setting queue size.

        gst_element_link_many(video_appsrc_,video_queue_, video_parse_,
            video_decoder_, video_sink_, NULL);
        static GstAppSrcCallbacks video_callbacks = {
            on_gst_start_video_feed_cb,
            on_gst_stop_video_feed_cb,
            on_gst_seek_video_feed_cb, {NULL}};

        // FIXME: Try fourth argument for destructy notification.
        gst_app_src_set_callbacks(GST_APP_SRC(video_appsrc_), &video_callbacks,
            this, NULL);
        break;
      }
      i++;
    }

    i = 0;
    while (AudioMapping[i].codec != kUnknownAudioCodec) {
      if(configs.audio_codec == AudioMapping[i].codec) {
        media_type |= MEDIA_AUDIO_MASK;
        audio_appsrc_ = gst_element_factory_make("appsrc", "audio-source");
        audio_queue_ = gst_element_factory_make("queue2", "audio-queue");
        audio_parse_ = gst_element_factory_make(AudioMapping[i].elements[0],
            "audio-parse");
        audio_decoder_ = gst_element_factory_make(AudioMapping[i].elements[1],
            "audio-decoder");
        audio_convert_ = gst_element_factory_make("audioconvert", "audio-convert");
        audio_resampler_ = gst_element_factory_make(
            "audioresample", "audio-resample");
        audio_volume_ = gst_element_factory_make("volume", "volume");
        audio_sink_ = gst_element_factory_make(
            AudioMapping[i].elements[2], "audio-sink");

        if (!audio_appsrc_ || !audio_queue_ || !audio_parse_ ||
            !audio_decoder_ || !audio_convert_ || !audio_resampler_ ||
            !audio_volume_ || !audio_sink_) {
          LOG(ERROR) << "Not  all elements of audio pipeline could be created";
          ReleaseAudioElements();
          HandleError(MediaPlayerTizen::NetworkStateDecodeError);
          return;
        }

        g_object_set(
            GST_OBJECT(audio_appsrc_),"format", GST_FORMAT_TIME, NULL);
        g_object_set(GST_OBJECT(audio_appsrc_),"stream-type",
            GST_APP_STREAM_TYPE_SEEKABLE, NULL);
        g_object_set(GST_OBJECT(audio_appsrc_), "do-timestamp", false, NULL);

        g_object_set(G_OBJECT(audio_queue_), "use-buffering", true, NULL);

#if defined(OS_TIZEN)
        g_object_set(
            G_OBJECT(audio_queue_), "low-percent", (double)30.0, NULL);
#else
        g_object_set(G_OBJECT(audio_queue_), "low-percent", 30, NULL);
#endif

        g_object_set(G_OBJECT(audio_volume_), "mute", false, NULL);

#if defined(OS_TIZEN_TV)
        if (!audio_session_manager_->AllocateResources(audio_decoder_))
          return;

        g_object_set(audio_sink_, "provide-clock", true, "device", "hw:0,0", NULL);
        if (!audio_session_manager_->AllocateResources(audio_sink_))
          return;
#endif

        gst_bin_add_many(GST_BIN(pipeline_), audio_appsrc_, audio_queue_,
            audio_parse_, audio_decoder_, audio_convert_, audio_resampler_,
            audio_volume_, audio_sink_, NULL);

        gst_element_link_many(audio_appsrc_, audio_queue_, audio_parse_,
            audio_decoder_, audio_convert_, audio_resampler_, audio_volume_,
            audio_sink_, NULL);

        static GstAppSrcCallbacks audio_callbacks = {
            on_gst_start_audio_feed_cb,
            on_gst_stop_audio_feed_cb,
            on_gst_seek_audio_feed_cb, {NULL}};

        gst_app_src_set_callbacks(GST_APP_SRC(audio_appsrc_), &audio_callbacks,
            this, NULL);
        break;
      }
      i++;
    }

    GstBus*bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline_));
    if (!bus) {
      LOG(ERROR) << "GStreamer bus creation failed";
      HandleError(MediaPlayerTizen::NetworkStateDecodeError);
      return;
    }
    gst_bus_set_sync_handler(
        bus, (GstBusSyncHandler)gst_pipeline_message_cb, this);
    gst_object_unref(bus);

    manager()->OnMediaDataChange(GetPlayerId(), video_format_,
        gst_height_, gst_width_, media_type);

    manager()->OnReadyStateChange(GetPlayerId(),
        MediaPlayerTizen::ReadyStateHaveMetadata);

    if (gst_element_set_state(pipeline_, GST_STATE_PAUSED) ==
        GST_STATE_CHANGE_FAILURE)
      LOG(ERROR) << "GStreamer state change failed";
  } else {
    LOG(ERROR) << "Unable to initialize GST";
    HandleError(MediaPlayerTizen::NetworkStateDecodeError);
  }
}

#if defined(TIZEN_MULTIMEDIA_PIXMAP_SUPPORT)
void MediaSourcePlayerGstreamer::PrepareForVideoSink() {
  video_sink_ = gst_element_factory_make("xvimagesink", "sink");
  if ( video_sink_ == NULL) {
    PrepareForVideoFrame();
  } else {
    VLOG(1) << "MediaSource using |xvimagesink| for Video Playback";
    is_backed_by_pixmap = true;
    video_sink_pad_ = gst_element_get_static_pad(video_sink_, "sink");
    if (video_sink_pad_) {
      g_signal_connect(video_sink_pad_, "notify::caps",
          G_CALLBACK(on_video_sink_caps_changed_cb), this);
    } else {
      LOG(ERROR) << " |video_sink_pad_| is NULL ?";
    }
  }
}
#endif

void MediaSourcePlayerGstreamer::PrepareForVideoFrame() {
  is_xwindow_handle_set_ = true;
  VLOG(1) << "MediaSource using |appsink| for Video Playback";
  video_sink_ = gst_element_factory_make("appsink", "sink");
  GstAppSinkCallbacks callbacks = {NULL,
      on_gst_appsink_preroll,
      on_gst_appsink_buffer,
      NULL, {NULL, NULL, NULL}};
  gst_app_sink_set_callbacks(GST_APP_SINK(video_sink_), &callbacks,
      this, NULL);
  g_object_set(G_OBJECT(video_sink_), "max-buffers", (guint)1, NULL);
}

void MediaSourcePlayerGstreamer::ReadDemuxedData(
    media::DemuxerStream::Type type) {
  if (IsPlayerDestructing())
    return;

  if (type == media::DemuxerStream::AUDIO) {
    should_feed_audio_ = true;
  } else if (type == media::DemuxerStream::VIDEO) {
    should_feed_video_ = true;
  } else {
    LOG(ERROR) << "Unknown Media Type in MediaSourcePlayerGstreamer::"
               << __FUNCTION__;
    return;
  }
  demuxer_->RequestDemuxerData(type);
}

void MediaSourcePlayerGstreamer::OnReadDemuxedData(
    media::DemuxerStream::Type type) {
  if (IsPlayerDestructing()) {
    LOG(ERROR) << __FUNCTION__ << "GST is deinitializing. Just return";
    return;
  }
  main_loop_->PostTask(FROM_HERE, base::Bind(
      &MediaSourcePlayerGstreamer::ReadDemuxedData, base::Unretained(this),
      type));
}

void MediaSourcePlayerGstreamer::OnStopDemuxedData(
    media::DemuxerStream::Type type) {
  if (type == media::DemuxerStream::AUDIO)
    should_feed_audio_ = false;
  else if (type == media::DemuxerStream::VIDEO)
    should_feed_video_ = false;
  else
    LOG(ERROR) << "Unknown Media Type in MediaSourcePlayerGstreamer::"
               << __FUNCTION__;
}

void MediaSourcePlayerGstreamer::OnDemuxerDataAvailable(
    base::SharedMemoryHandle foreign_memory_handle,
    const media::DemuxedBufferMetaData& meta_data) {
  if (!pipeline_ || error_occured_) {
    LOG(ERROR) << "Pipeline_ null or error occured";
    return;
  }
  if (meta_data.size <= 0) {
    LOG(ERROR) << "ERROR : Size of shared memory is Zero";
    return;
  }

  if (is_seeking_ && !is_seeking_iframe_) {
    if (meta_data.type == media::DemuxerStream::VIDEO) {
      is_seeking_iframe_ = true;
      if (video_seek_offset_ >
          (guint64)(meta_data.timestamp.InMicroseconds() * 1000)) {
        RequestPlayerSeek((double)(ConvertNanoSecondsToSeconds(
            meta_data.timestamp.InMicroseconds() * 1000
            + kSixteenMilliSeconds)));
        return;
      }
    } else if (meta_data.type == media::DemuxerStream::AUDIO) {
        if ( audio_seek_offset_ >
            (guint64)(meta_data.timestamp.InMicroseconds() * 1000))
          return;
    }
  }

  ReadFromQueueIfAny(meta_data.type);
  if (meta_data.type == media::DemuxerStream::VIDEO) {
    if (meta_data.size > raw_video_frame_size_) {
      // Dynamically Changing Video Queue Size for Smooth Playback.
      // The default queue size limits are 100 buffers, 2MB of data,
      // or two seconds worth of data, whichever is reached first.
      // Adjust queue to contain two seconds worth of data for smooth playback.
      // So need to adjust number of buffers (max-size-buffers >= 2*fps) and
      // maximum size of queue (max-size-bytes >= 2*fps*meta_data.size).
      //
      // 1000000 micro seconds = 1 second.
      // 2097152 bytes = 2 MB.
      int no_frames_per_two_second , queue_size_for_two_sec;
      raw_video_frame_size_ = meta_data.size;
      no_frames_per_two_second = 2 * (1000000 /
          (meta_data.time_duration.InMicroseconds()));
      queue_size_for_two_sec =
          raw_video_frame_size_ * no_frames_per_two_second;
      if (no_frames_per_two_second > 100) {
        g_object_set(G_OBJECT(video_queue_), "max-size-buffers",
            no_frames_per_two_second, NULL);
      }
      if (queue_size_for_two_sec > 2097152) {
        g_object_set(G_OBJECT(video_queue_), "max-size-bytes",
            queue_size_for_two_sec, NULL);
      }
    }
  }
  if (meta_data.type == media::DemuxerStream::AUDIO && !should_feed_audio_) {
    // Why store the DecoderBuffer? we have requested for buffer
    // from demuxer but gstreamer asked to stop. So need to save
    // this buffer and use it on next |need_data| call.
    SaveDecoderBuffer(foreign_memory_handle, meta_data);
    return;
  }
  if (meta_data.type == media::DemuxerStream::VIDEO && !should_feed_video_) {
    SaveDecoderBuffer(foreign_memory_handle, meta_data);
    return;
  }

  base::SharedMemory shared_memory(foreign_memory_handle, false);
  shared_memory.Map(meta_data.size);
  void* ptr;
  gint size = meta_data.size;
  GstFlowReturn ret = GST_FLOW_OK;
  ptr = g_malloc(size);
  memcpy(ptr, shared_memory.memory(), size);

  GstBuffer* buffer = gst_buffer_new();
  GST_BUFFER_MALLOCDATA(buffer) = (uint8*)ptr;
  GST_BUFFER_SIZE(buffer) = size;
  GST_BUFFER_DATA(buffer) = GST_BUFFER_MALLOCDATA(buffer);
  GST_BUFFER_TIMESTAMP (buffer) =
      (guint64)(meta_data.timestamp.InMicroseconds() * 1000);
  GST_BUFFER_DURATION (buffer) =
      (guint64)(meta_data.time_duration.InMicroseconds() * 1000);

  if (meta_data.type == media::DemuxerStream::AUDIO)
    ret = gst_app_src_push_buffer(GST_APP_SRC(audio_appsrc_), buffer);
  else if (meta_data.type == media::DemuxerStream::VIDEO)
    ret = gst_app_src_push_buffer(GST_APP_SRC(video_appsrc_), buffer);

  // gst_app_src_push_buffer() takes ownership of the buffer.
  // Hence no need to unref buffer.
  if (ret != GST_FLOW_OK) {
    LOG(ERROR) << __FUNCTION__ << " : Gstreamer appsrc push failed : " << ret;
    shared_memory.Close();
    return;
  }

  if (meta_data.type == media::DemuxerStream::AUDIO && should_feed_audio_)
    OnReadDemuxedData(media::DemuxerStream::AUDIO);
  else if (meta_data.type == media::DemuxerStream::VIDEO && should_feed_video_)
    OnReadDemuxedData(media::DemuxerStream::VIDEO);
  shared_memory.Close();
  return;
}

void MediaSourcePlayerGstreamer::OnBufferMetaDataAvailable(
    const media::DemuxedBufferMetaData& meta_data) {

  if (!pipeline_ || error_occured_) {
    LOG(ERROR) << "Pipeline_ null or error occured";
    return;
  }

  switch (meta_data.status) {
    case media::DemuxerStream::kAborted:
      // FIXME : Need to handle Aborted state Properly.
      if (meta_data.type == media::DemuxerStream::AUDIO && should_feed_audio_)
        OnReadDemuxedData(media::DemuxerStream::AUDIO);
      else if (meta_data.type == media::DemuxerStream::VIDEO &&
          should_feed_video_)
        OnReadDemuxedData(media::DemuxerStream::VIDEO);
      break;

    case media::DemuxerStream::kConfigChanged:
      if (meta_data.type == media::DemuxerStream::AUDIO && should_feed_audio_)
        OnReadDemuxedData(media::DemuxerStream::AUDIO);
      else if (meta_data.type == media::DemuxerStream::VIDEO &&
          should_feed_video_)
        OnReadDemuxedData(media::DemuxerStream::VIDEO);

      if (meta_data.type == media::DemuxerStream::AUDIO)
        VLOG(1) << "[BROWSER] : AUDIO::kConfigChanged";
      if (meta_data.type == media::DemuxerStream::VIDEO)
        VLOG(1) << "[BROWSER] : VIDEO::kConfigChanged";
      break;

    case media::DemuxerStream::kOk:
      if (meta_data.end_of_stream) {
        ReadFromQueueIfAny(meta_data.type);
        LOG(ERROR) <<"[BROWSER] : DemuxerStream::kOk but |end_of_stream|";
        if (meta_data.type == media::DemuxerStream::AUDIO)
          gst_app_src_end_of_stream(GST_APP_SRC(audio_appsrc_));
        if (meta_data.type == media::DemuxerStream::VIDEO)
          gst_app_src_end_of_stream(GST_APP_SRC(video_appsrc_));
        if (playing_)
          Play();
      }
      break;

    default:
      NOTREACHED();
  }
}

void MediaSourcePlayerGstreamer::ReadFromQueueIfAny(
    DemuxerStream::Type type) {
  if (!pipeline_ || error_occured_) {
    LOG(ERROR) << "Pipeline_ null or error occured";
    return;
  }

  if (type == media::DemuxerStream::AUDIO) {
    if (audio_buffer_queue_.empty() || !should_feed_audio_)
      return;
  }

  if (type == media::DemuxerStream::VIDEO) {
    if (video_buffer_queue_.empty() || !should_feed_video_)
      return;
  }

  scoped_refptr<DecoderBuffer> decoder_buffer;
  if (type == media::DemuxerStream::AUDIO) {
    decoder_buffer = audio_buffer_queue_.front();
    audio_buffer_queue_.pop_front();
  } else {
    decoder_buffer = video_buffer_queue_.front();
    video_buffer_queue_.pop_front();
  }

  void* ptr;
  GstFlowReturn ret;
  gint size = decoder_buffer.get()->data_size();
  ptr = g_malloc(size);
  memcpy(ptr, (void*)decoder_buffer.get()->writable_data(), size);
  GstBuffer* buffer = gst_buffer_new();
  GST_BUFFER_MALLOCDATA(buffer) = (uint8*)ptr;
  GST_BUFFER_SIZE(buffer) = size;
  GST_BUFFER_DATA(buffer) = GST_BUFFER_MALLOCDATA(buffer);
  GST_BUFFER_TIMESTAMP (buffer) =
      (guint64)(decoder_buffer.get()->timestamp().InMicroseconds() * 1000);
  GST_BUFFER_DURATION (buffer) =
      (guint64)(decoder_buffer.get()->duration().InMicroseconds() * 1000);

  if (type == media::DemuxerStream::AUDIO)
    ret = gst_app_src_push_buffer(GST_APP_SRC(audio_appsrc_), buffer);
  else
    ret = gst_app_src_push_buffer(GST_APP_SRC(video_appsrc_), buffer);
  if (ret != GST_FLOW_OK)
    return;

  //Empty the Buffer before reading the new buffer from render process.
  ReadFromQueueIfAny(type);
  return;
}

void MediaSourcePlayerGstreamer::SaveDecoderBuffer(
    base::SharedMemoryHandle foreign_memory_handle,
    const media::DemuxedBufferMetaData& meta_data) {
  if (!pipeline_ || error_occured_) {
    LOG(ERROR) << "Pipeline_ null or error occured";
    return;
  }

  base::SharedMemory shared_memory(foreign_memory_handle, false);
  shared_memory.Map(meta_data.size);
  scoped_refptr<DecoderBuffer> buffer;
  buffer = DecoderBuffer::CopyFrom(static_cast<const uint8*> (
      shared_memory.memory()), meta_data.size);

  if (!buffer.get()) {
    LOG(ERROR) << "DecoderBuffer::CopyFrom failed";
    shared_memory.Close();
    return;
  }

  buffer->set_timestamp(meta_data.timestamp);
  buffer->set_duration(meta_data.time_duration);

  if (meta_data.type == media::DemuxerStream::AUDIO)
    audio_buffer_queue_.push_back(buffer);
  else
    video_buffer_queue_.push_back(buffer);

  shared_memory.Close();
}

void MediaSourcePlayerGstreamer::GetFrameDetails() {
  if (!pipeline_ || error_occured_)
    return;

  GstState state;
  GstState pending;
  GstStateChangeReturn ret = gst_element_get_state(pipeline_,
      &state, &pending, 250 * GST_NSECOND);
  VLOG(1) << "MediaSourcePlayerGstreamer::"
          << __FUNCTION__
          << " GstStateChangeReturn: "
          << gst_element_state_change_return_get_name(ret)
          << " state: "
          << gst_element_state_get_name(state)
          << " pending: "
          << gst_element_state_get_name(pending)
          << " ID " << GetPlayerId();

  // Get details only after prerolling.
  if (pending >= GST_STATE_PAUSED)
    main_loop_->PostTask(FROM_HERE, base::Bind(
        &MediaSourcePlayerGstreamer::OnGetFrameDetails,
        base::Unretained(this)));
}

void MediaSourcePlayerGstreamer::OnGetFrameDetails() {
  if (!pipeline_ || IsPlayerDestructing() || error_occured_)
    return;

  GstBuffer* buffer = gst_app_sink_pull_preroll(GST_APP_SINK(video_sink_));

  if (!buffer)
    return;
  GstCaps* caps = gst_buffer_get_caps(GST_BUFFER(buffer));
  if (!caps) {
    gst_buffer_unref(buffer);
    return;
  }

  // No need to unref |GstStructure|
  const GstStructure* str = gst_caps_get_structure(caps, 0);
  gst_caps_unref(caps);
  gst_buffer_unref(buffer);
  if (!str)
    return;

  if (!gst_structure_get_int(str, "width", &gst_width_) ||
      !gst_structure_get_int(str, "height", &gst_height_) ||
      !gst_structure_get_fourcc(str, "format", &video_format_)) {
    LOG(ERROR) << "Pre-rolled buffer information could not be obtained";
  }

  if(video_format_ == GST_VIDEO_SN12) {
    uint tile_w_align = ((gst_width_ - 1) / SN12_TILE_WIDTH + 2) & ~1;
    bufsize_sn12_ =
        SN12_TILE_SIZE * tile_w_align * ((gst_height_-1)/SN12_TILE_HEIGHT+1)
        + (((gst_height_+1)&~1)/2) * tile_w_align * SN12_TILE_WIDTH;
  }
  manager()->OnMediaDataChange(GetPlayerId(), video_format_,
      gst_height_, gst_width_, media_type);
}

GstBuffer* MediaSourcePlayerGstreamer::PullBuffer() {
  return gst_app_sink_pull_buffer(GST_APP_SINK(video_sink_));
}

void MediaSourcePlayerGstreamer::OnNewFrameAvailable(const GstBuffer* buffer) {
  if (!pipeline_ || error_occured_)
    return;
  if (!buffer)
    return;
  if (!GST_BUFFER_DATA(buffer) || !GST_BUFFER_SIZE(buffer))
    return;

  if (!gst_width_ || !gst_height_)
    GetFrameDetails();
  // FIXME: Cross check the end results.
  base::TimeDelta timestamp =
      base::TimeDelta::FromMicroseconds(
          GST_BUFFER_TIMESTAMP(buffer) /
          base::Time::kNanosecondsPerMicrosecond);

  uint8* buffer_data = GST_BUFFER_DATA(buffer);
  gsize buffer_size = GST_BUFFER_SIZE(buffer);

  if(video_format_ == GST_VIDEO_SN12)
    shared_memory_size = (bufsize_sn12_);
  else
    shared_memory_size = (buffer_size);

  if (!shared_memory.CreateAndMapAnonymous(shared_memory_size)) {
    LOG (ERROR) << "Shared Memory creation failed.";
    gst_buffer_unref(GST_BUFFER(buffer));
    return;
  }

  if (!shared_memory.ShareToProcess(base::Process::Current().Handle(),
      &foreign_memory_handle)) {
    LOG (ERROR) << "Shared Memory handle could not be obtained";
    shared_memory.Close();
    gst_buffer_unref(GST_BUFFER(buffer));
    return;
  }

  memcpy(shared_memory.memory(), buffer_data, shared_memory_size);
  manager()->OnNewFrameAvailable(
      GetPlayerId(), foreign_memory_handle, shared_memory_size, timestamp);

  shared_memory.Close();
  gst_buffer_unref(GST_BUFFER(buffer));
}

#if defined(TIZEN_MULTIMEDIA_PIXMAP_SUPPORT)
void MediaSourcePlayerGstreamer::XWindowIdPrepared(GstMessage* message) {
  gint width, height;
  gst_structure_get_int(message->structure, "video-width", &width);
  gst_structure_get_int(message->structure, "video-height", &height);
  if ((gst_width_ != width) || (gst_height_ != height)) {
    LOG(ERROR) << "Demuxer Video Configs and Gstreamer Video Configs doesn't"
               <<" match.From Demuxer : width : "<<gst_width_
               << " and height :" <<gst_height_
               << " | From Gstreamer width : " <<width
               << " and Height : " <<height;
    gst_width_ = width;
    gst_height_ = height;
  }
  SetPixmap();
  manager()->OnMediaDataChange(
      GetPlayerId(), video_format_, gst_height_, gst_width_, media_type);
}

void MediaSourcePlayerGstreamer::PlatformSurfaceUpdated() {
  gint64 current_time = 0;
  GstFormat format = GST_FORMAT_TIME;
  gst_element_query_position(pipeline_, &format, &current_time);
  base::TimeDelta timestamp = base::TimeDelta::FromMicroseconds(
      current_time / base::Time::kNanosecondsPerMicrosecond);
  manager()->OnPlatformSurfaceUpdated(GetPlayerId(), pixmap_id_, timestamp);
}

int MediaSourcePlayerGstreamer::GetSurfaceID() const {
  return pixmap_id_;
}

void MediaSourcePlayerGstreamer::SetPixmap() {
#if defined(OS_TIZEN_TV)
  // Using below statements on mobile to set pixmap was causing two issue.
  // 1. Video size was different than the required one whenever configaration
  // changed
  // 2. Sometime black screen was appearing, while video was playing.
  // Hence for mobile keeping implementation which uses
  // |gst_x_overlay_set_window_handle|to sep Pixmap.
  g_object_set (video_sink_, "pixmap-id-callback", get_pixmap_id_cb, NULL);
  g_object_set (video_sink_, "pixmap-id-callback-userdata", this, NULL);
#else
  gst_x_overlay_set_window_handle(GST_X_OVERLAY(video_sink_), pixmap_id_);
#endif
  m_damage = ecore_x_damage_new(pixmap_id_,
      ECORE_X_DAMAGE_REPORT_RAW_RECTANGLES);
  m_damageHandler = ecore_event_handler_add(ECORE_X_EVENT_DAMAGE_NOTIFY,
      notify_damage_updated_cb, this);
  g_object_set(video_sink_, "rotate", 0, NULL);
  is_xwindow_handle_set_ = true;
}

void MediaSourcePlayerGstreamer::OnVideoConfigsChanged() {
  if (!pipeline_ || error_occured_)
    return;
  main_loop_->PostTask(FROM_HERE, base::Bind(
      &MediaSourcePlayerGstreamer::VideoConfigsChanged,
          base::Unretained(this)));
}

void MediaSourcePlayerGstreamer::VideoConfigsChanged() {
  if (!pipeline_ || IsPlayerDestructing() || error_occured_)
    return;
  VLOG(1) << "Video Configs Changed, so changing the pixmap";
  gint width, height;

  if (gst_video_get_size(video_sink_pad_, &width, &height)) {
    if ((gst_width_ != width) || (gst_height_ != height)) {
      LOG(ERROR) << "Demuxer Video Configs and Gstreamer Video Configs doesn't"
                 <<" match.From Demuxer : width : "<<gst_width_
                 << " and height :" <<gst_height_
                 << " | From Gstreamer width : " <<width
                 << " and Height : " <<height;
      gst_width_ = width;
      gst_height_ = height;
      UnregisterDamageHandler();
      CreatePixmap();
      SetPixmap();
      manager()->OnMediaDataChange(
          GetPlayerId(), video_format_, gst_height_, gst_width_, media_type);
    }
  }
}

void MediaSourcePlayerGstreamer::CreatePixmap() {
  bool is_create_new = false;
  std::string string_wh =
      ConvertWidthAndHeightToString(gst_width_, gst_height_);
  if (efl_pixmaps_map_.size() == 0)
    is_create_new = true;

  if (!is_create_new) {
    EflPixmapMap::iterator it = efl_pixmaps_map_.find(string_wh);
    if (it != efl_pixmaps_map_.end()) {
      is_create_new = false;
      efl_pixmap_ = it->second;
      pixmap_id_ = efl_pixmap_->GetId();
    } else {
      is_create_new = true;
    }
  }

  if (is_create_new) {
    efl_pixmap_ = gfx::EflPixmap::Create(gfx::EflPixmap::SURFACE,
        gfx::Size(gst_width_, gst_height_));
    if (!efl_pixmap_.get()) {
      LOG(ERROR) << "gfx::EflPixmap::Create() failed to create Pixmap";
      return;
    }
    pixmap_id_ = efl_pixmap_->GetId();
    efl_pixmaps_map_[string_wh] = efl_pixmap_;
  }
}
#endif

void MediaSourcePlayerGstreamer::OnDemuxerDurationChanged(
    base::TimeDelta duration) {
  duration_ = duration.InSecondsF();
}

void MediaSourcePlayerGstreamer::OnDemuxerSeekDone(
    const base::TimeDelta& actual_browser_seek_time) {
  VLOG(1) << "MediaSourcePlayerGstreamer::" << __FUNCTION__ << " : "
          << actual_browser_seek_time.InSecondsF();
  SeekInternal(ConvertToGstClockTime(actual_browser_seek_time.InSecondsF()));
}

bool MediaSourcePlayerGstreamer::HasVideo() {
  return media_type & MEDIA_VIDEO_MASK;
}

bool MediaSourcePlayerGstreamer::HasAudio() {
  return media_type & MEDIA_AUDIO_MASK;
}

void MediaSourcePlayerGstreamer::OnCurrentTimeUpdateTimerFired() {
  manager()->OnTimeUpdate(GetPlayerId(), GetCurrentTime());
}

void MediaSourcePlayerGstreamer::StartCurrentTimeUpdateTimer() {
  if (!current_time_update_timer_.IsRunning()) {
    current_time_update_timer_.Start(
        FROM_HERE,
        base::TimeDelta::FromMilliseconds(kDurationUpdateInterval),
        this, &MediaSourcePlayerGstreamer::OnCurrentTimeUpdateTimerFired);
  }
}

void MediaSourcePlayerGstreamer::StopCurrentTimeUpdateTimer() {
  if (current_time_update_timer_.IsRunning())
    current_time_update_timer_.Stop();
}

void MediaSourcePlayerGstreamer::HandleMessage(GstMessage* message) {
  if (!pipeline_ || error_occured_)
    return;
  if (!strcmp(kPipelineName, GST_MESSAGE_SRC_NAME(message)))
    VLOG(1) << "MediaSourcePlayerGstreamer::" << __FUNCTION__
            << " Message " << GST_MESSAGE_TYPE_NAME(message)
            << " received from element " << GST_MESSAGE_SRC_NAME(message)
            << " ID " << GetPlayerId();

#if defined(TIZEN_MULTIMEDIA_PIXMAP_SUPPORT)
  if (!IsXWindowHadleSet()) {
    if (message->structure && gst_structure_has_name(
        message->structure, "prepare-xid")) {
      VLOG(1) << "Received message-prepare-xid";
      XWindowIdPrepared(message);
      return;
    }
  }
#endif

  switch (GST_MESSAGE_TYPE(message)) {
  case GST_MESSAGE_ERROR:
    GError* error;
    gst_message_parse_error(message, &error, NULL);
    MediaPlayerTizen::NetworkState network_state_error;
    network_state_error = MediaPlayerTizen::NetworkStateEmpty;
    if (error->code == GST_STREAM_ERROR_CODEC_NOT_FOUND
        || error->code == GST_STREAM_ERROR_WRONG_TYPE
        || error->code == GST_STREAM_ERROR_FAILED
        || error->code == GST_RESOURCE_ERROR_NOT_FOUND) {
      network_state_error = MediaPlayerTizen::NetworkStateFormatError;
    } else if (error->domain == GST_RESOURCE_ERROR) {
      network_state_error = MediaPlayerTizen::NetworkStateNetworkError;
    } else {
      network_state_error = MediaPlayerTizen::NetworkStateDecodeError;
    }
#if defined(OS_TIZEN_TV)
    if (!audio_session_manager_->SetSoundState(ASM_STATE_STOP))
      return;
    if (!audio_session_manager_->DeallocateResources())
      return;
#endif
    LOG(ERROR) << "Error Message : " << error->message << " Recieved From : "
               << GST_MESSAGE_SRC_NAME(message)
               << ", and Blink Error Code  = " << network_state_error;
    g_error_free(error);
    HandleError(network_state_error);
    break;
  case GST_MESSAGE_EOS:
    VLOG(1) << "GST_MESSAGE_EOS";
#if defined(OS_TIZEN_TV)
    if (!audio_session_manager_->SetSoundState(ASM_STATE_STOP))
      return;
    if (!audio_session_manager_->DeallocateResources())
      return;
#endif
    main_loop_->PostTask(FROM_HERE, base::Bind(
        &MediaSourcePlayerGstreamer::OnPlaybackComplete,
        base::Unretained(this)));
    break;
  case GST_MESSAGE_ASYNC_DONE:
    VLOG(1) << "HandleMessage : GST_MESSAGE_ASYNC_DONE : is_seeking_ = "
            << is_seeking_;
    if (is_seeking_) {
      is_seeking_iframe_ = false;
      main_loop_->PostTask(FROM_HERE, base::Bind(
          &MediaSourcePlayerGstreamer::UpdateSeekState,
          base::Unretained(this), false));

      // Initiate play for internal seeks.
      if (playing_)
        main_loop_->PostTask(FROM_HERE, base::Bind(
            &MediaSourcePlayerGstreamer::Play, base::Unretained(this)));

      manager()->OnTimeUpdate(GetPlayerId(), GetCurrentTime());
      main_loop_->PostTask(FROM_HERE, base::Bind(
          &MediaSourcePlayerGstreamer::OnTimeChanged, base::Unretained(this)));
    }

    // FIXME: Do we need to pull caps from PARSER? Can ignore if its of no use.
    break;
  case GST_MESSAGE_STATE_CHANGED:
    if (strcmp(kPipelineName, GST_MESSAGE_SRC_NAME(message)))
      break;
    main_loop_->PostTask(FROM_HERE, base::Bind(
        &MediaSourcePlayerGstreamer::OnUpdateStates, base::Unretained(this)));
    break;
  case GST_MESSAGE_BUFFERING:
    int buffered;
    gst_message_parse_buffering(message, &buffered);

    if (audio_queue_ && GST_MESSAGE_SRC(message) == GST_OBJECT(audio_queue_))
      audio_buffered_ = buffered;
    if (video_queue_ && GST_MESSAGE_SRC(message) == GST_OBJECT(video_queue_))
      video_buffered_ = buffered;

    if (!is_paused_due_underflow_ && playing_) {
      if (audio_buffered_ < 100 || video_buffered_ < 100) {
        main_loop_->PostTask(FROM_HERE, base::Bind(
            &MediaSourcePlayerGstreamer::HandleBufferingMessage,
            base::Unretained(this)));
      }
    } else if (is_paused_due_underflow_ && playing_) {
      if ((!HasAudio() || audio_buffered_ == 100) &&
          (!HasVideo() || video_buffered_ == 100)) {
        main_loop_->PostTask(FROM_HERE, base::Bind(
            &MediaSourcePlayerGstreamer::HandleBufferingMessage,
            base::Unretained(this)));
      }
    }
    break;
  default:
    VLOG(1) << "Unhandled GStreamer message type: "
            << GST_MESSAGE_TYPE_NAME(message);
    break;
  }
}

void MediaSourcePlayerGstreamer::OnUpdateStates() {
  DCHECK(main_loop_->BelongsToCurrentThread());
  if (!pipeline_ || IsPlayerDestructing() || error_occured_)
    return;

  GstState state;
  GstState pending;
  GstStateChangeReturn ret = gst_element_get_state(
      pipeline_, &state, &pending, 250 * GST_NSECOND);

  VLOG(1) << "MediaSourcePlayerGstreamer::"
          << __FUNCTION__
          << " GstStateChangeReturn: "
          << gst_element_state_change_return_get_name(ret)
          << " state: "
          << gst_element_state_get_name(state)
          << " pending: "
          << gst_element_state_get_name(pending)
          << " ID " << GetPlayerId();

  // FIXME: Handle all state changes
  switch (ret) {
  case GST_STATE_CHANGE_SUCCESS:
    switch (state) {
    case GST_STATE_PAUSED:
      manager()->OnReadyStateChange(
          GetPlayerId(), MediaPlayerTizen::ReadyStateHaveEnoughData);
      break;
    default:
      VLOG(1) << "GStreamer unhandled state "
              << gst_element_state_get_name(state);
      break;
    }
    break;
  case GST_STATE_CHANGE_FAILURE:
    LOG(ERROR) << "Failure: State: "
               << gst_element_state_get_name(state)
               << " pending: "
               << gst_element_state_get_name(pending);
    HandleError(MediaPlayerTizen::NetworkStateDecodeError);
    break;
  case GST_STATE_CHANGE_NO_PREROLL:

    break;
  default:
    VLOG(1) << "Unhandled return type: " << ret;
    break;
  }
}

void MediaSourcePlayerGstreamer::HandleBufferingMessage() {
  if (IsPlayerDestructing())
    return;
  if (!is_paused_due_underflow_ &&
      (audio_buffered_ < 100 || video_buffered_ < 100)) {
    is_paused_due_underflow_ = true;
    Pause(true);
    manager()->OnReadyStateChange(GetPlayerId(),
        MediaPlayerTizen::ReadyStateHaveCurrentData);
    manager()->OnNetworkStateChange(GetPlayerId(),
        MediaPlayerTizen::NetworkStateLoading);
  } else if (is_paused_due_underflow_ &&
      (!HasAudio() || audio_buffered_ == 100) &&
      (!HasVideo() || video_buffered_ == 100)) {
    is_paused_due_underflow_ = false;
    Play();
    manager()->OnReadyStateChange(GetPlayerId(),
        MediaPlayerTizen::ReadyStateHaveEnoughData);
    manager()->OnNetworkStateChange(GetPlayerId(),
        MediaPlayerTizen::NetworkStateLoaded);
  }
}

void MediaSourcePlayerGstreamer::OnPlaybackComplete() {
  // GStreamer pipeline EOS time and media duration doesnt match.
  double time = GetCurrentTime() != duration_ ? duration_ : GetCurrentTime();
  is_end_reached_ = true;
  is_download_finished_ = false;
  StopCurrentTimeUpdateTimer();
  manager()->OnTimeUpdate(GetPlayerId(), time);
  manager()->OnTimeChanged(GetPlayerId());

#if defined(OS_TIZEN_MOBILE) && (defined(TIZEN_V_2_3) || defined(TIZEN_V_2_4))
  if (device_power_release_lock(POWER_LOCK_DISPLAY) != DEVICE_ERROR_NONE)
    LOG(ERROR) << "|device_power_release_lock| request failed";
#endif

}

void MediaSourcePlayerGstreamer::UpdateSeekState(bool state) {
  manager()->OnSeekStateChange(GetPlayerId(), state);
  is_seeking_ = state;
}

void MediaSourcePlayerGstreamer::OnTimeChanged() {
  VLOG(1) << "OnTimeChanged" << "Player ID : " << GetPlayerId();
  DCHECK(main_loop_->BelongsToCurrentThread());
  manager()->OnTimeChanged(GetPlayerId());
}

void MediaSourcePlayerGstreamer::HandleError(
    media::MediaPlayerTizen::NetworkState state) {
  error_occured_ = true;
  manager()->OnNetworkStateChange(GetPlayerId(), state);

#if defined(OS_TIZEN_MOBILE) && (defined(TIZEN_V_2_3) || defined(TIZEN_V_2_4))
  if (device_power_release_lock(POWER_LOCK_DISPLAY) != DEVICE_ERROR_NONE)
    LOG(ERROR) << "|device_power_release_lock| request failed";
#endif

}

void MediaSourcePlayerGstreamer::ReleaseAudioElements() {
  if (audio_appsrc_) {
    gst_object_unref(audio_appsrc_);
    audio_appsrc_ = NULL;
  }

  if (audio_queue_) {
      gst_object_unref(audio_queue_);
      audio_queue_ = NULL;
  }

  if (audio_parse_) {
      gst_object_unref(audio_parse_);
      audio_parse_ = NULL;
  }

  if (audio_decoder_) {
      gst_object_unref(audio_decoder_);
      audio_decoder_ = NULL;
  }

  if (audio_convert_) {
      gst_object_unref(audio_convert_);
      audio_convert_ = NULL;
  }

  if (audio_resampler_) {
      gst_object_unref(audio_resampler_);
      audio_resampler_ = NULL;
  }

  if (audio_volume_) {
      gst_object_unref(audio_volume_);
      audio_volume_ = NULL;
  }

  if (audio_sink_) {
    gst_object_unref(audio_sink_);
    audio_sink_ = NULL;
  }
}

void MediaSourcePlayerGstreamer::ReleaseVideoElements() {
  if(video_appsrc_) {
    gst_object_unref(video_appsrc_);
    video_appsrc_ = NULL;
  }

  if(video_parse_) {
    gst_object_unref(video_parse_);
    video_parse_ = NULL;
  }

  if(video_queue_) {
    gst_object_unref(video_queue_);
    video_queue_ = NULL;
  }

  if(video_decoder_) {
    gst_object_unref(video_decoder_);
    video_decoder_ = NULL;
  }

  if(video_sink_) {
    gst_object_unref(video_sink_);
    video_sink_ = NULL;
  }
}

}  // namespace media
