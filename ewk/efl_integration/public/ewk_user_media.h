// Copyright 2012 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ewk_user_media_h
#define ewk_user_media_h

#include <Eina.h>
#include "ewk_export.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _Ewk_User_Media_Permission_Request Ewk_User_Media_Permission_Request;

/**
 * Sets the permission to access local media
 *
 * @param request Ewk_View_User_Media_Permission_Request object for userMedia permission
 * @param allowed decided permission value from user
 */
EAPI void ewk_user_media_permission_request_set(Ewk_User_Media_Permission_Request *request, Eina_Bool allowed);

/**
 * Suspend the operation for user media permission
 *
 * @param request user media permission request object
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_user_media_permission_request_suspend(Ewk_User_Media_Permission_Request *request);

#ifdef __cplusplus
}
#endif
#endif // ewk_user_media_h
