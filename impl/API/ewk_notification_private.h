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
#include "API/ewk_security_origin_private.h"

struct _Ewk_Notification {
  Eina_Stringshare* body;
  Eina_Stringshare* iconURL;
  Eina_Stringshare* replaceID;
  Eina_Stringshare* title;
  uint64_t notificationID;
  _Ewk_Security_Origin* securityOrigin;

  explicit _Ewk_Notification(const content::ShowDesktopNotificationHostMsgParams& params)
      : body(eina_stringshare_add((base::UTF16ToUTF8(params.body)).c_str())),
        iconURL(eina_stringshare_add((params.icon_url).spec().c_str())),
        replaceID(eina_stringshare_add((base::UTF16ToUTF8(params.replace_id)).c_str())),
        title(eina_stringshare_add((base::UTF16ToUTF8(params.title)).c_str())),
#if 0
        notificationID(params.notification_id),
##else
#warning "[M37] params no longer has notification_id in it. Is it actually needed?"
        notificationID(0),
#endif
        securityOrigin(new _Ewk_Security_Origin(params.origin.host().c_str(), params.origin.scheme().c_str(), atoi(params.origin.port().c_str()))) {
  }

  ~_Ewk_Notification() {
    eina_stringshare_del(body);
    eina_stringshare_del(iconURL);
    eina_stringshare_del(replaceID);
    eina_stringshare_del(title);
    delete securityOrigin;
  }
};

struct _Ewk_Notification_Permission_Request {
  Evas_Object* ewkView;
  _Ewk_Security_Origin* origin;
  bool isDecided;
  bool isSuspended;
  int callback_context; // required by content API

  _Ewk_Notification_Permission_Request(Evas_Object* ewkView, int callback, const GURL& source_origin)
      : ewkView(ewkView),
        origin(new _Ewk_Security_Origin(source_origin.host().c_str(), source_origin.scheme().c_str(), atoi(source_origin.port().c_str()))),
        isDecided(false),
        isSuspended(false),
        callback_context(callback) {
  }

  ~_Ewk_Notification_Permission_Request() {
    delete origin;
  }
};

#endif // ewk_notification_private_h
