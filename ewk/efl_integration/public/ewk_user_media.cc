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


#include "ewk_user_media.h"
// TODO: remove dependency
#include <../impl/API/ewk_user_media_private.h>

Eina_Bool ewk_user_media_permission_request_suspend(Ewk_User_Media_Permission_Request* permissionRequest)
{
    EINA_SAFETY_ON_NULL_RETURN_VAL(permissionRequest, false);
    permissionRequest->isSuspended = true;
    return true;
}

void ewk_user_media_permission_request_set(Ewk_User_Media_Permission_Request *request, Eina_Bool allowed)
{
  if(request && request->web_contents){
    request->web_contents->OnAccessRequestResponse(allowed);
    delete request;
  }
}
