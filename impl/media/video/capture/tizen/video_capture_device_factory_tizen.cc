// Copyright 2014 The Samsung Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/video/capture/tizen/video_capture_device_factory_tizen.h"

namespace {

media::VideoPixelFormat toChromiumType(camera_pixel_format_e e) {
  switch (e) {
    case CAMERA_PIXEL_FORMAT_NV12:
      return media::PIXEL_FORMAT_NV12;
    case CAMERA_PIXEL_FORMAT_NV21:
      return media::PIXEL_FORMAT_NV21;
    case CAMERA_PIXEL_FORMAT_YUYV:
      return media::PIXEL_FORMAT_YUY2;
    case CAMERA_PIXEL_FORMAT_RGB888:
      return media::PIXEL_FORMAT_RGB24;
    case CAMERA_PIXEL_FORMAT_UYVY:
      return media::PIXEL_FORMAT_UYVY;
    case CAMERA_PIXEL_FORMAT_ARGB:
      return media::PIXEL_FORMAT_ARGB;
    case CAMERA_PIXEL_FORMAT_I420:
      return media::PIXEL_FORMAT_I420;
    case CAMERA_PIXEL_FORMAT_YV12:
      return media::PIXEL_FORMAT_YV12;
    case CAMERA_PIXEL_FORMAT_JPEG:
      return media::PIXEL_FORMAT_MJPEG;
    default:
      NOTREACHED();
  }
  return media::PIXEL_FORMAT_UNKNOWN;
}

bool OnCameraSupportedPreviewResolution(
   int width, int height, void* user_data) {
  std::vector<gfx::Size>* sizes =
      static_cast< std::vector<gfx::Size>* >(user_data);
  DCHECK(sizes);
  sizes->push_back(gfx::Size(width, height));

  return true;
}

bool OnCameraSupportedPreviewFormat(
    camera_pixel_format_e format, void* user_data) {
  std::vector<media::VideoPixelFormat>* list_format =
      static_cast< std::vector<media::VideoPixelFormat>* >(user_data);
  DCHECK(list_format);

  list_format->push_back(toChromiumType(format));

  return true;
}

bool OnCameraSupportedFPS(camera_attr_fps_e fps, void* user_data) {
  std::vector<int>* list_fps = static_cast< std::vector<int>* >(user_data);
  DCHECK(list_fps);
  if (CAMERA_ATTR_FPS_AUTO != fps) {
    // AUTO format is not defined on Chromium, so skip.
    list_fps->push_back(static_cast<int>(fps));
  }

  return true;
}

void GenerateChromiumVideoCaptureFormat(
    const std::vector<gfx::Size>& frame_sizes,
    const std::vector<int>& fps,
    const std::vector<media::VideoPixelFormat>& formats,
    std::vector<media::VideoCaptureFormat>& outSupportedFormats) {
  for (auto itrS = frame_sizes.begin(); itrS != frame_sizes.end(); itrS++) {
    for (auto itrFPS = fps.begin(); itrFPS != fps.end(); itrFPS++) {
      for (auto itrFMT = formats.begin(); itrFMT != formats.end(); itrFMT++) {
        media::VideoCaptureFormat format;
        format.frame_size = *itrS;
        format.frame_rate = *itrFPS;
        format.pixel_format = *itrFMT;
        outSupportedFormats.push_back(format);
      }
    }
  }
}

class CameraHandle final {
 public:
  explicit CameraHandle(const media::VideoCaptureDevice::Name& dev)
      : camera_handle_(NULL) {
    if (!dev.id().empty())
      if (CAMERA_ERROR_NONE != camera_create(
            media::VideoCaptureDeviceTizen::DeviceNameToCameraId(dev),
            &camera_handle_)) {
        camera_handle_ = NULL;
        LOG(ERROR) << "Cannot create camera";
      }
  }

  CameraHandle()
      : camera_handle_(NULL) {
    if (CAMERA_ERROR_NONE  != camera_create(CAMERA_DEVICE_CAMERA0, &camera_handle_)) {
      camera_handle_ = NULL;
      LOG(ERROR) << "Cannot create camera";
    }
  }

  ~CameraHandle() {
    if (camera_handle_ != NULL)
      camera_destroy(camera_handle_);
  }

  bool IsValid() const {
    return camera_handle_ != NULL;
  }

  void GetDeviceSupportedFormats(
      media::VideoCaptureFormats& supported_formats) const {
    std::vector<gfx::Size> supported_frame_sizes;
    std::vector<int> supported_frame_rates;
    std::vector<media::VideoPixelFormat> supported_pixel_formats;

    if (!IsValid()) {
      LOG(ERROR) << "Cannot use camera";
      return;
    }

    if (CAMERA_ERROR_NONE != camera_foreach_supported_preview_resolution(
          camera_handle_,
          OnCameraSupportedPreviewResolution,
          &supported_frame_sizes)) {
      LOG(ERROR) << "Cannot get the supported resolutions for camera";
      return;
    }

    if (CAMERA_ERROR_NONE != camera_foreach_supported_preview_format(
          camera_handle_,
          OnCameraSupportedPreviewFormat,
          &supported_pixel_formats)) {
      LOG(ERROR) << "Cannot get the supported formats for camera";
      return;
    }

    if (CAMERA_ERROR_NONE != camera_attr_foreach_supported_fps(
          camera_handle_,
          OnCameraSupportedFPS,
          &supported_frame_rates)) {
      LOG(ERROR) << "Cannot get the supported FPS for camera";
      return;
    }
    supported_formats.clear();
    GenerateChromiumVideoCaptureFormat(supported_frame_sizes,
                                       supported_frame_rates,
                                       supported_pixel_formats,
                                       supported_formats);
  }

  int GetDeviceCounts() const {
    int device_count = 0;
    if (CAMERA_ERROR_NONE !=
           camera_get_device_count(camera_handle_, &device_count)) {
      device_count = 0;
      LOG(ERROR) << "Cannot read camera count";
    }

    return device_count;
  }

 private:
  camera_h camera_handle_;
};

} // namespace

namespace media {

VideoCaptureDeviceFactoryTizen::VideoCaptureDeviceFactoryTizen(
    scoped_refptr<base::SingleThreadTaskRunner> ui_task_runner)
    : ui_task_runner_(ui_task_runner) {
}

VideoCaptureDeviceFactoryTizen::~VideoCaptureDeviceFactoryTizen() {
}

scoped_ptr<VideoCaptureDevice> VideoCaptureDeviceFactoryTizen::Create(
    const VideoCaptureDevice::Name& name) {
  DCHECK(thread_checker_.CalledOnValidThread());

  return scoped_ptr<VideoCaptureDevice>(new VideoCaptureDeviceTizen(name));
}

void VideoCaptureDeviceFactoryTizen::GetDeviceNames(
    VideoCaptureDevice::Names* const device_names) {
  DCHECK(thread_checker_.CalledOnValidThread());
  DCHECK(device_names->empty());
  VideoCaptureDevice::Name primary_camera("Primary camera", "0");
  CameraHandle camera_handle;

  if (!camera_handle.IsValid()) {
    LOG(ERROR) << "Cannot use camera";
    return;
  }

  int device_count = camera_handle.GetDeviceCounts();
  if (device_count == 0) {
    LOG(ERROR) << "No camera on this device.";
    return;
  }

  device_names->push_back(primary_camera);
  if (device_count == 2) {
    VideoCaptureDevice::Name secondary_camera("Secondary camera", "1");
    device_names->push_back(secondary_camera);
  }
}

void VideoCaptureDeviceFactoryTizen::GetDeviceSupportedFormats(
    const VideoCaptureDevice::Name& device,
    VideoCaptureFormats* supported_formats) {
  DCHECK(thread_checker_.CalledOnValidThread());
  DCHECK(supported_formats != NULL);
  CameraHandle camera_handle;
  camera_handle.GetDeviceSupportedFormats(*supported_formats);
}

}  // namespace media
