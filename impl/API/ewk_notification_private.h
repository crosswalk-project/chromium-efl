// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
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

#ifndef TIZEN_WEBVIEW_PRIVATE_TW_NOTIFICATION_IMPL_H_
#define TIZEN_WEBVIEW_PRIVATE_TW_NOTIFICATION_IMPL_H_

#include <Eina.h>
#include <Evas.h>

#include "base/strings/utf_string_conversions.h"
#include "content/public/common/show_desktop_notification_params.h"

#include "eweb_view.h"
#include "eweb_context.h"

namespace tizen_webview {
class Security_Origin;
class URL;
}

struct _Ewk_Notification {
  std::string body_;
  std::string iconURL_;
  std::string replaceID_;
  std::string title_;
  uint64_t notificationID_;
  tizen_webview::Security_Origin* securityOrigin_;

  _Ewk_Notification(const std::string& body,
                    const std::string& iconURL,
                    const std::string& replaceID,
                    const std::string& title,
                    uint64_t notificationID,
                    const tizen_webview::URL& securityOrigin);
  ~_Ewk_Notification();
};

struct _Ewk_Notification_Permission_Request {
  Evas_Object* webview_;
  tizen_webview::Security_Origin* origin_;
  bool decided_;
  bool suspended_;
  int callback_context_; // required by content API

  _Ewk_Notification_Permission_Request(Evas_Object* webview,
      int callback_context, const tizen_webview::URL& source_origin);
  ~_Ewk_Notification_Permission_Request();
};

#endif // TIZEN_WEBVIEW_PRIVATE_TW_NOTIFICATION_IMPL_H_
