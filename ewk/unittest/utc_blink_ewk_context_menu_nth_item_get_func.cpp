// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"
#include "utc_blink_ewk_context_menu.h"

class utc_blink_ewk_context_menu_nth_item_get : public utc_blink_ewk_base {
public:
  utc_blink_ewk_context_menu_nth_item_get()
  : utc_blink_ewk_base()
  , is_failed(EINA_FALSE)
  , item(0)
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

  virtual void PreTearDown()
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
    utc_blink_ewk_context_menu_nth_item_get *owner = static_cast<utc_blink_ewk_context_menu_nth_item_get*>(data);

    owner->item = ewk_context_menu_nth_item_get(contextmenu, 0);
    owner->EventLoopStop(utc_blink_ewk_base::Success);
  }

protected:
  Eina_Bool is_failed;
  Ewk_Context_Menu_Item* item;
};

/**
 * @brief Tests whether the appending item to context menu is performed properly.
 */
TEST_F(utc_blink_ewk_context_menu_nth_item_get, POS_TEST)
{
  if (!is_failed) {
    ASSERT_EQ(Success, EventLoopStart());
  }

  ASSERT_FALSE(is_failed);
  EXPECT_TRUE(NULL != item);
}

/**
 * @brief Tests whether the function works properly for case Ewk_Context_Menu object is NULL.
 */
TEST_F(utc_blink_ewk_context_menu_nth_item_get, NEG_TEST)
{
  EXPECT_EQ(0, ewk_context_menu_nth_item_get(NULL, 0));
}
