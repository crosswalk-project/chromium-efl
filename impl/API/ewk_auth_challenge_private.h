// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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
