/*
 * Copyright (C) 2013 Samsung Electronics. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 */

#include "common/content_client_efl.h"

#include "base/basictypes.h"
#include "base/command_line.h"
#include "content/public/common/content_switches.h"
#include "content/public/common/user_agent.h"
#include "common/version_info.h"

std::string ContentClientEfl::GetProduct() const {
  return EflWebView::VersionInfo::GetInstance()->ProductNameAndVersionForUserAgent();
}

std::string ContentClientEfl::GetUserAgent() const {
  std::string product = GetProduct();

#if defined(OS_TIZEN)
  product += " Mobile";
#else
  if (CommandLine::ForCurrentProcess()->HasSwitch(
      switches::kUseMobileUserAgent)) {
    product += " Mobile";
  }
#endif

// FIXME : The hard-coded user agent for tizen tv
#if defined(OS_TIZEN_TV)
  return "Mozilla/5.0 (SmartHub; SMART-TV; U; Linux/SmartTV+2013; Maple2012) "
    "AppleWebKit/535.20+ (KHTML, like Gecko) SmartTV Safari/535.20+";
#else
  return content::BuildUserAgentFromOSAndProduct(
      EflWebView::VersionInfo::GetInstance()->OSType(), product);
#endif
}
