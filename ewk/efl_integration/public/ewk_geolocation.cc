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

#include "public/ewk_geolocation.h"

// TODO: remove dependency
#include <../impl/API/ewk_geolocation_private.h>

#include "public/ewk_security_origin.h"
#include "private/ewk_security_origin_private.h"

const Ewk_Security_Origin* ewk_geolocation_permission_request_origin_get(const Ewk_Geolocation_Permission_Request* permissionRequest)
{
    EINA_SAFETY_ON_NULL_RETURN_VAL(permissionRequest, 0);
    return static_cast<const Ewk_Security_Origin*>(permissionRequest->origin);
}

Eina_Bool ewk_geolocation_permission_request_set(Ewk_Geolocation_Permission_Request* permissionRequest, Eina_Bool allow)
{
    EINA_SAFETY_ON_NULL_RETURN_VAL(permissionRequest, false);
    permissionRequest->isDecided = true;

    //Calling the geolocation callback function
    permissionRequest->callback.Run(allow);
    delete permissionRequest;
    return true;
}

void ewk_geolocation_permission_request_suspend(Ewk_Geolocation_Permission_Request* permissionRequest)
{
    EINA_SAFETY_ON_NULL_RETURN(permissionRequest);
    permissionRequest->isSuspended = true;
}

