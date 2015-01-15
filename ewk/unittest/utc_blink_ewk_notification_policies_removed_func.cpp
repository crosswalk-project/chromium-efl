// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_notification_policies_removed : public utc_blink_ewk_base {
protected:
  utc_blink_ewk_notification_policies_removed()
    : permission_requested_(false), proto_(NULL), host_(NULL)
  {
  }

  ~utc_blink_ewk_notification_policies_removed()
  {
    if (proto_) free(proto_);
    if (host_) free(host_);
  }

  void PostSetUp()
  {
    ewk_view_notification_permission_callback_set(GetEwkWebView(), cb_permission_request, this);
    evas_object_smart_callback_add(GetEwkWebView(), "console,message",
                                   ToSmartCallback(cb_console_message), this);
    // clear permissions
    ewk_notification_cached_permissions_set(ewk_view_context_get(GetEwkWebView()), NULL);
  }

  void PreTearDown()
  {
    ewk_view_notification_permission_callback_set(GetEwkWebView(), NULL, NULL);
    evas_object_smart_callback_del(GetEwkWebView(), "console,message",
                                   ToSmartCallback(cb_console_message));
  }

  /* Callback for notification permission request */
  static Eina_Bool cb_permission_request(Evas_Object *obj, Ewk_Notification_Permission_Request *request, void *data)
  {
    utc_message("[cb_permission_request()]");
    utc_blink_ewk_notification_policies_removed *owner = NULL;
    OwnerFromVoid(data, &owner);
    if (!request) {
      owner->EventLoopStop(Failure);
      return EINA_FALSE;
    }
    const Ewk_Security_Origin* origin = ewk_notification_permission_request_origin_get(request);
    if (!origin) {
      owner->EventLoopStop(Failure);
      return EINA_FALSE;
    }
    owner->proto_ = strdup(ewk_security_origin_protocol_get(origin));
    utc_message("[cb_permission_request] :: proto_ : %s", owner->proto_);
    owner->host_ = strdup(ewk_security_origin_host_get(origin));
    utc_message("[cb_permission_request] :: host_ : %s", owner->host_);
    owner->permission_requested_ = true;
    ewk_notification_permission_reply(request, EINA_TRUE);
    return EINA_TRUE;
  }

  static void cb_console_message(utc_blink_ewk_notification_policies_removed *owner, Evas_Object *obj, void *event_info)
  {
    Ewk_Console_Message* console = static_cast<Ewk_Console_Message*>(event_info);
    const char *cmsg = ewk_console_message_text_get(console);
    if (!cmsg) {
      return;
    }
    std::string msg(cmsg);
    std::string::size_type pos = msg.find(':');
    if (pos == std::string::npos) {
      return;
    }
    if (msg.substr(0, pos) == "event_loop_stop") {
      owner->EventLoopStop(Success);
    }
  }

  // helper function
  bool click()
  {
    utc_message("[click] :: ");
    return ewk_view_script_execute(GetEwkWebView(), "document.getElementById(\"startButton\").click();", NULL, NULL) == EINA_TRUE;
  }

protected:
  bool permission_requested_;
  char *proto_;
  char *host_;
  static const char * const expected_proto_;
  static const char * const expected_host_;
  static const char * const resource_file_;
};

const char * const utc_blink_ewk_notification_policies_removed::expected_proto_ = "";
const char * const utc_blink_ewk_notification_policies_removed::expected_host_ = "";
const char * const utc_blink_ewk_notification_policies_removed::resource_file_ = "common/sample_notification_2.html";

TEST_F(utc_blink_ewk_notification_policies_removed, POS_TEST)
{
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), GetResourceUrl(resource_file_).c_str()));
  ASSERT_EQ(Success, EventLoopStart());

  ASSERT_TRUE(permission_requested_);
  ASSERT_STREQ(expected_proto_, proto_);
  ASSERT_STREQ(expected_host_, host_);

  permission_requested_ = false;
  ASSERT_TRUE(click());
  ASSERT_EQ(Success, EventLoopStart());

  ASSERT_FALSE(permission_requested_);

  Eina_List *list = NULL;
  Ewk_Security_Origin *origin = ewk_security_origin_new_from_string((std::string(proto_) + "://" + std::string(host_)).c_str());
  list = eina_list_append(list, origin);
  ewk_notification_policies_removed(ewk_view_context_get(GetEwkWebView()), list);
  ewk_security_origin_free(origin);
  ASSERT_TRUE(click());
  ASSERT_EQ(Success, EventLoopStart());

  ASSERT_TRUE(permission_requested_);
}

TEST_F(utc_blink_ewk_notification_policies_removed, NEG_TEST)
{
  ewk_notification_policies_removed(ewk_view_context_get(GetEwkWebView()), NULL);
  SUCCEED();
}
