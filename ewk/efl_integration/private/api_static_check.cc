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

