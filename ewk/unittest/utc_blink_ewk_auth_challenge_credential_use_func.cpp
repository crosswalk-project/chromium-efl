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

#define URL "http://reference.vodafone.com/widgets/"

class utc_blink_ewk_auth_challenge_credential_use : public utc_blink_ewk_base
{
protected:
  void PostSetUp()
  {
    evas_object_smart_callback_add( GetEwkWebView(), "authentication,challenge", authentication_challenge, this);
  }

  void PreTearDown()
  {
    evas_object_smart_callback_del( GetEwkWebView(), "authentication,challenge", authentication_challenge);
  }

  void LoadFinished(Evas_Object* webview)
  {
    EventLoopStop( utc_blink_ewk_base::Failure ); // will noop if EventLoopStop was alraedy called
  }

  static void authentication_challenge(void* data, Evas_Object* webview, void* event_info)
  {
    utc_message("[authentication_challenge] :: ");
    utc_blink_ewk_auth_challenge_credential_use *owner = static_cast<utc_blink_ewk_auth_challenge_credential_use*>(data);

    Ewk_Auth_Challenge* auth_challenge = (Ewk_Auth_Challenge*)event_info;
    if (!auth_challenge) {
      owner->EventLoopStop( utc_blink_ewk_base::Failure ); // will noop if EventLoopStop was alraedy called
      utc_fail();
    }

    const char* realm = ewk_auth_challenge_realm_get(auth_challenge);
    const char* url = ewk_auth_challenge_url_get(auth_challenge);
    if (!realm || !url) {
      owner->EventLoopStop( utc_blink_ewk_base::Failure ); // will noop if EventLoopStop was alraedy called
      utc_fail();
    }

    const char* user = "access";
    const char* password = "early";
    ewk_auth_challenge_credential_use(auth_challenge, user, password);
    owner->EventLoopStop( utc_blink_ewk_base::Success );
    // TODO
    // Here is a logical error because success is called a question about authorization,
    // not about the result of the authorization.
    // In the casete of re-call up the callback should be return failuare
  }
};

  /**
  * @brief Checking whether sending cancellation notification for authentication challenge works properly.
  */
TEST_F(utc_blink_ewk_auth_challenge_credential_use, POS_TEST)
{
  utc_check_true( ewk_view_url_set( GetEwkWebView(), URL));

  utc_check_eq( EventLoopStart(), utc_blink_ewk_base::Success);

  evas_object_show( GetEwkWebView());
  evas_object_show( GetEwkWindow());
}

  /**
  * @brief Checking whether function works properly in case of NULL of a webview.
  */
TEST_F(utc_blink_ewk_auth_challenge_credential_use, NEG_TEST)
{
  Eina_Bool result = ewk_view_url_set( NULL, URL);

  if (result) {
    utc_fail();
  }

  evas_object_show( GetEwkWebView());
  evas_object_show( GetEwkWindow());
}

