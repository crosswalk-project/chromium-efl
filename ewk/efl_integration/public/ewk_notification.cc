// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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

void ewk_notification_cached_permissions_set(Ewk_Context *context, Eina_List *ewk_notification_permissions)
{
#if defined(ENABLE_NOTIFICATIONS)
  EINA_SAFETY_ON_NULL_RETURN(context);
  scoped_refptr<content::NotificationControllerEfl> notification_controller(context->browser_context()->GetNotificationController());
  notification_controller->ClearPermissions();
  Eina_List* list;
  void* data;
  EINA_LIST_FOREACH(ewk_notification_permissions, list, data) {
    Ewk_Notification_Permission* perm = static_cast<Ewk_Notification_Permission*>(data);
    notification_controller->AddPermission(GURL(perm->origin), (perm->allowed == EINA_TRUE));
  }
#else
  NOTIMPLEMENTED();
#endif
}

const Ewk_Security_Origin *ewk_notification_permission_request_origin_get(
    const Ewk_Notification_Permission_Request *request)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(request, 0);
  return static_cast<const Ewk_Security_Origin*>(request->GetSecurityOrigin());
}

void ewk_notification_permission_reply(Ewk_Notification_Permission_Request *request, Eina_Bool allow)
{
#if defined(ENABLE_NOTIFICATIONS)
  EINA_SAFETY_ON_NULL_RETURN(request);
  request->Reply(allow == EINA_TRUE);
#endif
}

void ewk_notification_permission_request_set(Ewk_Notification_Permission_Request *request, Eina_Bool allowed)
{
#if defined(ENABLE_NOTIFICATIONS)
  EINA_SAFETY_ON_NULL_RETURN(request);
  request->Reply(allowed == EINA_TRUE);
#endif
}

void ewk_notification_permission_request_suspend(Ewk_Notification_Permission_Request *request)
{
  EINA_SAFETY_ON_NULL_RETURN(request);
  request->SetSuspend(true);
}

void ewk_notification_policies_removed(Ewk_Context* ewkContext, Eina_List* origins)
{
#if defined(ENABLE_NOTIFICATIONS)
  EINA_SAFETY_ON_NULL_RETURN(ewkContext);
  EINA_SAFETY_ON_NULL_RETURN(origins);
  ewkContext->browser_context()->GetNotificationController()->RemovePermissions(origins);
#else
  NOTIMPLEMENTED();
#endif
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
