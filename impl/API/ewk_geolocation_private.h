/*
 * Copyright (C) 2013 Samsung Electronics
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#ifndef ewk_geolocation_private_h
#define ewk_geolocation_private_h

#include <Eina.h>
#include <Evas.h>

#include <base/callback.h>

#include "API/ewk_security_origin_private.h"

// This holds the geolocation permission request data.
// The callback present is the direct engine callback which need
// to be called once the permission is determined by app.
struct _Ewk_Geolocation_Permission_Request {
  Evas_Object* ewkView;
  _Ewk_Security_Origin* origin;
  bool isDecided;
  bool isSuspended;
  base::Callback<void(bool)> callback;

  _Ewk_Geolocation_Permission_Request(Evas_Object* ewkView, const char* host, const char* protocol, uint16_t port, base::Callback<void(bool)> inCallback)
      : ewkView(ewkView)
      , origin(new _Ewk_Security_Origin (host, protocol, port))
      , isDecided(false)
      , isSuspended(false)
      , callback(inCallback) {
  }

  ~_Ewk_Geolocation_Permission_Request() {
    delete origin;
  }
};

#endif // ewk_geolocation_private_h
