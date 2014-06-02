/*
 * Copyright (C) 2013 Samsung Electronics. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 */

#include "browser/vibration/vibration_provider_client.h"

VibrationProviderClient* VibrationProviderClient::vibration_provider_client_ = NULL;

VibrationProviderClient* VibrationProviderClient::GetInstance() {
  if (!vibration_provider_client_)
    vibration_provider_client_ = new VibrationProviderClient;
  return vibration_provider_client_;
}

void VibrationProviderClient::DeleteInstance() {
  if (vibration_provider_client_)
    delete vibration_provider_client_;
  vibration_provider_client_ = NULL;
}

void VibrationProviderClient::Vibrate(uint64_t vibration_time) {
  if (vibration_client_.get() && vibration_client_->vibrate_)
    vibration_client_->vibrate_(vibration_time, vibration_client_->user_data_);
}

void VibrationProviderClient::CancelVibration() {
  if (vibration_client_.get() && vibration_client_->cancel_vibration_)
    vibration_client_->cancel_vibration_(vibration_client_->user_data_);
}

void VibrationProviderClient::SetVibrationClientCallbacks(tizen_webview::Vibration_Client_Vibrate_Cb vibrate,
                                                          tizen_webview::Vibration_Client_Vibration_Cancel_Cb cancelVibration,
                                                          void* data) {
  vibration_client_.reset(new Ewk_Vibration_Client(vibrate, cancelVibration, data));
}
