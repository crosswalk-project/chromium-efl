// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

#define SAMPLE_HTML_FILE        ("/ewk_view/quota_ask.html")


class utc_blink_ewk_quota_permission_request_origin_host_get : public utc_blink_ewk_base
{
protected:
  utc_blink_ewk_quota_permission_request_origin_host_get()
    : host(NULL)
  {
  }

  void PreTearDown()
  {
    eina_stringshare_del(host);
  }

  static void quotaPermission(Evas_Object*,
                              const Ewk_Quota_Permission_Request* request,
                              utc_blink_ewk_quota_permission_request_origin_host_get* owner)
  {
    utc_message("[ %s ]", __PRETTY_FUNCTION__);
    ASSERT_TRUE(owner);
    EXPECT_TRUE(request);
    owner->host = eina_stringshare_add(ewk_quota_permission_request_origin_host_get(request));
    ewk_view_quota_permission_request_cancel(request);
    owner->EventLoopStop(Success);
  }

protected:
  Eina_Stringshare* host;
};


/**
 * @brief Positive  test case of ewk_quota_permission_request_origin_host_get()
 */
TEST_F(utc_blink_ewk_quota_permission_request_origin_host_get, POS_TEST)
{
  ewk_view_quota_permission_request_callback_set(
                           GetEwkWebView(),
                           reinterpret_cast<Ewk_Quota_Permission_Request_Callback>(quotaPermission),
                           this);
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(),
                                        GetResourceUrl(SAMPLE_HTML_FILE).c_str()));
  ASSERT_EQ(Success, EventLoopStart());
  // Host is "" when not origin
  EXPECT_STREQ("", host);
}

/**
* @brief Checking whether function works properly in case of NULL
*/
TEST_F(utc_blink_ewk_quota_permission_request_origin_host_get, NEG_TEST)
{
  EXPECT_STREQ(NULL, ewk_quota_permission_request_origin_host_get(NULL));
}
