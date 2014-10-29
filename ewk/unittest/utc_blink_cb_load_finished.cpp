// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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
