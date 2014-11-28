// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_view_add_in_incognito_mode : public utc_blink_ewk_base {
protected:
  utc_blink_ewk_view_add_in_incognito_mode()
    : utc_blink_ewk_base()
    , appcache_origins(NULL)
  {
  }

  ~utc_blink_ewk_view_add_in_incognito_mode()
  {
    SetAppCacheOrigins(NULL);
  }

  void PostSetUp()
  {
    // TODO: appcache is initialized asynchronously, loading pages helps with TC pass rate
    ASSERT_TRUE(ewk_view_url_set(GetEwkWebView(), "about:blank"));
    ASSERT_EQ(utc_blink_ewk_base::Success, EventLoopStart());

    Ewk_Context* ctx = ewk_view_context_get(GetEwkWebView());

    // we want empty cache at test start
    ASSERT_EQ(EINA_TRUE, ewk_context_application_cache_delete_all(ctx));
    GetAppCacheOrigins(GetEwkWebView(), 0);
    ASSERT_TRUE(appcache_origins == NULL);
    ASSERT_EQ(0, eina_list_count(appcache_origins));
  }

  void LoadFinished(Evas_Object* webview)
  {
    utc_message("LoadFinished");
    EventLoopStop(Success);
  }

  Eina_Bool FindSecurityOrigin(const char* origin_host, const char* origin_protocol)
  {
    utc_message("FindSecurityOrigin: %s in %p", origin_host, appcache_origins);

    if(appcache_origins && origin_host) {
      Eina_List* it = NULL;
      void* list_data = NULL;
      EINA_LIST_FOREACH(appcache_origins, it, list_data) {
        Ewk_Security_Origin* origin = static_cast<Ewk_Security_Origin*>(list_data);

        if (!strcmp(ewk_security_origin_protocol_get(origin), origin_protocol) &&
            !strcmp(ewk_security_origin_host_get(origin), origin_host)) {
          return EINA_TRUE;
        }
      }
    }
    return EINA_FALSE;
  }

  void GetAppCacheOrigins(Evas_Object* view, unsigned int expected_count)
  {
    utc_message("GetAppCacheOrigins: %d", expected_count);
    Ewk_Context* ctx = ewk_view_context_get(view);
    // appcache_origins operations are async, we must wait for changes to propagate
    for (int i = 0; i < 3; ++i) {
      Eina_Bool result = ewk_context_application_cache_origins_get(
                          ctx,
                          applicationCacheOriginsGet,
                          this);

      if (EINA_TRUE != result) {
        break;
      }

      if (Success != EventLoopStart()) {
        break;
      }

      if (expected_count == eina_list_count(appcache_origins)) {
        break;
      }

      if (!EventLoopWait(3.0)) {
        break;
      }
    }
  }

  void SetAppCacheOrigins(Eina_List* new_appcache_origins)
  {
    utc_message("SetAppCacheOrigins");
    if (new_appcache_origins != appcache_origins) {
      if (appcache_origins) {
        ewk_context_origins_free(appcache_origins);
      }

      appcache_origins = new_appcache_origins;
    }
  }

  static void applicationCacheOriginsGet(Eina_List* appcache_origins, void* data)
  {
    utc_message("[applicationCacheOriginsGet] : %p, %p", appcache_origins, data);

    if (!data) {
       ewk_context_origins_free(appcache_origins);
       FAIL();
    }

    utc_blink_ewk_view_add_in_incognito_mode* owner =
        static_cast<utc_blink_ewk_view_add_in_incognito_mode*>(data);

    owner->SetAppCacheOrigins(appcache_origins);
    owner->EventLoopStop(Success);
  }

  Evas_Object* CreateWindow(bool incognito)
  {
    Evas_Object* ewk_window = elm_win_add(NULL, "TCSubWindow", ELM_WIN_BASIC);
    elm_win_title_set(ewk_window, "TCSubWindow");
    Evas* evas = evas_object_evas_get(ewk_window);

    Evas_Object* retval = NULL;
    if (incognito) {
      retval = ewk_view_add_in_incognito_mode(evas);
    } else {
      Ewk_Context* ctx = ewk_context_new();
      retval = ewk_view_add_with_context(evas, ctx);
    }

    evas_object_smart_callback_add(retval,
                                   "load,finished",
                                   ToSmartCallback(load_finished_cb), this);

    return retval;
  }

  static void load_finished_cb(utc_blink_ewk_view_add_in_incognito_mode* owner,
                               Evas_Object*,
                               void*)
  {
    ASSERT_TRUE(owner);
    owner->EventLoopStop(Success);
  }

protected:
  static const char* const appCacheURL;
  Eina_List* appcache_origins;
};

#define SAMPLE_ORIGIN_HOST "htmlfive.appspot.com"
#define SAMPLE_ORIGIN_PROTOCOL "http"
const char* const utc_blink_ewk_view_add_in_incognito_mode::appCacheURL = SAMPLE_ORIGIN_PROTOCOL "://" SAMPLE_ORIGIN_HOST "/static/stickies.html";

/**
* @brief Load page and check if the appcache_origins is there
*/
TEST_F(utc_blink_ewk_view_add_in_incognito_mode, POS_TEST)
{
  utc_message("Loading web page: %s", appCacheURL);
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), appCacheURL));

  ASSERT_EQ(Success, EventLoopStart());

  // Setup some cached content

  GetAppCacheOrigins(GetEwkWebView(), 1);
  ASSERT_TRUE(appcache_origins != NULL);
  ASSERT_EQ(1, eina_list_count(appcache_origins));
  ASSERT_TRUE(FindSecurityOrigin(SAMPLE_ORIGIN_HOST, SAMPLE_ORIGIN_PROTOCOL));

  // Create another view in normal mode and check if cached content is available
  Evas_Object* win2 = CreateWindow(false);

  ASSERT_TRUE(ewk_view_url_set(win2, "about:blank"));
  ASSERT_EQ(utc_blink_ewk_base::Success, EventLoopStart());

  GetAppCacheOrigins(win2, 1);
  ASSERT_TRUE(appcache_origins != NULL);
  ASSERT_EQ(1, eina_list_count(appcache_origins));
  ASSERT_TRUE(FindSecurityOrigin(SAMPLE_ORIGIN_HOST, SAMPLE_ORIGIN_PROTOCOL));

  // Create incognito view - cached content should not be available
  Evas_Object* incognito = CreateWindow(true);

  ASSERT_TRUE(ewk_view_url_set(incognito, "about:blank"));
  ASSERT_EQ(utc_blink_ewk_base::Success, EventLoopStart());

  GetAppCacheOrigins(incognito, 0);
  ASSERT_TRUE(appcache_origins == NULL);
  ASSERT_EQ(0, eina_list_count(appcache_origins));
  ASSERT_FALSE(FindSecurityOrigin(SAMPLE_ORIGIN_HOST, SAMPLE_ORIGIN_PROTOCOL));
}

