// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "public/ewk_geolocation.h"

// TODO: remove dependency
#include <../impl/API/ewk_geolocation_private.h>

#include "public/ewk_security_origin.h"
#include "private/ewk_security_origin_private.h"

const Ewk_Security_Origin* ewk_geolocation_permission_request_origin_get(const Ewk_Geolocation_Permission_Request* permissionRequest)
{
    EINA_SAFETY_ON_NULL_RETURN_VAL(permissionRequest, 0);
    return static_cast<const Ewk_Security_Origin*>(permissionRequest->GetOrigin());
}

Eina_Bool ewk_geolocation_permission_request_set(Ewk_Geolocation_Permission_Request* permissionRequest, Eina_Bool allow)
{
    EINA_SAFETY_ON_NULL_RETURN_VAL(permissionRequest, EINA_FALSE);
    return permissionRequest->SetDecision(allow);
}

Eina_Bool ewk_geolocation_permission_reply(Ewk_Geolocation_Permission_Request* permissionRequest, Eina_Bool allow)
{
    EINA_SAFETY_ON_NULL_RETURN_VAL(permissionRequest, EINA_FALSE);
    return permissionRequest->SetDecision(allow); // the same as ewk_geolocation_permission_request_set
}

void ewk_geolocation_permission_request_suspend(Ewk_Geolocation_Permission_Request* permissionRequest)
{
    EINA_SAFETY_ON_NULL_RETURN(permissionRequest);
    permissionRequest->Suspend();
}

