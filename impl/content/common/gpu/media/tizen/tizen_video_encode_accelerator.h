// Copyright 2014 Samsung Electronics Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_COMMON_GPU_MEDIA_TIZEN_VIDEO_ENCODE_ACCELERATOR_H_
#define CONTENT_COMMON_GPU_MEDIA_TIZEN_VIDEO_ENCODE_ACCELERATOR_H_

#include "base/threading/thread.h"
#include "base/synchronization/lock.h"
#include "base/synchronization/waitable_event.h"
#include "content/common/content_export.h"
#include "media/video/video_encode_accelerator.h"

namespace content {

class CONTENT_EXPORT TizenVideoEncodeAccelerator
    : public media::VideoEncodeAccelerator {
 public:
  TizenVideoEncodeAccelerator();
  ~TizenVideoEncodeAccelerator() override;

  std::vector<media::VideoEncodeAccelerator::SupportedProfile>
      GetSupportedProfiles() override;
  bool Initialize(media::VideoFrame::Format input_format,
                  const gfx::Size& input_visible_size,
                  media::VideoCodecProfile output_profile,
                  uint32 initial_bitrate,
                  Client* client) override;
  void Encode(const scoped_refptr<media::VideoFrame>& frame,
              bool force_keyframe) override;
  void UseOutputBitstreamBuffer(const media::BitstreamBuffer& buffer) override;
  void RequestEncodingParametersChange(uint32 bitrate,
                                       uint32 framerate) override;
  void Destroy() override;

 private:
  struct BitstreamBufferRef;
  struct Impl;

  void OnEncode(scoped_ptr<BitstreamBufferRef> buffer_ref, bool force_keyframe);
  void OnUseOutputBitstreamBuffer(const media::BitstreamBuffer& buffer);
  bool StartEncoder();

  Impl* impl_;

  DISALLOW_COPY_AND_ASSIGN(TizenVideoEncodeAccelerator);
};

}  // namespace content

#endif  // CONTENT_COMMON_GPU_MEDIA_TIZEN_VIDEO_ENCODE_ACCELERATOR_TIZEN_H_
