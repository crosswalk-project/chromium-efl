// Copyright (c) 2012 The Samsung Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/video/capture/tizen/video_capture_device_tizen.h"

#include <errno.h>
#include <string>
#include <sstream>

#include "eweb_view.h"

#include "base/bind.h"
#include "content/browser/renderer_host/render_view_host_impl.h"
#include "media/video/capture/tizen/video_capture_device_factory_tizen.h"
#include "media/base/yuv_convert.h"
#include "third_party/libyuv/include/libyuv.h"

#if defined(WEBRTC_DEBUG_METRIC)
#include "base/time/time.h"
static base::Time debug_last_newbuf_captured;
static unsigned int debug_total_frames_captured = 0;
#endif

namespace {

enum { kMjpegWidth = 640 };
enum { kMjpegHeight = 480 };
enum { kTypicalFramerate = 30 };
enum { kTypicalFormat = CAMERA_PIXEL_FORMAT_NV12 };

camera_pixel_format_e toCapiType(media::VideoPixelFormat e) {
  switch (e) {
    case media::PIXEL_FORMAT_NV12:
      return CAMERA_PIXEL_FORMAT_NV12;
    case media::PIXEL_FORMAT_NV21:
      return CAMERA_PIXEL_FORMAT_NV21;
    case media::PIXEL_FORMAT_YUY2:
      return CAMERA_PIXEL_FORMAT_YUYV;
    case media::PIXEL_FORMAT_RGB24:
      return CAMERA_PIXEL_FORMAT_RGB888;
    case media::PIXEL_FORMAT_UYVY:
      return CAMERA_PIXEL_FORMAT_UYVY;
    case media::PIXEL_FORMAT_ARGB:
      return CAMERA_PIXEL_FORMAT_ARGB;
    case media::PIXEL_FORMAT_I420:
      return CAMERA_PIXEL_FORMAT_I420;
    case media::PIXEL_FORMAT_YV12:
      return CAMERA_PIXEL_FORMAT_YV12;
    case media::PIXEL_FORMAT_MJPEG:
      return CAMERA_PIXEL_FORMAT_JPEG;
    default:
      NOTREACHED();
  }
  return CAMERA_PIXEL_FORMAT_INVALID;
}

} // unnamed namespace

namespace media {

const std::string VideoCaptureDevice::Name::GetModel() const {
  return "tizen camera";
}

VideoCaptureDeviceTizen::VideoCaptureDeviceTizen(const Name& device_name)
    : state_(kIdle),
      device_name_(device_name),
      worker_("VideoCapture"),
      buffer_(),
      camera_(NULL) {
#if defined(WEBRTC_DEBUG_METRIC)
  debug_total_frames_captured = 0;
  debug_last_newbuf_captured = base::Time();
#endif
}

VideoCaptureDeviceTizen::~VideoCaptureDeviceTizen() {
  state_ = kIdle;
  DCHECK(!worker_.IsRunning());
}

void VideoCaptureDeviceTizen::AllocateAndStart(
    const VideoCaptureParams& params,
    scoped_ptr<VideoCaptureDevice::Client> client) {
  DCHECK(!worker_.IsRunning());
  worker_.Start();
  worker_.message_loop()->PostTask(
      FROM_HERE,
      base::Bind(&VideoCaptureDeviceTizen::OnAllocateAndStart,
                 base::Unretained(this),
                 params.requested_format.frame_size.width(),
                 params.requested_format.frame_size.height(),
                 params.requested_format.frame_rate,
                 params.requested_format.pixel_format,
                 base::Passed(&client)));
}

void VideoCaptureDeviceTizen::StopAndDeAllocate() {
  DCHECK(worker_.IsRunning());
  worker_.message_loop()->PostTask(
      FROM_HERE,
      base::Bind(&VideoCaptureDeviceTizen::OnStopAndDeAllocate,
                 base::Unretained(this)));
  worker_.Stop();
  DeAllocateVideoBuffers();
}

camera_device_e VideoCaptureDeviceTizen::DeviceNameToCameraId(
    const VideoCaptureDevice::Name& device_name) {
  if ("0" == device_name.id()) {
    return CAMERA_DEVICE_CAMERA0;
  } else if ("1" == device_name.id()) {
    return CAMERA_DEVICE_CAMERA1;
  } else {
    NOTREACHED();
  }
  return static_cast<camera_device_e>(-1);
}

void VideoCaptureDeviceTizen::OnCameraCaptured(camera_preview_data_s* frame,
                                               void* data) {
  if ( frame->format != CAMERA_PIXEL_FORMAT_NV12 ) {
    NOTIMPLEMENTED();
    return;
  }

  VideoCaptureDeviceTizen* self = static_cast<VideoCaptureDeviceTizen*>(data);
  camera_attr_fps_e current_fps =
      static_cast<camera_attr_fps_e>(kTypicalFramerate);
  camera_attr_get_preview_fps(self->camera_, &current_fps);

  int destination_width = frame->width;
  int destination_height = frame->height;

  // FIXME : find a way to get Orientation here.
  int orientation = 0;
  libyuv::RotationMode rotation_mode = libyuv::kRotate0;
#if defined(OS_TIZEN_MOBILE)
  rotation_mode = libyuv::kRotate270;
  switch (orientation) {
    case 0:
      rotation_mode = libyuv::kRotate270;
      break;
    case 90:
    case -270:
      rotation_mode = libyuv::kRotate180;
      break;
    case 180:
      rotation_mode = libyuv::kRotate90;
      break;
    case 270:
    case -90:
      rotation_mode = libyuv::kRotate0;
      break;
    default:
      LOG(ERROR) << " Invalid orientation from RenderViewHostImpl : " << orientation;
  }

  if (rotation_mode == libyuv::kRotate90 || rotation_mode == libyuv::kRotate270) {
    destination_width = frame->height;
    destination_height = frame->width;
  }
#endif

  const gfx::Size dimensions(destination_width, destination_height);

  uint8* yplane = reinterpret_cast<uint8*>(self->buffer_->data());
  uint8* uplane = yplane + VideoFrame::PlaneAllocationSize(VideoFrame::I420,
                                                           VideoFrame::kYPlane,
                                                           dimensions);
  uint8* vplane = uplane + VideoFrame::PlaneAllocationSize(VideoFrame::I420,
                                                           VideoFrame::kUPlane,
                                                           dimensions);
  int yplane_stride = dimensions.width();
  int uv_plane_stride = yplane_stride / 2;
  int frame_size =
      frame->data.double_plane.y_size + frame->data.double_plane.uv_size;

  libyuv::ConvertToI420(frame->data.double_plane.y, frame_size,
                        yplane, yplane_stride,
                        uplane, uv_plane_stride,
                        vplane, uv_plane_stride,
                        0 /*crop_x*/, 0 /*crop_y*/,
                        frame->width, frame->height,
                        frame->width, frame->height,
                        rotation_mode,
                        libyuv::FOURCC_NV12);

  VideoCaptureFormat capture_format_;
  capture_format_.frame_size.SetSize(destination_width, destination_height);
  capture_format_.frame_rate = current_fps;
  capture_format_.pixel_format = PIXEL_FORMAT_I420;
  self->client_->OnIncomingCapturedData(
      reinterpret_cast<uint8*>(self->buffer_->data()),
      frame_size,
      capture_format_,
      libyuv::kRotate0,
      base::TimeTicks::Now());
}

void VideoCaptureDeviceTizen::OnAllocateAndStart(int width,
                                                 int height,
                                                 int frame_rate,
                                                 VideoPixelFormat format,
                                                 scoped_ptr<Client> client) {
  DCHECK_EQ(worker_.message_loop(), base::MessageLoop::current());

  client_ = client.Pass();
  if (CAMERA_ERROR_NONE !=
      camera_create(DeviceNameToCameraId(device_name_), &camera_)) {
    LOG(ERROR) << "Fail to create camera";
    SetErrorState("Fail to create camera");
    return;
  }

  if (CAMERA_ERROR_NONE !=
      camera_set_display(camera_, CAMERA_DISPLAY_TYPE_NONE, NULL)) {
    LOG(ERROR) << "Fail to set using camera buffer";
    SetErrorState("Camera internal Error");
    return;
  }

  if (CAMERA_ERROR_NONE !=
      camera_set_preview_resolution(camera_, width, height)) {
    LOG(WARNING) << "Camera does not support "
                 << width << " x " << height << " resolution.";
    LOG(WARNING) << "trying default resolution: "
                 << kMjpegWidth << " x " << kMjpegHeight;
    if (CAMERA_ERROR_NONE !=
        camera_set_preview_resolution(camera_, kMjpegWidth, kMjpegHeight)) {
      SetErrorState("Camera internal Error");
      return;
    }
  }

  if (CAMERA_ERROR_NONE !=
      camera_set_preview_format(camera_, toCapiType(format))) {
    LOG(WARNING) << "Camera does not support file format :"
                 << toCapiType(format);
    LOG(WARNING) << "trying default format: " << kTypicalFormat;
    if (CAMERA_ERROR_NONE != camera_set_preview_format(
        camera_, (camera_pixel_format_e)kTypicalFormat)) {
      SetErrorState("Camera internal Error");
      return;
    }
  }

  if (CAMERA_ERROR_NONE !=
      camera_set_preview_cb(camera_, OnCameraCaptured, this)) {
    SetErrorState("Camera internal Error");
    return;
  }

  if (CAMERA_ERROR_NONE !=
      camera_attr_set_preview_fps(camera_, (camera_attr_fps_e)frame_rate)) {
    LOG(WARNING) << "Camera does not support frame rate:" <<  frame_rate;
    LOG(WARNING) << "trying default frame rate: " << kTypicalFramerate;
    if (CAMERA_ERROR_NONE !=
        camera_attr_set_preview_fps(camera_,
                                    (camera_attr_fps_e)kTypicalFramerate)) {
      SetErrorState("Camera internal Error");
      return;
    }
  }

  if (!AllocateVideoBuffers(width, height)) {
    LOG(ERROR) << "Allocate buffer failed";
    SetErrorState("Camera internal Error");
    return;
  }

  state_ = kCapturing;

  if (CAMERA_ERROR_NONE != camera_start_preview(camera_)) {
    LOG(ERROR) << "Fail to start camera";
    SetErrorState("Camera internal Error");
  }
}

void VideoCaptureDeviceTizen::OnStopAndDeAllocate() {
  DCHECK_EQ(worker_.message_loop(), base::MessageLoop::current());

  camera_stop_preview(camera_);
  camera_destroy(camera_);
  DeAllocateVideoBuffers();

  state_ = kIdle;
  client_.reset();
}

bool VideoCaptureDeviceTizen::AllocateVideoBuffers(int width, int height) {
  buffer_ = client_->ReserveOutputBuffer(media::VideoFrame::I420,
                                         gfx::Size(width,height) );
  if ( buffer_.get() != NULL ) {
    return true;
  }
  return false;
}

void VideoCaptureDeviceTizen::DeAllocateVideoBuffers() {
  /* Nothing to do */
}

void VideoCaptureDeviceTizen::SetErrorState(const std::string& reason) {
  DCHECK(!worker_.IsRunning() ||
         worker_.message_loop() == base::MessageLoop::current());
  state_ = kError;
  client_->OnError(reason);
}

}  // namespace media
