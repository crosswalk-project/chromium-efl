// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#if !defined(EWK_BRINGUP)

#include "public/ewk_view.h"
#include "public/ewk_cookie_manager.h"
#include "tizen_webview/public/tw_unfocus_direction.h"
#include "tizen_webview/public/tw_cookie_accept_policy.h"

template <bool> class StaticAssert;
template <> class StaticAssert<true> { };

#define STATIC_ASSERT(condition, message) \
    { StaticAssert<((condition) != 0)> ERROR_##message; (void)ERROR_##message; }

#define STATIC_ENUM_IS_EQUAL(left, right) \
    STATIC_ASSERT(static_cast<int>(left) == static_cast<int>(right), _ENUM_VALUE_MISMATCH)

void do_static_check()
{
  STATIC_ENUM_IS_EQUAL(tizen_webview::TW_UNFOCUS_DIRECTION_FORWARD, EWK_UNFOCUS_DIRECTION_FORWARD);
  STATIC_ENUM_IS_EQUAL(tizen_webview::TW_UNFOCUS_DIRECTION_BACKWARD, EWK_UNFOCUS_DIRECTION_BACKWARD);

  STATIC_ENUM_IS_EQUAL(EWK_COOKIE_ACCEPT_POLICY_ALWAYS, tizen_webview::TW_COOKIE_ACCEPT_POLICY_ALWAYS);
  STATIC_ENUM_IS_EQUAL(EWK_COOKIE_ACCEPT_POLICY_NEVER, tizen_webview::TW_COOKIE_ACCEPT_POLICY_NEVER);
  STATIC_ENUM_IS_EQUAL(EWK_COOKIE_ACCEPT_POLICY_NO_THIRD_PARTY, tizen_webview::TW_COOKIE_ACCEPT_POLICY_NO_THIRD_PARTY);
}
#endif

