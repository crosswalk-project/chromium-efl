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

#define URL  ("http://google.com")

class utc_blink_cb_load_finished : public utc_blink_ewk_base
{
public:
    /* Callback for "load,finished" */
    void LoadFinished(Evas_Object* webview)
    {
        EventLoopStop(utc_blink_ewk_base::Success);
    }
};

/**
 * @brief Tests "load,finished" callback with ewl_view_html_string_load
 */
TEST_F(utc_blink_cb_load_finished, html_string_load)
{
    char htmlBuffer[] = "<html>"
                          "<head><title>TC ewk_view_html_load_string</title></head>"
                          "<body><p>TC ewk_view_html_load_string</p></body>"
                        "</html>";

    Eina_Bool result = ewk_view_html_string_load(GetEwkWebView(), htmlBuffer, NULL, NULL);

    if (!result)
        utc_fail();

    utc_check_eq(EventLoopStart(), utc_blink_ewk_base::Success);
}

/**
 * @brief Tests "load,finished" callback with ewk_view_url_set http protocol
 */
TEST_F(utc_blink_cb_load_finished, url_set_http)
{
    utc_message("Loading url: %s", URL);
    Eina_Bool result = ewk_view_url_set(GetEwkWebView(), URL);

    if (!result)
        utc_fail();

    utc_check_eq(EventLoopStart(), utc_blink_ewk_base::Success);
}

/**
 * @brief Tests "load,finished" callback with ewk_view_urls_set local file
 */
TEST_F(utc_blink_cb_load_finished, url_set_file)
{
    std::string resource_url = GetResourceUrl("common/sample.html");
    utc_message("Loading file: %s", resource_url.c_str());
    Eina_Bool result = ewk_view_url_set(GetEwkWebView(), resource_url.c_str());

    if (!result)
        utc_fail();

    utc_check_eq(EventLoopStart(), utc_blink_ewk_base::Success);
}
