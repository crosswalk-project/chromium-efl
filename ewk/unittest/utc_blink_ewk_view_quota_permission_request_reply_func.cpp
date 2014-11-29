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

#define SAMPLE_HTML_FILE        ("/ewk_view/quota_ask.html")


class utc_blink_ewk_view_quota_permission_request_reply : public utc_blink_ewk_base
{
protected:

  void PreSetUp()
  {
    isTitleNull = true;
    title = "";
  }

  void PostSetUp()
  {
    evas_object_smart_callback_add(GetEwkWebView(), "title,changed",
                                   ToSmartCallback(title_changed), this);
  }
  void PreTearDown()
  {
    evas_object_smart_callback_del(GetEwkWebView(), "title,changed",
                                   ToSmartCallback(title_changed));
  }

  static void quotaPermission(Evas_Object*, const Ewk_Quota_Permission_Request* request,
                              utc_blink_ewk_view_quota_permission_request_reply* owner)
  {
    utc_message("[ %s ]", __PRETTY_FUNCTION__);
    if (!request)
    {
      ASSERT_TRUE(owner);
      owner->EventLoopStop(Failure);
      return;
    }
    ewk_view_quota_permission_request_reply(request, EINA_FALSE);
  }

  static void title_changed(utc_blink_ewk_view_quota_permission_request_reply* owner,
                            Evas_Object*, const char* title)
  {
    utc_message("[ %s ]", __PRETTY_FUNCTION__);
    ASSERT_TRUE(owner);
    if (!title)
    {
      owner->isTitleNull = true;
      owner->title = "";
      owner->EventLoopStop(Failure);
      return;
    }
    owner->isTitleNull = false;
    owner->title = title;
    // Ignore initial title
    if (owner->title != "No reply yet")
      owner->EventLoopStop(Success);
  }

protected:
  bool isTitleNull;
  std::string title;
};

/**
 * @brief Positive  test case of ewk_view_quota_permission_request_reply()
 * ewk_view_quota_permission_request_callback should be set if the page content triggers a request callback
 */
TEST_F(utc_blink_ewk_view_quota_permission_request_reply, POS_TEST)
{
  ewk_view_quota_permission_request_callback_set(
                           GetEwkWebView(),
                           reinterpret_cast<Ewk_Quota_Permission_Request_Callback>(quotaPermission),
                           this);
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(),
                                        GetResourceUrl(SAMPLE_HTML_FILE).c_str()));
  ASSERT_EQ(Success, EventLoopStart());
  ASSERT_STREQ("0", title.c_str());
}

/**
  * Test case with ewk_view_quota_permission_request_reply(request, EINA_TRUE)
  * is not possible because file protocol doesn't support quota for storage.
  * However engine asks for quota permission in case of file protocol
  * but in case of EINA_TRUE engine responds in HTML page with error callback.
  * We cannot determine if error was due to file protocol or internal error.
  */

/**
* @brief Checking whether function works properly in case of NULL of a webview and callback.
*/
TEST_F(utc_blink_ewk_view_quota_permission_request_reply, NEG_TEST)
{
  ewk_view_quota_permission_request_reply(NULL, EINA_TRUE);
}
