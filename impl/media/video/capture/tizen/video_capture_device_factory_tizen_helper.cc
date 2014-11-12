// Copyright 2014 The Samsung Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#if defined(TIZEN_VIDEO_CAPTURE_SUPPORT)
#include "media/video/capture/tizen/video_capture_device_factory_tizen.h"
#else
#include "media/video/capture/linux/video_capture_device_factory_linux.h"
#endif

namespace media {

scoped_ptr<VideoCaptureDeviceFactory> CreateVideoCaptureDeviceFactoryTizen(
    scoped_refptr<base::SingleThreadTaskRunner> ui_task_runner) {
#if defined(TIZEN_VIDEO_CAPTURE_SUPPORT)
  return scoped_ptr<VideoCaptureDeviceFactory>(
      new VideoCaptureDeviceFactoryTizen(ui_task_runner));
#else
  return scoped_ptr<VideoCaptureDeviceFactory>(
      new VideoCaptureDeviceFactoryLinux(ui_task_runner));
#endif
}

}  // namespace media
