// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

#define TITLE1 "TestTitle1"
#define TITLE2 "TestTitle2"

class utc_blink_cb_title_changed : public utc_blink_ewk_base
{
protected:
  utc_blink_cb_title_changed()
    : changed_title(NULL)
  {
  }

  ~utc_blink_cb_title_changed()
  {
    free(changed_title);
    changed_title = NULL;
  }

  void LoadFinished(Evas_Object*)
  {
    EventLoopStop(utc_blink_ewk_base::Failure); // won't fail the test if EventLoopStop was already called
  }

  static void title_changed_cb(void* data, Evas_Object*, void* eventInfo)
  {
    utc_blink_cb_title_changed* owner = NULL;
    OwnerFromVoid(data, &owner);
    ASSERT_TRUE(owner);

    free(owner->changed_title);
    owner->changed_title = NULL;

    if (eventInfo) {
      char* new_title = static_cast<char*>(eventInfo);
      owner->changed_title = strdup(new_title);
    }

    owner->EventLoopStop(Success);
  }

  void PostSetUp()
  {
    evas_object_smart_callback_add(GetEwkWebView(), "title,changed", title_changed_cb, this);
  }

  void PreTearDown()
  {
    evas_object_smart_callback_del(GetEwkWebView(), "title,changed", title_changed_cb);
  }

protected:
  char* changed_title;
};

/**
 * @brief Test case for changing non-empty title to other non-empty title
 */
TEST_F(utc_blink_cb_title_changed, TITLE_CHANGE)
{
  const char textStyleHTML[] =
          "<html>"
          "<head><title>" TITLE1 "</title>"
          "<script>"
          "window.onload=function(){"
          "document.title=\"" TITLE2 "\";"
          "}"
          "</script>"
          "</head>"
          "<body></body>"
          "</html>";

  ASSERT_EQ(EINA_TRUE, ewk_view_html_string_load(GetEwkWebView(), textStyleHTML, 0, 0));
  ASSERT_EQ(Success, EventLoopStart());
  ASSERT_STREQ(TITLE1, changed_title);
  ASSERT_EQ(Success, EventLoopStart());
  ASSERT_STREQ(TITLE2, changed_title);
}

/**
 * @brief Test case for loading page with no title set
 */
TEST_F(utc_blink_cb_title_changed, NO_TITLE)
{
  const char textStyleHTML[] =
          "<html>"
          "<head></head>"
          "<body></body>"
          "</html>";

  ASSERT_EQ(EINA_TRUE, ewk_view_html_string_load(GetEwkWebView(), textStyleHTML, 0, 0));
  // we don't want the callback to be actually called, so the Failure is an expected result
  ASSERT_EQ(Failure, EventLoopStart());
}

/**
 * @brief Test case for changing non-empty title to null title
 */
TEST_F(utc_blink_cb_title_changed, NULL_TITLE)
{
  const char textStyleHTML[] =
          "<html>"
          "<head><title>" TITLE1 "</title>"
          "<script>"
          "window.onload=function(){"
              "document.title=null;"
          "}"
          "</script>"
          "</head>"
          "<body></body>"
          "</html>";

  ASSERT_EQ(EINA_TRUE, ewk_view_html_string_load(GetEwkWebView(), textStyleHTML, 0, 0));
  ASSERT_EQ(Success, EventLoopStart());
  ASSERT_STREQ(TITLE1, changed_title);
  ASSERT_EQ(Success, EventLoopStart());
  ASSERT_STREQ("", changed_title); // after second change should be empty string
}
