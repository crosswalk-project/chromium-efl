// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "browser/navigation_policy_handler_efl.h"
#include "common/render_messages_efl.h"

#include "content/browser/renderer_host/render_view_host_delegate.h"
#include "content/public/browser/render_view_host.h"
#include "ipc/ipc_message.h"

NavigationPolicyHandlerEfl::NavigationPolicyHandlerEfl(content::RenderViewHost* rvh, const NavigationPolicyParams& params)
    : rvh_(rvh),
      decision_(Undecied),
      params_(params) {
  DCHECK(rvh);
}

NavigationPolicyHandlerEfl::~NavigationPolicyHandlerEfl() {
}

void NavigationPolicyHandlerEfl::DownloadNavigation() {
  // indicate that we handle it
  if (SetDecision(Handled)) {
#warning "[M37] RequestOpenURL was moved to WebContentsImpl, how to access it from here?"
    //WindowOpenDisposition disposition = SAVE_TO_DISK;
    //rvh_->GetDelegate()->RequestOpenURL(rvh_, params_.url, params_.referrer,
    //    disposition, params_.frame_id, params_.is_redirect, false);
  }
}

bool NavigationPolicyHandlerEfl::SetDecision(NavigationPolicyHandlerEfl::Decision d) {
  if (decision_ == Undecied && d != Undecied) {
    decision_ = d;
    return true;
  }

  return false;
}
