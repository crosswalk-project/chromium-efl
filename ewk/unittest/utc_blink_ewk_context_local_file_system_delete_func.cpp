// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_context_local_file_system_delete : public utc_blink_ewk_base
{
protected:
  utc_blink_ewk_context_local_file_system_delete()
    : utc_blink_ewk_base()
    , ctx(NULL)
    , origins(NULL)
    , origin_to_delete(NULL)
  {
  }

  ~utc_blink_ewk_context_local_file_system_delete()
  {
    origin_to_delete = NULL;
    SetOrigins(NULL);
  }

  void PreSetUp()
  {
    static char* args[]={"--allow-file-access-from-files"};
    ewk_set_arguments(1,args);
  }

  void PostSetUp()
  {
    utc_message("[PostSetUp] :: ");
    ctx = ewk_view_context_get(GetEwkWebView());
    evas_object_smart_callback_add(GetEwkWebView(), "title,changed", title_changed_cb, this);
  }

  void PreTearDown()
  {
    evas_object_smart_callback_del(GetEwkWebView(), "title,changed", title_changed_cb);
  }

  static void origins_get_cb(Eina_List* origins, void* user_data)
  {
    utc_message("[origins_get_cb] :: %p, %p", origins, user_data);
    ASSERT_TRUE(user_data);
    utc_blink_ewk_context_local_file_system_delete* owner = static_cast<utc_blink_ewk_context_local_file_system_delete*>(user_data);
    owner->SetOrigins(origins);
    owner->EventLoopStop(Success);
  }

  static void title_changed_cb(void* user_data, Evas_Object* webView, void* event_info)
  {
    utc_message("[title_changed_cb] :: \n");

    ASSERT_TRUE(user_data);
    utc_blink_ewk_context_local_file_system_delete* owner = static_cast<utc_blink_ewk_context_local_file_system_delete*>(user_data);

    if (!strcmp("SUCCESS", (char*)event_info)) {
      owner->EventLoopStop(Success);
    }
    else if (!strcmp("FAILURE", (char*)event_info)) {
      owner->EventLoopStop(Failure);
    }
  }

  void SetOrigins(Eina_List* new_origins)
  {
    utc_message("[SetOrigins] :: ");
    if (new_origins != origins) {
      if (origins) {
        ewk_context_origins_free(origins);
      }
      origins = new_origins;
    }
  }

  Eina_Bool HasOrigin(const char* protocol, const char* host, uint16_t port)
  {
    Eina_List* list = NULL;
    void* list_data = NULL;
    EINA_LIST_FOREACH(origins, list, list_data) {
      Ewk_Security_Origin* origin = (Ewk_Security_Origin*)(list_data);
      const char* protocol_ = ewk_security_origin_protocol_get(origin);
      const char* host_ = ewk_security_origin_host_get(origin);
      uint16_t port_ = ewk_security_origin_port_get(origin);
      if (!strcmp(protocol_, protocol) && !strcmp(host_, host) && (port_ == port)) {
        origin_to_delete = origin;
        return EINA_TRUE;
      }
    }
    return EINA_FALSE;
  }

  void GetOrigins(int expected_count)
  {
    utc_message("[GetOrigins] :: %d", eina_list_count(origins));
    // web database operations are async, we must wait for changes to propagate
    for (int i = 0; i < 3; ++i) {
      if (EINA_TRUE != ewk_context_local_file_system_origins_get(ctx, origins_get_cb, this))
        break;

      if (Success != EventLoopStart())
        break;

      if (expected_count == eina_list_count(origins))
        break;

      if (!EventLoopWait(3.0))
        break;
    }
  }

protected:
  static const char* const URL;
  static const char* const expected_protocol;
  static const char* const expected_host;
  static const uint16_t expected_port;

protected:
  Eina_List* origins;
  Ewk_Security_Origin* origin_to_delete;
  Ewk_Context* ctx;
};

const char* const utc_blink_ewk_context_local_file_system_delete::URL = "ewk_context_local_file_system/sample_context_local_file_system_write.html";
const char* const utc_blink_ewk_context_local_file_system_delete::expected_protocol = "file";
const char* const utc_blink_ewk_context_local_file_system_delete::expected_host = "";
const uint16_t utc_blink_ewk_context_local_file_system_delete::expected_port = 65535;

/**
* @brief Tests if there is possibility to get local file system origins
*/
TEST_F(utc_blink_ewk_context_local_file_system_delete, POS_TEST)
{
  // Delete the local file system and check if there are no origins left
  ASSERT_EQ(EINA_TRUE, ewk_context_local_file_system_all_delete(ctx));
  GetOrigins(0);
  ASSERT_EQ(0, eina_list_count(origins));
  ASSERT_TRUE(origins == NULL);

  // Create a local file system
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), GetResourceUrl(URL).c_str()));
  ASSERT_EQ(Success, EventLoopStart());

  // Check if origin is loaded
  GetOrigins(1);
  ASSERT_EQ(EINA_TRUE, HasOrigin(expected_protocol, expected_host, expected_port));

  // Delete selected origin
  ASSERT_TRUE(origin_to_delete);
  ASSERT_EQ(EINA_TRUE, ewk_context_local_file_system_delete(ctx, origin_to_delete));

  // Check if there is no expected origin
  GetOrigins(0);
  ASSERT_EQ(EINA_FALSE, HasOrigin(expected_protocol, expected_host, expected_port));
}

/**
* @brief Tests if cannot delete local file systems when origin is null
*/
TEST_F(utc_blink_ewk_context_local_file_system_delete, NEG_TEST1)
{
  ASSERT_EQ(EINA_FALSE, ewk_context_local_file_system_delete(ctx, NULL));
}

/**
* @brief Tests if cannot delete local file systems when context is null
*/
TEST_F(utc_blink_ewk_context_local_file_system_delete, NEG_TEST2)
{
  ASSERT_EQ(EINA_FALSE, ewk_context_local_file_system_delete(NULL, NULL));
}