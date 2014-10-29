// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NOTIFICATION_CONTROLLER_EFL_H
#define NOTIFICATION_CONTROLLER_EFL_H

#include <Eina.h>
#include <Evas.h>
#include "base/strings/utf_string_conversions.h"
#include "base/id_map.h"

namespace tizen_webview {
class NotificationPermissionRequest;
}

namespace content {

struct NotificationData {
  int render_process_id;
  int render_view_id;
  Eina_Stringshare* replace_id;

  NotificationData(int render_process_id, int render_view_id, const base::string16& replaceid)
      : render_process_id(render_process_id)
      , render_view_id(render_view_id)
      , replace_id(eina_stringshare_add((base::UTF16ToUTF8(replaceid)).c_str())) {
  }

  ~NotificationData() {
    eina_stringshare_del(replace_id);
  }
};

class NotificationControllerEfl {
 public:
  NotificationControllerEfl();
  ~NotificationControllerEfl();

  // Adds a new notification received from engine to a list
  void AddNotification(uint64_t notification_id, int render_process_id, int render_view_id, const base::string16& replace_id);

  // Removes a notification when user or engine deletes it
  void DeleteNotification(uint64_t notification_id, int by_user);

  // Notify engine when user clicked on the notification
  void NotifyNotificationClicked(uint64_t notification_id);

  // sets the permission for a particular pending notification
  void SetPermissionForNotification(
      tizen_webview::NotificationPermissionRequest* notification,
      bool isAllowed);

  // Checks if the notification is already present.
  // If present returns the notification id of the notification else false
  bool IsNotificationPresent(const base::string16& replaceid, uint64_t& notification_id);

 private:
  IDMap<NotificationData, IDMapOwnPointer> notification_map; // This stores the notifications displayed to the user

  DISALLOW_COPY_AND_ASSIGN(NotificationControllerEfl);
};

} //namespace

#endif // NOTIFICATION_CONTROLLER_EFL_H

