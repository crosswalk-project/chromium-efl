// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ewk_security_origin_h
#define ewk_security_origin_h

#include <stdint.h>
#include <Eina.h>
#include "ewk_export.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _Ewk_Security_Origin Ewk_Security_Origin;

/**
 * Requests for getting host of security origin.
 *
 * @param origin security origin
 *
 * @return host of security origin
 */
EAPI Eina_Stringshare* ewk_security_origin_host_get(const Ewk_Security_Origin* origin);

/**
 * Requests for getting host of security origin.
 *
 * @param origin security origin
 *
 * @return host of security origin
 */
EAPI Eina_Stringshare* ewk_security_origin_protocol_get(const Ewk_Security_Origin* origin);

/**
 * Requests for getting host of security origin.
 *
 * @param origin security origin
 *
 * @return host of security origin
 */
EAPI uint16_t ewk_security_origin_port_get(const Ewk_Security_Origin* origin);

/**
 * Release all resources allocated by a security origin object.
 *
 * @param o security origin object
 */
EAPI void ewk_security_origin_free(Ewk_Security_Origin *o);

/**
 * Creates a security origin for a url.
 *
 * @param url the url for the security origin.
 *
 * @return the security origin object
 */
EAPI Ewk_Security_Origin *ewk_security_origin_new_from_string(const char *url);


#ifdef __cplusplus
}
#endif
#endif // ewk_security_origin_h
