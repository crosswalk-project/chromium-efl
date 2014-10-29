// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "browser/device_sensors/data_fetcher_impl_tizen.h"

#include "base/logging.h"
#include "content/browser/device_sensors/data_fetcher_shared_memory.h"
#include "content/common/device_sensors/device_motion_hardware_buffer.h"
#include "content/common/device_sensors/device_orientation_hardware_buffer.h"

namespace content {

DataFetcherSharedMemory::DataFetcherSharedMemory() {
}

DataFetcherSharedMemory::~DataFetcherSharedMemory() {
}

bool DataFetcherSharedMemory::Start(ConsumerType consumer_type, void* buffer) {
  DCHECK(buffer);

  switch (consumer_type) {
    case CONSUMER_TYPE_MOTION:
      return DataFetcherImplTizen::GetInstance()->
          StartFetchingDeviceMotionData(
              static_cast<DeviceMotionHardwareBuffer*>(buffer));
    case CONSUMER_TYPE_ORIENTATION:
      return DataFetcherImplTizen::GetInstance()->
          StartFetchingDeviceOrientationData(
              static_cast<DeviceOrientationHardwareBuffer*>(buffer));
    default:
      NOTREACHED();
  }
  return false;
}

bool DataFetcherSharedMemory::Stop(ConsumerType consumer_type) {
  switch (consumer_type) {
    case CONSUMER_TYPE_MOTION:
      DataFetcherImplTizen::GetInstance()->StopFetchingDeviceMotionData();
      return true;
    case CONSUMER_TYPE_ORIENTATION:
      DataFetcherImplTizen::GetInstance()->StopFetchingDeviceOrientationData();
      return true;
    default:
      NOTREACHED();
  }
  return false;
}

#if !defined(EWK_BRINGUP)
DataFetcherSharedMemory::FetcherType DataFetcherSharedMemory::GetType() const {
  return FETCHER_TYPE_SEPARATE_THREAD;
}
#endif

}  // namespace content
