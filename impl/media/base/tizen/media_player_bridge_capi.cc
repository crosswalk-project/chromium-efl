// Copyright 2014 Samsung Electronics Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/base/tizen/media_player_bridge_capi.h"

#include "base/basictypes.h"
#include "base/command_line.h"
#include "base/logging.h"
#include "base/memory/shared_memory.h"
#include "base/message_loop/message_loop_proxy.h"
#include "base/strings/string_util.h"
#include "content/public/common/content_switches.h"
#include "media/base/tizen/media_player_manager_tizen.h"
#include "ui/gfx/size.h"

#if defined(OS_TIZEN_MOBILE)
#include <device/power.h>
#endif

namespace {
// Update duration every 100ms.
const int kDurationUpdateInterval = 100;
}

namespace media {
struct ErrorList {
  player_error_e error_code;
  std::string error_message;
};

// Modify this when new error information is added to |errorlist|.
const int ERROR_MAX = 18;

const struct ErrorList errorlist[ERROR_MAX] = {
    {PLAYER_ERROR_OUT_OF_MEMORY, "PLAYER_ERROR_OUT_OF_MEMORY"},
    {PLAYER_ERROR_INVALID_PARAMETER, "PLAYER_ERROR_INVALID_PARAMETER"},
    {PLAYER_ERROR_NO_SUCH_FILE, "PLAYER_ERROR_NO_SUCH_FILE"},
    {PLAYER_ERROR_INVALID_OPERATION, "PLAYER_ERROR_INVALID_OPERATION"},
    {PLAYER_ERROR_FILE_NO_SPACE_ON_DEVICE,
    "PLAYER_ERROR_FILE_NO_SPACE_ON_DEVICE"},
#if defined(OS_TIZEN_MOBILE)
    {PLAYER_ERROR_FEATURE_NOT_SUPPORTED_ON_DEVICE,
    "PLAYER_ERROR_FEATURE_NOT_SUPPORTED_ON_DEVICE"},
#endif
    {PLAYER_ERROR_SEEK_FAILED, "PLAYER_ERROR_SEEK_FAILED"},
    {PLAYER_ERROR_INVALID_STATE, "PLAYER_ERROR_INVALID_STATE"},
    {PLAYER_ERROR_NOT_SUPPORTED_FILE, "PLAYER_ERROR_NOT_SUPPORTED_FILE"},
    {PLAYER_ERROR_INVALID_URI, "PLAYER_ERROR_INVALID_URI"},
    {PLAYER_ERROR_SOUND_POLICY, "PLAYER_ERROR_SOUND_POLICY"},
    {PLAYER_ERROR_CONNECTION_FAILED, "PLAYER_ERROR_CONNECTION_FAILED"},
    {PLAYER_ERROR_VIDEO_CAPTURE_FAILED, "PLAYER_ERROR_VIDEO_CAPTURE_FAILED"},
    {PLAYER_ERROR_DRM_EXPIRED, "PLAYER_ERROR_DRM_EXPIRED"},
    {PLAYER_ERROR_DRM_NO_LICENSE, "PLAYER_ERROR_DRM_NO_LICENSE"},
    {PLAYER_ERROR_DRM_FUTURE_USE, "PLAYER_ERROR_DRM_FUTURE_USE"},
    {PLAYER_ERROR_DRM_NOT_PERMITTED, "PLAYER_ERROR_DRM_NOT_PERMITTED"},
    {PLAYER_ERROR_RESOURCE_LIMIT, "PLAYER_ERROR_RESOURCE_LIMIT"}};

static Eina_Bool notify_damage_updated_cb(void* data, int type, void* event) {
  MediaPlayerBridgeCapi* player = static_cast <MediaPlayerBridgeCapi*>(data);
  if (!player)
    return ECORE_CALLBACK_PASS_ON;

  player->PlatformSurfaceUpdated();
  return ECORE_CALLBACK_PASS_ON;
}

static double ConvertMilliSecondsToSeconds(int time) {
  double seconds = static_cast<double>(time) /
        (base::Time::kMillisecondsPerSecond);
  return seconds;
}

static double ConvertSecondsToMilliSeconds(double time) {
  if (time < 0) {
    LOG(ERROR) << "Invalid time:" << time << " Reset to 0";
    time = 0;
  }
  double seconds = static_cast<double>(time) *
        (base::Time::kMillisecondsPerSecond);
  return seconds;
}

// Called by player_prepare_async()
void player_prepared_cb(void* user_data) {
  MediaPlayerBridgeCapi* player =
    static_cast<MediaPlayerBridgeCapi*>(user_data);

  if (!player)
    return;

  player->SetPixmap();
  player->ExecuteDelayedPlayerState();
}

// Called by player_set_x11_display_pixmap()
unsigned int pixmap_buffer_ready_cb(void *user_data) {
  MediaPlayerBridgeCapi* player =
    static_cast<MediaPlayerBridgeCapi*>(user_data);

  if (!player)
    return -1;

  return player->GetSurfaceID();
}

// Called by player_set_completed_cb()
void playback_complete_cb(void* user_data) {
  MediaPlayerBridgeCapi* player =
    static_cast<MediaPlayerBridgeCapi*>(user_data);

  if (!player)
    return;

  player->PlaybackCompleteUpdate();
}

// Called by player_set_play_position()
void seek_complete_cb(void* user_data) {
  MediaPlayerBridgeCapi* player =
    static_cast<MediaPlayerBridgeCapi*>(user_data);

  if (!player)
    return;

  player->SeekCompleteUpdate();
}

// Called by player_set_buffering_cb()
void changed_buffering_status_cb(int percent, void *user_data) {
  MediaPlayerBridgeCapi* player =
    static_cast<MediaPlayerBridgeCapi*>(user_data);

  if (!player)
    return;

  player->HandleBufferingStatus(percent);
}

// Called by player_set_error_cb()
void error_cb(int error_code, void *user_data) {
  MediaPlayerBridgeCapi* player =
    static_cast<MediaPlayerBridgeCapi*>(user_data);

  if (!player)
    return;

  player->HandleError(error_code, "error_cb");
}

// Called by player_set_interrupted_cb()
void interrupt_cb(player_interrupted_code_e code, void *user_data) {
  MediaPlayerBridgeCapi* player =
    static_cast<MediaPlayerBridgeCapi*>(user_data);

  if (!player)
    return;

  // FIMXE: Add interrupt handling
}

MediaPlayerBridgeCapi::MediaPlayerBridgeCapi(
    int player_id,
    const GURL& url,
    double volume,
    MediaPlayerManager* manager_in)
    : MediaPlayerTizen(player_id, manager_in),
      pixmap_id_(0),
      efl_pixmap_(0),
      m_damage(0),
      m_damageHandler(NULL),
      main_loop_(base::MessageLoopProxy::current()),
      player_(NULL),
      url_(url),
      media_type_(0),
      player_width_(0),
      player_height_(0),
      video_format_(0),
      is_end_reached_(false),
      is_file_url_(false),
      is_paused_(true),
      is_pixmap_used_(true),
      is_seeking_(false),
      duration_(0),
      seek_duration_(0),
      playback_rate_(1.0),
      delayed_player_state_(0),
      shared_memory_size(0) {
  VLOG(1) << __FUNCTION__ << " : Player Id = " << GetPlayerId();
  int ret = player_create(&player_);
  if (ret == PLAYER_ERROR_NONE) {
    VLOG(1) << __FUNCTION__ << " : Create Player Success.";
    VLOG(1) << __FUNCTION__ << " : URL = " << url_.spec().c_str();
    player_set_uri(player_, url_.spec().c_str());
    player_set_sound_type(player_, SOUND_TYPE_MEDIA);
    player_set_volume(player_, (float)volume, (float)volume);

    // Use Pixmap
    ret = player_set_x11_display_pixmap(player_,
        pixmap_buffer_ready_cb, this);
    if (ret != PLAYER_ERROR_NONE) {
      HandleError(ret, "player_set_x11_display_pixmap");
      return;
    }
    player_set_completed_cb(player_, playback_complete_cb, this);
    player_set_buffering_cb(player_, changed_buffering_status_cb, this);
    player_set_interrupted_cb(player_, interrupt_cb, this);
    player_set_error_cb(player_, error_cb, this);
#if defined(OS_TIZEN_MOBILE)
    player_set_display_visible (player_, true);
#else // OS_TIZEN_TV
    player_set_x11_display_visible (player_, true);
#endif
    if(url_.SchemeIsFile())
      is_file_url_ = true;

    manager()->OnReadyStateChange(GetPlayerId(),
        MediaPlayerTizen::ReadyStateHaveEnoughData);
    manager()->OnNetworkStateChange(GetPlayerId(),
        MediaPlayerTizen::NetworkStateLoaded);

  } else {
    HandleError(ret, "player_create");
  }
}

MediaPlayerBridgeCapi::~MediaPlayerBridgeCapi() {
  VLOG(1) << __FUNCTION__ << " : Player Id = " << GetPlayerId();
}

void MediaPlayerBridgeCapi::Destroy() {
  VLOG(1) << __FUNCTION__ << " : Player Id = " << GetPlayerId();
  if (IsPlayerDestructing())
    return;
  destructing_ = true;
  StopCurrentTimeUpdateTimer();
  StopBufferingUpdateTimer();
  player_unset_completed_cb(player_);
  player_unset_interrupted_cb(player_);
  player_unset_error_cb(player_);
  player_unset_buffering_cb(player_);
  player_unset_subtitle_updated_cb(player_);
  player_destroy(player_);

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
  main_loop_->DeleteSoon(FROM_HERE, this);
}

void MediaPlayerBridgeCapi::Play() {
  VLOG(1) << __FUNCTION__ << " : Player Id = " << GetPlayerId();
  if (GetPlayerState() == PLAYER_STATE_IDLE) {
    if(delayed_player_state_ != 0) {
      delayed_player_state_ = DELAYED_PLAYER_STATE_PLAY;
      return;
    }
    delayed_player_state_ = DELAYED_PLAYER_STATE_PLAY;
    int ret = player_prepare_async(player_, player_prepared_cb, this);
    if (ret != PLAYER_ERROR_NONE)
      HandleError(ret, "player_prepare_async");
    return;
  }

  if (playback_rate_ == 0.0) {
    is_paused_ = false;
    return;
  }
  if (player_start(player_) != PLAYER_ERROR_NONE) {
    LOG(ERROR) <<"Play() -> |player_start| failed";
    return;
  }

#if defined(OS_TIZEN_MOBILE)
  if (device_power_request_lock(POWER_LOCK_DISPLAY, 0) != DEVICE_ERROR_NONE)
    LOG(ERROR) <<"Play() -> |device_power_request_lock| failed";
#endif

  StartCurrentTimeUpdateTimer();
  if (!is_file_url_)
    StartBufferingUpdateTimer();
  is_paused_ = false;
  is_end_reached_ = false;
}

void MediaPlayerBridgeCapi::Pause(bool is_media_related_action) {
  VLOG(1) << __FUNCTION__ << " : Player Id = " << GetPlayerId();
  if (GetPlayerState() == PLAYER_STATE_IDLE) {
    if(delayed_player_state_ != 0) {
      delayed_player_state_ = DELAYED_PLAYER_STATE_PAUSE;
      return;
    }
    delayed_player_state_ = DELAYED_PLAYER_STATE_PAUSE;
    int ret = player_prepare_async(player_, player_prepared_cb, this);
    if (ret != PLAYER_ERROR_NONE)
      HandleError(ret, "player_prepare_async");
    return;
  }

  if (player_pause(player_) != PLAYER_ERROR_NONE) {
    LOG(ERROR) << "Pause() -> |player_pause| failed";
    return;
  }

  if (!is_file_url_)
    StartBufferingUpdateTimer();

#if defined(OS_TIZEN_MOBILE)
  if (device_power_release_lock(POWER_LOCK_DISPLAY) != DEVICE_ERROR_NONE)
    LOG(ERROR) << "|device_power_release_lock| request failed";
#endif

  StopCurrentTimeUpdateTimer();
  is_paused_ = true;
}

void MediaPlayerBridgeCapi::SetRate(double rate) {
  VLOG(1) << __FUNCTION__ << " : Player Id = " << GetPlayerId();
  if (playback_rate_ == rate)
    return;

  if (rate == 0.0) {
    playback_rate_ = rate;
    Pause(true);
    return;
  }

  // Fixme: SetRate is always failing
  if (player_set_playback_rate(player_, (float)rate) != PLAYER_ERROR_NONE)
    LOG(ERROR) <<"|player_set_playback_rate|failed";
  else {
    // If previous rate was zero and requested rate is non-zero, change the
    // playback rate and call play
    if(playback_rate_ == 0.0 && rate != 0.0) {
      playback_rate_ = rate;
      Play();
    } else {
      playback_rate_ = rate;
    }
  }
}

void MediaPlayerBridgeCapi::Seek(const double time) {
  VLOG(1) << __FUNCTION__ << " : Player Id = " << GetPlayerId();
#if defined(OS_TIZEN_MOBILE)
  int err = player_set_play_position(player_,
      ConvertSecondsToMilliSeconds(time), true, seek_complete_cb, this);
#else // OS_TIZEN_TV
  int err = player_set_position(player_,
      ConvertSecondsToMilliSeconds(time), seek_complete_cb, this);
#endif
  if(err != PLAYER_ERROR_NONE) {
    LOG(ERROR) <<"|player_set_playback_rate|failed";
    manager()->OnTimeUpdate(GetPlayerId(), GetCurrentTime());
    manager()->OnTimeChanged(GetPlayerId());
    return;
  }

  if (!is_paused_)
    StopCurrentTimeUpdateTimer();

  StopBufferingUpdateTimer();
  UpdateSeekState(true);
  seek_duration_ = time;
  is_end_reached_ = time != duration_ ? false : true;
  manager()->OnTimeUpdate(GetPlayerId(), time);

  if (!is_paused_)
    StartCurrentTimeUpdateTimer();
}

void MediaPlayerBridgeCapi::Release() {
}

void MediaPlayerBridgeCapi::SetVolume(double volume) {
  VLOG(1) << __FUNCTION__ << " : Player Id = " << GetPlayerId()
    << " : Volume : " << volume;
  if (GetPlayerState() > PLAYER_STATE_IDLE){
    if(volume == 0.0){
      if(player_set_mute(player_,true)
        != PLAYER_ERROR_NONE)
      LOG(ERROR) << "|player_set_mute(true)| failed in MediaPlayerBridgeCapi::"
          << __FUNCTION__;

      return;
    }

    if(player_set_mute(player_,false)
        != PLAYER_ERROR_NONE)
      LOG(ERROR) << "|player_set_mute(false)| failed in MediaPlayerBridgeCapi::"
          << __FUNCTION__;

    if (player_set_volume(player_, (float)volume, (float)volume)
      != PLAYER_ERROR_NONE)
      LOG(ERROR) << "|player_set_volume| failed in MediaPlayerBridgeCapi::"
          << __FUNCTION__;
  }
}

void MediaPlayerBridgeCapi::UpdateMediaType() {
  int fps = 0 ;
  int bit_rate = 0;
  int sample_rate = 0;
  int channel = 0;
  int audio_bit_rate = 0;
  media_type_ = 0;

  int err = player_get_video_stream_info(player_, &fps, &bit_rate);
  int err_audio = player_get_audio_stream_info(player_,
      &sample_rate, &channel, &audio_bit_rate);

  VLOG(1) << "Audio Information: sample_rate = " << sample_rate
      << " , channel = " << channel
      << " , audio_bit_rate = " << audio_bit_rate;

  if (err != PLAYER_ERROR_NONE) {
    HandleError(err, "player_get_video_stream_info");
    return;
  }

  if (err_audio != PLAYER_ERROR_NONE) {
    HandleError(err_audio, "player_get_audio_stream_info");
    return;
  }

  if (sample_rate > 0)
    media_type_ |= MEDIA_AUDIO_MASK;

  err = player_get_video_size(player_,&player_width_,&player_height_);
  if (err != PLAYER_ERROR_NONE) {
    HandleError(err, " player_get_video_size");
    return;
  }

  VLOG(1) << "Video Information: fps = " << fps
      << " , bit_rate = " << bit_rate
      << " , Video Height = " << player_height_
      << " , Video Width = " << player_width_;

  // Video stream is present if both video width and video
  // height are non-zero.
  if (player_width_ != 0 && player_height_ != 0)
    media_type_ |= MEDIA_VIDEO_MASK;

  // Passing NULL value for video_format. Its not required but
  // |webmediaplayertizen| expects a value.
  manager()->OnMediaDataChange(GetPlayerId(), video_format_, player_height_,
      player_width_, media_type_);
}

void MediaPlayerBridgeCapi::UpdateDuration() {
  int duration = 0;
  player_get_duration(player_, &duration);

  if (duration_ != ConvertMilliSecondsToSeconds(duration)) {
    duration_ = ConvertMilliSecondsToSeconds(duration);
    manager()->OnDurationChange(GetPlayerId(), duration_);
  }

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

double MediaPlayerBridgeCapi::GetCurrentTime() {
  // For http://instagram.com/p/tMQOo0lWqm/
  // After playback completed current-time and duration are not equal.
  if (is_end_reached_) {
    if (is_seeking_)
      return seek_duration_;
    if (playback_rate_ < 0)
      return 0.0f;
    if (duration_)
      return duration_;
  }

  int postion = 0;
#if defined(OS_TIZEN_MOBILE)
  player_get_play_position(player_, &postion);
#else // OS_TIZEN_TV
  player_get_position(player_, &postion);
#endif
  return ConvertMilliSecondsToSeconds(postion);
}

void MediaPlayerBridgeCapi::OnCurrentTimeUpdateTimerFired() {
  if (IsPlayerDestructing())
    return;
  manager()->OnTimeUpdate(GetPlayerId(), GetCurrentTime());
  UpdateDuration();
}

void MediaPlayerBridgeCapi::StartCurrentTimeUpdateTimer() {
  if (!current_time_update_timer_.IsRunning()) {
    current_time_update_timer_.Start(
        FROM_HERE,
        base::TimeDelta::FromMilliseconds(kDurationUpdateInterval),
        this, &MediaPlayerBridgeCapi::OnCurrentTimeUpdateTimerFired);
  }
}

void MediaPlayerBridgeCapi::StopCurrentTimeUpdateTimer() {
  if (current_time_update_timer_.IsRunning())
    current_time_update_timer_.Stop();
}

void MediaPlayerBridgeCapi::OnBufferingUpdateTimerFired() {
  if (IsPlayerDestructing())
    return;

  int start, current;
  if (player_get_streaming_download_progress(player_,
      &start, &current) == PLAYER_ERROR_NONE) {
    if (current == 100) {
      StopBufferingUpdateTimer();
      manager()->OnNetworkStateChange(GetPlayerId(),
          MediaPlayerTizen::NetworkStateLoaded);
    }
    std::vector<media::MediaPlayerTizen::TimeRanges> buffer_range;
    media::MediaPlayerTizen::TimeRanges range;
    range.start = 0;
    range.end = static_cast<double>(current) * duration_ / 100
        * base::Time::kMicrosecondsPerSecond;
    buffer_range.push_back(range);
    manager()->OnBufferUpdate(GetPlayerId(), buffer_range);
  }
}

void MediaPlayerBridgeCapi::StartBufferingUpdateTimer() {
  if (!buffering_update_timer_.IsRunning()) {
    buffering_update_timer_.Start(
        FROM_HERE,
        base::TimeDelta::FromMilliseconds(kDurationUpdateInterval),
        this, &MediaPlayerBridgeCapi::OnBufferingUpdateTimerFired);
  }
}

void MediaPlayerBridgeCapi::StopBufferingUpdateTimer() {
  if (buffering_update_timer_.IsRunning())
    buffering_update_timer_.Stop();
}

void MediaPlayerBridgeCapi::OnTimeChanged() {
  DCHECK(main_loop_->BelongsToCurrentThread());
  manager()->OnTimeChanged(GetPlayerId());
}

void MediaPlayerBridgeCapi::PlaybackCompleteUpdate() {
  VLOG(1) << __FUNCTION__ << " : Player Id = " << GetPlayerId();
  is_end_reached_ = true;
#if defined(OS_TIZEN_MOBILE)
  if (device_power_release_lock(POWER_LOCK_DISPLAY) != DEVICE_ERROR_NONE)
    LOG(ERROR) << "|device_power_release_lock| request failed";
#endif

  StopCurrentTimeUpdateTimer();
  manager()->OnTimeUpdate(GetPlayerId(), GetCurrentTime());
  manager()->OnTimeChanged(GetPlayerId());
}

void MediaPlayerBridgeCapi::SeekCompleteUpdate() {
  UpdateSeekState(false);
  manager()->OnTimeChanged(GetPlayerId());

  if (!is_file_url_)
    StartBufferingUpdateTimer();
}

void MediaPlayerBridgeCapi::UpdateSeekState(bool state) {
  is_seeking_ = state;
}

void MediaPlayerBridgeCapi::PlatformSurfaceUpdated() {
  int postion = 0;
#if defined(OS_TIZEN_MOBILE)
  player_get_play_position(player_, &postion);
#else // OS_TIZEN_TV
  player_get_position(player_, &postion);
#endif
  base::TimeDelta timestamp = base::TimeDelta::FromMilliseconds(postion);
  manager()->OnPlatformSurfaceUpdated(GetPlayerId(), pixmap_id_, timestamp);
}

void MediaPlayerBridgeCapi::SetPixmap() {
  VLOG(1) << __FUNCTION__ << " : Player Id = " << GetPlayerId();
  UpdateMediaType();
  UpdateDuration();
  if (media_type_ == 0)
    return;

  if ((media_type_ & MEDIA_VIDEO_MASK) && !efl_pixmap_.get()) {
    efl_pixmap_ = gfx::EflPixmap::Create(gfx::EflPixmap::SURFACE,
        gfx::Size(player_width_, player_height_));
    if (!efl_pixmap_.get()) {
      HandleError(0, " PixmapSurfaceTizen::create");
      return ;
    }
    is_pixmap_used_ = true;
    pixmap_id_ = efl_pixmap_->GetId();

    //Register to get notification from ecore for damage updates.
    m_damage = ecore_x_damage_new(pixmap_id_,
        ECORE_X_DAMAGE_REPORT_RAW_RECTANGLES);
    m_damageHandler = ecore_event_handler_add(ECORE_X_EVENT_DAMAGE_NOTIFY,
        notify_damage_updated_cb, this);
  }

  manager()->OnReadyStateChange(GetPlayerId(),
      MediaPlayerTizen::ReadyStateHaveEnoughData);
  manager()->OnNetworkStateChange(GetPlayerId(),
      MediaPlayerTizen::NetworkStateLoaded);
}

int MediaPlayerBridgeCapi::GetSurfaceID() const {
  return pixmap_id_;
}

void MediaPlayerBridgeCapi::HandleBufferingStatus(int percent) {
  if (IsPlayerDestructing())
    return;

  if (percent == 100 && !is_paused_ && !is_seeking_) {
    if (GetPlayerState() == PLAYER_STATE_PAUSED) {
      VLOG(1) << __FUNCTION__ << " : Player Id = " << GetPlayerId()
        << " : Playing MediaPlayer as buffer reached 100%";
      if (player_start(player_) != PLAYER_ERROR_NONE) {
        LOG(ERROR) << "HandleBufferingStatus:player_start failed";
        return;
      }
      StartCurrentTimeUpdateTimer();
      if (!is_file_url_)
        StartBufferingUpdateTimer();
      manager()->OnReadyStateChange(GetPlayerId(),
          MediaPlayerTizen::ReadyStateHaveEnoughData);
      manager()->OnNetworkStateChange(GetPlayerId(),
          MediaPlayerTizen::NetworkStateLoading);
      return;
    }
  }

  if (percent != 100 && !is_paused_ && !is_seeking_) {
    if (GetPlayerState() == PLAYER_STATE_PLAYING) {
      VLOG(1) << __FUNCTION__ << " : Player Id = " << GetPlayerId()
        << " : Pausing MediaPlayer as buffer is < 100%";
      if (player_pause(player_) != PLAYER_ERROR_NONE) {
        LOG(ERROR) << "HandleBufferingStatus:player_pause failed";
        return;
      }
      StopCurrentTimeUpdateTimer();
      manager()->OnReadyStateChange(GetPlayerId(),
          MediaPlayerTizen::ReadyStateHaveCurrentData);
      manager()->OnNetworkStateChange(GetPlayerId(),
          MediaPlayerTizen::NetworkStateLoading);
    }
  }
}

// Helper method which prints errors occured while calling CAPI api's.
void MediaPlayerBridgeCapi::HandleError(int err, char const* from) {
  int index;
  for (index = 0; index < ERROR_MAX; index++) {
    if (errorlist[index].error_code == err) {
      LOG(ERROR) << "Stoping playback of media due to Error code : "<<err
          <<" Error message : "<<errorlist[index].error_message
          <<" from " << from;
      break;
    }
  }

  if (index == ERROR_MAX)
    LOG(ERROR) << "Stoping playback of media due to Unknown error : "
        <<err <<" from " << from;

  StopBufferingUpdateTimer();
  StopCurrentTimeUpdateTimer();
  manager()->OnNetworkStateChange(GetPlayerId(),
      MediaPlayerTizen::NetworkStateDecodeError);

#ifdef OS_TIZEN_MOBILE
  if (device_power_release_lock(POWER_LOCK_DISPLAY) != DEVICE_ERROR_NONE)
    LOG(ERROR) << "|device_power_release_lock| request failed";
#endif
}

player_state_e MediaPlayerBridgeCapi::GetPlayerState() {
  player_state_e state;
  player_get_state(player_,&state);
  return state;
}

void MediaPlayerBridgeCapi::ExecuteDelayedPlayerState() {
  switch (delayed_player_state_) {
    case DELAYED_PLAYER_STATE_PLAY :
      VLOG(1) << "Executing the delayed play command";
      Play();
      break;
    case DELAYED_PLAYER_STATE_PAUSE :
      VLOG(1) << "Executing the delayed pause command";
      Pause(false);
      break;
    default :
      break;
  }
}

}  // namespace media
