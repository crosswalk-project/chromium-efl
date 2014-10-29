// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DATA_FETCHER_IMPL_TIZEN_H_
#define DATA_FETCHER_IMPL_TIZEN_H_

#ifndef TIZEN_LEGACY_V_2_2_1
#include <sensor/sensor.h>
#else
#include <sensors.h>
#endif

#include "base/memory/scoped_ptr.h"
#include "base/synchronization/lock.h"
#include "content/browser/device_sensors/inertial_sensor_consts.h"
#include "content/common/device_sensors/device_motion_hardware_buffer.h"
#include "content/common/device_sensors/device_orientation_hardware_buffer.h"

template<typename T> struct DefaultSingletonTraits;

namespace content {

class DataFetcherImplTizen {
 public:
  static DataFetcherImplTizen* GetInstance();

  // Shared memory related methods.
  bool StartFetchingDeviceMotionData(DeviceMotionHardwareBuffer* buffer);
  void StopFetchingDeviceMotionData();

  bool StartFetchingDeviceOrientationData(
      DeviceOrientationHardwareBuffer* buffer);
  void StopFetchingDeviceOrientationData();

  virtual bool Start(ConsumerType);
  virtual void Stop(ConsumerType);

 protected:
  DataFetcherImplTizen();
  virtual ~DataFetcherImplTizen();
  static void onOrientationChanged(unsigned long long timestamp,
                                   sensor_data_accuracy_e, float azimuth,
                                   float pitch, float roll, void* userData);
  static void onAccelerationChanged(unsigned long long timestamp,
                                    sensor_data_accuracy_e, float x, float y,
                                    float z, void* userData);

 private:
  friend struct DefaultSingletonTraits<DataFetcherImplTizen>;

  void SetOrientationBufferReadyStatus(bool ready);

  DeviceMotionHardwareBuffer* device_motion_buffer_;
  DeviceOrientationHardwareBuffer* device_orientation_buffer_;
  blink::WebDeviceMotionData last_motion_data_;
  bool has_last_motion_data_;
  unsigned long long last_motion_timestamp_;
  bool is_orientation_buffer_ready_;

  base::Lock motion_buffer_lock_;
  base::Lock orientation_buffer_lock_;

  sensor_h handle_;

  DISALLOW_COPY_AND_ASSIGN(DataFetcherImplTizen);
};

} // namespace content

#endif  // DATA_FETCHER_IMPL_TIZEN_H_
