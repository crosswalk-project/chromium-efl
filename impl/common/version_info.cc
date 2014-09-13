// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "common/version_info.h"

#if defined(OS_TIZEN)
#ifndef TIZEN_LEGACY_V_2_2_1
#include "system_info_internal.h"
#else
#include "system_info.h"
#endif
#endif
#include "base/command_line.h"
#include "content/public/common/content_switches.h"
#include "content/public/common/user_agent.h"

#include "common/version_info_efl.h"

namespace EflWebView {

const char kOSTypeLinux[] = "Linux";

VersionInfo* VersionInfo::version_info_ = NULL;

VersionInfo* VersionInfo::GetInstance() {
  if(!version_info_)
    version_info_ = new VersionInfo;
  return version_info_;
}

void VersionInfo::DeleteInstance() {
  if(version_info_)
    delete version_info_;
  version_info_ = NULL;
}

VersionInfo::VersionInfo()
  : product_name_(PRODUCT_NAME),
    product_version_(PRODUCT_VERSION) {
}

void VersionInfo::SetProductName(const std::string& name) {
  if(name.empty())
    product_name_ = PRODUCT_NAME;
  else
    product_name_ = name;
}

std::string VersionInfo::LastChange() const {
  return LAST_CHANGE;
}

bool VersionInfo::IsOfficialBuild() const {
  return IS_OFFICIAL_BUILD;
}

std::string VersionInfo::CreateVersionString() const {
  std::string current_version;
  if (is_valid()) {
    current_version += Version();
    current_version += " (";
    current_version += " ";
    current_version += LastChange();
    current_version += " ";
    current_version += OSType();
    current_version += ")";
  }
  return current_version;
}

std::string VersionInfo::OSType() const {
#if defined(OS_TIZEN)
  char *device_model = NULL;
  char *tizen_version = NULL;
  char *platform_name = NULL;
  std::string device_model_str;
  std::string tizen_version_str;
  std::string platform_name_str;
  int result = system_info_get_value_string(SYSTEM_INFO_KEY_TIZEN_VERSION, &tizen_version);
  if (result == SYSTEM_INFO_ERROR_NONE) {
    tizen_version_str.assign(tizen_version);
    free(tizen_version);
  }
  result = system_info_get_value_string(SYSTEM_INFO_KEY_PLATFORM_NAME, &platform_name);
  if (result == SYSTEM_INFO_ERROR_NONE) {
    platform_name_str.assign(platform_name);
    free(platform_name);
  }
  result = system_info_get_value_string(SYSTEM_INFO_KEY_MODEL, &device_model);
  if (result == SYSTEM_INFO_ERROR_NONE) {
    device_model_str.assign(device_model);
    free(device_model);
  }
  return kOSTypeLinux +
         std::string("; ") +
         platform_name_str +
         std::string(" ") +
         tizen_version_str +
         std::string("; ") +
         device_model_str;
#else
  return kOSTypeLinux;
#endif
}

std::string VersionInfo::ProductNameAndVersionForUserAgent() const {
  if (!is_valid())
    return std::string();

  // max koo : Some WebRTC web-sites needs the Chrome version number to check
  // if the browser supports the WebRTC feature.
  // Currently Chumium also use the hard-code for the Chrome version number.
  // See http://crbug.com/297522
  // Does we need to open our real version number?
  // or just use 33.0.0.0 as Chromium does.
  return std::string("Chrome/34.0.1847.76 ") +
         Name() +
         std::string("/") +
         Version();
}

std::string VersionInfo::DefaultUserAgent() const {
  std::string product = ProductNameAndVersionForUserAgent();

  if (CommandLine::ForCurrentProcess()->HasSwitch(
      switches::kUseMobileUserAgent)) {
    product += " Mobile";
  }

// FIXME : The hard-coded user agent for tizen tv
#if defined(OS_TIZEN_TV)
  return "Mozilla/5.0 (SmartHub; SMART-TV; U; Linux/SmartTV+2013; Maple2012) "
    "AppleWebKit/535.20+ (KHTML, like Gecko) SmartTV Safari/535.20+";
#else
  return content::BuildUserAgentFromOSAndProduct(EflWebView::VersionInfo::GetInstance()->OSType(),
                                                     product);
#endif
}

} //namespace EflWebView
