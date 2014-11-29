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

class utc_blink_ewk_settings_link_magnifier_enabled_get_func : public utc_blink_ewk_base
{
};


/**
 * @brief Tests getting link magnifier enabled.
 */
TEST_F(utc_blink_ewk_settings_link_magnifier_enabled_get_func, POS_TEST_TRUE)
{
  Ewk_Settings* settings = ewk_view_settings_get(GetEwkWebView());
  ASSERT_TRUE(settings);

  ewk_settings_link_magnifier_enabled_set(settings, EINA_TRUE);
  ASSERT_TRUE(ewk_settings_link_magnifier_enabled_get(settings));
}

/**
 * @brief Tests if getting link magnifier enabled with NULL settings fails.
 */
TEST_F(utc_blink_ewk_settings_link_magnifier_enabled_get_func, NEG_TEST)
{

  Ewk_Settings* settings = ewk_view_settings_get(GetEwkWebView());
  ASSERT_TRUE(settings);
  ewk_settings_link_magnifier_enabled_set(settings, EINA_TRUE);
  ASSERT_TRUE(ewk_settings_link_magnifier_enabled_get(settings));

  ASSERT_FALSE(ewk_settings_link_magnifier_enabled_get(NULL));
}
