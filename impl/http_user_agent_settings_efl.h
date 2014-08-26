// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HTTP_USER_AGENT_SETTINGS_EFL_H_
#define HTTP_USER_AGENT_SETTINGS_EFL_H_

#include <string>

#include "base/basictypes.h"
#include "base/compiler_specific.h"
#include "net/url_request/http_user_agent_settings.h"

// An implementation of |HttpUserAgentSettings| that provides fixed value for
// the Accept-Language HTTP header and uses |content::GetUserAgent| to provide
// the HTTP User-Agent header value.
class HttpUserAgentSettingsEfl : public net::HttpUserAgentSettings {
 public:
  HttpUserAgentSettingsEfl();
  virtual ~HttpUserAgentSettingsEfl();

  // Set Accept language. In case when empty argument is passed keep previously assigned.
  static void SetAcceptLanguage(const std::string&);

  // HttpUserAgentSettings implementation
  virtual std::string GetAcceptLanguage() const override;
  virtual std::string GetUserAgent() const override;

  DISALLOW_COPY_AND_ASSIGN(HttpUserAgentSettingsEfl);
};

#endif  // HTTP_USER_AGENT_SETTINGS_EFL_H_
