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
