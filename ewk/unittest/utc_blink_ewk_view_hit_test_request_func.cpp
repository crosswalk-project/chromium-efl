/*
 * Chromium EFL
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

class utc_blink_ewk_view_hit_test_request: public utc_blink_ewk_base
{
protected:
  utc_blink_ewk_view_hit_test_request()
    : req_x(-1)
    , req_y(-1)
    , req_mode(-1)
  {
  }

  void LoadFinished(Evas_Object* webview)
  {
      EventLoopStop(utc_blink_ewk_base::Success);
  }

  static void hit_test_result(Evas_Object* o, int x, int y, int mode, Ewk_Hit_Test* hit_test, void* user_data)
  {
    utc_message("I'm here");
    utc_blink_ewk_view_hit_test_request* owner = NULL;
    OwnerFromVoid(user_data, &owner);
    ASSERT_TRUE(owner);

    EXPECT_EQ(owner->req_x, x);
    EXPECT_EQ(owner->req_y, y);
    EXPECT_EQ(owner->req_mode, mode);
    EXPECT_TRUE(hit_test);
    owner->EventLoopStop(Success);
  }

  void PostSetUp()
  {
    ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), url));
    ASSERT_EQ(Success, EventLoopStart());
  }

  void StartHitTest(int x, int y, int mode)
  {
    req_x = x;
    req_y = y;
    req_mode = mode;

    ASSERT_EQ(EINA_TRUE, ewk_view_hit_test_request(GetEwkWebView(), req_x, req_y, req_mode, hit_test_result, this));
    ASSERT_EQ(Success, EventLoopStart());
  }

protected:
  static const char*const url;
  int req_x;
  int req_y;
  int req_mode;
};

const char*const utc_blink_ewk_view_hit_test_request::url="http://m.naver.com";

/**
 * @brief Checking whether hit test instance is created properly by hit test mode.
 */
TEST_F(utc_blink_ewk_view_hit_test_request, POS_TEST1)
{
  StartHitTest(100, 150, EWK_HIT_TEST_MODE_DEFAULT);
  StartHitTest(120, 170, EWK_HIT_TEST_MODE_NODE_DATA);
  StartHitTest(140, 190, EWK_HIT_TEST_MODE_IMAGE_DATA);
  StartHitTest(160, 210, EWK_HIT_TEST_MODE_ALL);
  StartHitTest(180, 230, EWK_HIT_TEST_MODE_IMAGE_DATA|EWK_HIT_TEST_MODE_NODE_DATA);
}

/**
 * @brief Checking whether function works properly in case of NULL of a webview.
 */
TEST_F(utc_blink_ewk_view_hit_test_request, NEG_TEST1)
{
  EXPECT_EQ(EINA_FALSE, ewk_view_hit_test_request(NULL, 100, 150, EWK_HIT_TEST_MODE_DEFAULT, hit_test_result, this));
  EXPECT_EQ(EINA_FALSE, ewk_view_hit_test_request(NULL, 120, 170, EWK_HIT_TEST_MODE_NODE_DATA, hit_test_result, this));
  EXPECT_EQ(EINA_FALSE, ewk_view_hit_test_request(NULL, 140, 190, EWK_HIT_TEST_MODE_IMAGE_DATA, hit_test_result, this));
  EXPECT_EQ(EINA_FALSE, ewk_view_hit_test_request(NULL, 160, 210, EWK_HIT_TEST_MODE_ALL, hit_test_result, this));
  EXPECT_EQ(EINA_FALSE, ewk_view_hit_test_request(NULL, 180, 230, EWK_HIT_TEST_MODE_IMAGE_DATA|EWK_HIT_TEST_MODE_NODE_DATA, hit_test_result, this));
}
