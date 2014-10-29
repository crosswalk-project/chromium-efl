// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

#define baseURL "http://google.com"

class utc_blink_ewk_policy_decision_http_method_get : public utc_blink_ewk_base
{
 protected:

  void PostSetUp()
  {
    evas_object_smart_callback_add(GetEwkWebView(), "policy,navigation,decide", ToSmartCallback(policy_navigation_decide), this);
  }

  void PreTearDown()
  {
    evas_object_smart_callback_del(GetEwkWebView(), "policy,navigation,decide", ToSmartCallback(policy_navigation_decide));
  }

  static void policy_navigation_decide(utc_blink_ewk_policy_decision_http_method_get* owner,
          Evas_Object* webview, Ewk_Policy_Decision* policy_decision)
  {
    ASSERT_TRUE(owner);
    const char* method = ewk_policy_decision_http_method_get(policy_decision);
    owner->receivedMethod = method ? method : "";
    owner->EventLoopStop(Success);
  }

  std::string getDocumentForMethod(const std::string& method)
  {
    return std::string(
          "<html>"
          "<body>"
            "<form id='theForm' method='" + method + "' action='" + baseURL + "'>"
              "<input name='x' value='y'/>"
            "</form>"
          "</body>"
          "<script>"
            "document.getElementById('theForm').submit();"
          "</script>"
          "</html>");
  }

 protected:
   std::string receivedMethod;
   std::string currentMethod;
};

/**
 * @brief Tests if the method for policy decision is returned properly.
 */
TEST_F(utc_blink_ewk_policy_decision_http_method_get, TEST_DEFALUT_METHOD)
{
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), baseURL));
  ASSERT_EQ(Success, EventLoopStart());
  ASSERT_EQ("GET", receivedMethod);
}

TEST_F(utc_blink_ewk_policy_decision_http_method_get, TEST_METHOD_GET)
{
  currentMethod="GET";
  ASSERT_EQ(EINA_TRUE, ewk_view_html_string_load(GetEwkWebView(), getDocumentForMethod(currentMethod).c_str(), baseURL, 0));
  ASSERT_EQ(Success, EventLoopStart());
  ASSERT_EQ(currentMethod, receivedMethod);
}

TEST_F(utc_blink_ewk_policy_decision_http_method_get, TEST_METHOD_POST)
{
  currentMethod="POST";
  ASSERT_EQ(EINA_TRUE, ewk_view_html_string_load(GetEwkWebView(), getDocumentForMethod(currentMethod).c_str(), baseURL, 0));
  ASSERT_EQ(Success, EventLoopStart());
  ASSERT_EQ(currentMethod, receivedMethod);
}

TEST_F(utc_blink_ewk_policy_decision_http_method_get, TEST_INVALID_PARAM)
{
  ASSERT_EQ(NULL, ewk_policy_decision_http_method_get(NULL));
}
