// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "browser/geolocation/access_token_store_efl.h"

#include "base/bind.h"
#include "base/message_loop/message_loop.h"
#include "base/strings/utf_string_conversions.h"
#include "content/public/browser/browser_thread.h"
#include "eweb_context.h"
#include "tizen_webview/public/tw_web_context.h"

using tizen_webview::WebContext;

namespace content {

AccessTokenStoreEfl::AccessTokenStoreEfl()
    : system_request_context_(NULL)
{
}

AccessTokenStoreEfl::~AccessTokenStoreEfl()
{
}

void AccessTokenStoreEfl::LoadAccessTokens(const LoadAccessTokensCallbackType& callback)
{
  BrowserThread::PostTaskAndReply(BrowserThread::UI,
                                  FROM_HERE,
                                  base::Bind(&AccessTokenStoreEfl::GetRequestContextOnUIThread,
                                             this),
                                  base::Bind(&AccessTokenStoreEfl::RespondOnOriginatingThread,
                                             this,
                                             callback));
}

void AccessTokenStoreEfl::GetRequestContextOnUIThread()
{
  WebContext* default_context = WebContext::DefaultContext();
  net::URLRequestContextGetter* context_getter = default_context->browser_context()->GetRequestContext();
  system_request_context_ = context_getter;
}

void AccessTokenStoreEfl::RespondOnOriginatingThread(const LoadAccessTokensCallbackType& callback)
{
  AccessTokenSet access_token_set;
  callback.Run(access_token_set, system_request_context_);
}

void AccessTokenStoreEfl::SaveAccessToken(const GURL& /*server_url*/, const base::string16& /*access_token*/)
{
}

}  // namespace content
