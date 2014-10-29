// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TIZEN_WEBVIEW_PUBLIC_TW_SECURITY_ORIGIN_H_
#define TIZEN_WEBVIEW_PUBLIC_TW_SECURITY_ORIGIN_H_

#include <Eina.h>

namespace tizen_webview {

class URL;

class Security_Origin {
 public:
  Security_Origin(const tizen_webview::URL& url);
  Security_Origin(const char* host_in, const char* protocol_in, int port_in);
  ~Security_Origin();

  tizen_webview::URL GetURL() const;
  Eina_Stringshare* GetHost() const;
  Eina_Stringshare* GetProtocol() const;
  int GetPort() const;

 private:
  Eina_Stringshare* host;
  Eina_Stringshare* protocol;
  int port;

  // Just for safety in using Eina_Stringshare, disallow copy constructor and
  // assignment for now.
  // TODO: Need to revisit and add proper implementation.
  Security_Origin(const Security_Origin&);
  Security_Origin& operator=(const Security_Origin&);
};

} // namespace tizen_webview

#endif  // TIZEN_WEBVIEW_PUBLIC_TW_SECURITY_ORIGIN_H_
