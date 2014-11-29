/*
 * Chromium EFL
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

class utc_blink_ewk_settings_spdy_enabled_set : public utc_blink_ewk_base {
};

/**
* @brief Tests if returns TRUE when initiated with correct settings and
* enabled spdy set to TRUE.
*/
TEST_F(utc_blink_ewk_settings_spdy_enabled_set, POS_TEST)
{
  Ewk_Settings* settings = ewk_view_settings_get(GetEwkWebView());
  ASSERT_TRUE(settings);

  Eina_Bool result = ewk_settings_spdy_enabled_set(settings, EINA_TRUE);
  ASSERT_EQ(EINA_TRUE, result);
}

/**
* @brief Tests if returns FALSE when initiated with NULL settings.
*/
TEST_F(utc_blink_ewk_settings_spdy_enabled_set, NEG_TEST)
{
  Eina_Bool result = ewk_settings_spdy_enabled_set(NULL, EINA_FALSE);
  EXPECT_NE(result, EINA_TRUE);
}
