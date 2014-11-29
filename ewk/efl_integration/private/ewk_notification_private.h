/*
 * Copyright (C) 2014 Samsung Electronics
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
 */

#ifndef ewk_notification_private_h
#define ewk_notification_private_h

#include <tizen_webview/public/tw_notification.h>

struct Ewk_Notification : public tizen_webview::Notification {
};

struct Ewk_Notification_Permission_Request
    : public tizen_webview::NotificationPermissionRequest {
};

namespace chromium_glue {
  Ewk_Notification* from(tizen_webview::Notification*);

  //tizen_webview::NotificationPermissionRequest* to(tizen_webview::Notification*);
}

#endif // ewk_notification_private_h
