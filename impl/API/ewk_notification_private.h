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

#ifndef ewk_notification_private_h
#define ewk_notification_private_h

#include <Eina.h>
#include <Evas.h>

#include "base/strings/utf_string_conversions.h"
#include "content/public/common/show_desktop_notification_params.h"

#include "eweb_view.h"
#include "eweb_context.h"


namespace tizen_webview {
  class Security_Origin;
}

struct _Ewk_Notification {
  Eina_Stringshare* body;
  Eina_Stringshare* iconURL;
  Eina_Stringshare* replaceID;
  Eina_Stringshare* title;
  uint64_t notificationID;
  tizen_webview::Security_Origin* securityOrigin;

  explicit _Ewk_Notification(const content::ShowDesktopNotificationHostMsgParams& params);

  ~_Ewk_Notification();
};

struct _Ewk_Notification_Permission_Request {
  Evas_Object* ewkView;
  tizen_webview::Security_Origin* origin;
  bool isDecided;
  bool isSuspended;
  int callback_context; // required by content API

  _Ewk_Notification_Permission_Request(Evas_Object* ewkView, int callback, const GURL& source_origin);

  ~_Ewk_Notification_Permission_Request();
};

#endif // ewk_notification_private_h
