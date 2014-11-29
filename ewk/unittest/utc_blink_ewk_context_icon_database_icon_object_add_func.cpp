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

class utc_blink_ewk_context_icon_database_icon_object_add : public utc_blink_ewk_base
{
protected:
  void PostSetUp()
  {
    evas_object_smart_callback_add(GetEwkWebView(), "icon,received", cb_icon_received, this);
  }

  void PreTearDown()
  {
    evas_object_smart_callback_del(GetEwkWebView(), "icon,received", cb_icon_received);
  }

  static void cb_icon_received(void *data, Evas_Object *, void *)
  {
    ASSERT_TRUE(data != NULL);
    utc_blink_ewk_context_icon_database_icon_object_add *owner = static_cast<utc_blink_ewk_context_icon_database_icon_object_add *>(data);
    Evas_Object *favicon = ewk_context_icon_database_icon_object_add(ewk_context_default_get(), owner->GetResourceUrl(PAGE_PATH).c_str(), owner->GetEwkEvas());
    ASSERT_TRUE(favicon != NULL);

    Evas_Object *img = evas_object_image_filled_add(owner->GetEwkEvas());
    evas_object_image_file_set(img, owner->GetResourcePath(ICON_PATH).c_str(), NULL);

    int w = 0, h = 0;
    evas_object_image_size_get(favicon, &w, &h);

    ASSERT_EQ(0, memcmp(evas_object_image_data_get(favicon, EINA_FALSE),
                        evas_object_image_data_get(img, EINA_FALSE),
                        w * h * PIXEL_SIZE));
    owner->EventLoopStop(Success);
  }

  static const int PIXEL_SIZE = 4;
  static const char *PAGE_PATH;
  static const char *ICON_PATH;
};

const char *utc_blink_ewk_context_icon_database_icon_object_add::PAGE_PATH = "ewk_context_icon_database/favicon2.html";
const char *utc_blink_ewk_context_icon_database_icon_object_add::ICON_PATH = "ewk_context_icon_database/tizen-icon.bmp";

TEST_F(utc_blink_ewk_context_icon_database_icon_object_add, FAVICON_IMAGE_TEST)
{
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), GetResourceUrl(PAGE_PATH).c_str()));
  ASSERT_EQ(Success, EventLoopStart());
}
