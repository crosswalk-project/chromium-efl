// Copyright 2014 Samsung Electronics Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_COMMON_GPU_MEDIA_TIZEN_VIDEO_DECODE_ACCELERATOR_H_
#define CONTENT_COMMON_GPU_MEDIA_TIZEN_VIDEO_DECODE_ACCELERATOR_H_

#include "base/synchronization/lock.h"
#include "base/threading/thread.h"
#include "content/common/content_export.h"
#include "media/video/video_decode_accelerator.h"

namespace content {

class CONTENT_EXPORT TizenVideoDecodeAccelerator
    : public media::VideoDecodeAccelerator {
 public:
  TizenVideoDecodeAccelerator();
  ~TizenVideoDecodeAccelerator() override;

  bool Initialize(media::VideoCodecProfile profile, Client* client) override;
  void Decode(const media::BitstreamBuffer& bitstream_buffer) override;
  void AssignPictureBuffers(
      const std::vector<media::PictureBuffer>& buffers) override;
  void ReusePictureBuffer(int32 picture_buffer_id) override;
  void Flush() override;
  void Reset() override;
  void Destroy() override;
  bool CanDecodeOnIOThread() override;

 private:
  struct BitstreamBufferRef;
  struct Impl;

  void OnDecode(scoped_ptr<BitstreamBufferRef> buffer_ref);
  void NotifyError(media::VideoDecodeAccelerator::Error error);
  void StartDecoder();

  Impl* impl_;

  DISALLOW_COPY_AND_ASSIGN(TizenVideoDecodeAccelerator);
};

}  // namespace content

#endif  // CONTENT_COMMON_GPU_MEDIA_TIZEN_VIDEO_DECODE_ACCELERATOR_H_
