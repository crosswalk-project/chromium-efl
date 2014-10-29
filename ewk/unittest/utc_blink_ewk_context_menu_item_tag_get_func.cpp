// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"
#include "utc_blink_ewk_context_menu.h"

class utc_blink_ewk_context_menu_item_tag_get : public utc_blink_ewk_base {
public:
  utc_blink_ewk_context_menu_item_tag_get()
  : utc_blink_ewk_base()
  , result(EINA_FALSE)
  , is_failed(EINA_FALSE)
  , select_timer(0)
  , tag(EWK_CONTEXT_MENU_ITEM_BASE_APPLICATION_TAG)
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
    evas_object_smart_callback_add(GetEwkWebView(), "contextmenu,selected", contextmenu_selected_callback, this);
  }

  virtual void PreTearDown()
  {
    evas_object_smart_callback_del(GetEwkWebView(), "contextmenu,customize", contextmenu_customize_callback);
    evas_object_smart_callback_del(GetEwkWebView(), "contextmenu,selected", contextmenu_selected_callback);
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
    Ewk_Context_Menu_Item* item = ewk_context_menu_nth_item_get(contextmenu, 0);
    while (item) {
      ewk_context_menu_item_remove(contextmenu, item);
      item = ewk_context_menu_nth_item_get(contextmenu, 0);
    }

    ewk_context_menu_item_append_as_action(contextmenu, CUSTOM_CONTEXT_MENU_ITEM_TEST_TAG, "< Item for Test >", EINA_TRUE);
    ewk_context_menu_item_append_as_action(contextmenu, CUSTOM_CONTEXT_MENU_ITEM_TEST_TAG, "< Item for Test1 >", EINA_TRUE);
    ewk_context_menu_item_append_as_action(contextmenu, CUSTOM_CONTEXT_MENU_ITEM_TEST_TAG, "< Item for Test2 >", EINA_TRUE);

    utc_blink_ewk_context_menu_item_tag_get *owner = static_cast<utc_blink_ewk_context_menu_item_tag_get*>(data);
    owner->select_item();
  }

  static void contextmenu_selected_callback(void* data, Evas_Object* webview, void* event_info)
  {
    utc_message("[contextmenu_selected_callback] :: \n");
    if (!data || !event_info) {
      FAIL();
    }
    Ewk_Context_Menu_Item* item = static_cast<Ewk_Context_Menu_Item*>(event_info);
    utc_blink_ewk_context_menu_item_tag_get *owner = static_cast<utc_blink_ewk_context_menu_item_tag_get*>(data);

    owner->tag = ewk_context_menu_item_tag_get(item);
    owner->EventLoopStop(utc_blink_ewk_base::Success);
  }

  static Eina_Bool select_timer_callback(void* data)
  {
    utc_message("[select_timer_callback] :: \n");
    if (data) {
      utc_blink_ewk_context_menu_item_tag_get *owner = static_cast<utc_blink_ewk_context_menu_item_tag_get*>(data);
      feed_mouse_click(1, 50, 250, owner->GetEwkEvas());
      owner->select_timer = 0;
    }
    return ECORE_CALLBACK_CANCEL;
  }

  void select_item()
  {
    select_timer = ecore_timer_add(1, select_timer_callback, this);
  }

protected:
  Eina_Bool result;
  Eina_Bool is_failed;
  Ecore_Timer* select_timer;
  Ewk_Context_Menu_Item_Tag tag;
};


/**
 * @brief Tests whether the getting tag from context menu item is performed properly.
 */
TEST_F(utc_blink_ewk_context_menu_item_tag_get, POS_TEST)
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
  EXPECT_EQ(tag, CUSTOM_CONTEXT_MENU_ITEM_TEST_TAG);
}

/**
 * @brief Tests whether the function works properly for case Ewk_Context_Menu object is NULL.
 */
TEST_F(utc_blink_ewk_context_menu_item_tag_get, NEG_TEST)
{
  EXPECT_EQ(ewk_context_menu_item_tag_get(NULL), EWK_CONTEXT_MENU_ITEM_TAG_NO_ACTION);
}
