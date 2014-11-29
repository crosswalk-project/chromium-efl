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

/* Define those macros _before_ you include the utc_blink_ewk.h header file. */

#include "utc_blink_ewk_context_menu.h"

static Eina_Bool enabled = EINA_FALSE;

extern struct Ewk_Test_View test_view;

class utc_blink_ewk_context_menu_item_enabled_get : public ::testing::Test {
protected:
  utc_blink_ewk_context_menu_item_enabled_get() {
  }

  virtual ~utc_blink_ewk_context_menu_item_enabled_get() {
  }

  static void contextmenu_customize_callback(void* data, Evas_Object* webview, void* event_info)
  {
    utc_message("[contextmenu_customize_callback] :: \n");
    Ewk_Context_Menu* contextmenu = (Ewk_Context_Menu*)event_info;
    Ewk_Context_Menu_Item* item = ewk_context_menu_nth_item_get(contextmenu, 0);
    enabled = ewk_context_menu_item_enabled_get(item);

    utc_blink_main_loop_quit();
  }

  /* Startup and cleanup functions */
  virtual void SetUp()
  {
    utc_blink_ewk_context_menu_test_init();
    evas_object_smart_callback_add(test_view.webview, "contextmenu,customize", contextmenu_customize_callback, 0);
  }

  virtual void TearDown()
  {
    evas_object_smart_callback_del(test_view.webview, "contextmenu,customize", contextmenu_customize_callback);
    utc_blink_ewk_context_menu_test_end();
  }
};

/**
 * @brief Tests whether the getting enabled from context menu item is performed properly.
 */
TEST_F(utc_blink_ewk_context_menu_item_enabled_get, POS_TEST)
{
  if (!is_failed)
    utc_blink_main_loop_begin();

  if (is_failed)
    utc_fail();

  utc_check_eq(enabled, EINA_TRUE);
}

/**
 * @brief Tests whether the function works properly for case Ewk_Context_Menu object is NULL.
 */
TEST_F(utc_blink_ewk_context_menu_item_enabled_get, NEG_TEST)
{
  utc_check_eq(ewk_context_menu_item_enabled_get(NULL), EINA_FALSE);
}
