// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_view_web_application_capable_get : public utc_blink_ewk_base
{
protected:
  void LoadFinished(Evas_Object *)
  {
    EventLoopStop(Success);
  }

  static void webAppCapableGetCallback(Eina_Bool capable, void *data)
  {
    ASSERT_TRUE(data != NULL);
    utc_blink_ewk_view_web_application_capable_get *owner = 0;
    OwnerFromVoid(data, &owner);
    fprintf(stderr, "[webAppCapableGetCallback] :: capable = %s\n", capable == EINA_TRUE ? "EINA_TRUE" : "EINA_FALSE");
    ASSERT_EQ(owner->expectedCapable, capable);
    owner->EventLoopStop(Success);
  }

protected:
  Eina_Bool expectedCapable;
  static const char * const appleCapableYesPage;
  static const char * const appleCapableNoPage;
  static const char * const googleCapableYesPage;
  static const char * const googleCapableNoPage;
  static const char * const noMetaTagPage;
};

const char * const utc_blink_ewk_view_web_application_capable_get::appleCapableYesPage = "<html>" \
    "<head><meta name=\"apple-mobile-web-app-capable\" content=\"yes\"></head>" \
    "<body></body>" \
    "</html>";
const char * const utc_blink_ewk_view_web_application_capable_get::appleCapableNoPage = "<html>" \
    "<head><meta name=\"apple-mobile-web-app-capable\" content=\"no\"></head>" \
    "<body></body>" \
    "</html>";
const char * const utc_blink_ewk_view_web_application_capable_get::googleCapableYesPage = "<html>" \
    "<head><meta name=\"mobile-web-app-capable\" content=\"yes\"></head>" \
    "<body></body>" \
    "</html>";
const char * const utc_blink_ewk_view_web_application_capable_get::googleCapableNoPage = "<html>" \
    "<head><meta name=\"mobile-web-app-capable\" content=\"no\"></head>" \
    "<body></body>" \
    "</html>";
const char * const utc_blink_ewk_view_web_application_capable_get::noMetaTagPage = "<html><body></body></html>";

TEST_F(utc_blink_ewk_view_web_application_capable_get, APPLE_CAPABLE_YES_PAGE)
{
  ASSERT_EQ(EINA_TRUE, ewk_view_html_string_load(GetEwkWebView(), appleCapableYesPage, 0, 0));
  ASSERT_EQ(Success, EventLoopStart());
  expectedCapable = EINA_TRUE;
  ASSERT_EQ(EINA_TRUE, ewk_view_web_application_capable_get(GetEwkWebView(),
                                                            utc_blink_ewk_view_web_application_capable_get::webAppCapableGetCallback,
                                                            this));
  ASSERT_EQ(Success, EventLoopStart());
}

TEST_F(utc_blink_ewk_view_web_application_capable_get, APPLE_CAPABLE_NO_PAGE)
{
  ASSERT_EQ(EINA_TRUE, ewk_view_html_string_load(GetEwkWebView(), appleCapableNoPage, 0, 0));
  ASSERT_EQ(Success, EventLoopStart());
  expectedCapable = EINA_FALSE;
  ASSERT_EQ(EINA_TRUE, ewk_view_web_application_capable_get(GetEwkWebView(),
                                                            utc_blink_ewk_view_web_application_capable_get::webAppCapableGetCallback,
                                                            this));
  ASSERT_EQ(Success, EventLoopStart());
}

TEST_F(utc_blink_ewk_view_web_application_capable_get, GOOGLE_CAPABLE_YES_PAGE)
{
  ASSERT_EQ(EINA_TRUE, ewk_view_html_string_load(GetEwkWebView(), googleCapableYesPage, 0, 0));
  ASSERT_EQ(Success, EventLoopStart());
  expectedCapable = EINA_TRUE;
  ASSERT_EQ(EINA_TRUE, ewk_view_web_application_capable_get(GetEwkWebView(),
                                                            utc_blink_ewk_view_web_application_capable_get::webAppCapableGetCallback,
                                                            this));
  ASSERT_EQ(Success, EventLoopStart());
}

TEST_F(utc_blink_ewk_view_web_application_capable_get, GOOGLE_CAPABLE_NO_PAGE)
{
  ASSERT_EQ(EINA_TRUE, ewk_view_html_string_load(GetEwkWebView(), googleCapableNoPage, 0, 0));
  ASSERT_EQ(Success, EventLoopStart());
  expectedCapable = EINA_FALSE;
  ASSERT_EQ(EINA_TRUE, ewk_view_web_application_capable_get(GetEwkWebView(),
                                                            utc_blink_ewk_view_web_application_capable_get::webAppCapableGetCallback,
                                                            this));
  ASSERT_EQ(Success, EventLoopStart());
}

TEST_F(utc_blink_ewk_view_web_application_capable_get, NO_META_TAG_PAGE)
{
  ASSERT_EQ(EINA_TRUE, ewk_view_html_string_load(GetEwkWebView(), noMetaTagPage, 0, 0));
  ASSERT_EQ(Success, EventLoopStart());
  expectedCapable = EINA_FALSE;
  ASSERT_EQ(EINA_TRUE, ewk_view_web_application_capable_get(GetEwkWebView(),
                                                            utc_blink_ewk_view_web_application_capable_get::webAppCapableGetCallback,
                                                            this));
  ASSERT_EQ(Success, EventLoopStart());
}
