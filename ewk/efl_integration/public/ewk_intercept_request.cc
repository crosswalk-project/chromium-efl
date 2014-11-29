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

