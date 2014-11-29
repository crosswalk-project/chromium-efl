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

class utc_blink_cb_unfocus_direction : public utc_blink_ewk_base
{
protected:

  void LoadFinished(Evas_Object*)
  {
    evas_object_focus_set(GetEwkWebView(), EINA_TRUE);
    EventLoopStop(Success);
  }

  void PostSetUp()
  {
    evas_object_smart_callback_add(GetEwkWebView(), "unfocus,direction", ToSmartCallback(unfocus_direction_cb), this);
    received_direction = EWK_UNFOCUS_DIRECTION_NONE;
  }

  void PreTearDown()
  {
    evas_object_smart_callback_del(GetEwkWebView(), "unfocus,direction", ToSmartCallback(unfocus_direction_cb));
  }

  static void unfocus_direction_cb(utc_blink_cb_unfocus_direction* owner, Evas_Object*, Ewk_Unfocus_Direction* direction)
  {
    ASSERT_TRUE(owner);
    if (direction)
      owner->received_direction = *direction;
    owner->EventLoopStop(Success);
  }

protected:
  Ewk_Unfocus_Direction received_direction;
};

/**
 * @brief Tests unfocus in forward direction
 */
TEST_F(utc_blink_cb_unfocus_direction, Unfocus_direction_forward)
{
  const char htmlDocument[] =
        "<html><body><form>"
        "<input type='text' id='test1'/><br/>"
        "<input type='text' id='test2' autofocus/><br/>"
        "</form></body></html>";

  ASSERT_EQ(EINA_TRUE, ewk_view_html_string_load(GetEwkWebView(), htmlDocument, 0, 0));
  ASSERT_EQ(Success, EventLoopStart());
  evas_event_feed_key_down(GetEwkEvas(), "Tab", "Tab", "\t", NULL, 0, NULL);
  evas_event_feed_key_up(GetEwkEvas(), "Tab", "Tab", "\t", NULL, 0, NULL);
  ASSERT_EQ(Success, EventLoopStart()) << "Unfocus,direction callback never called";
  ASSERT_EQ(EWK_UNFOCUS_DIRECTION_FORWARD, received_direction);
}

/**
 * @brief Tests unfocus in backward direction
 */
TEST_F(utc_blink_cb_unfocus_direction, Unfocus_direction_backward)
{
  const char htmlDocument[] =
        "<html><body><form>"
        "<input type='text' id='test1' autofocus\><br\>"
        "<input type='text' id='test2'\><br\>"
        "</form></body></html>";

  ASSERT_EQ(EINA_TRUE, ewk_view_html_string_load(GetEwkWebView(), htmlDocument, 0, 0));
  ASSERT_EQ(Success, EventLoopStart());
  evas_key_modifier_on(GetEwkEvas(), "Shift");
  evas_event_feed_key_down(GetEwkEvas(), "Tab", "Tab", "\t", NULL, 0, NULL);
  evas_event_feed_key_up(GetEwkEvas(), "Tab", "Tab", "\t", NULL, 0, NULL);
  evas_key_modifier_off(GetEwkEvas(), "Shift");
  ASSERT_EQ(Success, EventLoopStart()) << "Unfocus,direction callback never called";
  ASSERT_EQ(EWK_UNFOCUS_DIRECTION_BACKWARD, received_direction);
}
