/*
 * Chromium EFL
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

class utc_blink_ewk_view_screenshot_contents_get : public utc_blink_ewk_base
{
 protected:
  /* Callback for load finished */
  void LoadFinished(Evas_Object* webview)
  {
    EventLoopStop(Success);
  }

protected:
  static const char*const url;
};

const char*const utc_blink_ewk_view_screenshot_contents_get::url="http://www.google.com";

/**
 * @brief Checking whether the evas image object of the specified viewArea of page is returned.
 */

TEST_F(utc_blink_ewk_view_screenshot_contents_get, POS_TEST)
{
  Eina_Bool url_result = ewk_view_url_set(GetEwkWebView(), url);
  Eina_Bool result = EINA_FALSE;

  if (!url_result)
    utc_fail();

  if (Success!=EventLoopStart())
    utc_fail();

  Eina_Rectangle snapshot_rect;
  snapshot_rect.x = snapshot_rect.y = 0;
  evas_object_geometry_get(GetEwkWebView(), NULL, NULL, &snapshot_rect.w, &snapshot_rect.h);

  Evas_Object *webview = ewk_view_screenshot_contents_get(GetEwkWebView(), snapshot_rect, 1.0f, evas_object_evas_get(GetEwkWebView()));

  if (webview)
    result = EINA_TRUE;

  utc_check_eq(result, EINA_TRUE);
}

/**
 * @brief Checking whether function works properly in case that webview and canvas object are NULL.
 */
TEST_F(utc_blink_ewk_view_screenshot_contents_get, NEG_TEST)
{
  Eina_Rectangle snapshot_rect;
  snapshot_rect.x = snapshot_rect.y = 0;
  evas_object_geometry_get(GetEwkWebView(), NULL, NULL, &snapshot_rect.w, &snapshot_rect.h);

  utc_check_eq(NULL, ewk_view_screenshot_contents_get(NULL, snapshot_rect, 1.0f, NULL));
}
