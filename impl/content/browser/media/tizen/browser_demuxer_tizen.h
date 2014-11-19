// Copyright 2014 Samsung Electronics Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_BROWSER_MEDIA_TIZEN_BROWSER_DEMUXER_TIZEN_H_
#define CONTENT_BROWSER_MEDIA_TIZEN_BROWSER_DEMUXER_TIZEN_H_

#include "base/id_map.h"
#include "base/memory/shared_memory.h"
#include "content/public/browser/browser_message_filter.h"
#include "media/base/tizen/demuxer_tizen.h"

namespace content {

// Represents the browser process half of an IPC-based demuxer proxy.
// It vends out media::DemuxerTizen instances that are registered with an
// end point in the renderer process.
//
// Refer to RendererDemuxerTizen for the renderer process half.
class CONTENT_EXPORT BrowserDemuxerTizen : public BrowserMessageFilter {
 public:
  BrowserDemuxerTizen();

  // BrowserMessageFilter overrides.
  virtual void OverrideThreadForMessage(
      const IPC::Message& message,
      BrowserThread::ID* thread) override;
  virtual bool OnMessageReceived(const IPC::Message& message) override;

  // Returns an uninitialized demuxer implementation associated with
  // |demuxer_client_id|, which can be used to communicate with the real demuxer
  // in the renderer process.
  scoped_ptr<media::DemuxerTizen> CreateDemuxer(int demuxer_client_id);

 protected:
  friend class base::RefCountedThreadSafe<BrowserDemuxerTizen>;
  virtual ~BrowserDemuxerTizen();

 private:
  class Internal;

  // Called by internal demuxer implementations to add/remove a client
  // association.
  void AddDemuxerClient(
      int demuxer_client_id,
      media::DemuxerTizenClient* client);
  void RemoveDemuxerClient(int demuxer_client_id);

  // IPC message handlers.
  void OnDemuxerReady(
      int demuxer_client_id,
      const media::DemuxerConfigs& configs);
  void OnReadFromDemuxerAck(
      int demuxer_client_id,
      base::SharedMemoryHandle foreign_memory_handle,
      const media::DemuxedBufferMetaData& meta_data);
  void OnBufferMetaDataAck(
      int demuxer_client_id,
      const media::DemuxedBufferMetaData& meta_data);
  void OnDemuxerSeekDone(
      int demuxer_client_id,
      const base::TimeDelta& actual_browser_seek_time);
  void OnDurationChanged(
      int demuxer_client_id,
      const base::TimeDelta& duration);

  IDMap<media::DemuxerTizenClient> demuxer_clients_;

  DISALLOW_COPY_AND_ASSIGN(BrowserDemuxerTizen);
};

}  // namespace content

#endif  // CONTENT_BROWSER_MEDIA_TIZEN_BROWSER_DEMUXER_TIZEN_H_
