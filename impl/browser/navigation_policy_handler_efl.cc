/*
 * Copyright (C) 2014 Samsung Electronics
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this program; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

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
