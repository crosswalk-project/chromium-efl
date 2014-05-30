/* 
 * Copyright (C) 2012 Intel Corporation.
 * Copyright (C) 2013 Samsung Electronics. All rights reserved.
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

/**
 * @file    ewk_error.h
 * @brief   Describes the Web Error API.
 */

#ifndef ewk_error_h
#define ewk_error_h

#include <Eina.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Creates a type name for @a Ewk_Error. */
typedef struct _Ewk_Error Ewk_Error;

/// Creates a type name for Ewk_Error_Type.
typedef enum {
    EWK_ERROR_TYPE_NONE,
    EWK_ERROR_TYPE_INTERNAL,
    EWK_ERROR_TYPE_NETWORK,
    EWK_ERROR_TYPE_POLICY,
    EWK_ERROR_TYPE_PLUGIN,
    EWK_ERROR_TYPE_DOWNLOAD,
    EWK_ERROR_TYPE_PRINT
} Ewk_Error_Type;

typedef enum {
    EWK_ERROR_CODE_CANNOTSHOWMIMETYPE = 100,
    EWK_ERROR_CODE_CANNOTSHOWURL,
    EWK_ERROR_CODE_FRAMELOADINTERRUPTEDBYPOLICYCHANGE,
    EWK_ERROR_CODE_CANNOTUSERESTRICTEDPORT,
    EWK_ERROR_CODE_CANNOTFINDPLUGIN = 200,
    EWK_ERROR_CODE_CANNOTLOADPLUGIN,
    EWK_ERROR_CODE_JAVAUNAVAILABLE,
    EWK_ERROR_CODE_PLUGINCANCELLEDCONNECTION,
    EWK_ERROR_CODE_PLUGINWILLHANDLELOAD,
} Ewk_Error_Code;

typedef enum {
    EWK_ERROR_NETWORK_STATUS_NONE,

    /* Transport Errors */
    EWK_ERROR_NETWORK_STATUS_CANCELLED = 1,
    EWK_ERROR_NETWORK_STATUS_CANT_RESOLVE,
    EWK_ERROR_NETWORK_STATUS_CANT_RESOLVE_PROXY,
    EWK_ERROR_NETWORK_STATUS_CANT_CONNECT,
    EWK_ERROR_NETWORK_STATUS_CANT_CONNECT_PROXY,
    EWK_ERROR_NETWORK_STATUS_SSL_FAILED,
    EWK_ERROR_NETWORK_STATUS_IO_ERROR,
    EWK_ERROR_NETWORK_STATUS_MALFORMED,
    EWK_ERROR_NETWORK_STATUS_TRY_AGAIN,
    EWK_ERROR_NETWORK_STATUS_TOO_MANY_REDIRECTS,
    EWK_ERROR_NETWORK_STATUS_TLS_FAILED,

    /* HTTP Status Codes */
    EWK_ERROR_NETWORK_STATUS_CONTINUE = 100,
    EWK_ERROR_NETWORK_STATUS_SWITCHING_PROTOCOLS,
    EWK_ERROR_NETWORK_STATUS_PROCESSING,

    EWK_ERROR_NETWORK_STATUS_OK = 200,
    EWK_ERROR_NETWORK_STATUS_CREATED,
    EWK_ERROR_NETWORK_STATUS_ACCEPTED,
    EWK_ERROR_NETWORK_STATUS_NON_AUTHORITATIVE,
    EWK_ERROR_NETWORK_STATUS_NO_CONTENT,
    EWK_ERROR_NETWORK_STATUS_RESET_CONTENT,
    EWK_ERROR_NETWORK_STATUS_PARTIAL_CONTENT,
    EWK_ERROR_NETWORK_STATUS_MULTI_STATUS,

    EWK_ERROR_NETWORK_STATUS_MULTIPLE_CHOICES = 300,
    EWK_ERROR_NETWORK_STATUS_MOVED_PERMANENTLY,
    EWK_ERROR_NETWORK_STATUS_FOUND = 302,
    EWK_ERROR_NETWORK_STATUS_MOVED_TEMPORARILY = 302,
    EWK_ERROR_NETWORK_STATUS_SEE_OTHER,
    EWK_ERROR_NETWORK_STATUS_NOT_MODIFIED,
    EWK_ERROR_NETWORK_STATUS_USE_PROXY,
    EWK_ERROR_NETWORK_STATUS_NOT_APPEARING_IN_THIS_PROTOCOL,
    EWK_ERROR_NETWORK_STATUS_TEMPORARY_REDIRECT,

    EWK_ERROR_NETWORK_STATUS_BAD_REQUEST = 400,
    EWK_ERROR_NETWORK_STATUS_UNAUTHORIZED,
    EWK_ERROR_NETWORK_STATUS_PAYMENT_REQUIRED,
    EWK_ERROR_NETWORK_STATUS_FORBIDDEN,
    EWK_ERROR_NETWORK_STATUS_NOT_FOUND,
    EWK_ERROR_NETWORK_STATUS_METHOD_NOT_ALLOWED,
    EWK_ERROR_NETWORK_STATUS_NOT_ACCEPTABLE,
    EWK_ERROR_NETWORK_STATUS_PROXY_AUTHENTICATION_REQUIRED,
    EWK_ERROR_NETWORK_STATUS_PROXY_UNAUTHORIZED = EWK_ERROR_NETWORK_STATUS_PROXY_AUTHENTICATION_REQUIRED,
    EWK_ERROR_NETWORK_STATUS_REQUEST_TIMEOUT,
    EWK_ERROR_NETWORK_STATUS_CONFLICT,
    EWK_ERROR_NETWORK_STATUS_GONE,
    EWK_ERROR_NETWORK_STATUS_LENGTH_REQUIRED,
    EWK_ERROR_NETWORK_STATUS_PRECONDITION_FAILED,
    EWK_ERROR_NETWORK_STATUS_REQUEST_ENTITY_TOO_LARGE,
    EWK_ERROR_NETWORK_STATUS_REQUEST_URI_TOO_LONG,
    EWK_ERROR_NETWORK_STATUS_UNSUPPORTED_MEDIA_TYPE,
    EWK_ERROR_NETWORK_STATUS_REQUESTED_RANGE_NOT_SATISFIABLE,
    EWK_ERROR_NETWORK_STATUS_INVALID_RANGE = EWK_ERROR_NETWORK_STATUS_REQUESTED_RANGE_NOT_SATISFIABLE,
    EWK_ERROR_NETWORK_STATUS_EXPECTATION_FAILED,
    EWK_ERROR_NETWORK_STATUS_UNPROCESSABLE_ENTITY = 422,
    EWK_ERROR_NETWORK_STATUS_LOCKED,
    EWK_ERROR_NETWORK_STATUS_FAILED_DEPENDENCY,

    EWK_ERROR_NETWORK_STATUS_INTERNAL_SERVER_ERROR = 500,
    EWK_ERROR_NETWORK_STATUS_NOT_IMPLEMENTED,
    EWK_ERROR_NETWORK_STATUS_BAD_GATEWAY,
    EWK_ERROR_NETWORK_STATUS_SERVICE_UNAVAILABLE,
    EWK_ERROR_NETWORK_STATUS_GATEWAY_TIMEOUT,
    EWK_ERROR_NETWORK_STATUS_HTTP_VERSION_NOT_SUPPORTED,
    EWK_ERROR_NETWORK_STATUS_INSUFFICIENT_STORAGE = 507,
    EWK_ERROR_NETWORK_STATUS_NOT_EXTENDED = 510
} Ewk_Error_Network_Status;

typedef enum {
    EWK_ERROR_IO_CODE_FAILED,
    EWK_ERROR_IO_CODE_NOT_FOUND,
    EWK_ERROR_IO_CODE_EXISTS,
    EWK_ERROR_IO_CODE_IS_DIRECTORY,
    EWK_ERROR_IO_CODE_NOT_DIRECTORY,
    EWK_ERROR_IO_CODE_NOT_EMPTY,
    EWK_ERROR_IO_CODE_NOT_REGULAR_FILE,
    EWK_ERROR_IO_CODE_NOT_SYMBOLIC_LINK,
    EWK_ERROR_IO_CODE_NOT_MOUNTABLE_FILE,
    EWK_ERROR_IO_CODE_FILENAME_TOO_LONG,
    EWK_ERROR_IO_CODE_INVALID_FILENAME,
    EWK_ERROR_IO_CODE_TOO_MANY_LINKS,
    EWK_ERROR_IO_CODE_NO_SPACE,
    EWK_ERROR_IO_CODE_INVALID_ARGUMENT,
    EWK_ERROR_IO_CODE_PERMISSION_DENIED,
    EWK_ERROR_IO_CODE_NOT_SUPPORTED,
    EWK_ERROR_IO_CODE_NOT_MOUNTED,
    EWK_ERROR_IO_CODE_ALREADY_MOUNTED,
    EWK_ERROR_IO_CODE_CLOSED,
    EWK_ERROR_IO_CODE_CANCELLED,
    EWK_ERROR_IO_CODE_PENDING,
    EWK_ERROR_IO_CODE_READ_ONLY,
    EWK_ERROR_IO_CODE_CANT_CREATE_BACKUP,
    EWK_ERROR_IO_CODE_WRONG_ETAG,
    EWK_ERROR_IO_CODE_TIMED_OUT,
    EWK_ERROR_IO_CODE_WOULD_RECURSE,
    EWK_ERROR_IO_CODE_BUSY,
    EWK_ERROR_IO_CODE_WOULD_BLOCK,
    EWK_ERROR_IO_CODE_HOST_NOT_FOUND,
    EWK_ERROR_IO_CODE_WOULD_MERGE,
    EWK_ERROR_IO_CODE_FAILED_HANDLED,
    EWK_ERROR_IO_CODE_TOO_MANY_OPEN_FILES,
    EWK_ERROR_IO_CODE_NOT_INITIALIZED,
    EWK_ERROR_IO_CODE_ADDRESS_IN_USE,
    EWK_ERROR_IO_CODE_PARTIAL_INPUT,
    EWK_ERROR_IO_CODE_INVALID_DATA,
    EWK_ERROR_IO_CODE_DBUS_ERROR,
    EWK_ERROR_IO_CODE_HOST_UNREACHABLE,
    EWK_ERROR_IO_CODE_NETWORK_UNREACHABLE,
    EWK_ERROR_IO_CODE_CONNECTION_REFUSED,
    EWK_ERROR_IO_CODE_PROXY_FAILED,
    EWK_ERROR_IO_CODE_PROXY_AUTH_FAILED,
    EWK_ERROR_IO_CODE_PROXY_NEED_AUTH,
    EWK_ERROR_IO_CODE_PROXY_NOT_ALLOWED
} Ewk_Error_Io_Code;

/**
 * Query type for this error.
 *
 * @param error error object to query.
 *
 * @return the error type, that may be @c NULL. This pointer is
 *         guaranteed to be eina_stringshare, so whenever possible
 *         save yourself some cpu cycles and use
 *         eina_stringshare_ref() instead of eina_stringshare_add() or
 *         strdup().
 */
EAPI Ewk_Error_Type ewk_error_type_get(const Ewk_Error *error);

/**
 * Query failing URL for this error.
 *
 * URL that failed loading.
 *
 * @param error error object to query.
 *
 * @return the URL pointer, that may be @c NULL. This pointer is
 *         guaranteed to be eina_stringshare, so whenever possible
 *         save yourself some cpu cycles and use
 *         eina_stringshare_ref() instead of eina_stringshare_add() or
 *         strdup().
 */
EAPI const char *ewk_error_url_get(const Ewk_Error *error);

/**
 * Query HTTP error code.
 *
 * @param error error object to query.
 *
 * #if PLATFORM(TIZEN)
 * @note If ewk_error_domain_get returns "WebKitErrorDomain", the error code represents Ewk_Error_Code.
 *       If "soup_http_error_quark", it represents Ewk_Error_Network_Status.
 *       If "g-io-error-quark", it represents Ewk_Error_Io_Code.
 * #endif
 *
 * @return the HTTP error code.
 */
EAPI int ewk_error_code_get(const Ewk_Error *error);

/**
 * Query description for this error.
 *
 * @param error error object to query.
 *
 * @return the description pointer, that may be @c NULL. This pointer is
 *         guaranteed to be eina_stringshare, so whenever possible
 *         save yourself some cpu cycles and use
 *         eina_stringshare_ref() instead of eina_stringshare_add() or
 *         strdup().
 */
EAPI const char *ewk_error_description_get(const Ewk_Error *error);

/**
 * Returns domain from Ewk_Error.
 *
 * @param error Ewk_Error object received from "load,error" evas object smart callback
 *
 * @return @c domain string on success or empty string on failure
 */
EAPI const char* ewk_error_domain_get(const Ewk_Error* error);

#ifdef __cplusplus
}
#endif

#endif // ewk_error_h
