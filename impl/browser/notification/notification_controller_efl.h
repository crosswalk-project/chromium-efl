// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NOTIFICATION_CONTROLLER_EFL_H
#define NOTIFICATION_CONTROLLER_EFL_H

#include <map>

#include <Eina.h>
#include <Evas.h>

#include "base/callback.h"
#include "base/id_map.h"
#include "base/memory/ref_counted.h"
#include "base/strings/utf_string_conversions.h"
#include "third_party/WebKit/public/platform/WebNotificationPermission.h"
#include "tizen_webview/public/tw_url.h"
#include "url/gurl.h"


namespace tizen_webview {
class NotificationPermissionRequest;
}

namespace content {

class WebContents;

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

class NotificationControllerEfl :
    public base::RefCountedThreadSafe<NotificationControllerEfl> {
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

  // Sets the permissions for given origins
  void SetPermissions(const std::map<const char*, Eina_Bool> &permissions);

  // Adds permission to map
  void AddPermission(const GURL origin, bool allowed);

  // Checks stored permission for given origin
  blink::WebNotificationPermission CheckPermissionForOrigin(
      const GURL& origin) const;

  // Removes all stored permissions
  void ClearPermissions();

  // Removes stored permissions for given origins
  void RemovePermissions(Eina_List* origins);

  // Checks if the notification is already present.
  // If present returns the notification id of the notification else false
  bool IsNotificationPresent(const base::string16& replaceid, uint64_t& notification_id);


  void RequestPermission(WebContents* web_contents,
                         const GURL& requesting_frame,
                         const base::Callback<void(bool)>& result_callback);

 private:
  IDMap<NotificationData, IDMapOwnPointer> notification_map; // This stores the notifications displayed to the user
  std::map<GURL, bool> permissions_map_;
  mutable base::Lock permissions_mutex_;

  DISALLOW_COPY_AND_ASSIGN(NotificationControllerEfl);
};

} //namespace

#endif // NOTIFICATION_CONTROLLER_EFL_H

