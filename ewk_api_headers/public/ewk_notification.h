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

#ifndef ewk_notification_h
#define ewk_notification_h

#include "ewk_context.h"
#include "ewk_security_origin.h"
#include <Eina.h>

#ifdef __cplusplus
extern "C" {
#endif

struct _Ewk_Notification_Permission {
    Eina_Stringshare* origin;
    Eina_Bool allowed;
};

typedef struct _Ewk_Notification Ewk_Notification;
typedef struct _Ewk_Notification_Permission Ewk_Notification_Permission;
typedef struct _Ewk_Notification_Permission_Request Ewk_Notification_Permission_Request;

/**
 * Requests for getting body of notification.
 *
 * @param ewk_notification pointer of notificaion data
 *
 * @return body of notification
 */
EAPI Eina_Stringshare *ewk_notification_body_get(const Ewk_Notification *ewk_notification);

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
 */
EAPI Eina_Stringshare *ewk_notification_icon_url_get(const Ewk_Notification *ewk_notification);

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
 */
EAPI const Ewk_Security_Origin *ewk_notification_permission_request_origin_get(const Ewk_Notification_Permission_Request *request);

/**
 * Sets permission of notification.
 *
 * @param request Ewk_Notification_Permission_Request object to allow/deny notification permission
 *        request is freed in this function.
 * @param allowed @c EINA_TRUE if permission is allowed, @c EINA_FALSE if permission is denied
 */
EAPI void ewk_notification_permission_request_set(Ewk_Notification_Permission_Request *request, Eina_Bool allowed);

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
 */
EAPI Eina_Stringshare *ewk_notification_title_get(const Ewk_Notification *ewk_notification);

#ifdef __cplusplus
}
#endif
#endif // ewk_notification_h
