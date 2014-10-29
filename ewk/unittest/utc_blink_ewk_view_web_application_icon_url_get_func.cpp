// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_view_web_application_icon_url_get : public utc_blink_ewk_base
{
protected:
  void LoadFinished(Evas_Object *)
  {
    EventLoopStop(Success);
  }

  static void webAppUrlGetCallback(const char *url, utc_blink_ewk_view_web_application_icon_url_get *owner)
  {
    ASSERT_TRUE(NULL != owner);
    ASSERT_TRUE(NULL != url);
    utc_message("[webAppUrlGetCallback] :: url = %s", url);
    if(!*url) {
      owner->EventLoopStop(Failure);
    } else {
      ASSERT_STREQ(owner->GetResourceUrl(owner->expectedUrl.c_str()).c_str(), url);
      owner->EventLoopStop(Success);
    }
  }

protected:
  std::string expectedUrl;
  static const char * const iconUrl;
  static const char * const appleTouchIconPage;
  static const char * const appleTouchIconPrecomposedPage;
  static const char * const googleIconPage;
  static const char * const googleAppleIconPage;
};

const char * const utc_blink_ewk_view_web_application_icon_url_get::iconUrl = "ewk_view_web_application/tizen-icon.png";
const char * const utc_blink_ewk_view_web_application_icon_url_get::appleTouchIconPage = "ewk_view_web_application/web_app_url_get.html";
const char * const utc_blink_ewk_view_web_application_icon_url_get::appleTouchIconPrecomposedPage = "ewk_view_web_application/web_app_url_get_precomposed.html";
const char * const utc_blink_ewk_view_web_application_icon_url_get::googleIconPage = "ewk_view_web_application/web_app_url_get_icon.html";
const char * const utc_blink_ewk_view_web_application_icon_url_get::googleAppleIconPage = "ewk_view_web_application/web_app_url_get_icon_apple.html";

TEST_F(utc_blink_ewk_view_web_application_icon_url_get, APPLE_TOUCH_ICON_PAGE)
{
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), GetResourceUrl(appleTouchIconPage).c_str()));
  ASSERT_EQ(Success, EventLoopStart());
  expectedUrl = iconUrl;
  ASSERT_EQ(EINA_TRUE, ewk_view_web_application_icon_url_get(GetEwkWebView(),
                                                             (void(*)(const char*,void*))webAppUrlGetCallback,
                                                             this));
  ASSERT_EQ(Success, EventLoopStart());
}

TEST_F(utc_blink_ewk_view_web_application_icon_url_get, APPLE_TOUCH_ICON_PRECOMPOSED_PAGE)
{
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), GetResourceUrl(appleTouchIconPrecomposedPage).c_str()));
  ASSERT_EQ(Success, EventLoopStart());
  expectedUrl = iconUrl;
  ASSERT_EQ(EINA_TRUE, ewk_view_web_application_icon_url_get(GetEwkWebView(),
                                                             (void(*)(const char*,void*))webAppUrlGetCallback,
                                                             this));
  ASSERT_EQ(Success, EventLoopStart());
}

TEST_F(utc_blink_ewk_view_web_application_icon_url_get, GOOGLE_ICON_PAGE)
{
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), GetResourceUrl(googleIconPage).c_str()));
  ASSERT_EQ(Success, EventLoopStart());
  expectedUrl = iconUrl;
  ASSERT_EQ(EINA_TRUE, ewk_view_web_application_icon_url_get(GetEwkWebView(),
                                                             (void(*)(const char*,void*))webAppUrlGetCallback,
                                                             this));
  ASSERT_EQ(Success, EventLoopStart());
}

TEST_F(utc_blink_ewk_view_web_application_icon_url_get, GOOGLE_APPLE_ICON_PAGE)
{
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), GetResourceUrl(googleAppleIconPage).c_str()));
  ASSERT_EQ(Success, EventLoopStart());
  expectedUrl = iconUrl;
  ASSERT_EQ(EINA_TRUE, ewk_view_web_application_icon_url_get(GetEwkWebView(),
                                                             (void(*)(const char*,void*))webAppUrlGetCallback,
                                                             this));
  ASSERT_EQ(Success, EventLoopStart());
}

TEST_F(utc_blink_ewk_view_web_application_icon_url_get, NO_ICON_PAGE)
{
  ASSERT_EQ(EINA_TRUE, ewk_view_html_string_load(GetEwkWebView(), "<html><body></body></html>", 0, 0));
  ASSERT_EQ(Success, EventLoopStart());
  expectedUrl = std::string();
  ASSERT_EQ(EINA_TRUE, ewk_view_web_application_icon_url_get(GetEwkWebView(),
                                                             (void(*)(const char*,void*))webAppUrlGetCallback,
                                                             this));
  ASSERT_EQ(Failure, EventLoopStart());
}
