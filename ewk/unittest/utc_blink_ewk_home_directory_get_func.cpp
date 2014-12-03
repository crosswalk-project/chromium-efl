// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"
#include "../private/ewk_main_private.h"

class utc_blink_ewk_home_directory_get : public utc_blink_ewk_base
{
};

/**
 * @brief Test if home directory is correctly returned when m_homeDirectory is set.
 */
TEST_F(utc_blink_ewk_home_directory_get, m_homeDirectory_is_set)
{
  ewk_home_directory_set("/someNewDir");
  const char* dir = ewk_home_directory_get();
  utc_message("Home dir == %s", dir);
  ASSERT_STREQ("/someNewDir", dir);
  ewk_home_directory_set(NULL);
}

/**
 * @brief Test if home directory is correctly returned when there is no m_homeDirectory and $HOME is set.
 */
TEST_F(utc_blink_ewk_home_directory_get, m_homeDirectory_not_set)
{
  ewk_home_directory_set(NULL);

  // If there is no $HOME set it
  const char* home = getenv("HOME");
  bool home_set = false;
  if (!home) {
    setenv("HOME", "/newHomeDir", 1);
    home = getenv("HOME");
    home_set = true;
  }

  // Get home directory
  const char* dir = ewk_home_directory_get();
  utc_message("Home dir == %s", dir);
  ASSERT_STREQ(home, dir);

  // Unset $HOME if it was previously set
  if (home_set) {
    unsetenv("HOME");
    ASSERT_EQ(NULL, getenv("HOME"));
  }
}

/**
 * @brief Test if home directory is correctly returned when $HOME is not set.
 */
TEST_F(utc_blink_ewk_home_directory_get, env_HOME_not_set)
{
  ewk_home_directory_set(NULL);
  // Save original $HOME
  const char* orig_home = getenv("HOME");
  if (orig_home) {
    unsetenv("HOME");
    ASSERT_EQ(NULL, getenv("HOME"));
  }

  // If there is no $TMPDIR set it
  const char* tmpdir = getenv("TMPDIR");
  bool tmpdir_set = false;
  if (!tmpdir) {
    setenv("TMPDIR", "/newtmpdir", 1);
    tmpdir = getenv("TMPDIR");
    tmpdir_set = true;
  }

  // Get home directory
  const char* dir = ewk_home_directory_get();
  utc_message("Home dir == %s", dir);
  ASSERT_STREQ(tmpdir, dir);

  // Restore original $HOME
  if (orig_home) {
    setenv("HOME", orig_home, 1);
    ASSERT_STREQ(orig_home, getenv("HOME"));
  }

  // Unset $TMPDIR if it was previously set
  if (tmpdir_set) {
    unsetenv("TMPDIR");
    ASSERT_EQ(NULL, getenv("TMPDIR"));
  }
}

/**
 * @brief Test if home directory is correctly returned when $TMPDIR is not set.
 */
TEST_F(utc_blink_ewk_home_directory_get, env_TMPDIR_not_set)
{
  ewk_home_directory_set(NULL);
  // Save $HOME
  const char* home = getenv("HOME");
  if (home) {
    unsetenv("HOME");
    ASSERT_EQ(NULL, getenv("HOME"));
  }

  // Save $TMPDIR
  const char* tmpdir = getenv("TMPDIR");
  if (tmpdir) {
    unsetenv("TMPDIR");
    ASSERT_EQ(NULL, getenv("TMPDIR"));
  }

  // Get home directory
  const char* dir = ewk_home_directory_get();
  utc_message("Home dir == %s", dir);
  ASSERT_STREQ("/tmp", dir);

  // Restore $HOME
  if (home) {
    setenv("HOME", home, 1);
    ASSERT_STREQ(home, getenv("HOME"));
  }

  // Restore $TMPDIR
  if (tmpdir) {
    setenv("TMPDIR", tmpdir, 1);
    ASSERT_STREQ(tmpdir, getenv("TMPDIR"));
  }
}

