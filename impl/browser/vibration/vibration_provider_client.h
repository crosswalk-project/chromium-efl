// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef Vibration_Provider_Client_h
#define Vibration_Provider_Client_h

#include "base/memory/scoped_ptr.h"
#include "tizen_webview/public/tw_callbacks.h"

/**
 * \struct  Ewk_Vibration_Client
 * @brief   Contains the vibration client callbacks.
 */
struct Ewk_Vibration_Client {
 public:
  Ewk_Vibration_Client(tizen_webview::Vibration_Client_Vibrate_Cb vibrate,
                       tizen_webview::Vibration_Client_Vibration_Cancel_Cb cancelVibration,
                       void* user_data)
    : vibrate_(vibrate),
      cancel_vibration_(cancelVibration),
      user_data_(user_data) {}

  tizen_webview::Vibration_Client_Vibrate_Cb vibrate_;
  tizen_webview::Vibration_Client_Vibration_Cancel_Cb cancel_vibration_;
  void* user_data_;
};

class VibrationProviderClient {
 public:
  static VibrationProviderClient* GetInstance();
  static void DeleteInstance();
  void Vibrate(uint64_t vibrationTime);
  void CancelVibration();
  void SetVibrationClientCallbacks(tizen_webview::Vibration_Client_Vibrate_Cb,
                                   tizen_webview::Vibration_Client_Vibration_Cancel_Cb,
                                   void*);

 private:
  VibrationProviderClient() {}
  ~VibrationProviderClient() {}
  static VibrationProviderClient* vibration_provider_client_;
  scoped_ptr<Ewk_Vibration_Client> vibration_client_;

  DISALLOW_COPY_AND_ASSIGN(VibrationProviderClient);
};

#endif // Vibration_Provider_Client_h
