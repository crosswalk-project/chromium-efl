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
 */

#include "ewk_notification.h"
#include <Evas.h>

// TODO: remove dependency
#include <../impl/eweb_view.h>
#include <../impl/browser_context_efl.h>

#include "private/ewk_context_private.h"
#include "private/ewk_security_origin_private.h"
#include "private/ewk_notification_private.h"
#include "private/ewk_private.h"


const char *ewk_notification_body_get(const Ewk_Notification *ewk_notification)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(ewk_notification, 0);
  return ewk_notification->GetBody();
}

void ewk_notification_clicked(Ewk_Context *context, uint64_t notification_id)
{
#if !defined(EWK_BRINGUP)
#warning "[M38] TODO: Fix and re-enable notifications (edit efl_integration/public/ewk_notifications.cc)"
  EINA_SAFETY_ON_NULL_RETURN(context);
  context->browser_context()->GetNotificationController()->NotifyNotificationClicked(notification_id);
#endif
}

const char *ewk_notification_icon_url_get(const Ewk_Notification *ewk_notification)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(ewk_notification, 0);
  return ewk_notification->GetIconUrl();
}

uint64_t ewk_notification_id_get(const Ewk_Notification *ewk_notification)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(ewk_notification, 0);
  return ewk_notification->GetID();
}

const Ewk_Security_Origin *ewk_notification_permission_request_origin_get(
    const Ewk_Notification_Permission_Request *request)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(request, 0);
  return static_cast<const Ewk_Security_Origin*>(request->GetSecurityOrigin());
}

void ewk_notification_permission_request_set(Ewk_Notification_Permission_Request *request, Eina_Bool allowed)
{
#if !defined(EWK_BRINGUP)
#warning "[M38] TODO: Fix and re-enable notifications (edit efl_integration/public/ewk_notifications.cc)"
  EINA_SAFETY_ON_NULL_RETURN(request);
  EWebView* view = EWebView::FromEvasObject(request->GetWebviewEvasObject());
  if (view)
    view->context()->browser_context()->GetNotificationController()->SetPermissionForNotification(request, allowed);
  delete request;
#endif
}

void ewk_notification_permission_request_suspend(Ewk_Notification_Permission_Request *request)
{
  EINA_SAFETY_ON_NULL_RETURN(request);
  request->SetSuspend(true);
}

void ewk_notification_policies_removed(Ewk_Context* ewkContext, Eina_List* origins)
{
  LOG_EWK_API_MOCKUP();
}

const Ewk_Security_Origin *ewk_notification_security_origin_get(const Ewk_Notification *ewk_notification)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(ewk_notification, 0);
  return static_cast<const Ewk_Security_Origin*>(ewk_notification->GetSecurityOrigin());
}

void ewk_notification_showed(Ewk_Context *context, uint64_t notification_id)
{
  // No Matching content API
}

const char *ewk_notification_title_get(const Ewk_Notification *ewk_notification)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(ewk_notification, 0);
  return ewk_notification->GetTitle();
}
