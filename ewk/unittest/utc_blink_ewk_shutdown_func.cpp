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

class utc_blink_ewk_shutdown : public utc_blink_ewk_base
{
  /*
   * We need to overwrite the base method so this hiding is on purpose.
   */
  void SetUp()
  {
  // This test check ewk_init (normaly call in utc_blink_ewk_base::SetUp )
  }

  /*
   * We need to overwrite the base method so this hiding is on purpose.
   */
  void TearDown()
  {
  // This test check ewk_init (normaly call in utc_blink_ewk_base::SetUp )
  }
};

TEST_F(utc_blink_ewk_shutdown, POS_TEST)
{
  int result = ewk_init();
  if (result <= 0)
    utc_fail();

  result = ewk_shutdown();
  fprintf(stderr,"ewk_shutdown =%d\n",result);
  if (result < 0)
    utc_fail();
}

TEST_F(utc_blink_ewk_shutdown, NEG_TEST1)
{
  //shtdown without doing init
  int result = ewk_shutdown();
  while (result > 0) {
    result = ewk_shutdown();
  }
  //do call after all cleanup to check behavior
  result = ewk_shutdown();
  fprintf(stderr,"ewk_shutdown=%d\n",result);
  if (result > 0)
    utc_fail();
}
