// Copyright 2014 The Samsung Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Implementation of a VideoCaptureDeviceFactoryTizen class.

#ifndef MEDIA_VIDEO_CAPTURE_TIZEN_VIDEO_CAPTURE_DEVICE_FACTORY_TIZEN_H_
#define MEDIA_VIDEO_CAPTURE_TIZEN_VIDEO_CAPTURE_DEVICE_FACTORY_TIZEN_H_



#include <camera.h>

#include "media/video/capture/video_capture_device_factory.h"
#include "media/video/capture/video_capture_device.h"
#include "media/video/capture/video_capture_types.h"
#include "media/video/capture/tizen/video_capture_device_tizen.h"

namespace media {

// Extension of VideoCaptureDeviceFactory to create and manipulate Tizen
// devices.
class MEDIA_EXPORT VideoCaptureDeviceFactoryTizen
    : public VideoCaptureDeviceFactory {
 public:
  explicit VideoCaptureDeviceFactoryTizen(
      scoped_refptr<base::SingleThreadTaskRunner> ui_task_runner);
  ~VideoCaptureDeviceFactoryTizen() override;

  scoped_ptr<VideoCaptureDevice> Create(
      const VideoCaptureDevice::Name& device_name) override;
  void GetDeviceNames(VideoCaptureDevice::Names* device_names) override;
  void GetDeviceSupportedFormats(
      const VideoCaptureDevice::Name& device,
      VideoCaptureFormats* supported_formats) override;

 private:
  scoped_refptr<base::SingleThreadTaskRunner> ui_task_runner_;

  DISALLOW_COPY_AND_ASSIGN(VideoCaptureDeviceFactoryTizen);
};

}  // namespace media

#endif  // MEDIA_VIDEO_CAPTURE_TIZEN_VIDEO_CAPTURE_DEVICE_FACTORY_TIZEN_H_
