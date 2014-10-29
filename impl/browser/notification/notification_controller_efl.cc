// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "browser/notification/notification_controller_efl.h"

#include "content/browser/renderer_host/render_view_host_impl.h"
#include "content/public/browser/web_contents.h"

#include "API/ewk_notification_private.h"
#include "eweb_view.h"

#include "tizen_webview/public/tw_notification.h"
#include "tizen_webview/public/tw_security_origin.h"

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

#warning "[M38] need proper impl. for desktop notification"
//  if (render_view_host) {
//    render_view_host->DesktopNotificationPermissionRequestDone(
//                          notification->GetInternalCallbackContext());
//    if (isAllowed)
//      render_view_host->DesktopNotificationPostDisplay(
//                            notification->GetInternalCallbackContext());
//  }
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

}//namespace
