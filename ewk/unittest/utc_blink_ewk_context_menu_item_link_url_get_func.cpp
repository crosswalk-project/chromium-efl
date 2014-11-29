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
#include "utc_blink_ewk_context_menu.h"

class utc_blink_ewk_context_menu_item_link_url_get : public utc_blink_ewk_base {
public:
  utc_blink_ewk_context_menu_item_link_url_get()
  : utc_blink_ewk_base()
  , is_failed(EINA_FALSE)
  , url(0)
  , select_timer(0)
  {
  }

protected:
  virtual void LoadFinished(Evas_Object* webview)
  {
    feed_mouse_click(3, 100, 300, GetEwkEvas());
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
    evas_object_smart_callback_add(GetEwkWebView(), "contextmenu,selected", contextmenu_selected_callback, this);
  }

  virtual void PreTearDown()
  {
    evas_object_smart_callback_del(GetEwkWebView(), "contextmenu,customize", contextmenu_customize_callback);
    evas_object_smart_callback_del(GetEwkWebView(), "contextmenu,selected", contextmenu_selected_callback);
    if (url) {
      free(url);
    }
    if (select_timer) {
      ecore_timer_del(select_timer);
    }
  }

  static void contextmenu_customize_callback(void* data, Evas_Object* webview, void* event_info)
  {
    utc_message("[contextmenu_customize_callback] :: \n");
    if (!data || !event_info) {
      FAIL();
    }
    Ewk_Context_Menu* contextmenu = static_cast<Ewk_Context_Menu*>(event_info);
    ewk_context_menu_item_append_as_action(contextmenu, CUSTOM_CONTEXT_MENU_ITEM_TEST_TAG, "< Item for Test >", EINA_TRUE);
    ewk_context_menu_item_append_as_action(contextmenu, CUSTOM_CONTEXT_MENU_ITEM_TEST_TAG, "< Item for Test >", EINA_TRUE);
    ewk_context_menu_item_append_as_action(contextmenu, CUSTOM_CONTEXT_MENU_ITEM_TEST_TAG, "< Item for Test >", EINA_TRUE);

    utc_blink_ewk_context_menu_item_link_url_get *owner = static_cast<utc_blink_ewk_context_menu_item_link_url_get*>(data);
    owner->select_item();
  }

  static void contextmenu_selected_callback(void* data, Evas_Object* webview, void* event_info)
  {
    utc_message("[contextmenu_selected_callback] :: \n");
    if (!data || !event_info) {
      FAIL();
    }
    Ewk_Context_Menu_Item* item = static_cast<Ewk_Context_Menu_Item*>(event_info);
    utc_blink_ewk_context_menu_item_link_url_get *owner = static_cast<utc_blink_ewk_context_menu_item_link_url_get*>(data);

    const char* link_url = ewk_context_menu_item_link_url_get(item);
    if (!link_url) {
      owner->is_failed = EINA_TRUE;
    }
    else {
      owner->url = strdup(link_url);
    }
    owner->EventLoopStop(utc_blink_ewk_base::Success);
  }

  static Eina_Bool select_timer_callback(void* data)
  {
    utc_message("[select_timer_callback] :: \n");
    if (data) {
      utc_blink_ewk_context_menu_item_link_url_get *owner = static_cast<utc_blink_ewk_context_menu_item_link_url_get*>(data);
      feed_mouse_click(1, 50, 450, owner->GetEwkEvas());
      owner->select_timer = 0;
    }
    return ECORE_CALLBACK_CANCEL;
  }

  void select_item()
  {
    select_timer = ecore_timer_add(1, select_timer_callback, this);
  }


protected:
  Eina_Bool is_failed;
  char* url;
  Ecore_Timer* select_timer;
};

/**
 * @brief Tests whether the getting link url from context menu item is performed properly.
 */
TEST_F(utc_blink_ewk_context_menu_item_link_url_get, POS_TEST)
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
  if (!url) {
    FAIL();
  }
  EXPECT_STREQ(url, "http://www.webkit.org/");
}

/**
 * @brief Tests whether the function works properly for case Ewk_Context_Menu object is NULL.
 */
TEST_F(utc_blink_ewk_context_menu_item_link_url_get, NEG_TEST)
{
  int check = 0;
  if (ewk_context_menu_item_link_url_get(NULL)) {
    check = 1;
  }
  EXPECT_EQ(check, 0);
}
