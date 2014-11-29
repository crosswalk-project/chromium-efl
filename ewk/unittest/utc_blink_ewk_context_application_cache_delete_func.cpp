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

#define SAMPLE_ORIGIN_HOST "htmlfive.appspot.com"
#define SAMPLE_ORIGIN_PROTOCOL "http"
// TODO consider using cache status check - possible implementation left for reference
class utc_blink_ewk_context_application_cache_delete : public utc_blink_ewk_base {

protected:
  utc_blink_ewk_context_application_cache_delete()
    : utc_blink_ewk_base()
    , ctx_(NULL)
    , origins_(NULL)
  {
  }

  ~utc_blink_ewk_context_application_cache_delete()
  {
    SetOrigins(NULL);
  }

  void PostSetUp()
  {
    ctx_ = ewk_view_context_get(GetEwkWebView());
    ASSERT_TRUE(ctx_);
  }

  void LoadFinished(Evas_Object* webview)
  {
    utc_message("[LoadFinished]");
    EventLoopStop(utc_blink_ewk_base::Success);
  }

  Eina_Bool DeleteCache(const char* origin_host, const char* origin_protocol)
  {
    utc_message("DeleteCache: %s in %p", origin_host, origins_);

    if (origins_ && origin_host) {
      Eina_List* it = NULL;
      void* list_data = NULL;
      EINA_LIST_FOREACH(origins_, it, list_data) {
        Ewk_Security_Origin* origin = static_cast<Ewk_Security_Origin*>(list_data);

        utc_message("%s, %s", ewk_security_origin_protocol_get(origin), ewk_security_origin_host_get(origin));

        if (!strcmp(ewk_security_origin_protocol_get(origin), origin_protocol) &&
            !strcmp(ewk_security_origin_host_get(origin), origin_host)) {
          utc_message("DeleteCache: found origin, deleting");
          return ewk_context_application_cache_delete(ctx_, origin);
        }
      }
    }
    return EINA_FALSE;
  }

  void GetOrigins(int expected_count)
  {
    utc_message("[GetOrigins] : expecting count %d", expected_count);

    // delete operations are async, we must wait for changes to propagate
    for (int i = 0; i < 3; ++i) {
      if (EINA_TRUE != ewk_context_application_cache_origins_get(ctx_, origins_get_cb, this)) {
        break;
      }

      if (Success != EventLoopStart()) {
        break;
      }

      int count = eina_list_count(origins_);
      utc_message("[GetOrigins] : origins count: %d", count);
      if (expected_count == count) {
        break;
      }

      if (!EventLoopWait(3.0)) {
        break;
      }
    }
  }

  void SetOrigins(Eina_List* new_origins)
  {
    utc_message("[SetOrigins]");
    if (new_origins != origins_) {
      if (origins_) {
        ewk_context_origins_free(origins_);
      }

      origins_ = new_origins;
    }
  }

  static void origins_get_cb(Eina_List* origins, void* user_data)
  {
    utc_message("origins_get_cb: %p, %p", origins, user_data);
    ASSERT_TRUE(user_data);

    utc_blink_ewk_context_application_cache_delete* owner =
        static_cast<utc_blink_ewk_context_application_cache_delete*>(user_data);

    Eina_List* it = NULL;
    void* list_data = NULL;
    EINA_LIST_FOREACH(origins, it, list_data) {
      Ewk_Security_Origin* origin = static_cast<Ewk_Security_Origin*>(list_data);

      utc_message("%s, %s", ewk_security_origin_protocol_get(origin), ewk_security_origin_host_get(origin));
    }
    owner->SetOrigins(origins);
    owner->EventLoopStop(Success);
  }

protected:
  static const char* const appCacheURL;
  static const char* const appCacheURLOther;

  Ewk_Context* ctx_;
  Eina_List* origins_;
};

const char* const utc_blink_ewk_context_application_cache_delete::appCacheURL = SAMPLE_ORIGIN_PROTOCOL "://" SAMPLE_ORIGIN_HOST "/static/stickies.html";
const char* const utc_blink_ewk_context_application_cache_delete::appCacheURLOther = "http://www.w3schools.com/html/tryhtml5_html_manifest.htm";

/**
* @brief Tests if delete application cache is working.
*/
TEST_F(utc_blink_ewk_context_application_cache_delete, POS_TEST)
{
  // Load page, so all related APIs are initialized
  ASSERT_TRUE(ewk_view_url_set(GetEwkWebView(), "http://www.google.pl"));
  ASSERT_EQ(utc_blink_ewk_base::Success, EventLoopStart());

  // we want empty cache at test start
  ASSERT_EQ(EINA_TRUE, ewk_context_application_cache_delete_all(ctx_));
  GetOrigins(0);
  ASSERT_TRUE(origins_ == NULL);
  ASSERT_EQ(0, eina_list_count(origins_));

  utc_message("set 1st url : %s", appCacheURL);
  ASSERT_TRUE(ewk_view_url_set(GetEwkWebView(), appCacheURL));
  ASSERT_EQ(utc_blink_ewk_base::Success, EventLoopStart());

  GetOrigins(1);
  ASSERT_TRUE(origins_ != NULL);
  ASSERT_EQ(1, eina_list_count(origins_));

  utc_message("set 2nd url : %s", appCacheURLOther);
  ASSERT_TRUE(ewk_view_url_set(GetEwkWebView(), appCacheURLOther));
  ASSERT_EQ(utc_blink_ewk_base::Success, EventLoopStart());

  GetOrigins(2);
  ASSERT_TRUE(origins_ != NULL);
  ASSERT_EQ(2, eina_list_count(origins_));

  ASSERT_TRUE(DeleteCache(SAMPLE_ORIGIN_HOST, SAMPLE_ORIGIN_PROTOCOL));

  // there should be one origin left; the delete call is asynhronous
  GetOrigins(1);
  ASSERT_TRUE(origins_ != NULL);
  ASSERT_EQ(1, eina_list_count(origins_));
}

/**
 * @brief Tests if returns false when parameters are wrong.
 */
TEST_F(utc_blink_ewk_context_application_cache_delete, NEG_INVALID_PARAMS)
{
  EXPECT_EQ(ewk_context_application_cache_delete(0, 0), EINA_FALSE);
  EXPECT_EQ(ewk_context_application_cache_delete(ctx_, 0), EINA_FALSE);
}
