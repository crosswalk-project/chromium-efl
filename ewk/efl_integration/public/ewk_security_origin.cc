// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ewk_security_origin.h"
#include "private/ewk_security_origin_private.h"

/*
 *
 * @param origin security origin
 *
 * @return host of security origin
 */
Eina_Stringshare* ewk_security_origin_host_get(const Ewk_Security_Origin* origin)
{
    EINA_SAFETY_ON_NULL_RETURN_VAL(origin, 0);
    return origin->GetHost();
}

/**
 * Requests for getting host of security origin.
 *
 * @param origin security origin
 *
 * @return protocol of security origin
 */
Eina_Stringshare* ewk_security_origin_protocol_get(const Ewk_Security_Origin* origin)
{
    EINA_SAFETY_ON_NULL_RETURN_VAL(origin, 0);
    return origin->GetProtocol();
}

/**
 * Requests for getting host of security origin.
 *
 * @param origin security origin
 *
 * @return port of security origin
 */
uint16_t ewk_security_origin_port_get(const Ewk_Security_Origin* origin)
{
    EINA_SAFETY_ON_NULL_RETURN_VAL(origin, 0);
    return static_cast<uint16_t>(origin->GetPort());
}

