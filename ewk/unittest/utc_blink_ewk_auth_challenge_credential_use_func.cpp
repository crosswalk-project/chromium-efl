// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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

