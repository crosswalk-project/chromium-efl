// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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
