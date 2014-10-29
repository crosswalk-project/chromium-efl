// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "tw_url.h"

namespace tizen_webview {

URL::URL()
  : protocol()
  , host()
  , port(PORT_UNSPECIFIED) { // url_parse::PORT_INVALID
}

URL::URL(const std::string& host_)
  : protocol()
  , host(host_)
  , port(PORT_UNSPECIFIED) { // url_parse::PORT_INVALID
}

URL::URL(const std::string& protocol_, const std::string& host_, int port_)
  : protocol(protocol_)
  , host(host_)
  , port(port_) {
}

} // namespace tizen_webview
