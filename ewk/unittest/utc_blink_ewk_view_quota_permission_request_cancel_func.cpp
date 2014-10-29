// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

#define SAMPLE_HTML_FILE        ("/ewk_view/quota_ask.html")


class utc_blink_ewk_view_quota_permission_request_cancel : public utc_blink_ewk_base
{
protected:

  void PostSetUp()
  {
    isTitleNull = true;
    title = "";
    evas_object_smart_callback_add(GetEwkWebView(), "title,changed",
                                   ToSmartCallback(title_changed), this);
  }

  void PreTearDown()
  {
    evas_object_smart_callback_del(GetEwkWebView(), "title,changed",
                                   ToSmartCallback(title_changed));
  }

  static void quotaPermission(Evas_Object*, const Ewk_Quota_Permission_Request* request,
                              utc_blink_ewk_view_quota_permission_request_cancel* owner)
  {
    utc_message("[ %s ]", __PRETTY_FUNCTION__);
    if (!request) {
      ASSERT_TRUE(owner);
      owner->EventLoopStop(Failure);
      return;
    }
    ewk_view_quota_permission_request_cancel(request);
  }

  static void title_changed(utc_blink_ewk_view_quota_permission_request_cancel* owner,
                            Evas_Object*, const char* title){
    utc_message("[ %s ]", __PRETTY_FUNCTION__);
    ASSERT_TRUE(owner);
    if (!title) {
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
 * @brief Positive  test case of ewk_view_quota_permission_request_cancel()
 * ewk_view_quota_permission_request_callback should be set if the page content triggers a request callback
 */
TEST_F(utc_blink_ewk_view_quota_permission_request_cancel, POS_TEST)
{
  ewk_view_quota_permission_request_callback_set(
                           GetEwkWebView(),
                           reinterpret_cast<Ewk_Quota_Permission_Request_Callback>(quotaPermission),
                           this);
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(),
                                        GetResourceUrl(SAMPLE_HTML_FILE).c_str()));
  ASSERT_EQ(Success, EventLoopStart());
  ASSERT_FALSE(isTitleNull);
  ASSERT_STREQ("0", title.c_str());
}

/**
* @brief Checking whether function works properly in case of NULL of a webview and callback.
*/
TEST_F(utc_blink_ewk_view_quota_permission_request_cancel, NEG_TEST)
{
  ewk_view_quota_permission_request_cancel(NULL);
}
