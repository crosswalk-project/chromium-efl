/*
   Copyright (C) 2013 Samsung Electronics

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

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
 * Request to allow / deny the geolocation permission request
 *
 * @param request permission request to allow or deny permission
 * @param allow allow or deny permission for geolocation
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE otherwise
 */
EAPI Eina_Bool ewk_geolocation_permission_request_set(Ewk_Geolocation_Permission_Request *request, Eina_Bool allow);

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
