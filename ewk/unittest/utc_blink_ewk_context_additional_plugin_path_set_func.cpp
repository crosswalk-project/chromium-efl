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

class utc_blink_ewk_context_additional_plugin_path_set : public utc_blink_ewk_base
{
protected:
  static const char*const plugin;
};

const char*const utc_blink_ewk_context_additional_plugin_path_set::plugin="/tmp";

/**
 * @brief Tests if can set additional plugin path.
 */
TEST_F(utc_blink_ewk_context_additional_plugin_path_set, POS_TEST)
{
  Eina_Bool result = ewk_context_additional_plugin_path_set(ewk_view_context_get(GetEwkWebView()),plugin);
  utc_check_eq(result, EINA_TRUE);
}

/**
 * @brief Tests if returns false which context is null.
 */
TEST_F(utc_blink_ewk_context_additional_plugin_path_set, NEG_TEST)
{
  Eina_Bool result = ewk_context_additional_plugin_path_set(NULL, NULL);
  utc_check_ne(result, EINA_TRUE);
}
