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

#ifndef ewk_quota_permission_request_h
#define ewk_quota_permission_request_h

#if !defined(EWK_BRINGUP)

#include <stdint.h>
#include <Eina.h>
#include "ewk_export.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _Ewk_Quota_Permission_Request Ewk_Quota_Permission_Request;

/**
 * Requests for getting protocol of quota permission request
 *
 * @param request quota permission request
 *
 * @return protocol of security origin
 */
EAPI Eina_Stringshare* ewk_quota_permission_request_origin_protocol_get(const Ewk_Quota_Permission_Request* request);

/**
 * Requests for getting host of quota permission request
 *
 * @param request quota permission request
 *
 * @return host of security origin
 */
EAPI Eina_Stringshare* ewk_quota_permission_request_origin_host_get(const Ewk_Quota_Permission_Request* request);

/**
 * Requests for getting port of quota permission request
 *
 * @param request quota permission request
 *
 * @return port of security origin
 */
EAPI uint16_t ewk_quota_permission_request_origin_port_get(const Ewk_Quota_Permission_Request* request);

/**
 * Requests for getting new quota size of quota permission request
 *
 * @param request quota permission request
 *
 * @return protocol of security origin
 */
EAPI int64_t ewk_quota_permission_request_quota_get(const Ewk_Quota_Permission_Request* request);

/**
 * Requests for checking if storage type of quota permission request is persistent
 *
 * @param request quota permission request
 *
 * @return @c EINA_TRUE if storage is persistent, otherwise @c EINA_FALSE
 */
EAPI Eina_Bool ewk_quota_permission_request_is_persistent_get(const Ewk_Quota_Permission_Request* request);

#ifdef __cplusplus
}
#endif
#endif // EWK_BRINGUP
#endif // ewk_quota_permission_request_h
