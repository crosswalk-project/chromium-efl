// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ewk_geolocation_private_h
#define ewk_geolocation_private_h

#include <Eina.h>
#include <Evas.h>

#include <base/callback.h>
#include "tizen_webview/public/tw_security_origin.h"

// This holds the geolocation permission request data.
// The callback present is the direct engine callback which need
// to be called once the permission is determined by app.
struct _Ewk_Geolocation_Permission_Request {
  Evas_Object* ewkView;
  tizen_webview::Security_Origin* origin;
  bool isDecided;
  bool isSuspended;
  base::Callback<void(bool)> callback;

  _Ewk_Geolocation_Permission_Request(Evas_Object* ewkView, const char* host, const char* protocol, uint16_t port, base::Callback<void(bool)> inCallback);
  ~_Ewk_Geolocation_Permission_Request();
};

#endif // ewk_geolocation_private_h
