// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ewk_notification_h
#define ewk_notification_h

#include <Eina.h>
#include "ewk_context.h"
#include "ewk_export.h"
#include "ewk_security_origin.h"

#ifdef __cplusplus
extern "C" {
#endif

struct _Ewk_Notification_Permission {
    const char* origin;
    Eina_Bool allowed;
};

typedef struct Ewk_Notification Ewk_Notification;
typedef struct _Ewk_Notification_Permission Ewk_Notification_Permission;
typedef struct Ewk_Notification_Permission_Request Ewk_Notification_Permission_Request;

/**
 * Requests for getting body of notification.
 *
 * @param ewk_notification pointer of notificaion data
 *
 * @return body of notification
 *         Lifetime only valid as long as @a ewk_notification is valid.
 */
EAPI const char *ewk_notification_body_get(const Ewk_Notification *ewk_notification);

/**
 * Notify that notification is clicked.
 *
 * @param context context object
 * @param notification_id identifier of notification
 */
EAPI void ewk_notification_clicked(Ewk_Context *context, uint64_t notification_id);

/**
 * Requests for getting icon url of notification.
 *
 * @param ewk_notification pointer of notification data
 *
 * @return icon url of notification
 *         Lifetime only valid as long as @a ewk_notification is valid.
 */
EAPI const char *ewk_notification_icon_url_get(const Ewk_Notification *ewk_notification);

/**
 * Requests for getting id of notification.
 *
 * @param ewk_notification pointer of notification data
 *
 * @return id of notification
 */
EAPI uint64_t ewk_notification_id_get(const Ewk_Notification *ewk_notification);

/**
 * Requests for setting cached notification permissions.
 *
 * By calling this notification permission is replaced as passed ewk_notification_permissions.
 *
 * @param context context object
 * @param ewk_notification_permissions list of cached permissions(Ewk_Notification_Permission)
 */
EAPI void ewk_notification_cached_permissions_set(Ewk_Context *context, Eina_List *ewk_notification_permissions);

/**
 * Requests for getting origin of notification permission request.
 *
 * @param request Ewk_Notification_Permission_Request object to get origin for notification permission request
 *
 * @return security origin of notification permission request
 *         Lifetime only valid as long as @a ewk_notification is valid.
 */
EAPI const Ewk_Security_Origin *ewk_notification_permission_request_origin_get(const Ewk_Notification_Permission_Request *request);

/**
 * Reply the result about notification permission.
 *
 * @param request Ewk_Notification_Permission_Request object to get the infomation about notification permission request.
 * @param allow result about notification permission
 */
EAPI void ewk_notification_permission_reply(Ewk_Notification_Permission_Request *request, Eina_Bool allow);

/**
 * Deprecated, use ewk_notification_permission_reply instead.
 * Sets permission of notification.
 *
 * @param request Ewk_Notification_Permission_Request object to allow/deny notification permission
 *        request is freed in this function.
 * @param allowed @c EINA_TRUE if permission is allowed, @c EINA_FALSE if permission is denied
 */
EINA_DEPRECATED EAPI void ewk_notification_permission_request_set(Ewk_Notification_Permission_Request *request, Eina_Bool allowed);

/**
 * Suspend the operation for permission request.
 *
 * This suspends the operation for permission request.
 * This is very useful to decide the policy from the additional UI operation like the popup.
 *
 * @param request Ewk_Notification_Permission_Request object to suspend notification permission request
 */
EAPI void ewk_notification_permission_request_suspend(Ewk_Notification_Permission_Request *request);

/**
 * Notify that notification policies are removed.
 *
 * @param context context object
 * @param origins list of security origins(made by UAs)
 */
EAPI void ewk_notification_policies_removed(Ewk_Context *context, Eina_List *origins);

/**
 * Requests for getting security origin of notification.
 *
 * @param ewk_notification pointer of notification data
 *
 * @return security origin of notification
 *         Lifetime only valid as long as @a ewk_notification is valid.
 */
EAPI const Ewk_Security_Origin *ewk_notification_security_origin_get(const Ewk_Notification *ewk_notification);

/**
 * Notify that notification is showed.
 *
 * @param context context object
 * @param notification_id identifier of notification
 */
EAPI void ewk_notification_showed(Ewk_Context *context, uint64_t notification_id);

/**
 * Requests for getting title of notification.
 *
 * @param ewk_notification pointer of notification data
 *
 * @return title of notification
 *         Lifetime only valid as long as @a ewk_notification is valid.
 */
EAPI const char *ewk_notification_title_get(const Ewk_Notification *ewk_notification);

#ifdef __cplusplus
}
#endif
#endif // ewk_notification_h
