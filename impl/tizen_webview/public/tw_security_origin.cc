// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "tw_security_origin.h"
#include "tizen_webview/public/tw_url.h"

namespace tizen_webview {

Security_Origin::Security_Origin(const tizen_webview::URL& url)
  : host(eina_stringshare_add(url.getHost()))
  , protocol(eina_stringshare_add(url.getProtocol()))
  , port(url.getPort()) {
}

Security_Origin::Security_Origin(const char* host_in, const char* protocol_in, int port_in)
  : host(eina_stringshare_add(host_in))
  , protocol(eina_stringshare_add(protocol_in))
  , port(port_in) {
}

Security_Origin::~Security_Origin() {
  eina_stringshare_del(host);
  eina_stringshare_del(protocol);
}

tizen_webview::URL Security_Origin::GetURL() const {
  return tizen_webview::URL(protocol, host, port);
}

Eina_Stringshare* Security_Origin::GetHost() const {
  return host;
}

Eina_Stringshare* Security_Origin::GetProtocol() const {
  return protocol;
}

int Security_Origin::GetPort() const {
  return port;
}

} // namespace tizen_webview
