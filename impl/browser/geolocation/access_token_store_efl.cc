/*
   Copyright (C) 2013 Samsung Electronics

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "browser/geolocation/access_token_store_efl.h"

#include "base/bind.h"
#include "base/message_loop/message_loop.h"
#include "base/strings/utf_string_conversions.h"
#include "content/public/browser/browser_thread.h"
#include "eweb_context.h"

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
  EWebContext* default_context = EWebContext::DefaultContext();
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
