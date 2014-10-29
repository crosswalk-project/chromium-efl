// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TIZEN_WEBVIEW_PUBLIC_TW_URL_H_
#define TIZEN_WEBVIEW_PUBLIC_TW_URL_H_

#include <string>

namespace tizen_webview {

class URL {
 public:
  enum {
    PORT_UNSPECIFIED = -1,
    PORT_INVALID = -2
  };

  URL();
  explicit URL(const std::string& host_);
  URL(const std::string& protocol_, const std::string& host_, int port_);

  const char* getProtocol() const { return protocol.c_str(); }
  const char* getHost() const { return host.c_str(); }
  int         getPort() const { return port; }

  bool isPortInvalid() const { return port == PORT_INVALID; }
  bool isPortUnspecified() const { return port == PORT_UNSPECIFIED; }

 private:
  std::string protocol;
  std::string host;
  int port;
};

} // namespace tizen_webview

#endif  // TIZEN_WEBVIEW_PUBLIC_TW_URL_H_
