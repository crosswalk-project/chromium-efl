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

#if !defined(EWK_BRINGUP)

#include "ewk_quota_permission_request.h"
#include "private/ewk_quota_permission_request_private.h"

EAPI Eina_Stringshare* ewk_quota_permission_request_origin_protocol_get(const Ewk_Quota_Permission_Request* request)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(request, 0);
  return request->GetProtocol();
}

EAPI Eina_Stringshare* ewk_quota_permission_request_origin_host_get(const Ewk_Quota_Permission_Request* request)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(request, 0);
  return request->GetHost();
}

EAPI uint16_t ewk_quota_permission_request_origin_port_get(const Ewk_Quota_Permission_Request* request)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(request, 0);
  return static_cast<uint16_t>(request->GetPort());
}

EAPI int64_t ewk_quota_permission_request_quota_get(const Ewk_Quota_Permission_Request* request)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(request, 0);
  return request->GetQuota();
}

EAPI Eina_Bool ewk_quota_permission_request_is_persistent_get(const Ewk_Quota_Permission_Request* request)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(request, EINA_FALSE);
  return request->IsPersistent() ? EINA_TRUE : EINA_FALSE;
}

#endif
