// Copyright 2014 Samsung Electronics Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/renderer/media/tizen/media_source_delegate_tizen.h"

#include "base/process/process.h"
#include "media/base/bind_to_current_loop.h"
#include "media/base/media_log.h"
#include "media/base/tizen/demuxer_stream_player_params_tizen.h"
#include "media/blink/webmediaplayer_util.h"
#include "media/blink/webmediasource_impl.h"
#include "media/filters/chunk_demuxer.h"
#include "media/filters/decrypting_demuxer_stream.h"
#include "third_party/WebKit/public/web/WebRuntimeFeatures.h"

namespace content {

static void LogMediaSourceError(
    const scoped_refptr<media::MediaLog>& media_log,
    const std::string& error) {
  media_log->AddEvent(media_log->CreateMediaSourceErrorEvent(error));
}

MediaSourceDelegateTizen::MediaSourceDelegateTizen(
    RendererDemuxerTizen* demuxer_client,
    int demuxer_client_id,
    const scoped_refptr<base::SingleThreadTaskRunner>& media_task_runner,
    media::MediaLog* media_log)
    : main_loop_(base::MessageLoopProxy::current()),
      main_weak_factory_(this),
      main_weak_this_(main_weak_factory_.GetWeakPtr()),
      media_task_runner_(media_task_runner),
      media_weak_factory_(this),
      demuxer_client_(demuxer_client),
      demuxer_client_id_(demuxer_client_id),
      media_log_(media_log),
      audio_stream_(NULL),
      video_stream_(NULL),
      seek_time_(media::kNoTimestamp()),
      pending_seek_(false),
      is_seeking_(false),
      seeking_pending_seek_(false),
      is_demuxer_seek_done_(false),
      pending_seek_time_(media::kNoTimestamp()),
      is_audio_read_fired_(false),
      is_video_read_fired_(false),
      is_demuxer_ready_(false),
      shared_memory_size_(0) {
  VLOG(1) << "MediaSourceDelegateTizen::" << __FUNCTION__
          << ": Demuxer Client Id = " << demuxer_client_id_;
  DCHECK(!chunk_demuxer_);
}

MediaSourceDelegateTizen::~MediaSourceDelegateTizen() {
  DCHECK(main_loop_->BelongsToCurrentThread());
  DCHECK(!chunk_demuxer_);
  DCHECK(!audio_stream_);
  DCHECK(!video_stream_);
}

void MediaSourceDelegateTizen::InitializeMediaSource(
    const MediaSourceOpenedCB& media_source_opened_cb,
    const UpdateNetworkStateCB& update_network_state_cb,
    const DurationChangeCB& duration_change_cb) {
  DCHECK(main_loop_->BelongsToCurrentThread());
  DCHECK(!media_source_opened_cb.is_null());
  media_source_opened_cb_ = media_source_opened_cb;
  update_network_state_cb_ = media::BindToCurrentLoop(update_network_state_cb);
  duration_change_cb_ = duration_change_cb;

  chunk_demuxer_.reset(new media::ChunkDemuxer(
      media::BindToCurrentLoop(base::Bind(
          &MediaSourceDelegateTizen::OnDemuxerOpened, main_weak_this_)),
      media::BindToCurrentLoop(base::Bind(
          &MediaSourceDelegateTizen::OnNeedKey, main_weak_this_)),
      base::Bind(&LogMediaSourceError, media_log_),
      false));

  media_task_runner_->PostTask(FROM_HERE,
      base::Bind(&MediaSourceDelegateTizen::InitializeDemuxer,
          base::Unretained(this)));
}

void MediaSourceDelegateTizen::InitializeDemuxer() {
  DCHECK(media_task_runner_->BelongsToCurrentThread());
  demuxer_client_->AddDelegate(demuxer_client_id_, this);
  chunk_demuxer_->Initialize(
      this,
      base::Bind(&MediaSourceDelegateTizen::OnDemuxerInitDone,
          media_weak_factory_.GetWeakPtr()),
      false);
}

void MediaSourceDelegateTizen::OnNeedKey(
    const std::string& type,
    const std::vector<uint8>& init_data) {
  // FIXME: Need to check the functionality of this method.
  NOTIMPLEMENTED();
}

void MediaSourceDelegateTizen::OnDemuxerOpened() {
  DCHECK(main_loop_->BelongsToCurrentThread());
  if (media_source_opened_cb_.is_null())
    return;
  media_source_opened_cb_.Run(new media::WebMediaSourceImpl(
      chunk_demuxer_.get(), base::Bind(&LogMediaSourceError, media_log_)));
}

void MediaSourceDelegateTizen::OnDemuxerError(
    media::PipelineStatus status) {
  if (status != media::PIPELINE_OK && !update_network_state_cb_.is_null())
    update_network_state_cb_.Run(PipelineErrorToNetworkState(status));
}

bool MediaSourceDelegateTizen::CanNotifyDemuxerReady() {
  DCHECK(media_task_runner_->BelongsToCurrentThread());
  return is_demuxer_ready_;
}

void MediaSourceDelegateTizen::OnDemuxerInitDone(
    media::PipelineStatus status) {
  DCHECK(media_task_runner_->BelongsToCurrentThread());
  DCHECK(chunk_demuxer_);
  if (status != media::PIPELINE_OK) {
    OnDemuxerError(status);
    return;
  }
  audio_stream_ = chunk_demuxer_->GetStream(media::DemuxerStream::AUDIO);
  video_stream_ = chunk_demuxer_->GetStream(media::DemuxerStream::VIDEO);
  if ((audio_stream_ && audio_stream_->audio_decoder_config().is_encrypted())
      || (video_stream_ && video_stream_->video_decoder_config().is_encrypted()
      )) {
    OnDemuxerError(media::PIPELINE_ERROR_DECODE);
    return;
  }

  // Notify demuxer ready when both streams are not encrypted.
  is_demuxer_ready_ = true;
  NotifyDemuxerReady();
}

void MediaSourceDelegateTizen::NotifyDemuxerReady() {
  DCHECK(media_task_runner_->BelongsToCurrentThread());
  DCHECK(CanNotifyDemuxerReady());

  scoped_ptr<media::DemuxerConfigs> configs(new media::DemuxerConfigs());
  if (audio_stream_) {
    media::AudioDecoderConfig audio_config =
        audio_stream_->audio_decoder_config();
    configs->audio_codec = audio_config.codec();
    configs->audio_channels =
        media::ChannelLayoutToChannelCount(audio_config.channel_layout());
    configs->audio_sampling_rate = audio_config.samples_per_second();
    configs->is_audio_encrypted = audio_config.is_encrypted();
    configs->audio_extra_data = std::vector<uint8>(audio_config.extra_data(),
        audio_config.extra_data() + audio_config.extra_data_size());
  }
  if (video_stream_) {
    media::VideoDecoderConfig video_config =
        video_stream_->video_decoder_config();
    configs->video_codec = video_config.codec();
    configs->video_size = video_config.natural_size();
    configs->is_video_encrypted = video_config.is_encrypted();
    configs->video_extra_data = std::vector<uint8>(video_config.extra_data(),
        video_config.extra_data() + video_config.extra_data_size());
  }
  if (demuxer_client_) {
    demuxer_client_->DemuxerReady(demuxer_client_id_, *configs);
  }
}

void MediaSourceDelegateTizen::OnReadFromDemuxer(
    media::DemuxerStream::Type type) {
  DCHECK(media_task_runner_->BelongsToCurrentThread());

  if (is_seeking_)
    return;

  if ((type == media::DemuxerStream::AUDIO) && audio_stream_
      && !is_audio_read_fired_) {
    is_audio_read_fired_ = true;
    audio_stream_->Read(base::Bind(
        &MediaSourceDelegateTizen::OnBufferReady,
        media_weak_factory_.GetWeakPtr() , type));
  }

  if ((type == media::DemuxerStream::VIDEO) && video_stream_
      && !is_video_read_fired_) {
    is_video_read_fired_ = true;
    video_stream_->Read(base::Bind(
        &MediaSourceDelegateTizen::OnBufferReady,
        media_weak_factory_.GetWeakPtr() , type));
  }
}

void MediaSourceDelegateTizen::Destroy() {
  DCHECK(main_loop_->BelongsToCurrentThread());
  VLOG(1) << "MediaSourceDelegateTizen::" << __FUNCTION__
          << ": Demuxer Client Id = " << demuxer_client_id_;

  if (!chunk_demuxer_) {
    DCHECK(!demuxer_client_);
    delete this;
    return;
  }

  duration_change_cb_.Reset();
  update_network_state_cb_.Reset();
  media_source_opened_cb_.Reset();

  main_weak_factory_.InvalidateWeakPtrs();
  DCHECK(!main_weak_factory_.HasWeakPtrs());

  // 1. shutdown demuxer.
  // 2. On media thread, call stop demuxer.
  // 3. On callback, post message and self destory.
  chunk_demuxer_->Shutdown();
  media_task_runner_->PostTask(
      FROM_HERE,
      base::Bind(&MediaSourceDelegateTizen::StopDemuxer,
      base::Unretained(this)));
}

void MediaSourceDelegateTizen::StopDemuxer() {
  DCHECK(media_task_runner_->BelongsToCurrentThread());
  DCHECK(chunk_demuxer_);

  demuxer_client_->RemoveDelegate(demuxer_client_id_);
  demuxer_client_ = NULL;
  audio_stream_ = NULL;
  video_stream_ = NULL;

  media_weak_factory_.InvalidateWeakPtrs();
  DCHECK(!media_weak_factory_.HasWeakPtrs());

  chunk_demuxer_->Stop();
  chunk_demuxer_.reset();

  main_loop_->PostTask(
      FROM_HERE,
      base::Bind(&MediaSourceDelegateTizen::DeleteSelf,
      base::Unretained(this)));
}

void MediaSourceDelegateTizen::DeleteSelf() {
  DCHECK(main_loop_->BelongsToCurrentThread());
  VLOG(1) << "MediaSourceDelegateTizen::" << __FUNCTION__
          << ": Demuxer Client Id = " << demuxer_client_id_;
  chunk_demuxer_.reset();
  delete this;
}

void MediaSourceDelegateTizen::OnMediaConfigRequest() {
  NotifyDemuxerReady();
}

void MediaSourceDelegateTizen::SeekInternal(
    const base::TimeDelta& seek_time) {
  DCHECK(media_task_runner_->BelongsToCurrentThread());
  chunk_demuxer_->Seek(seek_time, base::Bind(
      &MediaSourceDelegateTizen::OnDemuxerSeekDone,
      media_weak_factory_.GetWeakPtr()));
}

void MediaSourceDelegateTizen::OnBufferReady(
    media::DemuxerStream::Type type,
    media::DemuxerStream::Status status,
    const scoped_refptr<media::DecoderBuffer>& buffer) {

  scoped_ptr<media::DemuxedBufferMetaData> meta_data(
      new media::DemuxedBufferMetaData());
  meta_data->status = status;
  meta_data->type = type;

  if (type == media::DemuxerStream::AUDIO)
    is_audio_read_fired_ = false;
  if (type == media::DemuxerStream::VIDEO)
    is_video_read_fired_ = false;

  switch (status) {
    case media::DemuxerStream::kAborted:
      LOG (ERROR) << "[RENDER] : DemuxerStream::kAborted";
      break;

    case media::DemuxerStream::kConfigChanged:
      VLOG(1) << "[RENDER] : DemuxerStream::kConfigChanged";
      NotifyDemuxerReady();
      break;

    case media::DemuxerStream::kOk:
        if (buffer.get()->end_of_stream()) {
          VLOG(1) << "[RENDER] : DemuxerStream::kOk but |end_of_stream|";
          meta_data->end_of_stream = true;
          break;
        }
        shared_memory_size_ = buffer.get()->data_size();
        if (!shared_memory_.CreateAndMapAnonymous(shared_memory_size_)) {
          LOG (ERROR) << "Shared Memory creation failed.";
          return;
        }
        if (!shared_memory_.ShareToProcess(base::Process::Current().Handle(),
            &foreign_memory_handle_)) {
          LOG (ERROR) << "Shared Memory handle could not be obtained";
          shared_memory_.Close();
          return;
        }
        memcpy(shared_memory_.memory(), (void*)buffer.get()->writable_data(),
            shared_memory_size_);
        meta_data->timestamp = buffer.get()->timestamp();
        meta_data->time_duration = buffer.get()->duration();
        if (demuxer_client_) {
          meta_data->size = shared_memory_size_;
          demuxer_client_->ReadFromDemuxerAck(
              demuxer_client_id_, foreign_memory_handle_, *meta_data);
        }
        shared_memory_.Close();
        return;
      break;
    default:
      NOTREACHED();
  }

  if (demuxer_client_)
    demuxer_client_->BufferMetaDataAck(demuxer_client_id_, *meta_data);
}

void MediaSourceDelegateTizen::StartWaitingForSeek(
    const base::TimeDelta& seek_time) {
  DCHECK(main_loop_->BelongsToCurrentThread());

  if (!chunk_demuxer_)
    return;
  // Called from |webmediaplayertizen| only.
  is_demuxer_seek_done_ = false;
  seeking_pending_seek_ = false;
  is_seeking_ = true;
  chunk_demuxer_->StartWaitingForSeek(seek_time);
}

void MediaSourceDelegateTizen::CancelPendingSeek(
    const base::TimeDelta& seek_time) {
  DCHECK(main_loop_->BelongsToCurrentThread());
  if (!chunk_demuxer_)
    return;
  is_seeking_ = true;
  pending_seek_ = true;
  pending_seek_time_ = seek_time;

  if (is_demuxer_seek_done_) {
    // Since we already requested gstreamer to seek. And there are no pending
    // seeks in |chunk_demuxer|. Cancelling pending seek makes no sense.
    //
    // This block will handle when |gstreamer| is seeking and new seek came in
    // between.
    VLOG(1) << "No need to CancelPendingSeek";
    is_demuxer_seek_done_ = false;
    pending_seek_ = false;
    chunk_demuxer_->StartWaitingForSeek(seek_time);
    StartSeek(seek_time, true);
    return;
  }

  chunk_demuxer_->CancelPendingSeek(seek_time);
}

void MediaSourceDelegateTizen::StartSeek(
    const base::TimeDelta& seek_time,
    bool is_seeking_pending_seek) {
  DCHECK(media_task_runner_->BelongsToCurrentThread());

  VLOG(1)<< "MediaSourceDelegateTizen::" << __FUNCTION__
          << " : " << seek_time.InSecondsF();
  if (!chunk_demuxer_)
    return;
  is_seeking_ = true;
  is_demuxer_seek_done_ = false;
  if (is_seeking_pending_seek)
    seeking_pending_seek_ = is_seeking_pending_seek;
  else if (seeking_pending_seek_) {
    VLOG(1)<< "Ignoring seek request from Gstreamer";
    return;
  }
  seek_time_ = seek_time;
  SeekInternal(seek_time);
}

void MediaSourceDelegateTizen::OnDemuxerSeekDone(
    media::PipelineStatus status) {
  DCHECK(media_task_runner_->BelongsToCurrentThread());
  if (status != media::PIPELINE_OK) {
    OnDemuxerError(status);
    return;
  }
  if (pending_seek_) {
    pending_seek_ = false;
    StartSeek(pending_seek_time_, true);
    seek_time_ = pending_seek_time_;
  } else {
    VLOG(1) << "Actual time that Gstreamer seeks : "
            << seek_time_.InMilliseconds();
    seeking_pending_seek_ = false;
    is_seeking_ = false;
    is_demuxer_seek_done_ = true;
    demuxer_client_->DemuxerSeekDone(demuxer_client_id_, seek_time_);
  }
}

void MediaSourceDelegateTizen::SetDuration(base::TimeDelta duration) {
  DCHECK(main_loop_->BelongsToCurrentThread());
  main_loop_->PostTask(FROM_HERE, base::Bind(
      &MediaSourceDelegateTizen::OnDurationChanged,
      main_weak_this_, duration));
}

void MediaSourceDelegateTizen::OnDurationChanged(
    const base::TimeDelta& duration) {
  DCHECK(main_loop_->BelongsToCurrentThread());
  if (demuxer_client_)
    demuxer_client_->DurationChanged(demuxer_client_id_, duration);

  if (!duration_change_cb_.is_null())
    duration_change_cb_.Run(duration.InSecondsF());
}

}  // namespace content
