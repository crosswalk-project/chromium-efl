// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

#define SAMPLE_HTML_FILE        ("/ewk_view/quota_ask.html")


class utc_blink_ewk_quota_permission_request_is_persistent_get : public utc_blink_ewk_base
{
protected:

  void PreSetUp()
  {
    persistent = EINA_FALSE;
  }

  static void quotaPermission(Evas_Object*,
                              const Ewk_Quota_Permission_Request* request,
                              utc_blink_ewk_quota_permission_request_is_persistent_get* owner)
  {
    utc_message("[ %s ]", __PRETTY_FUNCTION__);
    ASSERT_TRUE(owner);
    EXPECT_TRUE(request);
    owner->persistent = ewk_quota_permission_request_is_persistent_get(request);
    ewk_view_quota_permission_request_cancel(request);
    owner->EventLoopStop(Success);
  }

protected:
  Eina_Bool persistent;
};

/**
 * @brief Positive  test case of ewk_quota_permission_request_is_persistent_get()
 */
TEST_F(utc_blink_ewk_quota_permission_request_is_persistent_get, POS_TEST_TRUE)
{
  ewk_view_quota_permission_request_callback_set(
                           GetEwkWebView(),
                           reinterpret_cast<Ewk_Quota_Permission_Request_Callback>(quotaPermission),
                           this);
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(),
                                        GetResourceUrl(SAMPLE_HTML_FILE).c_str()));
  ASSERT_EQ(Success, EventLoopStart());
  ASSERT_EQ(EINA_TRUE, persistent);
}

/**
  * Test case for ewk_quota_permission_request_is_persistent_get
  * where return value is EINA_FALSE is not possible for file protocol.
  * Temporary storage doesn't require quota permission for now.
  */

/**
* @brief Checking whether function works properly in case of NULL of a webview and callback.
*/
TEST_F(utc_blink_ewk_quota_permission_request_is_persistent_get, NEG_TEST)
{
  EXPECT_EQ(EINA_FALSE, ewk_quota_permission_request_is_persistent_get(NULL));
}
