// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_user_media_permission_suspend : public utc_blink_ewk_base
{
protected:
  void PostSetUp()
  {
    evas_object_smart_callback_add(GetEwkWebView(), "usermedia,permission,request", media_permission_handler, this);
  }

  void PreTearDown()
  {
    evas_object_smart_callback_del(GetEwkWebView(), "usermedia,permission,request", media_permission_handler);
  }

  static void media_permission_handler(void* data, Evas_Object* webview, void* event_info)
  {
    utc_message("[media permission handler] ::");
    if(data)
    {
      utc_blink_ewk_user_media_permission_suspend *owner= static_cast<utc_blink_ewk_user_media_permission_suspend*>(data);
      Ewk_User_Media_Permission_Request* media_permission_request = static_cast<Ewk_User_Media_Permission_Request*>(event_info);
      if (!media_permission_request) {
        owner->EventLoopStop(Failure);
        return;
      }
      ewk_user_media_permission_request_suspend(media_permission_request);
      owner->EventLoopStop(Success);
    }
  }
};

/**
  * @brief Checking whether sending cancellation notification for authentication challenge works properly.
  */
TEST_F(utc_blink_ewk_user_media_permission_suspend, POS_TEST)
{
  if( !ewk_view_url_set(GetEwkWebView(), "http://shinydemos.com/explode/"))
    utc_fail();

  if( Success != EventLoopStart())
    utc_fail();

  evas_object_show( GetEwkWebView());
  evas_object_show( GetEwkWindow());
}

/**
  * @brief Checking whether function works properly in case of NULL of a webview.
  */
TEST_F(utc_blink_ewk_user_media_permission_suspend, NEG_TEST)
{
  Eina_Bool result = ewk_view_url_set(NULL, "http://shinydemos.com/explode/");
  if (result)
    utc_fail();

  evas_object_show( GetEwkWebView());
  evas_object_show( GetEwkWindow());
}
