// Copyright (C) 2012 Intel Corporation. All rights reserved.
// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ewk_auth_request.h"
#include "private/ewk_private.h"

#include <../impl/API/ewk_auth_challenge_private.h>

Eina_Bool ewk_auth_request_authenticate(Ewk_Auth_Request* request, const char* username, const char* password)
{
#if defined(OS_TIZEN_TV)
  EINA_SAFETY_ON_NULL_RETURN_VAL(request, false);
  EINA_SAFETY_ON_NULL_RETURN_VAL(username, false);
  EINA_SAFETY_ON_NULL_RETURN_VAL(password, false);
  request->is_decided = true;
  request->login_delegate->Proceed(username, password);
  return true;
#else
  LOG_EWK_API_MOCKUP("Only for Tizen TV Browser");
  return false;
#endif
}

Eina_Bool ewk_auth_request_cancel(Ewk_Auth_Request* request)
{
#if defined(OS_TIZEN_TV)
  EINA_SAFETY_ON_NULL_RETURN_VAL(request, false);
  request->is_decided = true;
  request->login_delegate->Cancel();
  return true;
#else
  LOG_EWK_API_MOCKUP("Only for Tizen TV Browser");
  return false;
#endif
}

const char* ewk_auth_request_realm_get(const Ewk_Auth_Request* request)
{
#if defined(OS_TIZEN_TV)
  EINA_SAFETY_ON_NULL_RETURN_VAL(request, 0);
  return request->realm.c_str();
#else
  LOG_EWK_API_MOCKUP("Only for Tizen TV Browser");
  return NULL;
#endif
}
