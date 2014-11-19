// Copyright 2014 Samsung Electronics Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// IPC messages for tizen media player.
// Multiply-included message file, hence no include guard.

#include "content/common/content_export.h"
#include "ipc/ipc_message_macros.h"
#include "media/base/media_keys.h"
#include "media/base/tizen/demuxer_stream_player_params_tizen.h"
#include "media/base/tizen/media_player_manager_tizen.h"
#include "ui/gfx/rect_f.h"

#undef IPC_MESSAGE_EXPORT
#define IPC_MESSAGE_EXPORT CONTENT_EXPORT
#define IPC_MESSAGE_START MediaPlayerTizenMsgStart

IPC_ENUM_TRAITS(media::AudioCodec)
IPC_ENUM_TRAITS(media::MediaPlayerTizen::ReadyState)
IPC_ENUM_TRAITS(media::MediaPlayerTizen::NetworkState)
IPC_ENUM_TRAITS(media::DemuxerStream::Status)
IPC_ENUM_TRAITS(media::DemuxerStream::Type)
IPC_ENUM_TRAITS(media::VideoCodec)
IPC_ENUM_TRAITS(MediaPlayerHostMsg_Initialize_Type)

IPC_STRUCT_TRAITS_BEGIN(media::DemuxerConfigs)
  IPC_STRUCT_TRAITS_MEMBER(audio_codec)
  IPC_STRUCT_TRAITS_MEMBER(audio_channels)
  IPC_STRUCT_TRAITS_MEMBER(audio_sampling_rate)
  IPC_STRUCT_TRAITS_MEMBER(is_audio_encrypted)
  IPC_STRUCT_TRAITS_MEMBER(audio_extra_data)

  IPC_STRUCT_TRAITS_MEMBER(video_codec)
  IPC_STRUCT_TRAITS_MEMBER(video_size)
  IPC_STRUCT_TRAITS_MEMBER(is_video_encrypted)
  IPC_STRUCT_TRAITS_MEMBER(video_extra_data)

  IPC_STRUCT_TRAITS_MEMBER(duration_ms)
IPC_STRUCT_TRAITS_END()

IPC_STRUCT_TRAITS_BEGIN(media::DemuxedBufferMetaData)
  IPC_STRUCT_TRAITS_MEMBER(size)
  IPC_STRUCT_TRAITS_MEMBER(end_of_stream)
  IPC_STRUCT_TRAITS_MEMBER(timestamp)
  IPC_STRUCT_TRAITS_MEMBER(time_duration)
  IPC_STRUCT_TRAITS_MEMBER(type)
  IPC_STRUCT_TRAITS_MEMBER(status)
IPC_STRUCT_TRAITS_END()

IPC_STRUCT_TRAITS_BEGIN(media::SubsampleEntry)
  IPC_STRUCT_TRAITS_MEMBER(clear_bytes)
  IPC_STRUCT_TRAITS_MEMBER(cypher_bytes)
IPC_STRUCT_TRAITS_END()

IPC_STRUCT_TRAITS_BEGIN(media::MediaPlayerTizen::TimeRanges)
  IPC_STRUCT_TRAITS_MEMBER(start)
  IPC_STRUCT_TRAITS_MEMBER(end)
IPC_STRUCT_TRAITS_END()

// Initialize Gst player.
IPC_MESSAGE_ROUTED5(MediaPlayerGstHostMsg_Init,
                    int /* player_id */,
                    MediaPlayerHostMsg_Initialize_Type /* type */,
                    GURL /* URL */,
                    double /* volume */,
                    int /* demuxer client id */)

// Deinitialize Gst player.
IPC_MESSAGE_ROUTED1(MediaPlayerGstHostMsg_DeInit,
                    int /* player_id */)

// Start playback.
IPC_MESSAGE_ROUTED1(MediaPlayerGstHostMsg_Play,
                    int /* player_id */)

// Pause playback.
IPC_MESSAGE_ROUTED1(MediaPlayerGstHostMsg_Pause,
                    int /* player_id */)

// Set volume.
IPC_MESSAGE_ROUTED2(MediaPlayerGstHostMsg_SetVolume,
                    int /* player_id */,
                    double /* volume */)

// Set playback rate.
IPC_MESSAGE_ROUTED2(MediaPlayerGstHostMsg_SetRate,
                    int /* player_id */,
                    double /* rate */)

// Playback duration.
IPC_MESSAGE_ROUTED2(MediaPlayerGstMsg_DurationChanged,
                    int /* player_id */,
                    double /* time */)

// Current  duration.
IPC_MESSAGE_ROUTED2(MediaPlayerGstMsg_TimeUpdate,
                    int /* player_id */,
                    double /* time */)

// Pause state.
IPC_MESSAGE_ROUTED2(MediaPlayerGstMsg_OnPauseStateChange,
                    int /* player_id */,
                    bool /* state */)

// Seek state.
IPC_MESSAGE_ROUTED2(MediaPlayerGstMsg_OnSeekStateChange,
                    int /* player_id */,
                    bool /* state */)

// Current buffer range.
IPC_MESSAGE_ROUTED2(MediaPlayerGstMsg_BufferUpdate,
                    int /* player_id */,
                    std::vector<media::MediaPlayerTizen::TimeRanges> /*buffer_range*/)

// Playback completed.
IPC_MESSAGE_ROUTED1(MediaPlayerGstMsg_TimeChanged,
                    int /* player_id */)

// Ready state change.
IPC_MESSAGE_ROUTED2(MediaPlayerGstMsg_ReadyStateChange,
                    int /* player_id */,
                    media::MediaPlayerTizen::ReadyState /* state */)

// Network state change.
IPC_MESSAGE_ROUTED2(MediaPlayerGstMsg_NetworkStateChange,
                    int /* player_id */,
                    media::MediaPlayerTizen::NetworkState /* state */)

// Gst media data has changed.
IPC_MESSAGE_ROUTED5(MediaPlayerGstMsg_MediaDataChanged,
                    int /* player_id */,
                    int /* format */,
                    int /* height */,
                    int /* width */,
                    int /* media */)

// On new frame available.
IPC_MESSAGE_ROUTED4(MediaPlayerGstMsg_NewFrameAvailable,
                    int /* player_id */,
                    base::SharedMemoryHandle /* Handle */,
                    uint32 /* length */,
                    base::TimeDelta /* time stamp */)

#ifdef OS_TIZEN
IPC_MESSAGE_ROUTED3(MediaPlayerGstMsg_PlatformSurfaceUpdated,
                    int /* player_id */,
                    int /* pixmap_id */,
                    base::TimeDelta /* time stamp */)
#endif

// Seek.
IPC_MESSAGE_ROUTED2(MediaPlayerGstHostMsg_Seek,
                    int /* player_id */,
                    double /* time */)

// For MSE internal seek request.
IPC_MESSAGE_ROUTED2(MediaPlayerGstMsg_SeekRequest,
                    int /* player_id */,
                    double /* time_to_seek */)

// Sent after the renderer demuxer has seeked.
IPC_MESSAGE_CONTROL2(MediaPlayerGstHostMsg_DemuxerSeekDone,
                     int /* demuxer_client_id */,
                     base::TimeDelta /* actual_browser_seek_time */)

// Inform the media source player that the demuxer is ready.
IPC_MESSAGE_CONTROL2(MediaPlayerGstHostMsg_DemuxerReady,
                     int /* demuxer_client_id */,
                     media::DemuxerConfigs /* configs */)

// Sent when the data was read from the ChunkDemuxer.
IPC_MESSAGE_CONTROL3(MediaPlayerGstHostMsg_ReadFromDemuxerAck,
                     int /* demuxer_client_id */,
                     base::SharedMemoryHandle /* Handle */,
                     media::DemuxedBufferMetaData /* meta data of buffer*/)

// Sent when the data was read from the ChunkDemuxer.
IPC_MESSAGE_CONTROL2(MediaPlayerGstHostMsg_BufferMetaDataAck,
                     int /* demuxer_client_id */,
                     media::DemuxedBufferMetaData /* meta data of buffer*/)

// Inform the media source player of changed media duration from demuxer.
IPC_MESSAGE_CONTROL2(MediaPlayerGstHostMsg_DurationChanged,
                     int /* demuxer_client_id */,
                     base::TimeDelta /* duration */)

// The player needs new config data
IPC_MESSAGE_CONTROL1(MediaPlayerGstMsg_MediaConfigRequest,
                     int /* demuxer_client_id */)

// The media source player reads data from demuxer
IPC_MESSAGE_CONTROL2(MediaPlayerGstMsg_ReadFromDemuxer,
                     int /* demuxer_client_id */,
                     media::DemuxerStream::Type /* type */)

// Requests renderer demuxer seek.
IPC_MESSAGE_CONTROL2(MediaPlayerGstMsg_DemuxerSeekRequest,
                     int /* demuxer_client_id */,
                     base::TimeDelta /* time_to_seek */)
