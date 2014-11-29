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

/* Define those macros _before_ you include the utc_blink_ewk.h header file. */

#include "utc_blink_ewk.h"

#define SAMPLE_FILE "/common/sample.html"

typedef void (*File_System_Callback)();

static File_System_Callback finishedCallback;

extern struct Ewk_Test_View test_view;
static Eina_Bool hasOrigin;

class utc_blink_ewk_context_local_file_system_origins_get : public ::testing::Test {
  protected:
    utc_blink_ewk_context_local_file_system_origins_get() {
      finishedCallback = 0;
    }

    virtual ~utc_blink_ewk_context_local_file_system_origins_get() {
    }

    virtual void SetUp() {
      utc_blink_ewk_test_init();
      evas_object_smart_callback_add(test_view.webview, "load,finished", load_finished, NULL);
    }

    virtual void TearDown() {
      evas_object_smart_callback_del(test_view.webview, "load,finished", load_finished);
      utc_blink_ewk_test_end();
    }

    static void load_finished(void* data, Evas_Object* webview, void* event_info)
    {
      finishedCallback();
    }

    static void getOriginsCallback(Eina_List* origins, void* user_data)
    {
      if (origins) {
       if (eina_list_count(origins))
        hasOrigin = EINA_TRUE;
        ewk_context_origins_free(origins);
      }

      ewk_context_local_file_system_all_delete(ewk_view_context_get(test_view.webview));
      utc_blink_main_loop_quit();
    }

    static void loadURL(const char* url)
    {
      char* full_path = generate_full_resource_path(url);
      if (!full_path)
        utc_fail();

      Eina_Bool result = ewk_view_url_set(test_view.webview, full_path);
      free(full_path);

      if (!result)
        utc_fail();
    }

    static void deleteFileSystemsFinished()
    {
      ewk_context_local_file_system_origins_get(ewk_view_context_get(test_view.webview), getOriginsCallback, NULL);
    }

    static void deleteFileSystems(File_System_Callback finished)
    {
      //need to delete all file systems first
      Eina_Bool result = ewk_context_local_file_system_all_delete(ewk_view_context_get(test_view.webview));
      if (!result)
        utc_fail();
      else {
        finishedCallback = finished;
        loadURL(SAMPLE_FILE);
      }
    }

    static void addFileSystemsFinished()
    {
      if (!ewk_context_local_file_system_origins_get(ewk_view_context_get(test_view.webview), getOriginsCallback, NULL)) {
       utc_blink_main_loop_quit();
      }
    }

    static void addFileSystems(File_System_Callback finished)
    {
      finishedCallback = finished;
      ewk_view_url_set(test_view.webview, "http://www.html5rocks.com/en/tutorials/file/filesystem/");
      //Internet access to the above website is required for creating a local file system, since blink engine does not allow
      // creation of a new local file system from a locally stored HTML file
      //Reference : http://updates.html5rocks.com/2011/08/Debugging-the-Filesystem-API
    }
 };

/**
* @brief Tests if there is possibility to get local file system origins
*/
TEST_F(utc_blink_ewk_context_local_file_system_origins_get, POS_TEST)
{
  addFileSystems(addFileSystemsFinished);
  utc_blink_main_loop_begin();
  utc_check_eq(hasOrigin , EINA_TRUE);
}

/**
* @brief Tests if local file system origins are not returned when there is no local file system
*/
TEST_F(utc_blink_ewk_context_local_file_system_origins_get, NEG_TEST1)
{
  deleteFileSystems(deleteFileSystemsFinished);
  utc_blink_main_loop_begin();
  hasOrigin = EINA_FALSE;
  utc_check_ne(hasOrigin , EINA_TRUE);
}

/**
* @brief Tests if returns false when context is null.
*/
TEST_F(utc_blink_ewk_context_local_file_system_origins_get, NEG_TEST2)
{
  utc_check_eq(ewk_context_local_file_system_origins_get(0, getOriginsCallback, 0), EINA_FALSE);
}

/**
* @brief Tests if returns false which callback was null.
*/
TEST_F(utc_blink_ewk_context_local_file_system_origins_get, NEG_TEST3)
{
  utc_check_eq(ewk_context_local_file_system_origins_get(ewk_view_context_get(test_view.webview), 0, 0), EINA_FALSE);
}