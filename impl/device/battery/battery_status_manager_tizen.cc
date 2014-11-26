// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "battery_status_manager_tizen.h"

#include <vconf/vconf.h>
#include <system/device.h>

namespace device {

namespace {

typedef void (*battery_remaining_time_changed_cb)(int time, void* data);

battery_remaining_time_changed_cb timetofull_changed_cb = NULL;
battery_remaining_time_changed_cb timetoempty_changed_cb = NULL;
void* timetofull_cb_user_data = NULL;
void* timetoempty_cb_user_data = NULL;

void timetofull_vconf_cb(keynode_t* key, void* data) {
  BatteryStatusManagerTizen* bsm =
      static_cast<BatteryStatusManagerTizen*>(data);
  if (!bsm || !timetofull_changed_cb)
    return;

  char* keyname = vconf_keynode_get_name(key);
  if (keyname == NULL)
    return;

  int time = 0;
  if (strcmp(keyname, VCONFKEY_PM_BATTERY_TIMETOFULL) == 0 &&
      bsm->GetRemainingTimeUntilFullyCharged(&time) == DEVICE_ERROR_NONE &&
      time > 0) {
    timetofull_changed_cb(time, timetofull_cb_user_data);
  }

  free(keyname);
}

void timetoempty_vconf_cb(keynode_t* key, void* data) {
  BatteryStatusManagerTizen* bsm =
      static_cast<BatteryStatusManagerTizen*>(data);

  if (!bsm || !timetoempty_changed_cb)
    return;

  char* keyname = vconf_keynode_get_name(key);
  if (keyname == NULL)
    return;

  int time = 0;
  if (strcmp(keyname, VCONFKEY_PM_BATTERY_TIMETOEMPTY) == 0 &&
      bsm->GetRemainingTimeUntilDischarged(&time) == DEVICE_ERROR_NONE &&
      time > 0) {
    timetoempty_changed_cb(time, timetoempty_cb_user_data);
  }

  free(keyname);
}

void OnChargingStatusChanged(keynode_t* key, void* data) {
  BatteryStatusManagerTizen* bsm =
      static_cast<BatteryStatusManagerTizen*>(data);
  if (!bsm)
    return;

  int charging = 0;
  if (vconf_get_int(VCONFKEY_SYSMAN_BATTERY_CHARGE_NOW, &charging) < 0)
    return;

  BatteryStatus current_status = bsm->GetCurrentStatus();
  current_status.charging = charging > 0;
  current_status.charging_time = std::numeric_limits<double>::infinity();
  current_status.discharging_time = std::numeric_limits<double>::infinity();

  int left_time = 0;
  if (current_status.charging) {
    bsm->GetRemainingTimeUntilFullyCharged(&left_time);
    if (left_time > 0)
      current_status.charging_time = left_time;
  } else {
    bsm->GetRemainingTimeUntilDischarged(&left_time);
    if (left_time > 0)
      current_status.discharging_time = left_time;
  }

  bsm->SetCurrentStatus(current_status);
  bsm->GetBatteryUpdateCallback().Run(current_status);
}

void OnLevelChanged(int percent, void* data) {
  BatteryStatusManagerTizen* bsm =
      static_cast<BatteryStatusManagerTizen*>(data);
  if (!bsm)
    return;

  BatteryStatus current_status = bsm->GetCurrentStatus();
  current_status.level = percent / 100.0;
  bsm->SetCurrentStatus(current_status);
  bsm->GetBatteryUpdateCallback().Run(current_status);
}

void OnChargingTimeChanged(int time, void* data) {
  BatteryStatusManagerTizen* bsm =
      static_cast<BatteryStatusManagerTizen*>(data);
  if (!bsm)
    return;

  BatteryStatus current_status = bsm->GetCurrentStatus();
  current_status.charging_time = time;
  bsm->SetCurrentStatus(current_status);
  bsm->GetBatteryUpdateCallback().Run(current_status);
}

void OnDischargingTimeChanged(int time, void* data) {
  BatteryStatusManagerTizen* bsm =
      static_cast<BatteryStatusManagerTizen*>(data);
  if (!bsm)
    return;

  BatteryStatus current_status = bsm->GetCurrentStatus();
  current_status.discharging_time = time;
  bsm->SetCurrentStatus(current_status);
  bsm->GetBatteryUpdateCallback().Run(current_status);
}

int SetRemainingTimeUntilFullyChargedCb(
    battery_remaining_time_changed_cb callback, void* data) {
  if (callback == NULL)
    return DEVICE_ERROR_INVALID_PARAMETER;

  if (vconf_notify_key_changed(VCONFKEY_PM_BATTERY_TIMETOFULL,
                               timetofull_vconf_cb, NULL) < 0)
    return DEVICE_ERROR_OPERATION_FAILED;

  timetofull_changed_cb = callback;
  timetofull_cb_user_data = data;
  return DEVICE_ERROR_NONE;
}

int SetRemainingTimeUntilDischargedCb(
    battery_remaining_time_changed_cb callback, void* data) {
  if (callback == NULL)
    return DEVICE_ERROR_INVALID_PARAMETER;

  if (vconf_notify_key_changed(VCONFKEY_PM_BATTERY_TIMETOEMPTY,
                               timetoempty_vconf_cb, NULL) < 0)
    return DEVICE_ERROR_OPERATION_FAILED;

  timetoempty_changed_cb = callback;
  timetoempty_cb_user_data = data;
  return DEVICE_ERROR_NONE;
}

int UnsetRemainingTimeUntilFullyChargedCb() {
  if (vconf_ignore_key_changed(VCONFKEY_PM_BATTERY_TIMETOFULL,
                               timetofull_vconf_cb) < 0)
    return DEVICE_ERROR_OPERATION_FAILED;

  timetofull_changed_cb = NULL;
  timetofull_cb_user_data = NULL;
  return DEVICE_ERROR_NONE;
}

int UnsetRemainingTimeUntilDischargedCb() {
  if (vconf_ignore_key_changed(VCONFKEY_PM_BATTERY_TIMETOEMPTY,
                               timetoempty_vconf_cb) < 0)
    return DEVICE_ERROR_OPERATION_FAILED;

  timetoempty_changed_cb = NULL;
  timetoempty_cb_user_data = NULL;
  return DEVICE_ERROR_NONE;
}

}  // namespace

scoped_ptr<BatteryStatusManager> BatteryStatusManager::Create(
    const BatteryStatusService::BatteryUpdateCallback& callback) {
  return scoped_ptr<BatteryStatusManager>(
      new BatteryStatusManagerTizen(callback));
}

BatteryStatusManagerTizen::BatteryStatusManagerTizen(
    const BatteryStatusService::BatteryUpdateCallback& callback)
    : callback_(callback) {
}

BatteryStatusManagerTizen::~BatteryStatusManagerTizen() {
}

int BatteryStatusManagerTizen::GetRemainingTimeUntilFullyCharged(
    int* time) const {
  DCHECK(time != NULL);
  bool charging = false;
  if (device_battery_is_charging(&charging) != DEVICE_ERROR_NONE ||
      charging == false)
    return DEVICE_ERROR_OPERATION_FAILED;

  int temp_time = 0;
  if (vconf_get_int(VCONFKEY_PM_BATTERY_TIMETOFULL, &temp_time) < 0 ||
      temp_time < 0)
    return DEVICE_ERROR_OPERATION_FAILED;

  *time = temp_time;
  return DEVICE_ERROR_NONE;
}

int BatteryStatusManagerTizen::GetRemainingTimeUntilDischarged(
    int* time) const {
  DCHECK(time != NULL);
  bool charging = false;
  if (device_battery_is_charging(&charging) != DEVICE_ERROR_NONE ||
      charging == true)
    return DEVICE_ERROR_OPERATION_FAILED;

  int temp_time = 0;
  if (vconf_get_int(VCONFKEY_PM_BATTERY_TIMETOEMPTY, time) < 0 ||
      time < 0)
    return DEVICE_ERROR_OPERATION_FAILED;

  *time = temp_time;
  return DEVICE_ERROR_NONE;
}

bool BatteryStatusManagerTizen::StartListeningBatteryChange() {
  if (vconf_notify_key_changed(
          VCONFKEY_SYSMAN_BATTERY_CHARGE_NOW, OnChargingStatusChanged, this) ||
      device_battery_set_cb(OnLevelChanged, this) != DEVICE_ERROR_NONE ||
      SetRemainingTimeUntilFullyChargedCb(OnChargingTimeChanged, this)
          != DEVICE_ERROR_NONE ||
      SetRemainingTimeUntilDischargedCb(OnDischargingTimeChanged, this)
          != DEVICE_ERROR_NONE) {
    StopListeningBatteryChange();
    return false;
  }

  int charging = 0;
  if (vconf_get_int(VCONFKEY_SYSMAN_BATTERY_CHARGE_NOW, &charging) < 0)
    return false;

  int level = 0, charging_time = 0, discharging_time = 0;
  device_battery_get_percent(&level);
  GetRemainingTimeUntilFullyCharged(&charging_time);
  GetRemainingTimeUntilDischarged(&discharging_time);

  current_status_.charging = charging > 0;
  current_status_.level = level / 100.0;
  current_status_.charging_time = charging_time > 0 ? charging_time :
      std::numeric_limits<double>::infinity();
  current_status_.discharging_time = discharging_time > 0 ? discharging_time :
      std::numeric_limits<double>::infinity();

  callback_.Run(current_status_);
  return true;
}

void BatteryStatusManagerTizen::StopListeningBatteryChange() {
  UnsetRemainingTimeUntilFullyChargedCb();
  UnsetRemainingTimeUntilDischargedCb();
  vconf_ignore_key_changed(VCONFKEY_SYSMAN_BATTERY_CHARGE_NOW,
      OnChargingStatusChanged);
  device_battery_unset_cb();
}

}  // namespace device
