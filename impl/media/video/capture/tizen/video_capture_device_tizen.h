// Copyright (c) 2012 The Samsung Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Tizen specific implementation of VideoCaptureDevice.
// Tizen Core API is used to capture the video frames from the device.

#ifndef MEDIA_VIDEO_CAPTURE_TIZEN_VIDEO_CAPTURE_DEVICE_TIZEN_H_
#define MEDIA_VIDEO_CAPTURE_TIZEN_VIDEO_CAPTURE_DEVICE_TIZEN_H_

#include <camera.h>
#include <string>

#include "base/files/file_util.h"
#include "base/threading/thread.h"
#include "media/video/capture/video_capture_device.h"
#include "media/video/capture/video_capture_types.h"

namespace media {

class VideoCaptureDeviceTizen : public VideoCaptureDevice {
 public:
  explicit VideoCaptureDeviceTizen(const Name& device_name);
  virtual ~VideoCaptureDeviceTizen() override;

  virtual void AllocateAndStart(const VideoCaptureParams& params,
                                scoped_ptr<Client> client) override;

  virtual void StopAndDeAllocate() override;

  static camera_device_e DeviceNameToCameraId(
    const VideoCaptureDevice::Name& device_name);

 private:
  enum InternalState {
    kIdle,  // The device driver is opened but camera is not in use.
    kCapturing,  // Video is being captured.
    kError  // Error accessing HW functions.
            // User needs to recover by destroying the object.
  };

  static void OnCameraCaptured(camera_preview_data_s* frame, void* data);
  void OnAllocateAndStart(int width,
                          int height,
                          int frame_rate,
                          VideoPixelFormat format,
                          scoped_ptr<Client> client);
  void OnStopAndDeAllocate();

  bool AllocateVideoBuffers(int width, int height);
  void DeAllocateVideoBuffers();
  void SetErrorState(const std::string& reason);

  InternalState state_;
  scoped_ptr<VideoCaptureDevice::Client> client_;
  Name device_name_;
  base::Thread worker_;  // Thread used for reading data from the device.
  scoped_refptr<VideoCaptureDevice::Client::Buffer> buffer_;
  camera_h camera_;

  DISALLOW_IMPLICIT_CONSTRUCTORS(VideoCaptureDeviceTizen);
};

}  // namespace media

#endif  // MEDIA_VIDEO_CAPTURE_TIZEN_VIDEO_CAPTURE_DEVICE_TIZEN_H_
