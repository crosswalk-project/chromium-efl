// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ewk_intercept_request.h"
#include "private/ewk_private.h"


Eina_Bool ewk_intercept_request_ignore(Ewk_Intercept_Request* interceptRequest)
{
  LOG_EWK_API_MOCKUP();
  return true;
}

const char* ewk_intercept_request_url_get (Ewk_Intercept_Request* interceptRequest)
{
  LOG_EWK_API_MOCKUP();
  return NULL;
}

const char* ewk_intercept_request_http_method_get (Ewk_Intercept_Request* interceptRequest)
{
  LOG_EWK_API_MOCKUP();
  return NULL;
}

const char* ewk_intercept_request_body_get (Ewk_Intercept_Request* interceptRequest)
{
  LOG_EWK_API_MOCKUP();
  return NULL;
}

const Eina_Hash* ewk_intercept_request_headers_get (Ewk_Intercept_Request* interceptRequest)
{
  LOG_EWK_API_MOCKUP();
  return NULL;
}

Eina_Bool ewk_intercept_request_response_set (Ewk_Intercept_Request* interceptRequest, const char* headers, const char* body, int length)
{
  LOG_EWK_API_MOCKUP();
  return true;
}

