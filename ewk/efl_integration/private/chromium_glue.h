// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROMIUM_GLUE_H_
#define CHROMIUM_GLUE_H_

#ifndef __cplusplus
#error "Don't mix C and C++ in using chromium_glue.h"
#endif

#include "ewk_context.h"
#include "ewk_cookie_manager.h"
#include "ewk_settings.h"
#include "ewk_hit_test.h"
#include "ewk_policy_decision.h"
#include "ewk_touch.h"
#include "ewk_custom_handlers.h"
#include "ewk_view.h"

#include <tizen_webview/public/tw_cache_model.h>
#include <tizen_webview/public/tw_callbacks.h>
#include <tizen_webview/public/tw_content_security_policy.h>
#include <tizen_webview/public/tw_cookie_accept_policy.h>
#include <tizen_webview/public/tw_legacy_font_size_mode.h>
#include <tizen_webview/public/tw_hit_test.h>
#include <tizen_webview/public/tw_policy_decision.h>
#include <tizen_webview/public/tw_touch_event.h>
#include <tizen_webview/public/tw_custom_handlers.h>
#include <tizen_webview/public/tw_find_options.h>

namespace chromium_glue {

namespace tw = tizen_webview;

// --- ewk<->chromium type conversion ---
tw::Cache_Model to  (Ewk_Cache_Model);
Ewk_Cache_Model from(tw::Cache_Model);

tw::Cookie_Accept_Policy to  (Ewk_Cookie_Accept_Policy);
Ewk_Cookie_Accept_Policy from(tw::Cookie_Accept_Policy);

tw::Legacy_Font_Size_Mode to(Ewk_Legacy_Font_Size_Mode);
Ewk_Legacy_Font_Size_Mode from(tw::Legacy_Font_Size_Mode);


tw::Hit_Test_Mode to(Ewk_Hit_Test_Mode);
Ewk_Hit_Test_Mode from(tw::Hit_Test_Mode);

tw::Hit_Test_Result_Context to(Ewk_Hit_Test_Result_Context);
Ewk_Hit_Test_Result_Context from(tw::Hit_Test_Result_Context);

tw::View_Hit_Test_Request_Callback to(Ewk_View_Hit_Test_Request_Callback);

tw::Policy_Decision_Type to(Ewk_Policy_Decision_Type);
Ewk_Policy_Decision_Type from(tw::Policy_Decision_Type);

tw::Policy_Navigation_Type to(Ewk_Policy_Navigation_Type);
Ewk_Policy_Navigation_Type from(tw::Policy_Navigation_Type);

tw::Touch_Event_Type to(Ewk_Touch_Event_Type);
Ewk_Touch_Event_Type from(tw::Touch_Event_Type);

tw::Custom_Handlers_State to(Ewk_Custom_Handlers_State);
Ewk_Custom_Handlers_State from(tw::Custom_Handlers_State);

tw::Find_Options to(Ewk_Find_Options);
Ewk_Find_Options from(tw::Find_Options);

tw::ContentSecurityPolicyType to(Ewk_CSP_Header_Type);
Ewk_CSP_Header_Type from(tw::ContentSecurityPolicyType);

} // namespace chromium_glue

#endif  // CHROMIUM_GLUE_H_
