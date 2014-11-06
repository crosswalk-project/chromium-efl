// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_settings_scripts_can_open_windows_set : public utc_blink_ewk_base {
protected:
  utc_blink_ewk_settings_scripts_can_open_windows_set()
    : settings(NULL)
  {
  }

  void PostSetUp()
  {
    settings = ewk_view_settings_get(GetEwkWebView());
    ASSERT_TRUE(settings);
    // make sure default value is proper
    ASSERT_EQ(EINA_FALSE, ewk_settings_scripts_can_open_windows_get(settings));

    evas_object_smart_callback_add(GetEwkWebView(), "popup,blocked", ToSmartCallback(popup_blocked_cb), this);
    evas_object_smart_callback_add(GetEwkWebView(), "create,window", ToSmartCallback(create_window_cb), this);
  }

  void PreTearDown()
  {
    evas_object_smart_callback_del(GetEwkWebView(), "popup,blocked", ToSmartCallback(popup_blocked_cb));
    evas_object_smart_callback_del(GetEwkWebView(), "create,window", ToSmartCallback(create_window_cb));
  }

  static void popup_blocked_cb(utc_blink_ewk_settings_scripts_can_open_windows_set* owner, Evas_Object*, Eina_Stringshare* uri)
  {
    ASSERT_TRUE(owner);
    owner->EventLoopStop(Success);
  }

  static void create_window_cb(utc_blink_ewk_settings_scripts_can_open_windows_set* owner, Evas_Object*, void*)
  {
    ASSERT_TRUE(owner);
    owner->EventLoopStop(Failure);
  }

protected:
  Ewk_Settings* settings;
};

/**
 * @brief Positive test case of ewk_settings_scripts_can_open_windows_set()
 */
TEST_F(utc_blink_ewk_settings_scripts_can_open_windows_set, SetTrue)
{
  // first check if new windows are blocked
  char htmlBuffer[] = "<html>"
                        "<head></head>"
                        "<body onload=\"window.open('http://www.google.com/')\"></body>"
                      "</html>";

  ASSERT_EQ(EINA_TRUE, ewk_view_html_string_load(GetEwkWebView(), htmlBuffer, NULL, NULL));
  ASSERT_EQ(Success, EventLoopStart());

  // now toggle option
  ASSERT_EQ(EINA_TRUE, ewk_settings_scripts_can_open_windows_set(settings, EINA_TRUE));

  // check if option was toggled
  ASSERT_EQ(EINA_TRUE, ewk_settings_scripts_can_open_windows_get(settings));

  // reload page and expect create window smart callback
  ASSERT_EQ(EINA_TRUE, ewk_view_html_string_load(GetEwkWebView(), htmlBuffer, NULL, NULL));
  ASSERT_EQ(Failure, EventLoopStart());
}

TEST_F(utc_blink_ewk_settings_scripts_can_open_windows_set, ToggleBeforeLoad)
{
  // Got feedback, that whem property is modified before loading anything to the view
  // it may not work. This TC checks such case and tests provided solution

  // toggle option
  ASSERT_EQ(EINA_TRUE, ewk_settings_scripts_can_open_windows_set(settings, EINA_TRUE));

  // check if option was toggled
  ASSERT_EQ(EINA_TRUE, ewk_settings_scripts_can_open_windows_get(settings));

  char htmlBuffer[] = "<html>"
                        "<head></head>"
                        "<body onload=\"window.open('http://www.google.com/')\"></body>"
                      "</html>";

  ASSERT_EQ(EINA_TRUE, ewk_view_html_string_load(GetEwkWebView(), htmlBuffer, NULL, NULL));
  // Expect that window,create is called - option did work
  ASSERT_EQ(Failure, EventLoopStart());

  // toggle option again
  ASSERT_EQ(EINA_TRUE, ewk_settings_scripts_can_open_windows_set(settings, EINA_FALSE));

  // check if option was toggled
  ASSERT_EQ(EINA_FALSE, ewk_settings_scripts_can_open_windows_get(settings));

  // reload page and expect create window smart callback
  ASSERT_EQ(EINA_TRUE, ewk_view_html_string_load(GetEwkWebView(), htmlBuffer, NULL, NULL));
  // window.open should be blocked again - popup,blocked smart callback should be invoked
  ASSERT_EQ(Success, EventLoopStart());
}

/**
 * @brief Test case of ewk_settings_scripts_can_open_windows_set() when view is NULL
 */
TEST_F(utc_blink_ewk_settings_scripts_can_open_windows_set, InvalidArg)
{
  EXPECT_EQ(EINA_FALSE, ewk_settings_scripts_can_open_windows_set(NULL, EINA_TRUE));
}
