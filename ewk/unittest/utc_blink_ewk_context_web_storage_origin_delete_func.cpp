// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

/* Define those macros _before_ you include the utc_blink_ewk.h header file. */

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_context_web_storage_origin_delete: public utc_blink_ewk_base {
protected:
  utc_blink_ewk_context_web_storage_origin_delete()
    : utc_blink_ewk_base()
    , origins(NULL)
    , ctx(NULL)
  {
  }

  ~utc_blink_ewk_context_web_storage_origin_delete()
  {
    SetOrigins(NULL);
  }

  void PostSetUp()
  {
    ctx = ewk_view_context_get(GetEwkWebView());
    ASSERT_TRUE(ctx != NULL);

    // we want empty web storage at test start
    ASSERT_EQ(EINA_TRUE, ewk_context_web_storage_delete_all(ctx));

    GetOrigins(0);
    ASSERT_TRUE(origins == NULL);
  }

  void LoadFinished(Evas_Object* webview)
  {
    EventLoopStop(Success);
  }

  static void origins_get_cb(Eina_List* origins, void* user_data)
  {
    utc_message("origins_get_cb: %p, %p", origins, user_data);
    if (!user_data) {
      ewk_context_origins_free(origins);
      FAIL();
    }

    utc_blink_ewk_context_web_storage_origin_delete* owner = static_cast<utc_blink_ewk_context_web_storage_origin_delete*>(user_data);

    owner->SetOrigins(origins);
    owner->EventLoopStop(Success);
  }

  Ewk_Security_Origin* FindSecurityOrigin(const char* origin_host)
  {
    utc_message("FindSecurityOrigin: %s in %p", origin_host, origins);
    Ewk_Security_Origin* retval = NULL;

    if(origins && origin_host) {
      Eina_List* it;
      void *vorigin = NULL;
      EINA_LIST_FOREACH(origins, it, vorigin) {
        Ewk_Security_Origin* origin = static_cast<Ewk_Security_Origin*>(vorigin);

        const char* host = ewk_security_origin_host_get(origin);
        if (host && strcmp(host, origin_host) == 0) {
          retval = origin;
          break;
        }
      }
    }

    return retval;
  }

  void SetOrigins(Eina_List* new_origins)
  {
    if (new_origins != origins) {
      if (origins) {
        ewk_context_origins_free(origins);
      }

      origins = new_origins;
    }
  }

  void GetOrigins(int expected_count)
  {
    // web storage operations are async, we must wait for changes to propagate
    for (int i = 0; i < 3; ++i) {
      if (EINA_TRUE != ewk_context_web_storage_origins_get(ctx, origins_get_cb, this)) {
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

protected:
  static const char* const web_storage_sample_resource;
  static const char* const web_storage_sample_url;
  static const char* const sample_origin;

protected:
  Eina_List* origins;
  Ewk_Context* ctx;
};

const char* const utc_blink_ewk_context_web_storage_origin_delete::web_storage_sample_resource = "ewk_context_web_storage/web_storage_create_simple.html";
const char* const utc_blink_ewk_context_web_storage_origin_delete::web_storage_sample_url = "http://www.w3schools.com/html/tryit.asp?filename=tryhtml5_webstorage_local";
const char* const utc_blink_ewk_context_web_storage_origin_delete::sample_origin = "www.w3schools.com";

/**
* @brief Tests if can delete all web storages
*/
TEST_F(utc_blink_ewk_context_web_storage_origin_delete, POS_TEST)
{
  // load some pages with web storage
  std::string resurl = GetResourceUrl(web_storage_sample_resource);
  utc_message("Loading resource: %s", resurl.c_str());
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), resurl.c_str()));
  ASSERT_EQ(Success, EventLoopStart());

  utc_message("Loading web page: %s", web_storage_sample_url);
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), web_storage_sample_url));
  ASSERT_EQ(Success, EventLoopStart());

  GetOrigins(2);
  ASSERT_TRUE(origins != NULL);
  ASSERT_EQ(2, eina_list_count(origins));

  // remove sample origin
  Ewk_Security_Origin* origin = FindSecurityOrigin(sample_origin);
  ASSERT_TRUE(origin != NULL);

  ASSERT_EQ(EINA_TRUE, ewk_context_web_storage_origin_delete(ctx, origin));

  GetOrigins(1);
  ASSERT_TRUE(origins != NULL);
  ASSERT_EQ(1, eina_list_count(origins));

  // verify security origin was removed
  origin = FindSecurityOrigin(sample_origin);
  ASSERT_TRUE(origin == NULL);
}

/**
* @brief Tests if returns false when context is null.
*/
TEST_F(utc_blink_ewk_context_web_storage_origin_delete, invalid_args)
{
  EXPECT_EQ(EINA_FALSE, ewk_context_web_storage_origin_delete(NULL, NULL));
  EXPECT_EQ(EINA_FALSE, ewk_context_web_storage_origin_delete(ctx, NULL));
}
