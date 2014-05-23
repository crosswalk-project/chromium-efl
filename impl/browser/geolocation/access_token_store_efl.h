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

#ifndef ACCESS_TOKEN_STORE_EFL_H_
#define ACCESS_TOKEN_STORE_EFL_H_

#include "content/public/browser/access_token_store.h"
#include "browser_context_efl.h"

namespace content {

class BrowserContextEfl;

class AccessTokenStoreEfl : public content::AccessTokenStore {
 public:
  AccessTokenStoreEfl();

  // AccessTokenStore
  virtual void LoadAccessTokens(const LoadAccessTokensCallbackType& callback) OVERRIDE;
  virtual void SaveAccessToken(const GURL& server_url, const base::string16& access_token) OVERRIDE;

 private:
  virtual ~AccessTokenStoreEfl();

  void GetRequestContextOnUIThread();
  void RespondOnOriginatingThread(const LoadAccessTokensCallbackType& callback);

  net::URLRequestContextGetter* system_request_context_;

  DISALLOW_COPY_AND_ASSIGN(AccessTokenStoreEfl);
};

}  // namespace content

#endif  // ACCESS_TOKEN_STORE_EFL_H_

