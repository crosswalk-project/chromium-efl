// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ewk_geolocation_h
#define ewk_geolocation_h

#include <Eina.h>
#include "ewk_export.h"
#include "ewk_security_origin.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _Ewk_Geolocation_Permission_Request Ewk_Geolocation_Permission_Request;

/**
 * Requests for getting origin of geolocation permission request.
 *
 * @param request Ewk_Geolocation_Permission_Request object to get origin
 *
 * @return security origin of geolocation permission data
 */
EAPI const Ewk_Security_Origin *ewk_geolocation_permission_request_origin_get(const Ewk_Geolocation_Permission_Request *request);

/**
 * Deprecated
 * Request to allow / deny the geolocation permission request
 *
 * @param request permission request to allow or deny permission
 * @param allow allow or deny permission for geolocation
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE otherwise
 */
EAPI Eina_Bool ewk_geolocation_permission_request_set(Ewk_Geolocation_Permission_Request *request, Eina_Bool allow);

/**
 * Request to allow / deny the geolocation permission request
 *
 * @param request permission request to allow or deny permission
 * @param allow allow or deny permission for geolocation
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE otherwise
 */
EAPI Eina_Bool ewk_geolocation_permission_reply(Ewk_Geolocation_Permission_Request* permissionRequest, Eina_Bool allow);

/**
 * Suspend the operation for permission request.
 *
 * This suspends the operation for permission request.
 * This is very useful to decide the policy from the additional UI operation like the popup.
 *
 * @param request Ewk_Geolocation_Permission_Request object to suspend permission request
 */
EAPI void ewk_geolocation_permission_request_suspend(Ewk_Geolocation_Permission_Request *request);

#ifdef __cplusplus
}
#endif

#endif // ewk_geolocation_h
