/*
 * chromium EFL
 *
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_view_touch_events_enabled_set : public utc_blink_ewk_base {
};

/**
 * @brief Checking whether the state of mouse events behavior is set properly.
 */
TEST_F(utc_blink_ewk_view_touch_events_enabled_set, POS_TEST)
{
  ewk_view_touch_events_enabled_set(GetEwkWebView(), EINA_TRUE);
  Eina_Bool enable_get = ewk_view_touch_events_enabled_get(GetEwkWebView());
  if (enable_get != EINA_TRUE) {
    FAIL();
  }

  ewk_view_touch_events_enabled_set(GetEwkWebView(), EINA_FALSE);
  enable_get = ewk_view_touch_events_enabled_get(GetEwkWebView());
  if (enable_get != EINA_FALSE) {
    FAIL();
  }
}

/**
 * @brief There is no negative case for ewk_view_mouse_events_enabled_set function.
 */
TEST_F(utc_blink_ewk_view_touch_events_enabled_set, NEG_TEST)
{
  ewk_view_touch_events_enabled_set(NULL, EINA_FALSE);
  ewk_view_touch_events_enabled_set(NULL, EINA_TRUE);

  SUCCEED();
}

