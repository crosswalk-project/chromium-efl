/*
 * Copyright (C) 2013 Samsung Electronics. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "browser/notification/notification_controller_efl.h"

#include "content/browser/renderer_host/render_view_host_impl.h"
#include "content/public/browser/web_contents.h"

#include "API/ewk_notification_private.h"
#include "eweb_view.h"

#include "tizen_webview/public/tw_security_origin.h"
#include "tizen_webview/tw_misc_utility.h"

using tizen_webview::Security_Origin;
using tizen_webview::GetGURL;
using tizen_webview::GetURL;

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
  if (render_view_host)
    render_view_host->DesktopNotificationPostClose(notification_id, by_user);

  notification_map.Remove(notification_id);
}

void NotificationControllerEfl::NotifyNotificationClicked(uint64_t notification_id) {
  NotificationData* saved_data = notification_map.Lookup(notification_id);
  if (!saved_data)
    return;

  RenderViewHost* render_view_host = RenderViewHost::FromID(saved_data->render_process_id, saved_data->render_view_id);

  if (render_view_host)
    render_view_host->DesktopNotificationPostClick(notification_id);
}

void NotificationControllerEfl::SetPermissionForNotification(Ewk_Notification_Permission_Request* notification, bool isAllowed) {
  RenderViewHost* render_view_host = ToEWebView(notification->ewkView)->web_contents_delegate()->web_contents()->GetRenderViewHost();
  if (render_view_host) {
    render_view_host->DesktopNotificationPermissionRequestDone(notification->callback_context);
    if (isAllowed)
      render_view_host->DesktopNotificationPostDisplay(notification->callback_context);
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

}//namespace
