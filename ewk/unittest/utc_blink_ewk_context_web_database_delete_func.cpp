// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_context_web_database_delete : public utc_blink_ewk_base {
protected:
  utc_blink_ewk_context_web_database_delete()
  : ctx(NULL)
  , origins(NULL)
  {
  }

  ~utc_blink_ewk_context_web_database_delete()
  {
    SetOrigins(NULL);
  }

  void LoadFinished(Evas_Object* webview)
  {
    utc_message("[loadFinished] :: ");
    EventLoopStop(Success);
  }

  void PostSetUp()
  {
    utc_message("[postSetUp] :: ");
    ctx = ewk_view_context_get(GetEwkWebView());
    ASSERT_TRUE(ctx != NULL);

    ASSERT_EQ(EINA_TRUE, ewk_context_web_database_delete_all(ctx));

    GetOrigins(0);
    ASSERT_TRUE(origins == NULL);
  }

  static void origins_get_cb(Eina_List* origins, void* user_data)
  {
    utc_message("origins_get_cb: %p, %p", origins, user_data);
    if (!user_data) {
      ewk_context_origins_free(origins);
      FAIL();
    }
    utc_blink_ewk_context_web_database_delete* owner = static_cast<utc_blink_ewk_context_web_database_delete*>(user_data);

    owner->SetOrigins(origins);
    owner->EventLoopStop(Success);
  }

  static void ScriptExecuteCallback(Evas_Object* o, const char* result_value, void* user_data)
  {
    utc_message("[scriptExecuteCallback] :: ");
    ASSERT_TRUE(user_data);
    utc_blink_ewk_context_web_database_delete* owner = static_cast<utc_blink_ewk_context_web_database_delete*>(user_data);
    owner->EventLoopStop(Success);
  }

  void SetOrigins(Eina_List* new_origins)
  {
    utc_message("[setOrigins] :: ");
    if (new_origins != origins) {
      if (origins) {
        ewk_context_origins_free(origins);
      }

      origins = new_origins;

      // print all origins
      Eina_List* it;
      void *vorigin = NULL;
      EINA_LIST_FOREACH(origins, it, vorigin) {
        Ewk_Security_Origin* origin = static_cast<Ewk_Security_Origin*>(vorigin);
        const char* host = ewk_security_origin_host_get(origin);
        const char* protocol = ewk_security_origin_protocol_get(origin);
        const int port = ewk_security_origin_port_get(origin);
        utc_message("[ORIGIN] :: %s:%s:%d", protocol, host, port);
      }
    }
  }

  void GetOrigins(int expected_count)
  {
    // web database operations are async, we must wait for changes to propagate
    for (int i = 0; i < 3; ++i) {
      if (EINA_TRUE != ewk_context_web_database_origins_get(ctx, origins_get_cb, this)) {
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
        const char* protocol = ewk_security_origin_protocol_get(origin);
        if (host && strcmp(host, origin_host) == 0) {
          retval = origin;
          break;
        }
      }
    }

    return retval;
  }

protected:
  static const char* const web_database_file;
  static const char* const sample_origin;

protected:
  Ewk_Context* ctx;
  Eina_List* origins;
};

const char* const utc_blink_ewk_context_web_database_delete::web_database_file = "ewk_context/WebDB2.html";
const char* const utc_blink_ewk_context_web_database_delete::sample_origin = "";

/**
* @brief Tests if can delete web database.
* If you encounter sql error "14 unable to open database file" set /tmp/databases/ write permissions.
*/
TEST_F(utc_blink_ewk_context_web_database_delete, POS_TEST)
{
  // Load from file
  std::string resurl = GetResourceUrl(web_database_file);
  utc_message("Loading resource: %s", resurl.c_str());
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), resurl.c_str()));
  ASSERT_EQ(Success, EventLoopStart());

  // Number of different origins that are used in this test
  const int origin_count = 1;

  GetOrigins(origin_count);

  ASSERT_TRUE(origins != NULL);
  ASSERT_EQ(origin_count, eina_list_count(origins));

  // remove sample origin
  Ewk_Security_Origin* origin = FindSecurityOrigin(sample_origin);
  ASSERT_TRUE(origin != NULL);

  ASSERT_EQ(EINA_TRUE, ewk_context_web_database_delete(ctx, origin));

  // Wait for changes to propagate
  GetOrigins(origin_count - 1);

  // if there are no origins in db, the "origins" argument passed to callback is NULL
  // see "void OnGetWebDBOrigins()" in "eweb_context.cc"
  ASSERT_TRUE(origins == NULL);
  ASSERT_EQ(origin_count - 1, eina_list_count(origins));

  // verify security origin was removed
  origin = FindSecurityOrigin(sample_origin);
  ASSERT_TRUE(origin == NULL);
}

/**
* @brief Tests if returns false when context is null.
*/
TEST_F(utc_blink_ewk_context_web_database_delete, NEG_TEST1)
{
  EXPECT_EQ(EINA_FALSE, ewk_context_web_database_delete(0, 0));
}

/**
* @brief Tests if returns false when origin is null.
*/
TEST_F(utc_blink_ewk_context_web_database_delete, NEG_TEST2)
{
  EXPECT_EQ(EINA_FALSE, ewk_context_web_database_delete(ctx, 0));
}