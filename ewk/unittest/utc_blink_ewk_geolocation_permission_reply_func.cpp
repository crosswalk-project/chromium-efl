// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

/* Define those macros _before_ you include the utc_blink_ewk.h header file. */

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_geolocation_permission_reply : public utc_blink_ewk_base {

protected:

  void PostSetUp()
  {
    evas_object_smart_callback_add(GetEwkWebView(), "geolocation,permission,request",
                                   ToSmartCallback(request_geolocation_permisson), this);
  }

  void PreTearDown()
  {
    evas_object_smart_callback_del(GetEwkWebView(), "geolocation,permission,request",
                                   ToSmartCallback(request_geolocation_permisson));
  }

  void ConsoleMessage(Evas_Object*, const Ewk_Console_Message* msg)
  {
    utc_message("JavaScript::console:\t\"%s\"", ewk_console_message_text_get(msg));
    EventLoopStop(Success);
  }

  static void request_geolocation_permisson(utc_blink_ewk_geolocation_permission_reply* owner,
                               Evas_Object*, Ewk_Geolocation_Permission_Request* permission_request)
  {
    utc_message("%s", __PRETTY_FUNCTION__);
    EXPECT_TRUE(permission_request);
    ASSERT_TRUE(owner);
    EXPECT_EQ(EINA_TRUE, ewk_geolocation_permission_reply(permission_request, owner->set));
  }

protected:
  Eina_Bool set;
};

/**
 * @brief Check for NULL tolerance
 */
TEST_F(utc_blink_ewk_geolocation_permission_reply, NullArg)
{
  ASSERT_EQ(EINA_FALSE, ewk_geolocation_permission_reply(NULL, EINA_TRUE));
}

/**
 * @brief Test for reject to geolocation permission for frame.
 */
TEST_F(utc_blink_ewk_geolocation_permission_reply, False)
{
  set = EINA_FALSE;
  EXPECT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(),
            GetResourceUrl("common/sample_js_geolocation.html").c_str()));
  EXPECT_EQ(Success, EventLoopStart());
  ASSERT_STREQ("EWK Geolocation PERMISSION_DENIED", ewk_view_title_get(GetEwkWebView()));
}

/**
 * @brief Test for allowed to geolocation permission for frame.
 */
TEST_F(utc_blink_ewk_geolocation_permission_reply, True)
{
  set = EINA_TRUE;
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(),
            GetResourceUrl("common/sample_js_geolocation.html").c_str()));
  EXPECT_EQ(Success, EventLoopStart());
  EXPECT_STRNE("EWK Geolocation TIMEOUT", ewk_view_title_get(GetEwkWebView())) <<
                                                 "\e[43mIs GPS enabled and receives a signal?\e[0m";
  ASSERT_STREQ("EWK Geolocation SUCCESS", ewk_view_title_get(GetEwkWebView()));
}
