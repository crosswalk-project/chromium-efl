// Copyright (C) 2012 Intel Corporation. All rights reserved.
// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

/**
 * @file    ewk_auth_request.h
 * @brief   Describes the Ewk Authentication Request API.
 */

#ifndef ewk_auth_request_h
#define ewk_auth_request_h

#include <Eina.h>
#include "ewk_export.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Creates a type name for _Ewk_Auth_Challenge */
typedef struct _Ewk_Auth_Challenge Ewk_Auth_Request;

/**
 * Set credential for the authentication request.
 *
 * @param request request object to update
 *
 * @return @c EINA_TRUE if the credential was successfuly sent, @c EINA_FALSE otherwise.
 */
EAPI Eina_Bool ewk_auth_request_authenticate(Ewk_Auth_Request *request, const char *username, const char *password);

/**
 * Cancels the authentication request.
 *
 * @param request request object to cancel
 *
 * @return @c EINA_TRUE if successful, @c EINA_FALSE otherwise
 */
EAPI Eina_Bool ewk_auth_request_cancel(Ewk_Auth_Request *request);

/**
 * Queries the authentication realm.
 *
 * @param request request object to query
 *
 * @return the realm pointer, that may be @c NULL. This pointer is
 *         guaranteed to be eina_stringshare, so whenever possible
 *         save yourself some cpu cycles and use
 *         eina_stringshare_ref() instead of eina_stringshare_add() or
 *         strdup()
 */
EAPI const char *ewk_auth_request_realm_get(const Ewk_Auth_Request *request);

#ifdef __cplusplus
}
#endif

#endif /* ewk_auth_request_h */
