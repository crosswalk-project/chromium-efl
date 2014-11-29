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

