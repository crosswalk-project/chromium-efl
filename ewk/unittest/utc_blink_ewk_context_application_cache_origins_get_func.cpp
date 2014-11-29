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


class utc_blink_ewk_context_application_cache_origins_get : public utc_blink_ewk_base {
protected:
  utc_blink_ewk_context_application_cache_origins_get()
    : utc_blink_ewk_base()
    , ctx(NULL)
    , origins(NULL)
  {
  }

  ~utc_blink_ewk_context_application_cache_origins_get()
  {
    SetOrigins(NULL);
  }

  void PostSetUp()
  {
    ctx = ewk_view_context_get(GetEwkWebView());
    ASSERT_TRUE(ctx != NULL);
  }

  void LoadFinished(Evas_Object* webview)
  {
    utc_message("LoadFinished");
    EventLoopStop(Success);
  }

  Eina_Bool FindSecurityOrigin(const char* origin_host, const char* origin_protocol)
  {
    utc_message("FindSecurityOrigin: %s in %p", origin_host, origins);

    if(origins && origin_host) {
      Eina_List* it = NULL;
      void* list_data = NULL;
      EINA_LIST_FOREACH(origins, it, list_data) {
        Ewk_Security_Origin* origin = static_cast<Ewk_Security_Origin*>(list_data);

        if (!strcmp(ewk_security_origin_protocol_get(origin), origin_protocol) &&
            !strcmp(ewk_security_origin_host_get(origin), origin_host)) {
          return EINA_TRUE;
        }
      }
    }
    return EINA_FALSE;
  }

  void GetOrigins(unsigned int expected_count)
  {
    utc_message("GetOrigins: %d", expected_count);
    // origins operations are async, we must wait for changes to propagate
    for (int i = 0; i < 3; ++i) {
      if (EINA_TRUE != ewk_context_application_cache_origins_get(ctx, applicationCacheOriginsGet, this)) {
        break;
      }

      if (Success != EventLoopStart()) {
        break;
      }

      if (expected_count == eina_list_count(origins)) {
        break;
      }

      if (!EventLoopWait(3.0)) {
        break;
      }
    }
  }

  void SetOrigins(Eina_List* new_origins)
  {
    utc_message("SetOrigins");
    if (new_origins != origins) {
      if (origins) {
        ewk_context_origins_free(origins);
      }

      origins = new_origins;
    }
  }

  static void applicationCacheOriginsGet(Eina_List* origins, void* data)
  {
    utc_message("[applicationCacheOriginsGet] : %p, %p", origins, data);

    if (!data) {
       ewk_context_origins_free(origins);
       FAIL();
    }

    utc_blink_ewk_context_application_cache_origins_get* owner =
        static_cast<utc_blink_ewk_context_application_cache_origins_get*>(data);

    owner->SetOrigins(origins);
    owner->EventLoopStop(Success);
  }

protected:
  static const char* const appCacheURL;
  Ewk_Context* ctx;
  Eina_List* origins;
};

#define SAMPLE_ORIGIN_HOST "htmlfive.appspot.com"
#define SAMPLE_ORIGIN_PROTOCOL "http"
const char* const utc_blink_ewk_context_application_cache_origins_get::appCacheURL = SAMPLE_ORIGIN_PROTOCOL "://" SAMPLE_ORIGIN_HOST "/static/stickies.html";

/**
* @brief Load page and check if the origins is there
*/
TEST_F(utc_blink_ewk_context_application_cache_origins_get, POS_TEST)
{
  // Load page, so all related APIs are initialized
  ASSERT_TRUE(ewk_view_url_set(GetEwkWebView(), "http://www.google.pl"));
  ASSERT_EQ(utc_blink_ewk_base::Success, EventLoopStart());

  // we want empty cache at test start
  ASSERT_EQ(EINA_TRUE, ewk_context_application_cache_delete_all(ctx));
  GetOrigins(0);
  ASSERT_TRUE(origins == NULL);
  ASSERT_EQ(0, eina_list_count(origins));

  utc_message("Loading web page: %s", appCacheURL);
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), appCacheURL));

  ASSERT_EQ(Success, EventLoopStart());

  GetOrigins(1);
  ASSERT_TRUE(origins != NULL);
  ASSERT_EQ(1, eina_list_count(origins));

  ASSERT_TRUE(FindSecurityOrigin(SAMPLE_ORIGIN_HOST, SAMPLE_ORIGIN_PROTOCOL));
}

/**
 * @brief Tests in case of bad parameters
 */
TEST_F(utc_blink_ewk_context_application_cache_origins_get, INVALID_ARGS)
{
  EXPECT_EQ(EINA_FALSE, ewk_context_application_cache_origins_get(NULL, applicationCacheOriginsGet, NULL));
  EXPECT_EQ(EINA_FALSE, ewk_context_application_cache_origins_get(ctx, NULL, NULL));
}
