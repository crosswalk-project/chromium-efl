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

#include <algorithm>
#include "web_contents_delegate_efl.h"

using content::BrowserThread;
using content::RenderFrameHost;
using content::RenderViewHost;
using namespace tizen_webview;

namespace {
  void FreeStringShare(void *data) {
    eina_stringshare_del(static_cast<char*>(data));
  }

  // If the MIME type, |mime_type| is "application/octet-stream",
  // and if a URL, |request_url| has a webfont extension,
  // Let's set the policy decision type as "USE."
  tizen_webview::Policy_Decision_Type GetSanitizedDecisionTypeForWebFontIfNeeded(
    const GURL &request_url,
    const std::string& mime_type,
    tizen_webview::Policy_Decision_Type default_value) {

    const char* kWoff = ".woff";
    const char* kEot = ".eot";
    const char* kOtf = ".otf";
    const char* kApplicationOctetStream = "application/octet-stream";

    tizen_webview::Policy_Decision_Type return_value = default_value;

    if (request_url.is_empty() || !request_url.is_valid())
      return return_value;

    if (mime_type == kApplicationOctetStream) {
      std::string file_name = request_url.ExtractFileName();
      if (!file_name.empty()) {
        size_t pos = file_name.rfind('.');
        if (pos != std::string::npos) {
          std::string ext = file_name.substr(pos); // Extension
          if (ext == kWoff || ext == kEot || ext == kOtf)
            return_value = TW_POLICY_DECISION_USE;
        }
      }
    }
    return return_value;
  }
}

_Ewk_Policy_Decision::_Ewk_Policy_Decision(const GURL &request_url, const net::HttpResponseHeaders* response_headers, PolicyResponseDelegateEfl* delegate)
  : new_window_policy_delegate_(NULL)
  , policy_response_delegate_(delegate)
  , responseHeaders_(NULL)
  , decisionType_(TW_POLICY_DECISION_USE)
  , navigationType_(TW_POLICY_NAVIGATION_TYPE_OTHER)
  , isDecided_(false)
  , isSuspended_(false)
  , responseStatusCode_(0)
  , type_(POLICY_RESPONSE) {
  DCHECK(response_headers);
  DCHECK(delegate);

  ParseUrl(request_url);

  if (response_headers) {
    responseStatusCode_ = response_headers->response_code();

    std::string mime_type;

    if (response_headers->GetMimeType(&mime_type)) {
      responseMime_ = mime_type;

      if (!net::IsSupportedMimeType(mime_type))
        decisionType_ = TW_POLICY_DECISION_DOWNLOAD;

      // Fallback to TW_POLICY_DECISION_USE if it is a webfont.
      decisionType_ = GetSanitizedDecisionTypeForWebFontIfNeeded(
        request_url, mime_type, decisionType_);
    }

    if (request_url.has_password() && request_url.has_username())
      SetAuthorizationIfNecessary(request_url);

    std::string set_cookie_;

    if (response_headers->EnumerateHeader(NULL, "Set-Cookie", &set_cookie_))
      cookie_ = set_cookie_;

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
  , responseHeaders_(NULL)
  , decisionType_(TW_POLICY_DECISION_USE)
  , navigationType_(static_cast<tizen_webview::Policy_Navigation_Type>(params.type))
  , isDecided_(false)
  , isSuspended_(false)
  , responseStatusCode_(0)
  , type_(POLICY_NAVIGATION) {
  ParseUrl(params.url);
  if (!params.auth.isEmpty())
    SetAuthorizationIfNecessary(params.auth.utf8());
  else if (params.url.has_password() && params.url.has_username())
    SetAuthorizationIfNecessary(params.url);
}

_Ewk_Policy_Decision::_Ewk_Policy_Decision(content::WebContentsDelegateEfl* view, const GURL& url)
  : new_window_policy_delegate_(view)
  , responseHeaders_(NULL)
  , decisionType_(TW_POLICY_DECISION_USE)
  , navigationType_(TW_POLICY_NAVIGATION_TYPE_OTHER)
  , isDecided_(false)
  , isSuspended_(false)
  , responseStatusCode_(0)
  , type_(POLICY_NEWWINDOW) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  DCHECK(view);
  ParseUrl(url);

  RenderFrameHost* rfh = NULL;
  // we can use main frame here
  if (view) {
    view->web_contents().GetMainFrame();
  }

  if (url.has_password() && url.has_username())
    SetAuthorizationIfNecessary(url);

  frame_.reset(new Ewk_Frame(rfh));
}

_Ewk_Policy_Decision::~_Ewk_Policy_Decision() {
  eina_hash_free(responseHeaders_);
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
  DCHECK(policy_response_delegate_.get());

  if (policy_response_delegate_.get()) {
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
  url_ = url.spec();
  scheme_ = url.scheme();
  host_ = url.host();
}

void _Ewk_Policy_Decision::SetAuthorizationIfNecessary(const GURL& request_url) {
  // There is no need to check if username or password is empty.
  // It was checked befor in constructor
  AuthPassword_ = request_url.password();
  AuthUser_ = request_url.username();
}

void _Ewk_Policy_Decision::SetAuthorizationIfNecessary(const std::string request) {
  std::string type = request.substr(0, request.find_first_of(' '));
  std::transform(type.begin(), type.end(), type.begin(), ::toupper);

  if (type.compare(BASIC_AUTHORIZATION)) {
    AuthUser_.clear();
    AuthPassword_.clear();
    return;
  }

  std::size_t space = request.find(' ');
  std::size_t colon = request.find(':');

  DCHECK(space != std::string::npos && colon != std::string::npos && colon != request.length());
  if (space == std::string::npos || colon == std::string::npos || colon == request.length())
    return;

  AuthUser_ = request.substr(space + 1, request.length() - colon - 1);
  AuthPassword_ = request.substr(colon + 1);
}

const char* _Ewk_Policy_Decision::GetCookie() const {
  return cookie_.empty() ? NULL : cookie_.c_str();
}

const char* _Ewk_Policy_Decision::GetAuthUser() const {
  return AuthUser_.empty() ? NULL : AuthUser_.c_str();
}

const char* _Ewk_Policy_Decision::GetAuthPassword() const {
  return AuthPassword_.empty() ? NULL : AuthPassword_.c_str();
}

const char* _Ewk_Policy_Decision::GetUrl() const {
  return url_.empty() ? NULL : url_.c_str();
}

const char* _Ewk_Policy_Decision::GetScheme() const {
  return scheme_.empty() ? NULL : scheme_.c_str();
}

const char* _Ewk_Policy_Decision::GetHost() const {
  return host_.empty() ? NULL : host_.c_str();
}

const char* _Ewk_Policy_Decision::GetResponseMime() const {
  return responseMime_.empty() ? NULL : responseMime_.c_str();
}
