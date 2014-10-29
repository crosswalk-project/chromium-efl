// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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

class utc_blink_cb_editorclient_candidate_closed : public utc_blink_ewk_base
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

  static void editorclient_candidate_cb(utc_blink_cb_editorclient_candidate_closed* owner, Evas_Object*, void*)
  {
    ASSERT_TRUE(owner);
    owner->EventLoopStop(utc_blink_ewk_base::Success);
  }
};

/**
 * @brief Tests "editorclient,candidate,closed" callback
 *
 * We use Tizen ISF API to open candidate window on current input method panel and after it was shown
 * we close it.
 */
TEST_F(utc_blink_cb_editorclient_candidate_closed, callback)
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
    evas_object_smart_callback_auto ime(GetEwkWebView(), "editorclient,ime,opened", ToSmartCallback(editorclient_candidate_cb), this);
    // This script execute should produce user gesture and should show software keyboard on mobile
    ASSERT_EQ(EINA_TRUE, ewk_view_script_execute(GetEwkWebView(), "document.getElementById('b').click();", NULL, NULL));
    // Wait for keyboard
    ASSERT_EQ(utc_blink_ewk_base::Success, EventLoopStart()) << "editorclient,ime,opened smart callback timeout";
  }

#ifdef OS_TIZEN
  scim::HelperAgent agent;

  {
    Display* display = static_cast<Display*>(ecore_x_display_get());
    ASSERT_TRUE(display) << "No X11 display available!";

    evas_object_smart_callback_auto candidate(GetEwkWebView(), "editorclient,candidate,opened", ToSmartCallback(editorclient_candidate_cb), this);

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

    // wait for candidate to show
    ASSERT_EQ(utc_blink_ewk_base::Success, EventLoopStart()) << "editorclient,candidate,opened smart callback timeout";
  }

  {
    evas_object_smart_callback_auto candidate(GetEwkWebView(), "editorclient,candidate,closed", ToSmartCallback(editorclient_candidate_cb), this);
    agent.hide_candidate_string();
    ASSERT_EQ(utc_blink_ewk_base::Success, EventLoopStart()) << "editorclient,candidate,closed smart callback timeout";
  }


  agent.close_connection();
#else // OS_TIZEN
  ASSERT_TRUE(false) << "This TC won't work outside Tizen OS";
#endif // OS_TIZEN
}
