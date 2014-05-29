/*
 * Copyright (C) 2013 Samsung Electronics
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
 */

#ifndef ewk_auth_challenge_private_h
#define ewk_auth_challenge_private_h

#include "url/gurl.h"
#include "net/base/auth.h"
#include "browser/login_delegate_efl.h"
/**
 * @brief  Structure used to send credential for authentication challenge.
 *
 * Details If Authentication challenge requirement is received, AuthenticationChallenge is created,
 * and realm, host url are received from server.
 * These information are sent to notify by evas_object_smart_callback_call.
 */
struct _Ewk_Auth_Challenge {
  _Ewk_Auth_Challenge(LoginDelegateEfl* delegate, const GURL& url, const std::string& realm)
      : login_delegate(delegate),
        url(url.possibly_invalid_spec()),
        realm(realm),
        is_decided(false),
        is_suspended(false) {
  }

  scoped_refptr<LoginDelegateEfl> login_delegate;
  std::string url;
  std::string realm;
  bool is_decided;
  bool is_suspended;
};

#endif // ewk_auth_challenge_private_h
