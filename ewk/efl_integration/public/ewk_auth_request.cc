/*
 * Copyright (C) 2012 Intel Corporation. All rights reserved.
 * Copyright (C) 2014 Samsung Electronics.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

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
