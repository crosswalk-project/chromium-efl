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

#ifdef OS_TIZEN
#include <Ecore_X.h>
// using scim to simulate keyboard typing
#include <scim.h>
#include <scim_event.h>
#include <scim_attribute.h>
#include <scim_lookup_table.h>
#include <scim_socket.h>
#include <scim_property.h>
#include <scim_transaction.h>
#include <scim_helper.h>
#include <scim_helper_manager.h>
#endif

#include "utc_blink_ewk_base.h"

#include <X11/Xlib.h>
#undef Success // conflicts with utc_blink_ewk_base enum

class utc_blink_cb_editorclient_candidate_opened : public utc_blink_ewk_base
{
protected:
  void PreSetUp()
  {
    const char* default_context_id = ecore_imf_context_default_id_get();
    ASSERT_TRUE(default_context_id) << "This TC can't be run without default imf context - it will fail anyway";
  }

  void LoadFinished(Evas_Object*)
  {
    EventLoopStop(utc_blink_ewk_base::Success);
  }

  static void editorclient_candidate_opened_cb(utc_blink_cb_editorclient_candidate_opened* owner, Evas_Object*, void*)
  {
    ASSERT_TRUE(owner);
    owner->EventLoopStop(utc_blink_ewk_base::Success);
  }
};

/**
 * @brief Tests "editorclient,candidate,opened" callback
 *
 * We use Tizen ISF API to open candidate window on current input method panel
 */
TEST_F(utc_blink_cb_editorclient_candidate_opened, callback)
{
  char htmlBuffer[] = "<html>"
                        "<head></head>"
                        "<body>"
                          "<input id=\"e\" type=\"text\"><a id=\"b\" onclick=\"document.getElementById('e').focus();\">Button</a>"
                        "</body>"
                      "</html>";


  ASSERT_EQ(EINA_TRUE, ewk_view_html_string_load(GetEwkWebView(), htmlBuffer, NULL, NULL));
  // This TC will timeout on desktop as there is no input method context
  ASSERT_EQ(utc_blink_ewk_base::Success, EventLoopStart());

  // We need to focus on window
  elm_object_focus_set(GetEwkWebView(), EINA_TRUE);

  // We scope this callback as it is only needed once
  {
    evas_object_smart_callback_auto ime(GetEwkWebView(), "editorclient,ime,opened", ToSmartCallback(editorclient_candidate_opened_cb), this);
    // This script execute should produce user gesture and should show software keyboard on mobile
    ASSERT_EQ(EINA_TRUE, ewk_view_script_execute(GetEwkWebView(), "document.getElementById('b').click();", NULL, NULL));
    // Wait for keyboard
    ASSERT_EQ(utc_blink_ewk_base::Success, EventLoopStart()) << "editorclient,ime,opened smart callback timeout";
  }

#ifdef OS_TIZEN
  {
    Display* display = static_cast<Display*>(ecore_x_display_get());
    ASSERT_TRUE(display) << "No X11 display available!";

    evas_object_smart_callback_auto candidate(GetEwkWebView(), "editorclient,candidate,opened", ToSmartCallback(editorclient_candidate_opened_cb), this);

    scim::HelperAgent agent;
    scim::HelperManager manager;

    int count = manager.number_of_helpers();

    scim::HelperInfo info;

    for (int i = 0; i < count; ++i) {
      if (manager.get_helper_info(i, info)) {
        agent.open_connection(info, XDisplayString(display));
      }
    }

    ASSERT_TRUE(agent.is_connected()) << "Could not connect to SCIM - can't send proper events to virtual keyboard";

    scim::WideString content = scim::utf8_mbstowcs("a");
    agent.commit_string(-1, scim::String(), content);
    agent.show_candidate_string();

    agent.close_connection();

    // wait for candidate to show
    ASSERT_EQ(utc_blink_ewk_base::Success, EventLoopStart()) << "editorclient,candidate,opened smart callback timeout";
  }
#else // OS_TIZEN
    ASSERT_TRUE(false) << "This TC won't work outside Tizen OS";
#endif // OS_TIZEN
}
