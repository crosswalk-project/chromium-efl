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
using content::RenderViewHost;
using namespace tizen_webview;

namespace {
  void FreeStringShare(void *data) {
    eina_stringshare_del(static_cast<char*>(data));
  }
}

_Ewk_Policy_Decision::_Ewk_Policy_Decision(const GURL &request_url, const net::HttpResponseHeaders* response_headers, PolicyResponseDelegateEfl* delegate)
  : new_window_policy_delegate_(NULL)
  , policy_response_delegate_(delegate)
  , cookie_(NULL)
  , url_(NULL)
  , host_(NULL)
  , scheme_(NULL)
  , responseMime_(NULL)
  , responseHeaders_(NULL)
  , decisionType_(TW_POLICY_DECISION_USE)
  , navigationType_(TW_POLICY_NAVIGATION_TYPE_OTHER)
  , isDecided_(false)
  , isSuspended_(false)
  , responseStatusCode_(0)
  , AuthUser_(NULL)
  , AuthPassword_(NULL)
  , type_(POLICY_RESPONSE) {
  DCHECK(response_headers);
  DCHECK(delegate);

  ParseUrl(request_url);

  if (response_headers) {
    responseStatusCode_ = response_headers->response_code();

    std::string mime_type;

    if (response_headers->GetMimeType(&mime_type)) {
      responseMime_ = eina_stringshare_add(mime_type.c_str());

      if (!net::IsSupportedMimeType(mime_type))
        decisionType_ = TW_POLICY_DECISION_DOWNLOAD;
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
  , frame_(new Ewk_Frame(params))
  , cookie_(NULL)
  , url_(NULL)
  , host_(NULL)
  , scheme_(NULL)
  , responseMime_(NULL)
  , responseHeaders_(NULL)
  , decisionType_(TW_POLICY_DECISION_USE)
  , navigationType_(static_cast<tizen_webview::Policy_Navigation_Type>(params.type))
  , isDecided_(false)
  , isSuspended_(false)
  , responseStatusCode_(0)
  , AuthUser_(NULL)
  , AuthPassword_(NULL)
  , type_(POLICY_NAVIGATION) {
  ParseUrl(params.url);
}

_Ewk_Policy_Decision::_Ewk_Policy_Decision(content::WebContentsDelegateEfl* view, const GURL& url)
  : new_window_policy_delegate_(view)
  , cookie_(NULL)
  , url_(NULL)
  , host_(NULL)
  , scheme_(NULL)
  , responseMime_(NULL)
  , responseHeaders_(NULL)
  , decisionType_(TW_POLICY_DECISION_USE)
  , navigationType_(TW_POLICY_NAVIGATION_TYPE_OTHER)
  , isDecided_(false)
  , isSuspended_(false)
  , responseStatusCode_(0)
  , AuthUser_(NULL)
  , AuthPassword_(NULL)
  , type_(POLICY_NEWWINDOW) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  DCHECK(view);
  ParseUrl(url);

  RenderFrameHost* rfh = NULL;
  // we can use main frame here
  if (view && view->web_contents()) {
    view->web_contents()->GetMainFrame();
  }

  frame_.reset(new Ewk_Frame(rfh));
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
    case _Ewk_Policy_Decision::POLICY_RESPONSE:
      policy_response_delegate_->IgnoreResponse();
      break;
    case _Ewk_Policy_Decision::POLICY_NAVIGATION:
      navigation_policy_handler_->SetDecision(NavigationPolicyHandlerEfl::Handled);
      break;
    case _Ewk_Policy_Decision::POLICY_NEWWINDOW:
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
    case _Ewk_Policy_Decision::POLICY_RESPONSE:
      policy_response_delegate_->UseResponse();
      break;
    case _Ewk_Policy_Decision::POLICY_NAVIGATION:
      navigation_policy_handler_->DownloadNavigation();
      break;
    case _Ewk_Policy_Decision::POLICY_NEWWINDOW:
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

void _Ewk_Policy_Decision::InitializeOnUIThread() {
  DCHECK(type_ == _Ewk_Policy_Decision::POLICY_RESPONSE);
  DCHECK(policy_response_delegate_);

  if (policy_response_delegate_) {
    RenderFrameHost *host = RenderFrameHost::FromID(policy_response_delegate_->GetRenderProcessId(), policy_response_delegate_->GetRenderFrameId());

    // Download request has no render frame id, they're detached. We override it with main frame from render view id
    if (!host) {
      RenderViewHost *viewhost = RenderViewHost::FromID(policy_response_delegate_->GetRenderProcessId(), policy_response_delegate_->GetRenderViewId());

      //DCHECK(viewhost);
      if (viewhost) {
        host = viewhost->GetMainFrame();
      }
    }

    if (host) {
      /*
       * In some situations there is no renderer associated to the response
       * Such case can be observed while running TC utc_blink_ewk_geolocation_permission_request_suspend_func
       */
      frame_.reset(new Ewk_Frame(host));
    }
  }
}

Ewk_Frame* _Ewk_Policy_Decision::GetFrameRef() const {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  // Ups, forgot to initialize something?
  DCHECK(frame_.get());
  return frame_.get();
}

void _Ewk_Policy_Decision::ParseUrl(const GURL& url) {
  url_ = eina_stringshare_add(url.spec().c_str());
  scheme_ = eina_stringshare_add(url.scheme().c_str());
  host_ = eina_stringshare_add(url.host().c_str());
}
