// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "browser/notification/notification_controller_efl.h"

#include <Eina.h>

#include "eweb_view.h"
#include "content/browser/renderer_host/render_view_host_impl.h"
#include "content/public/browser/web_contents.h"
#include "tizen_webview/public/tw_notification.h"
#include "tizen_webview/public/tw_security_origin.h"
#include "tizen_webview/tw_misc_utility.h"

using tizen_webview::NotificationPermissionRequest;
using tizen_webview::Security_Origin;

namespace content {

NotificationControllerEfl::NotificationControllerEfl() {
}

NotificationControllerEfl::~NotificationControllerEfl() {
  notification_map.Clear();
}

void NotificationControllerEfl::AddNotification(uint64_t notification_id, int render_process_id, int render_view_id, const base::string16& replace_id) {
  NotificationData* new_notification(new NotificationData(render_process_id, render_view_id, replace_id));
  notification_map.AddWithID(new_notification, notification_id);
}

void NotificationControllerEfl::DeleteNotification(uint64_t notification_id, int by_user) {
  NotificationData* saved_data = notification_map.Lookup(notification_id);
  if (!saved_data)
    return;

  RenderViewHost* render_view_host = RenderViewHost::FromID(saved_data->render_process_id, saved_data->render_view_id);
#warning "[M38] need proper impl. for desktop notification"
  //if (render_view_host)
  //  render_view_host->DesktopNotificationPostClose(notification_id, by_user);

  notification_map.Remove(notification_id);
}

void NotificationControllerEfl::NotifyNotificationClicked(uint64_t notification_id) {
  NotificationData* saved_data = notification_map.Lookup(notification_id);
  if (!saved_data)
    return;

  RenderViewHost* render_view_host = RenderViewHost::FromID(saved_data->render_process_id, saved_data->render_view_id);

#warning "[M38] need proper impl. for desktop notification"
  //if (render_view_host)
  //  render_view_host->DesktopNotificationPostClick(notification_id);
}

void NotificationControllerEfl::SetPermissionForNotification(
    NotificationPermissionRequest* notification, bool isAllowed) {
  EWebView* wv = EWebView::FromEvasObject(notification->GetWebviewEvasObject());
  DCHECK(wv);
  RenderViewHost* render_view_host =
      wv->web_contents().GetRenderViewHost();

  if (render_view_host) {
    GURL origin = tizen_webview::GetGURL(
        notification->GetSecurityOrigin()->GetURL());
    // save decision in permissions map
    permissions_mutex_.Acquire();
    permissions_map_[origin] = isAllowed;
    permissions_mutex_.Release();
  }
}

void NotificationControllerEfl::SetPermissions(
    const std::map<const char*, Eina_Bool>& permissions) {
  base::AutoLock locker(permissions_mutex_);
  permissions_map_.clear();
  std::map<const char*, Eina_Bool>::const_iterator it = permissions.begin();
  for (; it != permissions.end(); ++it) {
    permissions_map_[GURL(it->first)] = (it->second == EINA_TRUE);
  }
}

void NotificationControllerEfl::AddPermission(const GURL origin,
                                              bool allowed) {
  base::AutoLock locker(permissions_mutex_);
  permissions_map_[origin] = allowed;
}

blink::WebNotificationPermission
NotificationControllerEfl::CheckPermissionForOrigin(
    const GURL &origin) const {
  base::AutoLock locker(permissions_mutex_);
  std::map<GURL, bool>::const_iterator it = permissions_map_.find(origin);
  if (it == permissions_map_.end())
    return blink::WebNotificationPermissionDefault;
  return it->second ?
      blink::WebNotificationPermissionAllowed :
      blink::WebNotificationPermissionDenied;
}

void NotificationControllerEfl::ClearPermissions() {
  base::AutoLock locker(permissions_mutex_);
  permissions_map_.clear();
}

void NotificationControllerEfl::RemovePermissions(Eina_List* origins) {
  Eina_List* list = NULL;
  void* data = NULL;
  base::AutoLock locker(permissions_mutex_);
  EINA_LIST_FOREACH(origins, list, data) {
    tizen_webview::Security_Origin* o =
        static_cast<tizen_webview::Security_Origin*>(data);
    GURL origin = tizen_webview::GetGURL(o->GetURL());
    permissions_map_.erase(origin);
  }
}

bool NotificationControllerEfl::IsNotificationPresent(const base::string16& replaceid, uint64_t& notification_id) {
  std::string in_replaceid(base::UTF16ToUTF8(replaceid).c_str());
  IDMap<NotificationData, IDMapOwnPointer>::const_iterator it(&notification_map);
  for (; !it.IsAtEnd(); it.Advance()) {
    if (!strcmp(in_replaceid.c_str(), it.GetCurrentValue()->replace_id)) {
      notification_id = it.GetCurrentKey();
      return true;
    }
  }
  return false;
}

void NotificationControllerEfl::RequestPermission(
    WebContents* web_contents,
    const GURL& requesting_frame,
    const base::Callback<void(bool)>& result_callback) {
  const bool kDefaultResponse = false;

  WebContentsDelegate* delegate = web_contents->GetDelegate();
  if (!delegate) {
    LOG(ERROR) << "Dropping PermissionNotification request caused by lack "
                  "of the WebContents delegate";
    result_callback.Run(kDefaultResponse);
    return;
  }

  WebContentsDelegateEfl* delegat_efl =
      static_cast<WebContentsDelegateEfl*>(delegate);
  EWebView* web_view = delegat_efl->web_view();
  if (!web_view) {
    LOG(ERROR) << "Dropping PermissionNotification request caused by lack "
                  "of the WebView";
    result_callback.Run(kDefaultResponse);
    return;
  }
  scoped_ptr<NotificationPermissionRequest> notification_permission(
      new NotificationPermissionRequest(web_view->evas_object(),
          result_callback,    tizen_webview::GetURL(requesting_frame)));

  if (!web_view->IsNotificationPermissionCallbackSet()) {
    LOG(ERROR) << "Dropping PermissionNotification request caused by lack "
                  "of the Notification Permission Callback";
    result_callback.Run(kDefaultResponse);
    return;
  }

  blink::WebNotificationPermission web_notification_permission =
      CheckPermissionForOrigin(requesting_frame);
  if (web_notification_permission ==
      blink::WebNotificationPermissionDefault) {
    web_view->InvokeNotificationPermissionCallback(
        notification_permission.get());
    if (notification_permission->IsSuspended()) {
      notification_permission.release();
      return;
    }
  } else {
    result_callback.Run(web_notification_permission ==
        blink::WebNotificationPermissionAllowed);
  }
}

}//namespace
