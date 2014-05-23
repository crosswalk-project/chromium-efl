/*
   Copyright (C) 2014 Samsung Electronics

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

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

DataFetcherSharedMemory::FetcherType DataFetcherSharedMemory::GetType() const {
  return FETCHER_TYPE_SEPARATE_THREAD;
}

}  // namespace content
