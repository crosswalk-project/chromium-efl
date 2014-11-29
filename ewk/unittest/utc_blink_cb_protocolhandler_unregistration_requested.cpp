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

class utc_blink_cb_protocolhandler_unregistration_requested : public utc_blink_ewk_base
{
protected:
  utc_blink_cb_protocolhandler_unregistration_requested()
    : handler_target_(NULL)
    , handler_base_url_(NULL)
    , handler_url_(NULL)
    , handler_title_(NULL)
  {}

  ~utc_blink_cb_protocolhandler_unregistration_requested()
  {
    if (handler_target_) free(handler_target_);
    if (handler_base_url_) free(handler_base_url_);
    if (handler_url_) free(handler_url_);
    if (handler_title_) free(handler_title_);
  }

  void LoadFinished(Evas_Object *)
  {
    EventLoopStop(Failure);
  }

  void PostSetUp()
  {
    evas_object_smart_callback_add(GetEwkWebView(), "protocolhandler,unregistration,requested", cb_protocolhandler_unregistration_requested, this);
  }

  void PreTearDown()
  {
    evas_object_smart_callback_del(GetEwkWebView(), "protocolhandler,unregistration,requested", cb_protocolhandler_unregistration_requested);
  }

  static void cb_protocolhandler_unregistration_requested(void *data, Evas_Object *, void *info)
  {
    ASSERT_TRUE(data != NULL);
    utc_message("protocol handler unregistered");
    utc_blink_cb_protocolhandler_unregistration_requested *owner;
    OwnerFromVoid(data, &owner);
    EXPECT_TRUE(info);
    Ewk_Custom_Handlers_Data *handler_data_ = static_cast<Ewk_Custom_Handlers_Data *>(info);
    owner->handler_target_ = ewk_custom_handlers_data_target_get(handler_data_) ? strdup(ewk_custom_handlers_data_target_get(handler_data_)) : 0;
    owner->handler_base_url_ = ewk_custom_handlers_data_base_url_get(handler_data_) ? strdup(ewk_custom_handlers_data_base_url_get(handler_data_)) : 0;
    owner->handler_url_ = ewk_custom_handlers_data_url_get(handler_data_) ? strdup(ewk_custom_handlers_data_url_get(handler_data_)) : 0;
    owner->handler_title_ = ewk_custom_handlers_data_title_get(handler_data_) ? strdup(ewk_custom_handlers_data_title_get(handler_data_)) : 0;
    owner->EventLoopStop(Success);
  }

protected:
  char *handler_target_;
  char *handler_base_url_;
  char *handler_url_;
  char *handler_title_;

  static const char * const TARGET;
  static const char * const BASE_URL;
  static const char * const URL;
};

const char * const utc_blink_cb_protocolhandler_unregistration_requested::TARGET = "mailto";
const char * const utc_blink_cb_protocolhandler_unregistration_requested::BASE_URL = "file:///";
const char * const utc_blink_cb_protocolhandler_unregistration_requested::URL = "file:///opt/usr/resources/protocol_handler/handler.html?url=%s";

TEST_F(utc_blink_cb_protocolhandler_unregistration_requested, MAILTO_PROTOCOL_UNREGISTRATION)
{
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), GetResourceUrl("protocol_handler/unregister_protocol_handler.html").c_str()));
  ASSERT_EQ(Success, EventLoopStart());
  ASSERT_STREQ(TARGET, handler_target_);
  ASSERT_STREQ(BASE_URL, handler_base_url_);
  ASSERT_STREQ(URL, handler_url_);
  utc_message("target:   %s", handler_target_);
  utc_message("base url: %s", handler_base_url_);
  utc_message("url:      %s", handler_url_);
  utc_message("title:    %s", handler_title_);
}
