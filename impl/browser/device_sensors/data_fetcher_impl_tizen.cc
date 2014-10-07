// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "browser/device_sensors/data_fetcher_impl_tizen.h"

#include "base/logging.h"
#include "base/memory/singleton.h"

namespace content {

#if !defined(TIZEN_LEGACY_V_2_2_1)
static sensor_h sensor_orientation_;
static sensor_h sensor_accelerometer_;
static sensor_h sensor_gyroscope_;
static sensor_listener_h listener_orientation_;
static sensor_listener_h listener_accelerometer_;
static sensor_listener_h listener_gyroscope_;
#endif

DataFetcherImplTizen::DataFetcherImplTizen()
    : device_motion_buffer_(NULL),
      device_orientation_buffer_(NULL),
      has_last_motion_data_(false),
      last_motion_timestamp_(0),
      is_orientation_buffer_ready_(false) {
#if !defined(TIZEN_LEGACY_V_2_2_1)
  sensor_get_default_sensor(SENSOR_ORIENTATION, &sensor_orientation_);
  sensor_get_default_sensor(SENSOR_ACCELEROMETER, &sensor_accelerometer_);
  sensor_get_default_sensor(SENSOR_GYROSCOPE, &sensor_gyroscope_);

  sensor_create_listener(sensor_orientation_, &listener_orientation_);
  sensor_create_listener(sensor_accelerometer_, &listener_accelerometer_);
  sensor_create_listener(sensor_gyroscope_, &listener_gyroscope_);
#else
  sensor_create(&handle_);
  sensor_orientation_set_cb(handle_,
      kInertialSensorIntervalMicroseconds / 1000,
      DataFetcherImplTizen::onOrientationChanged, this);
#endif
}

DataFetcherImplTizen::~DataFetcherImplTizen() {
#if !defined(TIZEN_LEGACY_V_2_2_1)
  sensor_listener_unset_event_cb(listener_orientation_);
  sensor_listener_stop(listener_orientation_);
  sensor_destroy_listener(listener_orientation_);

  sensor_listener_unset_event_cb(listener_accelerometer_);
  sensor_listener_stop(listener_accelerometer_);
  sensor_destroy_listener(listener_accelerometer_);

  sensor_listener_stop(listener_gyroscope_);
  sensor_destroy_listener(listener_gyroscope_);
#else
  sensor_destroy(handle_);
#endif
}

DataFetcherImplTizen* DataFetcherImplTizen::GetInstance() {
  return Singleton<DataFetcherImplTizen,
                   LeakySingletonTraits<DataFetcherImplTizen> >::get();
}

bool DataFetcherImplTizen::StartFetchingDeviceMotionData(
    DeviceMotionHardwareBuffer* buffer) {
  DCHECK(buffer);
  {
    base::AutoLock autolock(motion_buffer_lock_);
    device_motion_buffer_ = buffer;
  }
#if !defined(TIZEN_LEGACY_V_2_2_1)
  sensor_listener_set_event_cb(listener_accelerometer_,
      kInertialSensorIntervalMicroseconds / 1000,
      DataFetcherImplTizen::onAccelerationChanged, this);
#else
  sensor_accelerometer_set_cb(handle_,
      kInertialSensorIntervalMicroseconds / 1000,
      DataFetcherImplTizen::onAccelerationChanged, this);
#endif
  return Start(CONSUMER_TYPE_MOTION);
}

void DataFetcherImplTizen::StopFetchingDeviceMotionData() {
  Stop(CONSUMER_TYPE_MOTION);
  {
    base::AutoLock autolock(motion_buffer_lock_);
    if (device_motion_buffer_) {
#if !defined(TIZEN_LEGACY_V_2_2_1)
      sensor_listener_unset_event_cb(listener_accelerometer_);
#else
      sensor_accelerometer_set_cb(handle_,
          kInertialSensorIntervalMicroseconds / 1000, NULL, this);
#endif
      device_motion_buffer_ = NULL;
    }
  }
}

bool DataFetcherImplTizen::StartFetchingDeviceOrientationData(
    DeviceOrientationHardwareBuffer* buffer) {
  DCHECK(buffer);
  {
    base::AutoLock autolock(orientation_buffer_lock_);
    device_orientation_buffer_ = buffer;
  }
#if !defined(TIZEN_LEGACY_V_2_2_1)
  sensor_listener_set_event_cb(listener_orientation_,
      kInertialSensorIntervalMicroseconds / 1000,
      DataFetcherImplTizen::onOrientationChanged, this);
#else
  sensor_orientation_set_cb(handle_,
      kInertialSensorIntervalMicroseconds / 1000,
      DataFetcherImplTizen::onOrientationChanged, this);
#endif
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
#if !defined(TIZEN_LEGACY_V_2_2_1)
      sensor_listener_unset_event_cb(listener_orientation_);
#else
      sensor_orientation_set_cb(handle_,
          kInertialSensorIntervalMicroseconds / 1000, NULL, this);
#endif
      device_orientation_buffer_ = NULL;
    }
  }
}

bool DataFetcherImplTizen::Start(ConsumerType type) {
  switch(type) {
#if !defined(TIZEN_LEGACY_V_2_2_1)
  case CONSUMER_TYPE_ORIENTATION:
    return (SENSOR_ERROR_NONE == sensor_listener_start(listener_orientation_));
  case CONSUMER_TYPE_MOTION:
    if (SENSOR_ERROR_NONE != sensor_listener_start(listener_accelerometer_)) {
      return false;
    }
    return (SENSOR_ERROR_NONE == sensor_listener_start(listener_gyroscope_));
#else
  case CONSUMER_TYPE_ORIENTATION:
    return (SENSOR_ERROR_NONE == sensor_start(handle_, SENSOR_ORIENTATION));
  case CONSUMER_TYPE_MOTION:
    if (SENSOR_ERROR_NONE != sensor_start(handle_, SENSOR_ACCELEROMETER)) {
      return false;
    }
    return (SENSOR_ERROR_NONE == sensor_start(handle_, SENSOR_GYROSCOPE));
#endif
  default:
    NOTREACHED();
    return false;
  }

}

void DataFetcherImplTizen::Stop(ConsumerType type) {
  switch(type) {
  case CONSUMER_TYPE_ORIENTATION:
#if !defined(TIZEN_LEGACY_V_2_2_1)
    sensor_listener_stop(listener_orientation_);
#else
    sensor_stop(handle_, SENSOR_ORIENTATION);
#endif
    return;
  case CONSUMER_TYPE_MOTION:
#if !defined(TIZEN_LEGACY_V_2_2_1)
    sensor_listener_stop(listener_accelerometer_);
    sensor_listener_stop(listener_gyroscope_);
#else
    sensor_stop(handle_, SENSOR_ACCELEROMETER);
    sensor_stop(handle_, SENSOR_GYROSCOPE);
#endif
    memset(&last_motion_data_, 0, sizeof(last_motion_data_));
    has_last_motion_data_ = false;
    return;
  default:
    NOTREACHED();
    return;
  }
}

//static
#if !defined(TIZEN_LEGACY_V_2_2_1)
void DataFetcherImplTizen::onOrientationChanged(sensor_h sensor,
    sensor_event_s *event, void* userData) {
#else
void DataFetcherImplTizen::onOrientationChanged(unsigned long long timestamp,
    sensor_data_accuracy_e, float azimuth, float pitch, float roll, void* userData) {
#endif
  DataFetcherImplTizen *fetcher = static_cast<DataFetcherImplTizen*>(userData);
  base::AutoLock autolock(fetcher->orientation_buffer_lock_);

  if (!fetcher->device_orientation_buffer_)
    return;

  fetcher->device_orientation_buffer_->seqlock.WriteBegin();

#if !defined(TIZEN_LEGACY_V_2_2_1)
  float azimuth = event->values[0];
  float pitch = event->values[1];
  float roll = event->values[2];
#endif

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
#if !defined(TIZEN_LEGACY_V_2_2_1)
void DataFetcherImplTizen::onAccelerationChanged(sensor_h sensor,
    sensor_event_s *event, void* userData) {
#else
void DataFetcherImplTizen::onAccelerationChanged(unsigned long long timestamp,
    sensor_data_accuracy_e, float x, float y, float z, void* userData) {
#endif
  DataFetcherImplTizen *self = static_cast<DataFetcherImplTizen*>(userData);

#if !defined(TIZEN_LEGACY_V_2_2_1)
  float x = event->values[0];
  float y = event->values[1];
  float z = event->values[2];
#endif

  float gravityX = x * 0.2f;
  float gravityY = y * 0.2f;
  float gravityZ = z * 0.2f;
  bool accelerationAvailable = false;

#if !defined(TIZEN_LEGACY_V_2_2_1)
  unsigned long long timestamp = event->timestamp;
#endif

  double interval = static_cast<double>(self->last_motion_timestamp_ ?
      (timestamp - self->last_motion_timestamp_) / 1000 :
      kInertialSensorIntervalMicroseconds / 1000);
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

#if !defined(TIZEN_LEGACY_V_2_2_1)
  sensor_event_s event_gyroscope;
  if (!sensor_listener_read_data(listener_gyroscope_, &event_gyroscope))
    rotationRateAvailable = true;

  alpha = event_gyroscope.values[0];
  beta = event_gyroscope.values[1];
  gamma = event_gyroscope.values[2];
#else
  if (!sensor_gyroscope_read_data(self->handle_, 0, &alpha, &beta, &gamma))
    rotationRateAvailable = true;
#endif

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

} // namespace content
