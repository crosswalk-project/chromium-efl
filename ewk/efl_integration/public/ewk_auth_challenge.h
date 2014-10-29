// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

/**
 * @file    ewk_auth_challenge.h
 * @brief   Describes the authentication challenge API.
 */

#ifndef ewk_auth_challenge_h
#define ewk_auth_challenge_h

#include <Eina.h>
#include "ewk_export.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Creates a type name for _Ewk_Auth_Challenge */
typedef struct _Ewk_Auth_Challenge Ewk_Auth_Challenge;

/**
 * Gets the realm string of authentication challenge received from "authentication,challenge" evas object smart callback.
 *
 * @param auth_challenge authentication challenge instance received from "authentication,challenge" evas object smart callback.
 * @return the realm of authentication challenge on success, @c 0 otherwise
 *
 */
EAPI const char* ewk_auth_challenge_realm_get(Ewk_Auth_Challenge* auth_challenge);

/**
 * Gets the url string of authentication challenge received from "authentication,challenge" evas object smart callback.
 *
 * @param auth_challenge authentication challenge request instance received from "authentication,challenge" evas object smart callback.
 * @return the url of authentication challenge on success, @c 0 otherwise
 *
 */
EAPI const char* ewk_auth_challenge_url_get(Ewk_Auth_Challenge* auth_challenge);

/**
 * Suspend the operation for authentication challenge.
 *
 * @param auth_challenge authentication challenge instance received from "authentication,challenge" evas object smart callback.
 *
 */
EAPI void ewk_auth_challenge_suspend(Ewk_Auth_Challenge* auth_challenge);

/**
 *  If user select ok, send credential for authentication challenge from user input.
 *
 * @param auth_challenge authentication challenge instance received from "authentication,challenge" evas object smart callback.
 * @param user user id from user input.
 * @param password user password from user input.
 *
 */
EAPI void ewk_auth_challenge_credential_use(Ewk_Auth_Challenge* auth_challenge, const char* user, const char* password);

/**
 *  If user select cancel, send cancellation notification for authentication challenge.
 *
 * @param auth_challenge authentication challenge instance received from "authentication,challenge" evas object smart callback.
 *
 */
EAPI void ewk_auth_challenge_credential_cancel(Ewk_Auth_Challenge* auth_challenge);

#ifdef __cplusplus
}
#endif

#endif // ewk_auth_challenge_h
