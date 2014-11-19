// Copyright 2014 Samsung Electronics Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/base/tizen/media_player_bridge_gstreamer.h"

#include <gst/app/gstappsink.h>
#include <gst/pbutils/install-plugins.h>
#include <gst/pbutils/missing-plugins.h>

#if defined(TIZEN_MULTIMEDIA_PIXMAP_SUPPORT)
#include <gst/interfaces/xoverlay.h>
#include <gst/video/gstvideosink.h>
#include <gst/video/video.h>
#endif

#include "base/basictypes.h"
#include "base/message_loop/message_loop_proxy.h"
#include "media/base/tizen/media_player_manager_tizen.h"
#include "ui/gfx/size.h"

#if defined(OS_TIZEN_MOBILE) && defined(TIZEN_V_2_3)
#include <device/power.h>
#endif

namespace {

// fourcc for gst-video-format
const uint32 GST_VIDEO_SN12 = GST_MAKE_FOURCC('S','N','1','2');

// tile size for SN12
const uint SN12_TILE_WIDTH = 64;
const uint SN12_TILE_HEIGHT = 32;
const uint SN12_TILE_SIZE = SN12_TILE_WIDTH * SN12_TILE_HEIGHT;

// GstPlayFlags in |gstplay-enum.h|
typedef enum {
  GST_PLAY_FLAG_VIDEO             = (1 << 0),
  GST_PLAY_FLAG_AUDIO             = (1 << 1),
  GST_PLAY_FLAG_TEXT              = (1 << 2),
  GST_PLAY_FLAG_VIS               = (1 << 3),
  GST_PLAY_FLAG_SOFT_VOLUME       = (1 << 4),
  GST_PLAY_FLAG_NATIVE_AUDIO      = (1 << 5),
  GST_PLAY_FLAG_NATIVE_VIDEO      = (1 << 6),
  GST_PLAY_FLAG_DOWNLOAD          = (1 << 7),
  GST_PLAY_FLAG_BUFFERING         = (1 << 8),
  GST_PLAY_FLAG_DEINTERLACE       = (1 << 9),
  GST_PLAY_FLAG_SOFT_COLORBALANCE = (1 << 10)
} GstPlayFlags;

#if defined(TIZEN_MULTIMEDIA_PIXMAP_SUPPORT)
typedef enum {
  DEGREE_0, // No rotate
  DEGREE_90, // Rotate 90 degree count clockwise
  DEGREE_180, // Rotate 180 degree count clockwise
  DEGREE_270 // Rotate 270 degree count clockwise
} RotateAngle;
#endif

// Playbin element name
const char* kPlaybinName = "gst_playbin";
const char* kVideoSink = "gst_video_sink";
const char* kPropertyBufferSize = "buffer-size";
const char* kPropertyMaxBuffers = "max-buffers";
const char* kPropertyVolume = "volume";
const char* kPropertyUri = "uri";
const char* kPropertyAudioStream = "n-audio";
const char* kPropertyVideoStream = "n-video";
const char* kPropertyTextStream = "n-text";

// Update duration every 100ms.
const int kDurationUpdateInterval = 100;
const int kMaxBuffer = 1;

// buffer size for audio/video buffering
const int kPreloadBufferSize = (3 * 1024 * 1024);  // 3MB

const GstClockTime ConvertToGstClockTime(double time) {
  if (time < 0) {
    LOG(ERROR) << "Invalid time:" << time << " Reset to 0";
    time = 0;
  }

  // Extract the integer part of the time (seconds) and the fractional part
  // (microseconds). Attempt to round the microseconds so no floating point
  // precision is lost and we can perform an accurate seek.
  double seconds = 0;
  double microSeconds = std::modf(time, &seconds) * 1000000;
  GTimeVal timeValue;
  timeValue.tv_sec = static_cast<glong>(seconds);
  timeValue.tv_usec = static_cast<glong>(lround(microSeconds / 10) * 10);
  return GST_TIMEVAL_TO_TIME(timeValue);
}

const double ConvertNanoSecondsToSeconds(int64 time) {
  double seconds = static_cast<double>(time) /
      (base::Time::kNanosecondsPerSecond);
  return seconds;
}

}  // namespace

namespace media {

#if defined(TIZEN_MULTIMEDIA_PIXMAP_SUPPORT)
static Eina_Bool notify_damage_updated_cb(void* data, int type, void* event) {
  MediaPlayerBridgeGstreamer* player =
      static_cast <MediaPlayerBridgeGstreamer*>(data);
  if (!player)
    return ECORE_CALLBACK_PASS_ON;

  player->PlatformSurfaceUpdated();
  return ECORE_CALLBACK_PASS_ON;
}

static int get_pixmap_id_cb(void* data) {
  MediaPlayerBridgeGstreamer* player =
      static_cast <MediaPlayerBridgeGstreamer*>(data);
  return player->GetSurfaceID();
}
#endif

static GstBusSyncReply gst_pipeline_message_cb(
    GstBus* bus,
    GstMessage* message,
    gpointer user_data) {
  MediaPlayerBridgeGstreamer* player =
      static_cast<MediaPlayerBridgeGstreamer*>(user_data);
  if (!player)
    return GST_BUS_PASS;

  player->HandleMessage(message);
  gst_message_unref(message);
  return GST_BUS_DROP;
}

static void on_gst_appsink_eos(
    GstAppSink* appsink,
    gpointer user_data) {
  // EOS is Handled in state handling. Do nothing.
}

// Audio part of GStreamer is not yet implemented. Workaround to handle
// |PREROLLED| for audio files is to update based on |GST_MESSAGE_BUFFERING|.
static GstFlowReturn on_gst_appsink_preroll(
    GstAppSink* sink,
    gpointer user_data) {
  MediaPlayerBridgeGstreamer* player =
      static_cast<MediaPlayerBridgeGstreamer*>(user_data);
  if (!player)
    return GST_FLOW_ERROR;
  player->PrerollComplete();
  return GST_FLOW_OK;
}

static GstFlowReturn on_gst_appsink_buffer(
    GstAppSink* sink,
    gpointer user_data) {
  MediaPlayerBridgeGstreamer* player =
      static_cast<MediaPlayerBridgeGstreamer*>(user_data);
  if (!player)
    return GST_FLOW_ERROR;

  player->BufferReady(player->PullBuffer());
  return GST_FLOW_OK;
}

static void on_gst_installer_result_function(
    GstInstallPluginsReturn result,
    gpointer userData) {
  MediaPlayerBridgeGstreamer* player =
      reinterpret_cast<MediaPlayerBridgeGstreamer*>(userData);
  player->HandlePluginInstallerResult(result);
}

MediaPlayerBridgeGstreamer::MediaPlayerBridgeGstreamer(
    int player_id,
    const GURL& url,
    double volume,
    MediaPlayerManager* manager_in)
    : MediaPlayerTizen(player_id, manager_in),
      main_loop_(base::MessageLoopProxy::current()),
      gst_playbin_(NULL),
      gst_appsink_(NULL),
      url_(url),
      volume_(volume),
      gst_width_(0),
      gst_height_(0),
      is_prerolled_(false),
      is_paused_(true),
      duration_(0),
      playback_rate_(1.0f),
      buffered_(0),
      video_format_(0),
      media_type_(0),
      is_live_stream_(false),
      is_file_url_(false),
      is_end_reached_(false),
      is_seeking_(false),
      is_seek_pending_(false),
      seek_duration_(0),
      error_occured_(false),
      missing_plugins_(false),
      is_pixmap_used_(false),
#if defined(TIZEN_MULTIMEDIA_PIXMAP_SUPPORT)
      pixmap_id_(0),
      efl_pixmap_(NULL),
      m_damage(0),
      m_damageHandler(NULL),
#endif
      bufsize_sn12_(0),
      shared_memory_size(0) {
  LOG(INFO) << "MediaPlayerBridgeGstreamer - URL = " << url_.spec().c_str();

  // gstreamer port
  if (!gst_is_initialized()) {
    gst_init_check(NULL, NULL, 0);
  }

  if (gst_is_initialized()) {
    gst_playbin_ = gst_element_factory_make("playbin2", kPlaybinName);
    gst_appsink_ = GetVideoSink();
    if (gst_playbin_ && gst_appsink_) {
      g_object_set(gst_playbin_, "video-sink", gst_appsink_, NULL);

    // QoS property will enable the quality-of-service features of the
    // basesink which gather statistics about the real-time performance
    // of the clock synchronisation. For each buffer received in the sink,
    // statistics are gathered and a QOS event is sent upstream with
    // these numbers. This information can then be used by upstream
    // elements to reduce their processing rate, for example.
    if(!gst_base_sink_is_qos_enabled(GST_BASE_SINK(gst_appsink_)))
      gst_base_sink_set_qos_enabled(GST_BASE_SINK(gst_appsink_), true);

      GstBus* bus = gst_pipeline_get_bus(GST_PIPELINE(gst_playbin_));
      if (!bus) {
        LOG(ERROR) << "GStreamer bus creation failed";
        HandleError(MediaPlayerTizen::NetworkStateDecodeError);
        return;
      }
      gst_bus_set_sync_handler(
          bus, (GstBusSyncHandler)gst_pipeline_message_cb, this);
      gst_object_unref (bus);

#if defined(TIZEN_MULTIMEDIA_PIXMAP_SUPPORT)
      PrepareForVideoSink();
#else
      PrepareForVideoFrame();
#endif

      g_object_set(G_OBJECT(gst_playbin_), kPropertyVolume, volume_, NULL);
      g_object_set(
          G_OBJECT(gst_playbin_), kPropertyUri, url_.spec().c_str(), NULL);

    if (gst_element_set_state(
        gst_playbin_, GST_STATE_READY) == GST_STATE_CHANGE_FAILURE)
      LOG(ERROR) << "GStreamer state change failed";

    manager()->OnReadyStateChange(
        GetPlayerId(), MediaPlayerTizen::ReadyStateHaveEnoughData);
    manager()->OnNetworkStateChange(
        GetPlayerId(), MediaPlayerTizen::NetworkStateLoaded);

      if(url_.SchemeIsFile())
        is_file_url_ = true;
    } else {
      if (gst_playbin_) {
        gst_object_unref(gst_playbin_);
        gst_playbin_ = NULL;
      }
      if(gst_appsink_) {
        gst_object_unref(gst_appsink_);
        gst_appsink_ = NULL;
      }
      LOG(ERROR) << "Unable to create GStreamer elements";
      HandleError(MediaPlayerTizen::NetworkStateDecodeError);
    }
  } else {
    LOG(ERROR) << "Unable to initialize GST";
    HandleError(MediaPlayerTizen::NetworkStateDecodeError);
  }
}

MediaPlayerBridgeGstreamer::~MediaPlayerBridgeGstreamer() {
  VLOG(1) << __FUNCTION__ << " : Player Id = " << GetPlayerId();
}

void MediaPlayerBridgeGstreamer::Destroy() {
  VLOG(1) << __FUNCTION__ << " : Player Id = " << GetPlayerId();
  if (IsPlayerDestructing())
    return;

  destructing_ = true;
  Release();
  main_loop_->DeleteSoon(FROM_HERE, this);
}

void MediaPlayerBridgeGstreamer::Play() {
  VLOG(1) << __FUNCTION__ << " : Player Id = " << GetPlayerId();
  if (!gst_playbin_)
    return;
  if (error_occured_ || is_end_reached_)
    return;
  if (playback_rate_ == 0.0) {
    is_paused_ = false;
    return;
  }
  if (gst_element_set_state(
      gst_playbin_, GST_STATE_PLAYING) == GST_STATE_CHANGE_FAILURE) {
    LOG(ERROR) << "GStreamer state change failed in PLAY";
    return;
  }

#if defined(OS_TIZEN_MOBILE) && defined(TIZEN_V_2_3)
  if (device_power_request_lock(POWER_LOCK_DISPLAY, 0) != DEVICE_ERROR_NONE)
    LOG(ERROR) << "|device_power_request_lock| request failed";
#endif

  StartCurrentTimeUpdateTimer();
  is_paused_ = false;
  is_end_reached_ = false;
}

void MediaPlayerBridgeGstreamer::Pause(bool is_media_related_action) {
  VLOG(1) << __FUNCTION__ << " : Player Id = " << GetPlayerId();
  if (!gst_playbin_)
    return;
  if (error_occured_)
    return;
  if (gst_element_set_state(
      gst_playbin_, GST_STATE_PAUSED) == GST_STATE_CHANGE_FAILURE) {
    LOG(ERROR) << "GStreamer state change failed in PAUSE";
    return;
  }

#if defined(OS_TIZEN_MOBILE) && defined(TIZEN_V_2_3)
  if (device_power_release_lock(POWER_LOCK_DISPLAY) != DEVICE_ERROR_NONE)
    LOG(ERROR) << "|device_power_release_lock| request failed";
#endif

  StopCurrentTimeUpdateTimer();
  is_paused_ = true;
}

void MediaPlayerBridgeGstreamer::SetRate(double rate) {
  VLOG(1) << __FUNCTION__ << "Rate (" << rate << ")";
  if (!gst_playbin_)
    return;
  if (error_occured_)
    return;
  if (playback_rate_ == rate)
    return;
  if (is_live_stream_)
    return;

  GstState state = GST_STATE_NULL;
  GstState pending = GST_STATE_NULL;
  gst_element_get_state(gst_playbin_, &state, &pending, 0);
  if ((state != GST_STATE_PLAYING && state != GST_STATE_PAUSED)
      || (pending == GST_STATE_PAUSED))
    return;
  if (rate == 0.0) {
    playback_rate_ = rate;
    Pause(true);
    return;
  }

  // If rate was zero and requested rate is non-zero, change the paused state
  if(playback_rate_ == 0.0 && rate != 0.0) {
    Play();
    StartCurrentTimeUpdateTimer();
  }

  double current_position = GetCurrentTime() * GST_SECOND;

  if (rate < 0 && current_position == 0.0f)
    current_position = -1.0f;
  if (SeekTo(
      current_position, rate,
      static_cast<GstSeekFlags>(GST_SEEK_FLAG_FLUSH))) {

    // FIXME: Is is required to mute at abnormal playback rate?
    playback_rate_ = rate;
  } else {
    LOG(ERROR) << "Setting Rate " << rate << " failed";
    HandleError(MediaPlayerTizen::NetworkStateDecodeError);
  }
}

void MediaPlayerBridgeGstreamer::Seek(const double time) {
  VLOG(1) << __FUNCTION__ << "Time (" << time<< ")";

  if (IsPlayerDestructing())
    return;

  if (!gst_playbin_ || error_occured_ || is_live_stream_ ||
      time == GetCurrentTime()) {
    main_loop_->PostTask(FROM_HERE, base::Bind(
        &MediaPlayerBridgeGstreamer::OnTimeChanged, base::Unretained(this)));
    return;
  }

  GstState state = GST_STATE_NULL;
  GstStateChangeReturn ret = gst_element_get_state(
      gst_playbin_, &state, NULL, 250 * GST_NSECOND);
  if (ret == GST_STATE_CHANGE_FAILURE || ret == GST_STATE_CHANGE_NO_PREROLL) {
    LOG(ERROR) << "Cannot seek in "
               << gst_element_state_change_return_get_name(ret) << " state";
    return;
  }

  StopCurrentTimeUpdateTimer();
  if (SeekTo(
      ConvertToGstClockTime(time), playback_rate_, static_cast<GstSeekFlags>
      (GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_ACCURATE))) {
    UpdateSeekState(true);
    seek_duration_ = time;
    is_end_reached_ = time != duration_ ? false : true;
    manager()->OnTimeUpdate(GetPlayerId(), time);
    if (!is_paused_)
      StartCurrentTimeUpdateTimer();
  } else {
    LOG(ERROR) << "MediaPlayerBridgeGstreamer::" << __FUNCTION__<<": Failed!";
    manager()->OnTimeUpdate(GetPlayerId(), GetCurrentTime());
    manager()->OnTimeChanged(GetPlayerId());
  }
}

bool MediaPlayerBridgeGstreamer::SeekTo(
    gint64 position,
    float rate,
    GstSeekFlags seekType) {
  gint64 startTime = 0, endTime = 0;
  if (rate > 0) {
    startTime = position;
    endTime = GST_CLOCK_TIME_NONE;
  } else {
    startTime = 0;
    // If we are at beginning of media, start from the end to
    // avoid immediate EOS.
    if (position < 0)
      endTime = static_cast<gint64>(GetCurrentTime() * GST_SECOND);
    else
      endTime = position;
  }

  return gst_element_seek(gst_playbin_, rate, GST_FORMAT_TIME, seekType,
      GST_SEEK_TYPE_SET, startTime, GST_SEEK_TYPE_SET, endTime);
}

void MediaPlayerBridgeGstreamer::Release() {
  VLOG(1) << __FUNCTION__ << " : Player Id = " << GetPlayerId();
  StopCurrentTimeUpdateTimer();
  StopBufferingUpdateTimer();
  if (gst_playbin_) {
    GstBus* bus = gst_pipeline_get_bus(GST_PIPELINE(gst_playbin_));
    if (bus) {
      g_signal_handlers_disconnect_by_func(
          bus, reinterpret_cast<gpointer>(gst_pipeline_message_cb), this);
      gst_bus_set_sync_handler(bus, NULL, NULL);
      gst_object_unref(bus);
    }

    gst_element_set_state(gst_playbin_, GST_STATE_NULL);
    gst_object_unref(gst_playbin_);
    gst_playbin_ = NULL;
    gst_appsink_ = NULL;
  }
#if defined(TIZEN_MULTIMEDIA_PIXMAP_SUPPORT)
  if (m_damage) {
    ecore_x_damage_free(m_damage);
    m_damage = 0;
  }
  if (m_damageHandler) {
    ecore_event_handler_del(m_damageHandler);
    m_damageHandler = NULL;
  }
  if (efl_pixmap_.get()) {
    efl_pixmap_ = NULL;
  }
#endif
}

void MediaPlayerBridgeGstreamer::SetVolume(double volume) {
  g_object_set(G_OBJECT(gst_playbin_), kPropertyVolume, volume, NULL);
}

void MediaPlayerBridgeGstreamer::UpdateMediaType() {
  int audio_stream_count = 0;
  int video_stream_count = 0;
  int text_stream_count = 0;

  g_object_get(
      G_OBJECT(gst_playbin_), kPropertyAudioStream, &audio_stream_count, NULL);
  g_object_get(
      G_OBJECT(gst_playbin_), kPropertyVideoStream, &video_stream_count, NULL);
  g_object_get(
      G_OBJECT(gst_playbin_), kPropertyTextStream, &text_stream_count, NULL);
  media_type_ = ((audio_stream_count ? MEDIA_AUDIO_MASK : 0) |
      (video_stream_count ? MEDIA_VIDEO_MASK : 0));

  // For Video-Sink Implementation we won't be getting Prepare-xid if we have
  // only audio.
  if (is_pixmap_used_ && video_stream_count == 0)
    manager()->OnMediaDataChange(
      GetPlayerId(), video_format_, gst_height_, gst_width_, media_type_);
}

void MediaPlayerBridgeGstreamer::UpdateDuration() {
  if (error_occured_)
    return;

  gint64 duration = 0;
  GstFormat format = GST_FORMAT_TIME;
  gst_element_query_duration(gst_playbin_, &format, &duration);
  duration_ = ConvertNanoSecondsToSeconds(duration);
  manager()->OnDurationChange(GetPlayerId(), duration_);
  // No need to buffer 'local file'. Update buffered percentage.
  if(is_file_url_) {
    std::vector<media::MediaPlayerTizen::TimeRanges> buffer_range;
    media::MediaPlayerTizen::TimeRanges range;
    range.start = 0;
    range.end = duration_ * base::Time::kMicrosecondsPerSecond;
    buffer_range.push_back(range);
    manager()->OnBufferUpdate(GetPlayerId(), buffer_range);
  }
}

double MediaPlayerBridgeGstreamer::GetCurrentTime() {
  if (error_occured_)
    return 0.0;

  gint64 current_time = 0;
  GstFormat format = GST_FORMAT_TIME;

  if (is_end_reached_) {
    // Position queries on a null pipeline return 0. If we're at
    // the end of the stream the pipeline is null but we want to
    // report either the seek time or the duration because this is
    // what the Media element spec expects us to do.
    if (is_seeking_)
      return seek_duration_;

    // Workaround for
    // https://bugzilla.gnome.org/show_bug.cgi?id=639941 In GStreamer
    // 0.10.35 basesink reports wrong duration in case of EOS and
    // negative playback rate. There's no upstream accepted patch for
    // this bug yet, hence this temporary workaround.
    if (playback_rate_ < 0)
      return 0.0f;
    if (duration_)
      return duration_;

    // FIXME: Should null be sent here?
  }

  gst_element_query_position(gst_playbin_, &format, &current_time);
  return ConvertNanoSecondsToSeconds(current_time);
}

void MediaPlayerBridgeGstreamer::OnCurrentTimeUpdateTimerFired() {
  manager()->OnTimeUpdate(GetPlayerId(), GetCurrentTime());
}

void MediaPlayerBridgeGstreamer::StartCurrentTimeUpdateTimer() {
  if (!current_time_update_timer_.IsRunning()) {
    current_time_update_timer_.Start(
        FROM_HERE,
        base::TimeDelta::FromMilliseconds(kDurationUpdateInterval),
        this, &MediaPlayerBridgeGstreamer::OnCurrentTimeUpdateTimerFired);
  }
}

void MediaPlayerBridgeGstreamer::StopCurrentTimeUpdateTimer() {
  if (current_time_update_timer_.IsRunning())
    current_time_update_timer_.Stop();
}

void MediaPlayerBridgeGstreamer::OnBufferingUpdateTimerFired() {
  if (IsPlayerDestructing())
    return;
  GetBufferedTimeRanges();
}

void MediaPlayerBridgeGstreamer::StartBufferingUpdateTimer() {
  if (!buffering_update_timer_.IsRunning()) {
    buffering_update_timer_.Start(
        FROM_HERE,
        base::TimeDelta::FromMilliseconds(kDurationUpdateInterval),
        this, &MediaPlayerBridgeGstreamer::OnBufferingUpdateTimerFired);
  }
}

void MediaPlayerBridgeGstreamer::StopBufferingUpdateTimer() {
  if (buffering_update_timer_.IsRunning())
    buffering_update_timer_.Stop();
}

GstBuffer* MediaPlayerBridgeGstreamer::PullBuffer() {
  return gst_app_sink_pull_buffer(GST_APP_SINK(gst_appsink_));
}

void MediaPlayerBridgeGstreamer::BufferReady(
    const GstBuffer* buffer) {
  if (!GST_BUFFER_DATA(buffer) || !GST_BUFFER_SIZE(buffer))
    return;

  if (!gst_width_ || !gst_height_)
    GetFrameDetails();
  // FIXME: Cross check the end results.

  base::TimeDelta timestamp = base::TimeDelta::FromMicroseconds(
      GST_BUFFER_TIMESTAMP(buffer) / base::Time::kNanosecondsPerMicrosecond);

  uint8* buffer_data = GST_BUFFER_DATA(buffer);
  gsize buffer_size = GST_BUFFER_SIZE(buffer);
  if (video_format_ == GST_VIDEO_SN12) {
    shared_memory_size = (bufsize_sn12_);
  } else {
    shared_memory_size = (buffer_size);
  }

  if (!shared_memory.CreateAndMapAnonymous(shared_memory_size)) {
    LOG (ERROR) << "Shared Memory creation failed.";
    gst_buffer_unref(GST_BUFFER(buffer));
    return;
  }
  if (!shared_memory.ShareToProcess(
      base::Process::Current().Handle(), &foreign_memory_handle)) {
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

// Updates networkState and ReadyState based on buffering percentage.
void MediaPlayerBridgeGstreamer::ProcessBufferingStats(GstMessage* message) {
  if (IsPlayerDestructing())
    return;

  gst_message_parse_buffering(message, &buffered_);
    main_loop_->PostTask(FROM_HERE, base::Bind(
        &MediaPlayerBridgeGstreamer::OnUpdateStates, base::Unretained(this)));
}

// To Update Buffered Ranges for Media Playback
void MediaPlayerBridgeGstreamer::GetBufferedTimeRanges() {
  GstQuery *query = NULL;
  gboolean result = false;
  media::Ranges<base::TimeDelta> time_ranges;
  std::vector<media::MediaPlayerTizen::TimeRanges> buffer_range;
  query = gst_query_new_buffering (GST_FORMAT_PERCENT);
  result = gst_element_query(gst_playbin_, query);
  if (result) {
    gint n_ranges = 0, range = 0;
    n_ranges = gst_query_get_n_buffering_ranges(query);
    for (range = 0; range < n_ranges; range++) {
      gint64 start = 0, stop = 0;
      gst_query_parse_nth_buffering_range(query, range, &start, &stop);
      media::MediaPlayerTizen::TimeRanges b_range;
      // Stop the Timer on Buffer completion
      if(start == 0 && stop == 100)
        StopBufferingUpdateTimer();
      // Parsed value is in percentage. Converted into time range
      b_range.start = static_cast<double>(start) * duration_ / 100
          * base::Time::kMicrosecondsPerSecond;
      b_range.end = static_cast<double>(stop) * duration_ / 100
          * base::Time::kMicrosecondsPerSecond;
      buffer_range.push_back(b_range);
    }
    manager()->OnBufferUpdate(GetPlayerId(), buffer_range);
  }
}

void MediaPlayerBridgeGstreamer::HandleMessage(GstMessage* message) {
  if (IsPlayerDestructing())
      return;

  if (!strcmp(kPlaybinName, GST_MESSAGE_SRC_NAME(message)))
    VLOG(1) << "MediaPlayerBridgeGstreamer::" << __FUNCTION__
              << " received from element " << GST_MESSAGE_SRC_NAME(message)
              << " ID " << GetPlayerId();

  const GstStructure* structure = gst_message_get_structure(message);
  if (structure) {
    const gchar* messageTypeName = gst_structure_get_name(structure);

    // Redirect messages are sent from elements, like qtdemux, to
    // notify of the new location(s) of the media.
    if (!g_strcmp0(messageTypeName, "redirect")) {
      // FIXME: for changing media location
      return;
    }
  }
#if defined(TIZEN_MULTIMEDIA_PIXMAP_SUPPORT)
  if (!IsXWindowHadleSet()) {
    if (message->structure &&
        gst_structure_has_name(message->structure, "prepare-xid")) {
      VLOG(1) << "Received message : prepare-xid";
      XWindowIdPrepared(message);
      gst_message_unref(message);
      return;
    }
  }
#endif
  //FIXME: Add and handle all conditions
  switch (GST_MESSAGE_TYPE(message)) {
  case GST_MESSAGE_ERROR:
    if (missing_plugins_)
      break;

    StopBufferingUpdateTimer();
    GError* error;
    gst_message_parse_error(message, &error, NULL);
    MediaPlayerTizen::NetworkState network_state_error;
    network_state_error = MediaPlayerTizen::NetworkStateEmpty;
    if (error->code == GST_STREAM_ERROR_CODEC_NOT_FOUND
        || error->code == GST_STREAM_ERROR_WRONG_TYPE
        || error->code == GST_STREAM_ERROR_FAILED
        || error->code == GST_CORE_ERROR_MISSING_PLUGIN
        || error->code == GST_RESOURCE_ERROR_NOT_FOUND) {
      network_state_error = MediaPlayerTizen::NetworkStateFormatError;
    } else if (error->domain == GST_RESOURCE_ERROR) {
      network_state_error = MediaPlayerTizen::NetworkStateNetworkError;
    } else {
      network_state_error = MediaPlayerTizen::NetworkStateDecodeError;
    }

    LOG(ERROR) << "Error Message : " << error->message << " Recieved From : "
               << GST_MESSAGE_SRC_NAME(message)
               << ", and Blink Error Code  = " << network_state_error;
    g_error_free(error);

    HandleError(network_state_error);
    break;
  case GST_MESSAGE_EOS:
    VLOG(1) << "Got GST_MESSAGE_EOS";
    main_loop_->PostTask(FROM_HERE, base::Bind(
      &MediaPlayerBridgeGstreamer::OnPlaybackComplete, base::Unretained(this)));
    break;
  case GST_MESSAGE_ASYNC_DONE:
    if (is_seeking_) {
      UpdateSeekState(false);
      main_loop_->PostTask(FROM_HERE, base::Bind(
          &MediaPlayerBridgeGstreamer::OnTimeChanged, base::Unretained(this)));
    }

    // Without |audio-sink| for audio tracks no preroll message is received. To
    // update track details |PrerollComplete| is called. For Video tracks,
    // preroll message is received long before control reaches here.
    if (!is_prerolled_)
      PrerollComplete();
    break;
  case GST_MESSAGE_STATE_CHANGED:
    if (strcmp(kPlaybinName, GST_MESSAGE_SRC_NAME(message)))
      break;

    main_loop_->PostTask(FROM_HERE, base::Bind(
        &MediaPlayerBridgeGstreamer::OnUpdateStates, base::Unretained(this)));
    break;
  case GST_MESSAGE_BUFFERING:
      ProcessBufferingStats(message);
    break;
  case GST_MESSAGE_ELEMENT:
    if (gst_is_missing_plugin_message(message)) {
      gchar* detail = gst_missing_plugin_message_get_installer_detail(message);
      gchar* detailArray[2] = {detail, 0};
      GstInstallPluginsReturn result = gst_install_plugins_async(
          detailArray, 0, on_gst_installer_result_function, this);
      missing_plugins_ = result == GST_INSTALL_PLUGINS_STARTED_OK;
      g_free(detail);
    }
    break;
  default:
    LOG(ERROR) << "Unhandled GStreamer message type: "
               << GST_MESSAGE_TYPE_NAME(message);
    break;
  }
}

void MediaPlayerBridgeGstreamer::HandlePluginInstallerResult(
    GstInstallPluginsReturn result) {
  missing_plugins_ = false;
  if (result == GST_INSTALL_PLUGINS_SUCCESS) {
    // FIXME: CAN CAUSE DEADLOCK?
    gst_element_set_state(gst_playbin_, GST_STATE_READY);
    gst_element_set_state(gst_playbin_, GST_STATE_PAUSED);
  } else {
    LOG(ERROR) << "GST Plugin Installation failed";
    HandleError(MediaPlayerTizen::NetworkStateDecodeError);
  }
}

void MediaPlayerBridgeGstreamer::UpdateStates() {
  if (IsPlayerDestructing())
    return;

  if (error_occured_)
    return;

  GstState state = GST_STATE_NULL;
  GstState pending = GST_STATE_NULL;
  GstStateChangeReturn ret = gst_element_get_state(
      gst_playbin_, &state, &pending, 250 * GST_NSECOND);

  VLOG(1) << "MediaPlayerBridgeGstreamer::"
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
    if (!duration_)
      UpdateDuration();

    switch (state) {
    case GST_STATE_NULL:
      manager()->OnReadyStateChange(
          GetPlayerId(), MediaPlayerTizen::ReadyStateHaveNothing);
      manager()->OnNetworkStateChange(
          GetPlayerId(), MediaPlayerTizen::NetworkStateEmpty);
      break;
    case GST_STATE_READY:
      manager()->OnReadyStateChange(
          GetPlayerId(), MediaPlayerTizen::ReadyStateHaveMetadata);
      manager()->OnNetworkStateChange(
          GetPlayerId(), MediaPlayerTizen::NetworkStateEmpty);
      break;
    case GST_STATE_PAUSED:
    case GST_STATE_PLAYING:
      if (!is_file_url_)
        StartBufferingUpdateTimer();

      VLOG(1) << "state " << gst_element_state_get_name(state)
              << " buffered_ " << buffered_
              << " is_file_url_ " << is_file_url_
              << " is_paused_ " << is_paused_
              << " is_seeking_ " << is_seeking_;

      if (buffered_ == 100 || is_file_url_) {
        if (state == GST_STATE_PAUSED && !is_paused_) {
          if (gst_element_set_state(gst_playbin_, GST_STATE_PLAYING)
              == GST_STATE_CHANGE_FAILURE)
            LOG(ERROR) << "GStreamer state change failed";
        }

        // FIXME: On rapid playback on PIANO Widget, there are instences
        // when 'ASYNC-DONE' message is not received on replay.
        if (is_seeking_ && state == GST_STATE_PLAYING) {
          LOG(ERROR) << "PLAYING state changed on seek";
          UpdateSeekState(false);
          main_loop_->PostTask(FROM_HERE,
              base::Bind(&MediaPlayerBridgeGstreamer::OnTimeChanged,
                  base::Unretained(this)));
        }
        manager()->OnReadyStateChange(
            GetPlayerId(), MediaPlayerTizen::ReadyStateHaveEnoughData);
        manager()->OnNetworkStateChange(
            GetPlayerId(), MediaPlayerTizen::NetworkStateLoaded);
      } else {
        if (state == GST_STATE_PLAYING) {
          if (gst_element_set_state(gst_playbin_, GST_STATE_PAUSED)
              == GST_STATE_CHANGE_FAILURE)
            LOG(ERROR) << "GStreamer state change failed";
          manager()->OnReadyStateChange(
              GetPlayerId(), MediaPlayerTizen::ReadyStateHaveCurrentData);
          manager()->OnNetworkStateChange(
              GetPlayerId(), MediaPlayerTizen::NetworkStateLoading);
        }
      }
      break;
    default:
      LOG(ERROR) << "GStreamer unhandled state "
                << gst_element_state_get_name(state);
      break;
    }
    break;
  case GST_STATE_CHANGE_ASYNC:
    // FIXME: For live stream.
    break;
  case GST_STATE_CHANGE_FAILURE:
    LOG(ERROR) << "Failure: State: "
               << gst_element_state_get_name(state)
               << " pending: "
               << gst_element_state_get_name(pending);
    HandleError(MediaPlayerTizen::NetworkStateDecodeError);
    break;
  case GST_STATE_CHANGE_NO_PREROLL:
    if (state == GST_STATE_READY) {
      manager()->OnReadyStateChange(
          GetPlayerId(), MediaPlayerTizen::ReadyStateHaveNothing);
    } else if (state == GST_STATE_PAUSED) {
      manager()->OnReadyStateChange(
          GetPlayerId(), MediaPlayerTizen::ReadyStateHaveEnoughData);
      is_paused_ = true;
      is_live_stream_ = true;
    } else if (state == GST_STATE_PLAYING) {
      is_paused_ = false;
    }
    manager()->OnNetworkStateChange(
        GetPlayerId(), MediaPlayerTizen::NetworkStateLoading);
    break;
  default:
    LOG(ERROR) << "Unhandled return type: " << ret;
    break;
  }
}

void MediaPlayerBridgeGstreamer::FrameReady(
  const scoped_refptr<media::VideoFrame>& frame) {
}

void MediaPlayerBridgeGstreamer::GetFrameDetails() {
  GstBuffer* buffer = gst_app_sink_pull_preroll(GST_APP_SINK(gst_appsink_));
  if (!buffer)
    return;

  GstCaps* caps = gst_buffer_get_caps(GST_BUFFER(buffer));
  if (!caps)
    return;

  const GstStructure* str = gst_caps_get_structure(caps, 0);
  gst_caps_unref(caps);
  if (!str)
    return;

  if (!gst_structure_get_int(str, "width", &gst_width_) ||
      !gst_structure_get_int(str, "height", &gst_height_) ||
      !gst_structure_get_fourcc(str, "format", &video_format_))
    LOG(ERROR) << "Pre-roll buffer info could not be obtained";

  // Need to update frame details before sending buffer.
  manager()->OnMediaDataChange(
      GetPlayerId(), static_cast<int>(video_format_),
      gst_height_, gst_width_, media_type_);

  if (video_format_ == GST_VIDEO_SN12) {
    uint tile_w_align = ((gst_width_ - 1) / SN12_TILE_WIDTH + 2) & ~1;
    bufsize_sn12_ = SN12_TILE_SIZE * tile_w_align * ((gst_height_ - 1) /
        SN12_TILE_HEIGHT + 1) + (((gst_height_ + 1) & ~1) / 2) * tile_w_align *
        SN12_TILE_WIDTH;
  }

  BufferReady(buffer);
}

// FIXME: Works well for video playback. Do the same for /consider audio
// prerolling.
void MediaPlayerBridgeGstreamer::PrerollComplete() {
  if(!is_pixmap_used_) {
    UpdateMediaType();
    if (media_type_ & MEDIA_VIDEO_MASK)
      GetFrameDetails();
    else
      manager()->OnMediaDataChange(
          GetPlayerId(), static_cast<int>(video_format_),
          gst_height_, gst_width_, media_type_);

    VLOG(1) << "MediaPlayerBridgeGstreamer::PrerollComplete."
            << " video_format " << video_format_
            << " width " << gst_width_
            << " height " << gst_height_
            << " media_type " << media_type_;
  } else {
    UpdateMediaType();
  }
  is_prerolled_  = true;
}

void MediaPlayerBridgeGstreamer::OnPlaybackComplete() {
  VLOG(1) << "OnPlaybackComplete" << " ID " << GetPlayerId();
  is_end_reached_ = true;

#if defined(OS_TIZEN_MOBILE) && defined(TIZEN_V_2_3)
  if (device_power_release_lock(POWER_LOCK_DISPLAY) != DEVICE_ERROR_NONE)
    LOG(ERROR) << "|device_power_release_lock| request failed";
#endif

  StopCurrentTimeUpdateTimer();
  manager()->OnTimeUpdate(GetPlayerId(), GetCurrentTime());
  manager()->OnTimeChanged(GetPlayerId());
}

void MediaPlayerBridgeGstreamer::UpdateSeekState(bool state) {
  is_seeking_ = state;
}

void MediaPlayerBridgeGstreamer::OnTimeChanged() {
  VLOG(1) << __FUNCTION__ << " : Player Id = " << GetPlayerId();
  DCHECK(main_loop_->BelongsToCurrentThread());
  manager()->OnTimeChanged(GetPlayerId());
}

void MediaPlayerBridgeGstreamer::OnUpdateStates() {
  VLOG(1) << __FUNCTION__ << " : Player Id = " << GetPlayerId();
  DCHECK(main_loop_->BelongsToCurrentThread());
  UpdateStates();
}

GstElement* MediaPlayerBridgeGstreamer::GetVideoSink() {
#if defined(TIZEN_MULTIMEDIA_PIXMAP_SUPPORT)
  is_pixmap_used_ = true;
  return gst_element_factory_make("xvimagesink", kVideoSink);
#else
  is_pixmap_used_ = false;
  return gst_element_factory_make("appsink", kVideoSink);
#endif
}

void MediaPlayerBridgeGstreamer::PrepareForVideoFrame() {
  VLOG(1) << "MediaElement using shared memory to pass the frames";
  int flags = 0;
  g_object_get(gst_playbin_, "flags", &flags, NULL);
  flags = GST_PLAY_FLAG_NATIVE_VIDEO | GST_PLAY_FLAG_NATIVE_AUDIO
      | GST_PLAY_FLAG_DOWNLOAD;
#ifdef OS_TIZEN_TV
  flags |= GST_PLAY_FLAG_NATIVE_VIDEO | GST_PLAY_FLAG_NATIVE_AUDIO;
  flags &= ~GST_PLAY_FLAG_VIDEO & ~GST_PLAY_FLAG_AUDIO &
           ~GST_PLAY_FLAG_DEINTERLACE & ~GST_PLAY_FLAG_SOFT_COLORBALANCE;
#endif
  g_object_set(gst_playbin_, "flags", flags, NULL);
  GstAppSinkCallbacks callbacks = {on_gst_appsink_eos,
      on_gst_appsink_preroll,
      on_gst_appsink_buffer,
      NULL, {NULL, NULL, NULL}};
  gst_app_sink_set_callbacks(GST_APP_SINK(gst_appsink_), &callbacks,
      this, NULL);
  g_object_set(G_OBJECT(gst_playbin_), kPropertyBufferSize,
      kPreloadBufferSize, NULL);
  g_object_set(G_OBJECT(gst_appsink_), kPropertyMaxBuffers,
      (guint)kMaxBuffer, NULL);
}

#if defined(TIZEN_MULTIMEDIA_PIXMAP_SUPPORT)
void MediaPlayerBridgeGstreamer::PrepareForVideoSink() {
  VLOG(1) << "MediaElement Backed by Video-Sink (xvimagesink)";
  int flags = 0;
  g_object_get(gst_playbin_, "flags", &flags, NULL);
  flags |= GST_PLAY_FLAG_NATIVE_VIDEO;
  flags &= ~GST_PLAY_FLAG_TEXT;
  g_object_set(gst_playbin_, "flags", flags, NULL);
}

void MediaPlayerBridgeGstreamer::XWindowIdPrepared(GstMessage* message) {
  // It is called just once after video src is set.
  // FIXME: video resolution change should be handled Separatly
  gst_structure_get_int(message->structure, "video-width", &gst_width_);
  gst_structure_get_int(message->structure, "video-height", &gst_height_);
  SetPixmap();
}

int MediaPlayerBridgeGstreamer::GetSurfaceID() const {
  return pixmap_id_;
}

void MediaPlayerBridgeGstreamer::SetPixmap() {
  efl_pixmap_ = gfx::EflPixmap::Create(gfx::EflPixmap::SURFACE, gfx::Size(
      gst_width_, gst_height_));
  if (!efl_pixmap_.get()) {
    LOG(ERROR) << "gfx::EflPixmap::Create() failed to create Pixmap";
    return ;
  }
  pixmap_id_ = efl_pixmap_->GetId();
  g_object_set (gst_appsink_, "pixmap-id-callback", get_pixmap_id_cb, NULL);
  g_object_set (gst_appsink_, "pixmap-id-callback-userdata", this, NULL);

  //Register to get notification from ecore for damage updates.
  m_damage = ecore_x_damage_new(
      pixmap_id_, ECORE_X_DAMAGE_REPORT_RAW_RECTANGLES);
  m_damageHandler = ecore_event_handler_add(
      ECORE_X_EVENT_DAMAGE_NOTIFY, notify_damage_updated_cb, this);
  g_object_set(gst_appsink_, "rotate", DEGREE_0, NULL);

  // Passing NUL value for video_format. Its not required but
  // |webmediaplayertizen| expects a value.
  UpdateMediaType();
  manager()->OnMediaDataChange(
      GetPlayerId(), video_format_, gst_height_, gst_width_, media_type_);
}

void MediaPlayerBridgeGstreamer::PlatformSurfaceUpdated() {
  gint64 current_time = 0;
  GstFormat format = GST_FORMAT_TIME;
  gst_element_query_position(gst_playbin_, &format, &current_time);
  base::TimeDelta timestamp = base::TimeDelta::FromMicroseconds(
      current_time / base::Time::kNanosecondsPerMicrosecond);
  manager()->OnPlatformSurfaceUpdated(GetPlayerId(), pixmap_id_, timestamp);
}
#endif

void MediaPlayerBridgeGstreamer::HandleError(
    media::MediaPlayerTizen::NetworkState state) {
  LOG(ERROR) << "Error in MediaPlayerBridgeGstreamer::HandleError";
#if defined(OS_TIZEN_MOBILE) && defined(TIZEN_V_2_3)
  if (device_power_release_lock(POWER_LOCK_DISPLAY) != DEVICE_ERROR_NONE)
    LOG(ERROR) << "|device_power_release_lock| request failed";
#endif

  error_occured_ = true;
  manager()->OnNetworkStateChange(GetPlayerId(), state);
}

}  // namespace media
