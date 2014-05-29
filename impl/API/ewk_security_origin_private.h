/*
 * Copyright (C) 2013 Samsung Electronics
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#ifndef ewk_security_origin_private_h
#define ewk_security_origin_private_h

#include <Eina.h>
#include <Evas.h>

#include "base/logging.h"
#include "base/callback.h"
#include "url/gurl.h"

// Security Origin Structure
struct _Ewk_Security_Origin {
  _Ewk_Security_Origin(const GURL& url)
      : host(eina_stringshare_add(url.host().c_str())),
        protocol(eina_stringshare_add(url.scheme().c_str())) {
    assert(url.IntPort() != url::PORT_INVALID);
    port = url.IntPort();
  }

  //FIXME: Replace uint16_t by int for this constructor and use GURL::IntPort for populating.
  // Better yet, just hold GURL::GetOrigin() as member and implement ewk API using that
  _Ewk_Security_Origin(const char* host_in, const char* protocol_in, uint16_t port_in)
      : host(eina_stringshare_add(host_in)),
        protocol(eina_stringshare_add(protocol_in)),
        port(port_in) {
  }

  ~_Ewk_Security_Origin() {
    eina_stringshare_del(host);
    eina_stringshare_del(protocol);
  }

  GURL GetAsGURL() const {
    std::ostringstream url;
    url << protocol << "://" << host;
    if (port != url::PORT_UNSPECIFIED)
      url << ":" << port;
    GURL result(url.str());
    DCHECK(result.is_valid());
    return result;
  }

  Eina_Stringshare* host;
  Eina_Stringshare* protocol;
  int port;
};

#endif // ewk_security_origin_private_h
