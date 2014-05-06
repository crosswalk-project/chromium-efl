/*
 * Copyright (C) 2014 Samsung Electronics
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#include "ewk_policy_decision_private.h"

#include <content/public/browser/browser_thread.h>
#include <content/public/browser/render_view_host.h>
#include <net/http/http_response_headers.h>

#include "web_contents_delegate_efl.h"

using content::BrowserThread;
using content::RenderFrameHost;

namespace {
  void FreeStringShare(void *data) {
    eina_stringshare_del(static_cast<char*>(data));
  }
}

_Ewk_Policy_Decision::_Ewk_Policy_Decision(const GURL& request_url, const net::HttpResponseHeaders* response_headers, PolicyResponseDelegateEfl* delegate)
  : new_window_policy_delegate_(NULL)
  , policy_response_delegate_(delegate)
  , frame_id_(-1)
  , process_id_(-1)
  , cookie_(NULL)
  , url_(NULL)
  , host_(NULL)
  , scheme_(NULL)
  , responseMime_(NULL)
  , responseHeaders_(NULL)
  , decisionType_(EWK_POLICY_DECISION_USE)
  , navigationType_(EWK_POLICY_NAVIGATION_TYPE_OTHER)
  , isDecided_(false)
  , isSuspended_(false)
  , responseStatusCode_(0)
  , AuthUser_(NULL)
  , AuthPassword_(NULL)
  , isMainFrame_(true)
  , type_(POLICY_RESPONSE) {
  DCHECK(response_headers);
  DCHECK(delegate);

  if (delegate) {
    frame_id_ = delegate->GetRenderFrameId();
    process_id_ = delegate->GetRenderProcessId();
  }

  ParseUrl(request_url);

  if (response_headers) {
    responseStatusCode_ = response_headers->response_code();

    std::string mime_type;

    if (response_headers->GetMimeType(&mime_type)) {
      responseMime_ = eina_stringshare_add(mime_type.c_str());

      if (!net::IsSupportedMimeType(mime_type))
        decisionType_ = EWK_POLICY_DECISION_DOWNLOAD;
    }

    std::string set_cookie_;

    if (response_headers->EnumerateHeader(NULL, "Set-Cookie", &set_cookie_))
      cookie_ = eina_stringshare_add(set_cookie_.c_str());

    void* iter = NULL;
    std::string name;
    std::string value;
    responseHeaders_ = eina_hash_string_small_new(FreeStringShare);
    while (response_headers->EnumerateHeaderLines(&iter, &name, &value))
      eina_hash_add(responseHeaders_, name.c_str(), eina_stringshare_add(value.c_str()));
  }
}

_Ewk_Policy_Decision::_Ewk_Policy_Decision(const NavigationPolicyParams &params, content::RenderViewHost* rvh)
  : new_window_policy_delegate_(NULL)
  , navigation_policy_handler_(new NavigationPolicyHandlerEfl(rvh, params))
  , frame_id_(params.frame_id)
  , process_id_(-1)
  , cookie_(NULL)
  , url_(NULL)
  , host_(NULL)
  , scheme_(NULL)
  , responseMime_(NULL)
  , responseHeaders_(NULL)
  , decisionType_(EWK_POLICY_DECISION_USE)
  , navigationType_(static_cast<Ewk_Policy_Navigation_Type>(params.type))
  , isDecided_(false)
  , isSuspended_(false)
  , responseStatusCode_(0)
  , AuthUser_(NULL)
  , AuthPassword_(NULL)
  , isMainFrame_(params.is_main_frame)
  , type_(POLICY_NAVIGATION) {
  ParseUrl(params.url);

  if (rvh)
    process_id_ = rvh->GetProcess()->GetID();
}

_Ewk_Policy_Decision::_Ewk_Policy_Decision(content::WebContentsDelegateEfl* view, const GURL& url, const base::string16& frame)
  : new_window_policy_delegate_(view)
  , frame_id_(-1)
  , process_id_(-1)
  , cookie_(NULL)
  , url_(NULL)
  , host_(NULL)
  , scheme_(NULL)
  , responseMime_(NULL)
  , responseHeaders_(NULL)
  , decisionType_(EWK_POLICY_DECISION_USE)
  , navigationType_(EWK_POLICY_NAVIGATION_TYPE_OTHER)
  , frame_name_(frame)
  , isDecided_(false)
  , isSuspended_(false)
  , responseStatusCode_(0)
  , AuthUser_(NULL)
  , AuthPassword_(NULL)
  , isMainFrame_(true)
  , type_(POLICY_NEWWINDOW) {
  DCHECK(view);
  ParseUrl(url);

  // we can use main frame here
  if (view && view->web_contents()) {
    RenderFrameHost* rfh = view->web_contents()->GetMainFrame();

    if (rfh) {
      // not sure if it's right to use GetRoutingID
      frame_id_ = rfh->GetRoutingID();
      process_id_ = rfh->GetProcess()->GetID();
    }
  }
}

_Ewk_Policy_Decision::~_Ewk_Policy_Decision() {
  eina_stringshare_del(cookie_);
  eina_stringshare_del(url_);
  eina_stringshare_del(host_);
  eina_stringshare_del(scheme_);
  eina_stringshare_del(responseMime_);
  eina_hash_free(responseHeaders_);
  eina_stringshare_del(AuthUser_);
  eina_stringshare_del(AuthPassword_);
}

void _Ewk_Policy_Decision::Use() {
  isDecided_ = true;
  switch (type_) {
    case POLICY_RESPONSE:
      policy_response_delegate_->UseResponse();
      break;
    case POLICY_NAVIGATION:
      navigation_policy_handler_->SetDecision(NavigationPolicyHandlerEfl::Unhandled);
      break;
    case POLICY_NEWWINDOW:
      new_window_policy_delegate_->set_new_window_policy(true);
      break;
    default:
      NOTREACHED();
      break;
  }
}

void _Ewk_Policy_Decision::Ignore() {
  isDecided_ = true;
  switch (type_) {
    case Ewk_Policy_Decision::POLICY_RESPONSE:
      policy_response_delegate_->IgnoreResponse();
      break;
    case Ewk_Policy_Decision::POLICY_NAVIGATION:
      navigation_policy_handler_->SetDecision(NavigationPolicyHandlerEfl::Handled);
      break;
    case Ewk_Policy_Decision::POLICY_NEWWINDOW:
      new_window_policy_delegate_->set_new_window_policy(false);
      break;
    default:
      NOTREACHED();
      break;
  }
}

void _Ewk_Policy_Decision::Download() {
  isDecided_ = true;
  switch (type_) {
    case Ewk_Policy_Decision::POLICY_RESPONSE:
      policy_response_delegate_->UseResponse();
      break;
    case Ewk_Policy_Decision::POLICY_NAVIGATION:
      navigation_policy_handler_->DownloadNavigation();
      break;
    case Ewk_Policy_Decision::POLICY_NEWWINDOW:
      new_window_policy_delegate_->set_new_window_policy(false);
      break;
    default:
      NOTREACHED();
      break;
  }
}

void _Ewk_Policy_Decision::Suspend() {
  isSuspended_ = true;
}

Ewk_Frame_Ref _Ewk_Policy_Decision::GetFrameRef() const {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  RenderFrameHost* host = RenderFrameHost::FromID(process_id_, frame_id_);
  return static_cast<Ewk_Frame_Ref>(host);
}

void _Ewk_Policy_Decision::ParseUrl(const GURL& url) {
  url_ = eina_stringshare_add(url.spec().c_str());
  scheme_ = eina_stringshare_add(url.scheme().c_str());
  host_ = eina_stringshare_add(url.host().c_str());
}
