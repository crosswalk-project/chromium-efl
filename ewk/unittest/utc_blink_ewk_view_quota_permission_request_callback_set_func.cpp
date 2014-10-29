// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

#define SAMPLE_HTML_FILE        ("/ewk_view/quota_ask.html")


class utc_blink_ewk_view_quota_permission_request_callback_set : public utc_blink_ewk_base
{
protected:

  static void quotaPermission(Evas_Object*,
                              const Ewk_Quota_Permission_Request* request,
                              utc_blink_ewk_view_quota_permission_request_callback_set* owner)
  {
    utc_message("[ %s ]", __PRETTY_FUNCTION__);
    ASSERT_TRUE(owner);
    EXPECT_TRUE(request);
    ewk_view_quota_permission_request_cancel(request);
    owner->EventLoopStop(Success);
  }
};

/**
 * @brief Positive  test case of ewk_view_quota_permission_request_callback_set()
 * ewk_view_quota_permission_request_callback should be set if the page content triggers a request callback
 */
TEST_F(utc_blink_ewk_view_quota_permission_request_callback_set, POS_TEST)
{
  /*
   * Because we cannot predict when quota permission callback is called
   * (can be called after LoadFinish event!) a timeout is required.
   * Callback is pure asynchronous.
   */
  ewk_view_quota_permission_request_callback_set(
                           GetEwkWebView(),
                           reinterpret_cast<Ewk_Quota_Permission_Request_Callback>(quotaPermission),
                           this);
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(),
                                        GetResourceUrl(SAMPLE_HTML_FILE).c_str()));
  ASSERT_EQ(Success, EventLoopStart(10));

  ewk_view_quota_permission_request_callback_set(GetEwkWebView(), NULL, NULL);
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(),
                                        GetResourceUrl(SAMPLE_HTML_FILE).c_str()));
  ASSERT_EQ(Failure, EventLoopStart(10));
}

/**
* @brief Checking whether function works properly in case of NULL of a webview and callback.
*/
TEST_F(utc_blink_ewk_view_quota_permission_request_callback_set, NEG_TEST)
{
  ewk_view_quota_permission_request_callback_set(NULL, NULL, NULL);
}
