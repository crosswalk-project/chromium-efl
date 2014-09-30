// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ewk_geolocation_private_h
#define ewk_geolocation_private_h

#include <base/callback.h>
#include "tizen_webview/public/tw_security_origin.h"

// This holds the geolocation permission request data.
// The callback present is the direct engine callback which need
// to be called once the permission is determined by app.
class _Ewk_Geolocation_Permission_Request {
 public:
  _Ewk_Geolocation_Permission_Request(const char* host, const char* protocol, uint16_t port, base::Callback<void(bool)> inCallback);
  ~_Ewk_Geolocation_Permission_Request();

  bool isDecided() const { return isDecided_; }
  bool isSuspended() const { return isSuspended_; }
  void Suspend();
  bool SetDecision(Eina_Bool allow);
  tizen_webview::Security_Origin* GetOrigin() const { return origin_; }

 private:
  tizen_webview::Security_Origin* origin_;
  bool isDecided_;
  bool isSuspended_;
  base::Callback<void(bool)> callback_;

};

#endif // ewk_geolocation_private_h
