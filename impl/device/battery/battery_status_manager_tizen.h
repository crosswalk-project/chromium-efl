// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DEVICE_BATTERY_BATTERY_STATUS_MANAGER_TIZEN_H_
#define DEVICE_BATTERY_BATTERY_STATUS_MANAGER_TIZEN_H_

#include "device/battery/battery_status_manager.h"

namespace device {

class BatteryStatusManagerTizen : public BatteryStatusManager {
 public:
  explicit BatteryStatusManagerTizen(
      const BatteryStatusService::BatteryUpdateCallback& callback);
  ~BatteryStatusManagerTizen() override;

  BatteryStatus GetCurrentStatus() const { return current_status_; }
  void SetCurrentStatus(BatteryStatus status) { current_status_ = status; }

  BatteryStatusService::BatteryUpdateCallback GetBatteryUpdateCallback() const {
    return callback_;
  }

  int GetRemainingTimeUntilFullyCharged(int* time) const;
  int GetRemainingTimeUntilDischarged(int* time) const;

 private:
  bool StartListeningBatteryChange() override;
  void StopListeningBatteryChange() override;

  BatteryStatus current_status_;
  BatteryStatusService::BatteryUpdateCallback callback_;

  DISALLOW_COPY_AND_ASSIGN(BatteryStatusManagerTizen);
};

}  // namespace content

#endif // DEVICE_BATTERY_BATTERY_STATUS_MANAGER_TIZEN_H_
