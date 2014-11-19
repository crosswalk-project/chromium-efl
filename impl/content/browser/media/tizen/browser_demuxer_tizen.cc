// Copyright 2014 Samsung Electronics Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/browser/media/tizen/browser_demuxer_tizen.h"
#include "content/common/media/tizen/media_player_messages_tizen.h"

namespace content {

class BrowserDemuxerTizen::Internal : public media::DemuxerTizen {
 public:
  Internal(const scoped_refptr<BrowserDemuxerTizen>& demuxer,
      int demuxer_client_id)
      : demuxer_(demuxer),
        demuxer_client_id_(demuxer_client_id) { }

  virtual ~Internal() {
    DCHECK(ClientIDExists()) << demuxer_client_id_;
    demuxer_->RemoveDemuxerClient(demuxer_client_id_);
  }

  // media::DemuxerTizen implementation.
  virtual void Initialize(media::DemuxerTizenClient* client) override {
    DCHECK(!ClientIDExists()) << demuxer_client_id_;
    demuxer_->AddDemuxerClient(demuxer_client_id_, client);
  }

  virtual void RequestDemuxerConfigs() override {
    DCHECK(ClientIDExists()) << demuxer_client_id_;
    demuxer_->Send(new MediaPlayerGstMsg_MediaConfigRequest(
        demuxer_client_id_));
  }

  virtual void RequestDemuxerData(media::DemuxerStream::Type type) override {
    DCHECK(ClientIDExists()) << demuxer_client_id_;
    demuxer_->Send(new MediaPlayerGstMsg_ReadFromDemuxer(
        demuxer_client_id_, type));
  }

  virtual void RequestDemuxerSeek(
      const base::TimeDelta& time_to_seek) override {
    DCHECK(ClientIDExists()) << demuxer_client_id_;
    demuxer_->Send(new MediaPlayerGstMsg_DemuxerSeekRequest(
        demuxer_client_id_, time_to_seek));
  }

 private:
  // Helper for DCHECKing that the ID is still registered.
  bool ClientIDExists() {
    return demuxer_->demuxer_clients_.Lookup(demuxer_client_id_);
  }

  scoped_refptr<BrowserDemuxerTizen> demuxer_;
  int demuxer_client_id_;

  DISALLOW_COPY_AND_ASSIGN(Internal);
};

BrowserDemuxerTizen::BrowserDemuxerTizen()
    : BrowserMessageFilter(MediaPlayerTizenMsgStart) {}

BrowserDemuxerTizen::~BrowserDemuxerTizen() {}

void BrowserDemuxerTizen::OverrideThreadForMessage(
    const IPC::Message& message,
    BrowserThread::ID* thread) {
  switch (message.type()) {
    case MediaPlayerGstHostMsg_DemuxerReady::ID:
    case MediaPlayerGstHostMsg_ReadFromDemuxerAck::ID:
    case MediaPlayerGstHostMsg_BufferMetaDataAck::ID:
    case MediaPlayerGstHostMsg_DurationChanged::ID:
    case MediaPlayerGstHostMsg_DemuxerSeekDone::ID:
      *thread = BrowserThread::UI;
      return;
  }
}

bool BrowserDemuxerTizen::OnMessageReceived(const IPC::Message& message) {
  bool handled = true;
  IPC_BEGIN_MESSAGE_MAP(BrowserDemuxerTizen, message)
    IPC_MESSAGE_HANDLER(MediaPlayerGstHostMsg_DemuxerReady, OnDemuxerReady)
    IPC_MESSAGE_HANDLER(MediaPlayerGstHostMsg_ReadFromDemuxerAck,
                        OnReadFromDemuxerAck)
    IPC_MESSAGE_HANDLER(MediaPlayerGstHostMsg_BufferMetaDataAck,
                        OnBufferMetaDataAck)
    IPC_MESSAGE_HANDLER(MediaPlayerGstHostMsg_DurationChanged,
                        OnDurationChanged)
    IPC_MESSAGE_HANDLER(MediaPlayerGstHostMsg_DemuxerSeekDone,
                        OnDemuxerSeekDone)
    IPC_MESSAGE_UNHANDLED(handled = false)
  IPC_END_MESSAGE_MAP()
  return handled;
}

scoped_ptr<media::DemuxerTizen> BrowserDemuxerTizen::CreateDemuxer(
    int demuxer_client_id) {
  return scoped_ptr<media::DemuxerTizen>(
      new Internal(this, demuxer_client_id));
}

void BrowserDemuxerTizen::AddDemuxerClient(
    int demuxer_client_id,
    media::DemuxerTizenClient* client) {
  VLOG(1) << __FUNCTION__ << " peer_pid=" << peer_pid()
          << " demuxer_client_id=" << demuxer_client_id;
  demuxer_clients_.AddWithID(client, demuxer_client_id);
}

void BrowserDemuxerTizen::RemoveDemuxerClient(int demuxer_client_id) {
  VLOG(1) << __FUNCTION__ << " peer_pid=" << peer_pid()
          << " demuxer_client_id=" << demuxer_client_id;
  demuxer_clients_.Remove(demuxer_client_id);
}

void BrowserDemuxerTizen::OnDemuxerReady(
    int demuxer_client_id,
    const media::DemuxerConfigs& configs) {
  media::DemuxerTizenClient* client =
      demuxer_clients_.Lookup(demuxer_client_id);
  if (client)
    client->OnDemuxerConfigsAvailable(configs);
}

void BrowserDemuxerTizen::OnReadFromDemuxerAck(
    int demuxer_client_id,
    base::SharedMemoryHandle foreign_memory_handle,
    const media::DemuxedBufferMetaData& meta_data) {
  media::DemuxerTizenClient* client =
      demuxer_clients_.Lookup(demuxer_client_id);
  if (client)
    client->OnDemuxerDataAvailable(foreign_memory_handle, meta_data);
}

void BrowserDemuxerTizen::OnBufferMetaDataAck(
    int demuxer_client_id,
    const media::DemuxedBufferMetaData& meta_data) {
  media::DemuxerTizenClient* client =
      demuxer_clients_.Lookup(demuxer_client_id);
  if (client)
    client->OnBufferMetaDataAvailable( meta_data);
}

void BrowserDemuxerTizen::OnDemuxerSeekDone(
    int demuxer_client_id,
    const base::TimeDelta& actual_browser_seek_time) {
  media::DemuxerTizenClient* client =
      demuxer_clients_.Lookup(demuxer_client_id);
  if (client)
    client->OnDemuxerSeekDone(actual_browser_seek_time);
}

void BrowserDemuxerTizen::OnDurationChanged(
    int demuxer_client_id,
    const base::TimeDelta& duration) {
  media::DemuxerTizenClient* client =
      demuxer_clients_.Lookup(demuxer_client_id);
  if (client)
    client->OnDemuxerDurationChanged(duration);
}

}  // namespace content
