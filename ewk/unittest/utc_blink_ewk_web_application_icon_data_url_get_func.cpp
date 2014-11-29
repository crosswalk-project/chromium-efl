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
#include <map>

// this test uses parts of code from:
// utc_blink_ewk_view_web_application_icon_urls_get_func.cpp
// because this test provides exemplary use of ewk_view_web_application_icon_urls_get
// which is only way to get Ewk_Web_App_Icon_Data class objects without using
// private implementation

class utc_blink_ewk_web_application_icon_data_url_get : public utc_blink_ewk_base
{
protected:
  void LoadFinished(Evas_Object *)
  {
    EventLoopStop(Success);
  }

  static void webAppUrlsGetCallback(Eina_List *urls, utc_blink_ewk_web_application_icon_data_url_get *owner)
  {
    ASSERT_TRUE(NULL != owner);
    ASSERT_TRUE(NULL != urls);
    utc_message("[webAppUrlsGetCallback] :: ");
    ASSERT_EQ(owner->expectedUrls.size(), eina_list_count(urls));
    Eina_List *l = 0;
    void *d = 0;
    EINA_LIST_FOREACH(urls, l, d) {
      Ewk_Web_App_Icon_Data *data = static_cast<Ewk_Web_App_Icon_Data*>(d);
      ASSERT_TRUE(NULL != data);
      const char* size = ewk_web_application_icon_data_size_get(data);
      ASSERT_TRUE(NULL != size);
      const char* url = ewk_web_application_icon_data_url_get(data);
      ASSERT_TRUE(NULL != url);
      utc_message("[URLS]: %s ; %s", size, url);
      ASSERT_STREQ(owner->GetResourceUrl(owner->expectedUrls[size].c_str()).c_str(), url);
    }
    owner->EventLoopStop(Success);
  }

  std::map<std::string, std::string> expectedUrls;
  static const char * const webAppUrlsPage;

};

const char * const utc_blink_ewk_web_application_icon_data_url_get::webAppUrlsPage = "ewk_view_web_application/web_app_urls_get.html";

TEST_F(utc_blink_ewk_web_application_icon_data_url_get, POS_TEST)
{
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), GetResourceUrl(webAppUrlsPage).c_str()));
  ASSERT_EQ(Success, EventLoopStart());
  expectedUrls["64x64"] = "ewk_view_web_application/tizen-icon1.png";
  expectedUrls["128x128"] = "ewk_view_web_application/tizen-icon2.png";
  expectedUrls["144x144"] = "ewk_view_web_application/tizen-icon3.png";
  ASSERT_EQ(EINA_TRUE, ewk_view_web_application_icon_urls_get(GetEwkWebView(),
            (void(*)(_Eina_List*,void*))webAppUrlsGetCallback,
            this));
  ASSERT_EQ(Success, EventLoopStart());
}
