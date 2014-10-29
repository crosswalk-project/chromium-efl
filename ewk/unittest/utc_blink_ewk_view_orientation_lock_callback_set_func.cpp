// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_view_orientation_lock_callback_set : public utc_blink_ewk_base
{
protected:
  //This function will be invoked when orientation has to be locked or unlocked
  //The API for orientation lock and unlock has not been implemented yet.
  static Eina_Bool lockCallback(Evas_Object* o, Eina_Bool need_lock, int orientation, void* user_data)
  {
  }
};

/**
* @brief Tests if EINA_SAFETY check fails if view object is NULL
*/
TEST_F(utc_blink_ewk_view_orientation_lock_callback_set, NEG_TEST1)
{
  ewk_view_orientation_lock_callback_set(NULL, lockCallback, this);
}

/**
* @brief Tests if EINA_SAFETY check fails if callback is NULL
*/
TEST_F(utc_blink_ewk_view_orientation_lock_callback_set, NEG_TEST2)
{
  ewk_view_orientation_lock_callback_set(GetEwkWebView(), NULL, this);
}

/**
* @brief Tests if ewk_view_orientation_lock_callback_set is called without any error
*/
TEST_F(utc_blink_ewk_view_orientation_lock_callback_set, POS_TEST)
{
  ewk_view_orientation_lock_callback_set(GetEwkWebView(), lockCallback, this);
}
