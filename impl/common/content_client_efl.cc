// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "common/content_client_efl.h"

#include "base/basictypes.h"
#include "common/version_info.h"
#include "ipc/ipc_message.h"
#include "ui/base/resource/resource_bundle.h"
#include "ui/base/l10n/l10n_util.h"

std::string ContentClientEfl::GetProduct() const {
  return EflWebView::VersionInfo::GetInstance()->ProductNameAndVersionForUserAgent();
}

std::string ContentClientEfl::GetUserAgent() const {
  return EflWebView::VersionInfo::GetInstance()->DefaultUserAgent();
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
