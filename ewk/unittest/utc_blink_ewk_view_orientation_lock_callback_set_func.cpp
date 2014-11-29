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
