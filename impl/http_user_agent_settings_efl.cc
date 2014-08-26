// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/public/common/content_client.h"
#include "http_user_agent_settings_efl.h"

namespace {
  const std::string ACCEPT_LANGUAGES_DEFAULT_VALUE = "en-us,en";
  std::string accept_language_ = ACCEPT_LANGUAGES_DEFAULT_VALUE;
}

namespace content {
  ContentClient* GetContentClient();
}

HttpUserAgentSettingsEfl::HttpUserAgentSettingsEfl() {
}

HttpUserAgentSettingsEfl::~HttpUserAgentSettingsEfl() {
}

void HttpUserAgentSettingsEfl::SetAcceptLanguage(const std::string& accept_language) {
  if (accept_language.length() == 0) {
    return;
  }
  accept_language_ = accept_language;
}

std::string HttpUserAgentSettingsEfl::GetAcceptLanguage() const {
  return accept_language_;
}

std::string HttpUserAgentSettingsEfl::GetUserAgent() const {
  return content::GetContentClient()->GetUserAgent();
}
