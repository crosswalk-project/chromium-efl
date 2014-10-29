// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.


#include "ewk_auth_challenge.h"

#include <../impl/API/ewk_auth_challenge_private.h>

void ewkAuthChallengeDelete(Ewk_Auth_Challenge* authChallenge)
{
  delete authChallenge;
}

bool ewkAuthChallengeDecided(Ewk_Auth_Challenge* authChallenge)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(authChallenge, false);
  return authChallenge->is_decided;
}

bool ewkAuthChallengeSuspended(Ewk_Auth_Challenge* authChallenge)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(authChallenge, false);
  return authChallenge->is_suspended;
}

const char* ewk_auth_challenge_realm_get(Ewk_Auth_Challenge* authChallenge)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(authChallenge, 0);
  return authChallenge->realm.c_str();
}

const char* ewk_auth_challenge_url_get(Ewk_Auth_Challenge* authChallenge)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(authChallenge, 0);
  return authChallenge->url.c_str();
}

void ewk_auth_challenge_suspend(Ewk_Auth_Challenge* authChallenge)
{
  EINA_SAFETY_ON_NULL_RETURN(authChallenge);
  authChallenge->is_suspended = true;
}

void ewk_auth_challenge_credential_use(Ewk_Auth_Challenge* authChallenge, const char* user, const char* password)
{
  EINA_SAFETY_ON_NULL_RETURN(authChallenge);
  EINA_SAFETY_ON_NULL_RETURN(user);
  EINA_SAFETY_ON_NULL_RETURN(password);

  authChallenge->is_decided = true;
  authChallenge->login_delegate->Proceed(user, password);
}

void ewk_auth_challenge_credential_cancel(Ewk_Auth_Challenge* authChallenge)
{
  EINA_SAFETY_ON_NULL_RETURN(authChallenge);
  authChallenge->is_decided = true;
  authChallenge->login_delegate->Cancel();
}
