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

class utc_blink_ewk_auth_challenge_suspend : public utc_blink_ewk_base
{
protected:

  void PostSetUp()
  {
    evas_object_smart_callback_add(GetEwkWebView(), "authentication,challenge", authentication_challenge, this);
  }

  void PreTearDown()
  {
    evas_object_smart_callback_del(GetEwkWebView(), "authentication,challenge", authentication_challenge);
  }

  void LoadFinished(Evas_Object* webview)
  {
    EventLoopStop( utc_blink_ewk_base::Failure ); // will noop if EventLoopStop was alraedy called
  }

  static void authentication_challenge(void* data, Evas_Object* webview, void* event_info)
  {
    ASSERT_TRUE(data != NULL);

    utc_blink_ewk_auth_challenge_suspend *owner = static_cast<utc_blink_ewk_auth_challenge_suspend*>(data);

    Ewk_Auth_Challenge* auth_challenge = static_cast<Ewk_Auth_Challenge*>(event_info);

    if(auth_challenge)
    {
      ewk_auth_challenge_suspend(auth_challenge);

      owner->EventLoopStop(utc_blink_ewk_base::Success);
      return;
    }

    owner->EventLoopStop(utc_blink_ewk_base::Failure);
  }
};

/**
 * @brief Checking whether the suspend of authentication challenge works properly.
 */
TEST_F(utc_blink_ewk_auth_challenge_suspend, POS_TEST)
{
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), "http://reference.vodafone.com/widgets/"));
  EXPECT_EQ(Success, EventLoopStart());
}

/**
 * @brief Checking whether function works properly in case of NULL of a webview.
 */
TEST_F(utc_blink_ewk_auth_challenge_suspend, NEG_TEST)
{
  ASSERT_EQ(EINA_FALSE, ewk_view_url_set(NULL,"http://reference.vodafone.com/widgets/"));
}
