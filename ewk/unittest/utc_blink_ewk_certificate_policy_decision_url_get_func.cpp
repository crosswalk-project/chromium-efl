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

/* Define those macros _before_ you include the utc_blink_ewk.h header file. */

#include "utc_blink_ewk_base.h"

#define URL "https://www.pcwebshop.co.uk"

class utc_blink_ewk_certificate_url_get  : public utc_blink_ewk_base
{

 protected:

    void PostSetUp()
    {
        evas_object_smart_callback_add(GetEwkWebView(), "request,certificate,confirm", policy_decision, this);
    }

    void PreTearDown()
    {
        evas_object_smart_callback_del(GetEwkWebView(), "request,certificate,confirm", policy_decision);
    }

    void LoadFinished(Evas_Object* webview)
    {
        EventLoopStop(utc_blink_ewk_base::Failure); // will noop if EventLoopStop was alraedy called
    }

    static void policy_decision(void* data, Evas_Object* webview, void* event_info)
    {
        utc_message("[policy decision] :: \n");
        utc_blink_ewk_certificate_url_get *owner = static_cast<utc_blink_ewk_certificate_url_get*>(data);

        Ewk_Certificate_Policy_Decision* policy = (Ewk_Certificate_Policy_Decision*)event_info;

        if (policy && ewk_certificate_policy_decision_url_get(policy)) {
            owner->EventLoopStop(utc_blink_ewk_base::Success);
        }
    }
};

/**
* @brief Checking whether certification request confirmation works properly.
*/
TEST_F(utc_blink_ewk_certificate_url_get, POS_TEST)
{
    Eina_Bool result = ewk_view_url_set(GetEwkWebView(), URL);

    if (!result)
        FAIL();

    utc_blink_ewk_base::MainLoopResult main_result = EventLoopStart();

    if (main_result != utc_blink_ewk_base::Success)
        FAIL();

    evas_object_show(GetEwkWebView());
    evas_object_show(GetEwkWindow());
}

/**
* @brief Checking whether function works properly in case of NULL of a webview.
*/
/*
//TODO: should be implemented properly
TEST_F(utc_blink_ewk_certificate_url_get, NEG_TEST)
{
  is_failed = EINA_FALSE;
  is_Accepted = EINA_FALSE;

  Eina_Bool result = ewk_view_url_set(NULL, URL);
  if (result)
    utc_fail();

  result = is_Accepted;

  evas_object_show(test_view.webview);
  evas_object_show(test_view.window);
  utc_check_ne(result, EINA_TRUE);
}
*/
