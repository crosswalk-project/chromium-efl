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

#include "utc_blink_ewk_base.h"
#include "utc_blink_ewk_context_menu.h"

class utc_blink_ewk_context_menu_item_remove : public utc_blink_ewk_base {
public:
  utc_blink_ewk_context_menu_item_remove()
  : utc_blink_ewk_base()
  , result(EINA_FALSE)
  , is_failed(EINA_FALSE)
  {
  }

protected:
  virtual void LoadFinished(Evas_Object* webview)
  {
    feed_mouse_click(3, 100, 100, GetEwkEvas());
  }

  virtual bool LoadError(Evas_Object* webview, Ewk_Error *error)
  {
    is_failed = EINA_TRUE;
    EventLoopStop(utc_blink_ewk_base::Success);
    return false;
  }

  virtual void PostSetUp()
  {
    /* Enable mouse events to feed events directly. */
    ewk_view_mouse_events_enabled_set(GetEwkWebView(), EINA_TRUE);

    Eina_Bool result_set = ewk_view_url_set(GetEwkWebView(), GetResourceUrl("/ewk_context_menu/index.html").c_str());
    if (!result_set) {
      FAIL();
    }
    evas_object_smart_callback_add(GetEwkWebView(), "contextmenu,customize", contextmenu_customize_callback, this);
  }

  virtual void TearDown()
  {
    evas_object_smart_callback_del(GetEwkWebView(), "contextmenu,customize", contextmenu_customize_callback);
  }

  static void contextmenu_customize_callback(void* data, Evas_Object* webview, void* event_info)
  {
    utc_message("[contextmenu_customize_callback] :: \n");
    if (!data || !event_info) {
      FAIL();
    }
    Ewk_Context_Menu* contextmenu = static_cast<Ewk_Context_Menu*>(event_info);
    Ewk_Context_Menu_Item* item = ewk_context_menu_nth_item_get(contextmenu, 0);

    utc_blink_ewk_context_menu_item_remove *owner = static_cast<utc_blink_ewk_context_menu_item_remove*>(data);
    owner->result = ewk_context_menu_item_remove(contextmenu, item);

    owner->EventLoopStop(utc_blink_ewk_base::Success);
  }

protected:
  Eina_Bool result;
  Eina_Bool is_failed;
};


/**
 * @brief Tests whether the removing item from context menu is performed properly.
 */
TEST_F(utc_blink_ewk_context_menu_item_remove, POS_TEST)
{
  if (!is_failed) {
    utc_blink_ewk_base::MainLoopResult result_loop = EventLoopStart();
    if (result_loop != utc_blink_ewk_base::Success) {
      FAIL();
    }
  }

  if (is_failed) {
    FAIL();
  }
  EXPECT_EQ(result, EINA_TRUE);
}

/**
 * @brief Tests whether the function works properly for case Ewk_Context_Menu object is NULL.
 */
TEST_F(utc_blink_ewk_context_menu_item_remove, NEG_TEST)
{
  EXPECT_EQ(ewk_context_menu_item_remove(NULL, NULL), EINA_FALSE);
}
