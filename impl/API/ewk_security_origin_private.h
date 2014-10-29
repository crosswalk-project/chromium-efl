// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ewk_security_origin_private_h
#define ewk_security_origin_private_h

#include <Eina.h>
#include <Evas.h>
#include <assert.h>
#include <sstream>

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
    assert(result.is_valid());
    return result;
  }

  Eina_Stringshare* host;
  Eina_Stringshare* protocol;
  int port;
};

#endif // ewk_security_origin_private_h
