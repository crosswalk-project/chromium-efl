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
#include "ipc/ipc_message.h"
#include "ui/base/resource/resource_bundle.h"
#include "ui/base/l10n/l10n_util.h"

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

base::string16 ContentClientEfl::GetLocalizedString(int message_id) const {
  // TODO(boliu): Used only by WebKit, so only bundle those resources for
  // Android WebView.
  return l10n_util::GetStringUTF16(message_id);
}

base::StringPiece ContentClientEfl::GetDataResource(
      int resource_id,
      ui::ScaleFactor scale_factor) const {
  // TODO(boliu): Used only by WebKit, so only bundle those resources for
  // Android WebView.
  return ResourceBundle::GetSharedInstance().GetRawDataResourceForScale(
    resource_id, scale_factor);
}

bool ContentClientEfl::CanSendWhileSwappedOut(const IPC::Message* message) {
  // For legacy API support we perform a few browser -> renderer synchronous IPC
  // messages that block the browser. However, the synchronous IPC replies might
  // be dropped by the renderer during a swap out, deadlocking the browser.
  // Because of this we should never drop any synchronous IPC replies.
  return message->type() == IPC_REPLY_ID;
}
