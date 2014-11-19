// Copyright 2014 Samsung Electronics Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/renderer/media/tizen/renderer_demuxer_tizen.h"

#include "base/bind.h"
#include "base/message_loop/message_loop.h"
#include "content/child/thread_safe_sender.h"
#include "content/common/media/tizen/media_player_messages_tizen.h"
#include "content/renderer/media/tizen/media_source_delegate_tizen.h"
#include "content/renderer/media/tizen/renderer_media_player_manager_tizen.h"
#include "content/renderer/render_thread_impl.h"

namespace content {

RendererDemuxerTizen::RendererDemuxerTizen()
    : thread_safe_sender_(RenderThreadImpl::current()->thread_safe_sender()),
      media_task_runner_(
          RenderThreadImpl::current()->GetMediaThreadTaskRunner()) {}

RendererDemuxerTizen::~RendererDemuxerTizen() {}

int RendererDemuxerTizen::GetNextDemuxerClientID() {
  // Don't use zero for IDs since it can be interpreted as having no ID.
  return next_demuxer_client_id_.GetNext() + 1;
}

void RendererDemuxerTizen::AddDelegate(
    int demuxer_client_id,
    MediaSourceDelegateTizen* delegate) {
  DCHECK(media_task_runner_->BelongsToCurrentThread());
  delegates_.AddWithID(delegate, demuxer_client_id);
}

void RendererDemuxerTizen::RemoveDelegate(int demuxer_client_id) {
  DCHECK(media_task_runner_->BelongsToCurrentThread());
  delegates_.Remove(demuxer_client_id);
}

bool RendererDemuxerTizen::OnMessageReceived(const IPC::Message& message) {
  switch (message.type()) {
    case MediaPlayerGstMsg_ReadFromDemuxer::ID:
    case MediaPlayerGstMsg_MediaConfigRequest::ID:
    case MediaPlayerGstMsg_DemuxerSeekRequest::ID:
      media_task_runner_->PostTask(FROM_HERE, base::Bind(
          &RendererDemuxerTizen::DispatchMessage, this, message));
      return true;
   }
  return false;
}

void RendererDemuxerTizen::DemuxerReady(
    int demuxer_client_id,
    const media::DemuxerConfigs& configs) {
  thread_safe_sender_->Send(new MediaPlayerGstHostMsg_DemuxerReady(
      demuxer_client_id, configs));
}

void RendererDemuxerTizen::ReadFromDemuxerAck(
    int demuxer_client_id,
    base::SharedMemoryHandle foreign_memory_handle,
    const media::DemuxedBufferMetaData& meta_data) {
  thread_safe_sender_->Send(new MediaPlayerGstHostMsg_ReadFromDemuxerAck(
      demuxer_client_id, foreign_memory_handle, meta_data));
}

void RendererDemuxerTizen::BufferMetaDataAck(
    int demuxer_client_id,
    const media::DemuxedBufferMetaData& meta_data) {
  thread_safe_sender_->Send(new MediaPlayerGstHostMsg_BufferMetaDataAck(
      demuxer_client_id, meta_data));
}

void RendererDemuxerTizen::DemuxerSeekDone(
    int demuxer_client_id,
    const base::TimeDelta& actual_browser_seek_time) {
  thread_safe_sender_->Send(new MediaPlayerGstHostMsg_DemuxerSeekDone(
      demuxer_client_id, actual_browser_seek_time));
}

void RendererDemuxerTizen::DurationChanged(int demuxer_client_id,
    const base::TimeDelta& duration) {
  thread_safe_sender_->Send(new MediaPlayerGstHostMsg_DurationChanged(
      demuxer_client_id, duration));
}

void RendererDemuxerTizen::DispatchMessage(const IPC::Message& message) {
  IPC_BEGIN_MESSAGE_MAP(RendererDemuxerTizen, message)
    IPC_MESSAGE_HANDLER(MediaPlayerGstMsg_ReadFromDemuxer, OnReadFromDemuxer)
    IPC_MESSAGE_HANDLER(MediaPlayerGstMsg_MediaConfigRequest,
                        OnMediaConfigRequest)
    IPC_MESSAGE_HANDLER(MediaPlayerGstMsg_DemuxerSeekRequest,
                        OnDemuxerSeekRequest)
  IPC_END_MESSAGE_MAP()
}

void RendererDemuxerTizen::OnReadFromDemuxer(
    int demuxer_client_id,
    media::DemuxerStream::Type type) {
  MediaSourceDelegateTizen* delegate = delegates_.Lookup(demuxer_client_id);
  if (delegate)
    delegate->OnReadFromDemuxer(type);
}

void RendererDemuxerTizen::OnDemuxerSeekRequest(
    int demuxer_client_id,
    const base::TimeDelta& time_to_seek) {
  MediaSourceDelegateTizen* delegate = delegates_.Lookup(demuxer_client_id);
  if (delegate)
    delegate->StartSeek(time_to_seek, false);
}

void RendererDemuxerTizen::OnMediaConfigRequest(int demuxer_client_id) {
  MediaSourceDelegateTizen* delegate = delegates_.Lookup(demuxer_client_id);
  if (delegate)
    delegate->OnMediaConfigRequest();
}

}  // namespace content
