// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.


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
