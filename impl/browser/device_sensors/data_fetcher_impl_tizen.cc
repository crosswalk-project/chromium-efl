// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "browser/device_sensors/data_fetcher_impl_tizen.h"

#include "base/logging.h"
#include "base/memory/singleton.h"

namespace content {

DataFetcherImplTizen::DataFetcherImplTizen()
    : device_motion_buffer_(NULL),
      device_orientation_buffer_(NULL),
      has_last_motion_data_(false),
      last_motion_timestamp_(0),
      is_orientation_buffer_ready_(false) {
  // FIXME: sensor API's changed in Tizen 2.3, we have to adapt.
#if !defined(EWK_BRINGUP)
  sensor_create(&handle_);
  sensor_orientation_set_cb(handle_, kInertialSensorIntervalMillis,
      DataFetcherImplTizen::onOrientationChanged, this);
  sensor_accelerometer_set_cb(handle_, kInertialSensorIntervalMillis,
      DataFetcherImplTizen::onAccelerationChanged, this);
#endif
}

DataFetcherImplTizen::~DataFetcherImplTizen() {
#if !defined(EWK_BRINGUP)
  sensor_destroy(handle_);
#endif
}

DataFetcherImplTizen* DataFetcherImplTizen::GetInstance() {
  return Singleton<DataFetcherImplTizen,
                   LeakySingletonTraits<DataFetcherImplTizen> >::get();
}

#if !defined(EWK_BRINGUP)

bool DataFetcherImplTizen::StartFetchingDeviceMotionData(
    DeviceMotionHardwareBuffer* buffer) {
  DCHECK(buffer);
  {
    base::AutoLock autolock(motion_buffer_lock_);
    device_motion_buffer_ = buffer;
  }
  return Start(CONSUMER_TYPE_MOTION);
}

void DataFetcherImplTizen::StopFetchingDeviceMotionData() {
  Stop(CONSUMER_TYPE_MOTION);
  {
    base::AutoLock autolock(motion_buffer_lock_);
    if (device_motion_buffer_)
      device_motion_buffer_ = NULL;
  }
}

bool DataFetcherImplTizen::StartFetchingDeviceOrientationData(
    DeviceOrientationHardwareBuffer* buffer) {
  DCHECK(buffer);
  {
    base::AutoLock autolock(orientation_buffer_lock_);
    device_orientation_buffer_ = buffer;
  }
  bool success = Start(CONSUMER_TYPE_ORIENTATION);

  {
    base::AutoLock autolock(orientation_buffer_lock_);
    // If Start() was unsuccessful then set the buffer ready flag to true
    // to start firing all-null events.
    SetOrientationBufferReadyStatus(!success);
  }
  return success;
}

void DataFetcherImplTizen::StopFetchingDeviceOrientationData() {
  Stop(CONSUMER_TYPE_ORIENTATION);
  {
    base::AutoLock autolock(orientation_buffer_lock_);
    if (device_orientation_buffer_) {
      SetOrientationBufferReadyStatus(false);
      device_orientation_buffer_ = NULL;
    }
  }
}

bool DataFetcherImplTizen::Start(ConsumerType type) {
  switch(type) {
  case CONSUMER_TYPE_ORIENTATION:
    return (SENSOR_ERROR_NONE == sensor_start(handle_, SENSOR_ORIENTATION));
  case CONSUMER_TYPE_MOTION:
    if (SENSOR_ERROR_NONE != sensor_start(handle_, SENSOR_ACCELEROMETER)) {
      return false;
    }
    return (SENSOR_ERROR_NONE == sensor_start(handle_, SENSOR_GYROSCOPE));
  default:
    NOTREACHED();
    return false;
  }
}

void DataFetcherImplTizen::Stop(ConsumerType type) {
  switch(type) {
  case CONSUMER_TYPE_ORIENTATION:
    sensor_stop(handle_, SENSOR_ORIENTATION);
    return;
  case CONSUMER_TYPE_MOTION:
    sensor_stop(handle_, SENSOR_ACCELEROMETER);
    sensor_stop(handle_, SENSOR_GYROSCOPE);
    memset(&last_motion_data_, 0, sizeof(last_motion_data_));
    has_last_motion_data_ = false;
    return;
  default:
    NOTREACHED();
    return;
  }
}

//static
void DataFetcherImplTizen::onOrientationChanged(unsigned long long timestamp,
    sensor_data_accuracy_e, float azimuth, float pitch, float roll, void* userData) {
  DataFetcherImplTizen *fetcher = static_cast<DataFetcherImplTizen*>(userData);
  base::AutoLock autolock(fetcher->orientation_buffer_lock_);

  if (!fetcher->device_orientation_buffer_)
    return;

  fetcher->device_orientation_buffer_->seqlock.WriteBegin();
  fetcher->device_orientation_buffer_->data.alpha = azimuth;
  fetcher->device_orientation_buffer_->data.hasAlpha = true;
  fetcher->device_orientation_buffer_->data.beta = pitch;
  fetcher->device_orientation_buffer_->data.hasBeta = true;
  fetcher->device_orientation_buffer_->data.gamma = roll;
  fetcher->device_orientation_buffer_->data.hasGamma = true;
  fetcher->device_orientation_buffer_->seqlock.WriteEnd();

  if (!fetcher->is_orientation_buffer_ready_)
    fetcher->SetOrientationBufferReadyStatus(true);
}

//static
void DataFetcherImplTizen::onAccelerationChanged(unsigned long long timestamp,
    sensor_data_accuracy_e, float x, float y, float z, void* userData) {
  DataFetcherImplTizen *self = static_cast<DataFetcherImplTizen*>(userData);

  float gravityX = x * 0.2f;
  float gravityY = y * 0.2f;
  float gravityZ = z * 0.2f;
  bool accelerationAvailable = false;

  double interval = static_cast<double>(self->last_motion_timestamp_ ?
                  (timestamp - self->last_motion_timestamp_) / 1000 :
                  kInertialSensorIntervalMillis);
  self->last_motion_timestamp_ = timestamp;

  if (self->has_last_motion_data_) {
    const blink::WebDeviceMotionData* m = &self->last_motion_data_;
    gravityX += (m->accelerationIncludingGravityX - m->accelerationX) * 0.8f;
    gravityY += (m->accelerationIncludingGravityY - m->accelerationY) * 0.8f;
    gravityZ += (m->accelerationIncludingGravityZ - m->accelerationZ) * 0.8f;
    accelerationAvailable = true;
  }

  float alpha, beta, gamma;
  bool rotationRateAvailable = false;
  if (!sensor_gyroscope_read_data(self->handle_, 0, &alpha, &beta, &gamma))
    rotationRateAvailable = true;

  self->device_motion_buffer_->seqlock.WriteBegin();

  self->device_motion_buffer_->data.accelerationIncludingGravityX = x;
  self->device_motion_buffer_->data.hasAccelerationIncludingGravityX = true;
  self->device_motion_buffer_->data.accelerationIncludingGravityY = y;
  self->device_motion_buffer_->data.hasAccelerationIncludingGravityY = true;
  self->device_motion_buffer_->data.accelerationIncludingGravityZ = z;
  self->device_motion_buffer_->data.hasAccelerationIncludingGravityZ = true;

  self->device_motion_buffer_->data.accelerationX = x - gravityX;
  self->device_motion_buffer_->data.hasAccelerationX = accelerationAvailable;
  self->device_motion_buffer_->data.accelerationY = y - gravityY;
  self->device_motion_buffer_->data.hasAccelerationY = accelerationAvailable;
  self->device_motion_buffer_->data.accelerationZ = z - gravityZ;
  self->device_motion_buffer_->data.hasAccelerationZ = accelerationAvailable;

  self->device_motion_buffer_->data.rotationRateAlpha = alpha;
  self->device_motion_buffer_->data.hasRotationRateAlpha = rotationRateAvailable;
  self->device_motion_buffer_->data.rotationRateBeta = beta;
  self->device_motion_buffer_->data.hasRotationRateBeta = rotationRateAvailable;
  self->device_motion_buffer_->data.rotationRateGamma = gamma;
  self->device_motion_buffer_->data.hasRotationRateGamma = rotationRateAvailable;

  self->device_motion_buffer_->data.allAvailableSensorsAreActive =
      (accelerationAvailable && rotationRateAvailable);

  self->last_motion_data_ = self->device_motion_buffer_->data;
  self->has_last_motion_data_ = true;

  self->device_motion_buffer_->data.interval = interval;

  self->device_motion_buffer_->seqlock.WriteEnd();
}

void DataFetcherImplTizen::SetOrientationBufferReadyStatus(bool ready) {
  device_orientation_buffer_->seqlock.WriteBegin();
  device_orientation_buffer_->data.absolute = ready;
  device_orientation_buffer_->data.hasAbsolute = ready;
  device_orientation_buffer_->data.allAvailableSensorsAreActive = ready;
  device_orientation_buffer_->seqlock.WriteEnd();
  is_orientation_buffer_ready_ = ready;
}

#else

bool DataFetcherImplTizen::StartFetchingDeviceMotionData(DeviceMotionHardwareBuffer* buffer) { return false; }
void DataFetcherImplTizen::StopFetchingDeviceMotionData() {}
bool DataFetcherImplTizen::StartFetchingDeviceOrientationData(DeviceOrientationHardwareBuffer* buffer) { return false; }
void DataFetcherImplTizen::StopFetchingDeviceOrientationData() {}
bool DataFetcherImplTizen::Start(ConsumerType) {}
void DataFetcherImplTizen::Stop(ConsumerType) {}

#endif // EWK_BRINGUP

} // namespace content
