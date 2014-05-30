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

#ifndef ewk_intercept_request_h
#define ewk_intercept_request_h

#include <Eina.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _Ewk_Intercept_Request Ewk_Intercept_Request;

/**
 * When application doesn't have a response for intercept request url, it
 * calls this api which notifies with FALSE value to the resource handle
 * indicates to continue with normal load procedure for this resource.
 *
 * @param interceptRequest intercept request instance received from "intercept,request" evas object smart callback
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_intercept_request_ignore(Ewk_Intercept_Request* interceptRequest);

/**
 * Returns url from Intercept Request object.
 *
 * @param interceptRequest intercept request instance received from "intercept,request" evas object smart callback
 *
 * @return @c url string on success or empty string on failure
 */
EAPI const char* ewk_intercept_request_url_get (Ewk_Intercept_Request* interceptRequest);

/**
 * Returns method for the url from Intercept Request object.
 *
 * @param interceptRequest intercept request instance received from "intercept,request" evas object smart callback
 *
 * @return @c method string on success or empty string on failure
 */
EAPI const char* ewk_intercept_request_http_method_get (Ewk_Intercept_Request* interceptRequest);

/**
 * Returns request body for the url from Intercept Request object.
 *
 * @param interceptRequest intercept request instance received from "intercept,request" evas object smart callback
 *
 * @return @c body string on success or empty string on failure
 */
EAPI const char* ewk_intercept_request_body_get (Ewk_Intercept_Request* interceptRequest);

/**
 * Returns request headers for the intercept Request.
 *
 * @param intercpetRequest intercept request instance received from "intercept, request" evas object smart callback
 *
 * @return @c header string on success or empty string on failure
*/
EAPI const Eina_Hash* ewk_intercept_request_headers_get (Ewk_Intercept_Request* interceptRequest);

/**
 * When application has the response headers, body for the intercept request url, it call this api to set those.
 *
 * @param interceptRequest intercept request instance received from "intercept,request" evas object smart callback
 * @param headers response headers for the intercept request
 * @param body response body for the intercept request
 * @param length of response body
 * @return @c EINA_TRUE on success or EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_intercept_request_response_set (Ewk_Intercept_Request* interceptRequest, const char* headers, const char* body, int length);

#ifdef __cplusplus
}
#endif
#endif // ewk_intercept_request_h
