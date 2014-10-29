// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

#define SAMPLE_HTML_FILE        ("/ewk_view/quota_ask.html")


class utc_blink_ewk_quota_permission_request_quota_get : public utc_blink_ewk_base
{
protected:

  void PreSetUp()
  {
    quota = 0;
  }

  static void quotaPermission(Evas_Object*,
                              const Ewk_Quota_Permission_Request* request,
                              utc_blink_ewk_quota_permission_request_quota_get* owner)
  {
    utc_message("[ %s ]", __PRETTY_FUNCTION__);
    ASSERT_TRUE(owner);
    EXPECT_TRUE(request);
    owner->quota = ewk_quota_permission_request_quota_get(request);
    ewk_view_quota_permission_request_cancel(request);
    owner->EventLoopStop(Success);
  }

protected:
  int64_t quota;
};

/**
 * @brief Positive  test case of ewk_quota_permission_request_quota_get()
 */
TEST_F(utc_blink_ewk_quota_permission_request_quota_get, POS_TEST)
{
  ewk_view_quota_permission_request_callback_set(
                           GetEwkWebView(),
                           reinterpret_cast<Ewk_Quota_Permission_Request_Callback>(quotaPermission),
                           this);
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(),
                                        GetResourceUrl(SAMPLE_HTML_FILE).c_str()));
  ASSERT_EQ(Success, EventLoopStart());
  // 10000 is requestet quota in quota_ask.html
  EXPECT_EQ(10000, quota);
}

/**
* @brief Checking whether function works properly in case of NULL
*/
TEST_F(utc_blink_ewk_quota_permission_request_quota_get, NEG_TEST)
{
  EXPECT_EQ(0, ewk_quota_permission_request_quota_get(NULL));
}
