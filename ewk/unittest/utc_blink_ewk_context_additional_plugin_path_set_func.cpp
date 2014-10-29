// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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
