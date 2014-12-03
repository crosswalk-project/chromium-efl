// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"
#include "../private/ewk_main_private.h"

class utc_blink_ewk_home_directory_set : public utc_blink_ewk_base
{
};

/**
 * @brief Test if home directory is correctly set.
 */
TEST_F(utc_blink_ewk_home_directory_set, POS_TEST1)
{
  const char* home_dir = ewk_home_directory_get();
  ASSERT_TRUE(home_dir);
  const char* new_dir = "/foobar";
  ASSERT_STRNE(home_dir, new_dir);
  ewk_home_directory_set(new_dir);
  ASSERT_STREQ(new_dir, ewk_home_directory_get());
}

/**
 * @brief Test if home directory path is internally stored.
 */
TEST_F(utc_blink_ewk_home_directory_set, POS_TEST2)
{
  char* test = strdup("/foobar");
  ewk_home_directory_set(test);
  const char* home_dir = ewk_home_directory_get();
  test[0] = 'A';
  ASSERT_STRNE(test, home_dir);
}

/**
 * @brief Test if function works correctly with NULL argument.
 */
TEST_F(utc_blink_ewk_home_directory_set, NEG_TEST1)
{
  // Set any home dir
  const char* new_dir = "/foobar";
  ewk_home_directory_set(new_dir);
  ASSERT_STREQ(new_dir, ewk_home_directory_get());

  // Set it to NULL
  ewk_home_directory_set(NULL);
  const char* ewk_home_dir = ewk_home_directory_get();
  utc_message("Home dir == %s", ewk_home_dir);
  // Check if it was changed
  ASSERT_STRNE(new_dir, ewk_home_dir);
  ASSERT_STRNE("", ewk_home_dir);
  ASSERT_TRUE(ewk_home_dir);

  // Check behavior with empty string
  ewk_home_directory_set("");
  ewk_home_dir = ewk_home_directory_get();
  utc_message("Home dir == %s", ewk_home_dir);
  // Assuming $HOME is not defined as empty string
  ASSERT_STRNE("", ewk_home_dir);
  ASSERT_TRUE(ewk_home_dir);

  // Check if it was returned as one of the the expected values
  const char* home_dir = getenv("HOME");
  const char* tmp_dir = getenv("TMPDIR");
  if (home_dir && home_dir[0]) {
    ASSERT_STREQ(home_dir, ewk_home_dir);
  } else if (tmp_dir && tmp_dir[0]) {
    ASSERT_STREQ(tmp_dir, ewk_home_dir);
  } else {
    ASSERT_STREQ("/tmp", ewk_home_dir);
  }
}

