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
 *
 */

#ifndef Vibration_Provider_Client_h
#define Vibration_Provider_Client_h

#include "base/memory/scoped_ptr.h"
#include "public/ewk_context.h"

/**
 * \struct  Ewk_Vibration_Client
 * @brief   Contains the vibration client callbacks.
 */
struct Ewk_Vibration_Client {
 public:
  Ewk_Vibration_Client(Ewk_Vibration_Client_Vibrate_Cb vibrate,
                       Ewk_Vibration_Client_Vibration_Cancel_Cb cancelVibration,
                       void* user_data)
    : vibrate_(vibrate),
      cancel_vibration_(cancelVibration),
      user_data_(user_data) {}

  Ewk_Vibration_Client_Vibrate_Cb vibrate_;
  Ewk_Vibration_Client_Vibration_Cancel_Cb cancel_vibration_;
  void* user_data_;
};

class VibrationProviderClient {
 public:
  static VibrationProviderClient* GetInstance();
  static void DeleteInstance();
  void Vibrate(uint64_t vibrationTime);
  void CancelVibration();
  void SetVibrationClientCallbacks(Ewk_Vibration_Client_Vibrate_Cb,
                                   Ewk_Vibration_Client_Vibration_Cancel_Cb,
                                   void*);

 private:
  VibrationProviderClient() {}
  ~VibrationProviderClient() {}
  static VibrationProviderClient* vibration_provider_client_;
  scoped_ptr<Ewk_Vibration_Client> vibration_client_;

  DISALLOW_COPY_AND_ASSIGN(VibrationProviderClient);
};

#endif // Vibration_Provider_Client_h
