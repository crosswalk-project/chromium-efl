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
