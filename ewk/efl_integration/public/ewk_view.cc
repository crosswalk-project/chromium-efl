/*
 * Copyright (C) 2009-2010 ProFUSION embedded systems
 * Copyright (C) 2009-2014 Samsung Electronics. All rights reserved.
 * Copyright (C) 2012 Intel Corporation
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 *
 */

#include "ewk_view.h"

#include <Evas.h>
#include <tizen_webview/public/tw_webview.h>

// TODO: remove dependency
#include <ui/events/gesture_detection/gesture_configuration.h>
#if !defined(EWK_BRINGUP)
#include <webkit/common/webpreferences.h>
#else
#include <content/public/common/web_preferences.h>
#endif
#include <../impl/eweb_view.h>
#include <../impl/web_contents_delegate_efl.h>
#include <../impl/cookie_manager.h>

#include "public/ewk_back_forward_list.h"
#include "public/ewk_context.h"
#include "public/ewk_enums.h"
#include "public/ewk_settings.h"
#include "private/ewk_context_private.h"
#include "private/ewk_hit_test_private.h"
#include "private/ewk_notification_private.h"
#include "private/ewk_private.h"
#if !defined(EWK_BRINGUP)
#include "private/ewk_quota_permission_request_private.h"
#endif
#include "private/chromium_glue.h"
#include "private/ewk_back_forward_list_private.h"
#include "private/ewk_history_private.h"
#include "private/ewk_view_private.h"

using tizen_webview::WebView;
using tizen_webview::WebContext;

Eina_Bool ewk_view_smart_class_set(Ewk_View_Smart_Class* api)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(api, false);
  return InitSmartClassInterface(*api);
}

// TODO: Evas_Object *ewk_view_smart_add(Evas *e, Evas_Smart *smart, Ewk_Context *context, Ewk_Page_Group *pageGroup)

Evas_Object* ewk_view_add_with_session_data(Evas* canvas, const char* data, unsigned length)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(canvas, NULL);

  if (!length)
    return NULL;

  EINA_SAFETY_ON_NULL_RETURN_VAL(data, NULL);

  Evas_Object* ret = ewk_view_add(canvas);

  WebView *webView = GetWebViewFromEvasObject(ret);

  if (!webView)
    return ret;

  if (webView->RestoreFromSessionData(data, length))
    return ret;

  evas_object_del(ret);

  return NULL;
}

Evas_Object* ewk_view_add_with_context(Evas* e, Ewk_Context* context)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(context, 0);
  return CreateWebViewAsEvasObject(context, e);
}

Evas_Object* ewk_view_add(Evas* e)
{
  // TODO: shouldn't this function create new EWebContext for each new EWebView?
  // when using default context like that it makes unclear who should release
  // default web context. It won't be released by destroyed eweb_view because
  // ewk_context_default_get does AddRef
  Ewk_Context* context = ewk_context_default_get();
  return ewk_view_add_with_context(e, context);
}

Ewk_Context *ewk_view_context_get(const Evas_Object *view)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(view, impl, 0);
  return static_cast<Ewk_Context*>(impl->GetWebContext());
}

Eina_Bool ewk_view_url_set(Evas_Object* view, const char* url)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(view, impl, false);
  EINA_SAFETY_ON_NULL_RETURN_VAL(url, false);
  impl->SetURL(url);
  return true;
}

const char* ewk_view_url_get(const Evas_Object* view)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(view, impl, 0);
  return impl->GetURL();
}


Eina_Bool ewk_view_reload(Evas_Object *view)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(view, impl, false);
  impl->Reload();
  return true;
}


Eina_Bool ewk_view_reload_bypass_cache(Evas_Object *view)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(view, impl, false);
  impl->ReloadIgnoringCache();
  return true;
}

Eina_Bool ewk_view_stop(Evas_Object* view)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(view, impl, false);
  impl->Stop();
  return true;
}


Ewk_Settings *ewk_view_settings_get(const Evas_Object *ewkView)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(ewkView, impl,0);
  return impl->GetSettings();
}

const char* ewk_view_title_get(const Evas_Object* view)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(view, impl, 0);
  return impl->GetTitle();
}

double ewk_view_load_progress_get(const Evas_Object* view)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(view, impl, -1);
  return impl->GetProgressValue();
}

Eina_Bool ewk_view_scale_set(const Evas_Object* view, double scale_factor, int x, int y)
{
  // TODO: We should implement AC in order to work this API normally.
  EWK_VIEW_IMPL_GET_OR_RETURN(view, impl, EINA_FALSE);
  impl->SetScale(scale_factor, x, y);
  return EINA_TRUE;
}

double ewk_view_scale_get(const Evas_Object *view)
{
  // TODO: We should implement AC in order to work this API normally.
  EWK_VIEW_IMPL_GET_OR_RETURN(view, impl, -1);
  return impl->GetScale();
}

Eina_Bool ewk_view_back(Evas_Object *view)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(view, impl, false);
  return impl->GoBack();
}

Eina_Bool ewk_view_forward(Evas_Object *view)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(view, impl, false);
  return impl->GoForward();
}

Eina_Bool ewk_view_back_possible(Evas_Object *view)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(view, impl, false);
  return impl->CanGoBack();
}

Eina_Bool ewk_view_forward_possible(Evas_Object *view)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(view, impl, false);
  return impl->CanGoForward();
}

Eina_Bool ewk_view_web_login_request(Evas_Object* ewkView)
{
  LOG_EWK_API_MOCKUP();
  return false;
}

Eina_Bool ewk_view_html_string_load(Evas_Object* view, const char* html, const char* base_uri, const char* unreachable_uri)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(view, impl, EINA_FALSE);
  EINA_SAFETY_ON_NULL_RETURN_VAL(html, EINA_FALSE);
  impl->LoadHTMLString(html, base_uri, unreachable_uri);
  return EINA_TRUE;
}

Eina_Bool ewk_view_mouse_events_enabled_set(Evas_Object *view, Eina_Bool enabled)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(view, impl, false);

  impl->SetMouseEventsEnabled(!!enabled);

  return true;
}

Eina_Bool ewk_view_mouse_events_enabled_get(const Evas_Object *view)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(view, impl, false);

  return impl->MouseEventsEnabled();
}

Eina_Bool ewk_view_color_picker_color_set(Evas_Object* ewkView, int r, int g, int b, int a)
{
  LOG_EWK_API_MOCKUP();
  return false;
}

double ewk_view_text_zoom_get(const Evas_Object* view)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(view, impl, -1.0);
  return impl->GetTextZoomFactor();
}

Eina_Bool ewk_view_text_zoom_set(Evas_Object* view, double text_zoom_level)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(view, impl, false);
  if (ewk_settings_text_zoom_enabled_get(ewk_view_settings_get(view))) {
   impl->SetTextZoomFactor(text_zoom_level);
   return true;
  }
  return false;
}

void ewk_view_not_found_error_page_load(Evas_Object* ewkView, const char* errorUrl)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(ewkView, impl);
  std::string errorhtml = impl->GetErrorPage(std::string(errorUrl));

   ewk_view_html_string_load(ewkView, errorhtml.c_str(), errorUrl, errorUrl);
}

void ewk_view_scale_range_get(Evas_Object* view, double* min_scale, double* max_scale)
{
  if (!min_scale && !max_scale)
    return;

  EWK_VIEW_IMPL_GET_OR_RETURN(view, impl);
  impl->GetPageScaleRange(min_scale, max_scale);
}

void ewk_view_suspend(Evas_Object* ewkView)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(ewkView, impl);
  impl->Suspend();
}

void ewk_view_resume(Evas_Object* ewkView)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(ewkView, impl);
  impl->Resume();
}

Eina_Bool ewk_view_url_request_set(Evas_Object* ewkView, const char* url, Ewk_Http_Method method, Eina_Hash* headers, const char* body)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(ewkView, impl, EINA_FALSE);
  EINA_SAFETY_ON_NULL_RETURN_VAL(url, EINA_FALSE);
  std::string str_method;
  switch (method) {
  case EWK_HTTP_METHOD_GET:
    str_method = "GET";
    break;
  case EWK_HTTP_METHOD_HEAD:
    str_method = "HEAD";
    break;
  case EWK_HTTP_METHOD_POST:
    str_method = "POST";
    break;
  case EWK_HTTP_METHOD_PUT:
    str_method = "PUT";
    break;
  case EWK_HTTP_METHOD_DELETE:
    str_method = "DELETE";
    break;
  default:
    return EINA_FALSE;
  }
  impl->UrlRequestSet(url, str_method, headers, body);
  return EINA_TRUE;
}

Eina_Bool ewk_view_plain_text_set(Evas_Object* view, const char* plain_text)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(view, impl, EINA_FALSE);
  EINA_SAFETY_ON_NULL_RETURN_VAL(plain_text, EINA_FALSE);
  impl->LoadPlainTextString(plain_text);
  return EINA_TRUE;
}

Eina_Bool ewk_view_contents_set(Evas_Object* view, const char* contents, size_t contents_size, char* mime_type, char* encoding, char* base_uri)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(view, impl, EINA_FALSE);
  EINA_SAFETY_ON_NULL_RETURN_VAL(contents, EINA_FALSE);
  impl->LoadData(contents, contents_size, mime_type, encoding, base_uri);
  return EINA_TRUE;
}

Eina_Bool ewk_view_html_contents_set(Evas_Object* view, const char* html, const char* base_uri)
{
  return ewk_view_html_string_load(view, html, base_uri, NULL);
}

Eina_Bool ewk_view_page_visibility_state_set(Evas_Object* ewkView, Ewk_Page_Visibility_State page_visibility_state, Eina_Bool initial_state)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(ewkView, impl,false);
  switch (page_visibility_state) {
    case EWK_PAGE_VISIBILITY_STATE_VISIBLE :
      impl->Show();
      break;
    case EWK_PAGE_VISIBILITY_STATE_HIDDEN :
      impl->Hide();
      break;
    default:
      break;
  }
  return true;
}

Eina_Bool ewk_view_user_agent_set(Evas_Object* ewkView, const char* user_agent)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(ewkView, impl, false);
  EINA_SAFETY_ON_NULL_RETURN_VAL(user_agent, false);
  if (0 != strcmp(user_agent, "")) //check for empty string
    return impl->SetUserAgent(user_agent);
  return false;
}

const char* ewk_view_user_agent_get(const Evas_Object* ewkView)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(ewkView, impl, 0);
  return impl->GetUserAgent();
}

Eina_Bool ewk_view_application_name_for_user_agent_set(Evas_Object* ewkView, const char* application_name)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(ewkView, impl, false);
  EINA_SAFETY_ON_NULL_RETURN_VAL(application_name, false);
  if (0 != strcmp(application_name, "")) //check for empty string
    return impl->SetUserAgentAppName(application_name);
  return false;
}

const char* ewk_view_application_name_for_user_agent_get(const Evas_Object* ewkView)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(ewkView, impl, 0);
  return impl->GetUserAgentAppName();
}

Eina_Bool ewk_view_custom_header_add(const Evas_Object* ewkView, const char* name, const char* value)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(ewkView, impl, EINA_FALSE);
  EINA_SAFETY_ON_NULL_RETURN_VAL(name, EINA_FALSE);
  EINA_SAFETY_ON_NULL_RETURN_VAL(value, EINA_FALSE);
  EINA_SAFETY_ON_NULL_RETURN_VAL(impl->GetWebContext(),EINA_FALSE);
  return impl->GetWebContext()->HTTPCustomHeaderAdd(name, value);
}

Eina_Bool ewk_view_custom_header_remove(const Evas_Object* ewkView, const char* name)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(ewkView, impl, EINA_FALSE);
  EINA_SAFETY_ON_NULL_RETURN_VAL(name, EINA_FALSE);
  EINA_SAFETY_ON_NULL_RETURN_VAL(impl->GetWebContext(),EINA_FALSE);
  return impl->GetWebContext()->HTTPCustomHeaderRemove(name);

}

Eina_Bool ewk_view_custom_header_clear(const Evas_Object* ewkView)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(ewkView, impl, EINA_FALSE);
  EINA_SAFETY_ON_NULL_RETURN_VAL(impl->GetWebContext(),EINA_FALSE);
  impl->GetWebContext()->HTTPCustomHeaderClear();
  return EINA_TRUE;
}


Eina_Bool ewk_view_visibility_set(const Evas_Object* view, Eina_Bool enable)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(view, impl, EINA_FALSE);

  if (enable)
    impl->Show();
  else
    impl->Hide();

  return EINA_TRUE;
}

Evas_Object* ewk_view_screenshot_contents_get(const Evas_Object* view, Eina_Rectangle view_area, float scale_factor, Evas* canvas)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(canvas, NULL);
  EWK_VIEW_IMPL_GET_OR_RETURN(view, impl, NULL);
  // Zoom level equality (<=0.001) is sufficient compared to high precision std::numeric_value::epsilon.
  if (!content::ZoomValuesEqual(scale_factor, 1.0)) {
    LOG(ERROR) << "We only support scale factor of 1.0."
               << "Scaling option will be supported after hardware acceleration is enabled.";
    return NULL;
  }
  return impl->GetSnapshot(view_area);
}

unsigned int ewk_view_inspector_server_start(Evas_Object* ewkView, unsigned int port)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(ewkView, impl, false);
  return impl->StartInspectorServer(port);
}

Eina_Bool ewk_view_inspector_server_stop(Evas_Object* ewkView)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(ewkView, impl, false);
  return impl->StopInspectorServer();
}

Evas_Object* ewk_view_cache_image_get(const Evas_Object* ewkView, const char* imageUrl, Evas* canvas)
{
  LOG_EWK_API_MOCKUP();
	return NULL;
}

void ewk_view_scroll_by(Evas_Object* ewkView, int deltaX, int deltaY)
{
  int x, y;

  if (EINA_TRUE == ewk_view_scroll_pos_get(ewkView, &x, &y)) {
    ewk_view_scroll_set(ewkView, x + deltaX, y + deltaY);
  }
}

Eina_Bool ewk_view_scroll_pos_get(Evas_Object* ewkView, int* x, int* y)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(ewkView, impl, false);
  EINA_SAFETY_ON_NULL_RETURN_VAL(x, EINA_FALSE);
  EINA_SAFETY_ON_NULL_RETURN_VAL(y, EINA_FALSE);
  impl->GetScrollPosition(x, y);
  return true;
}

Eina_Bool ewk_view_scroll_set(Evas_Object* view, int x, int y)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(view, impl, EINA_FALSE);
  impl->SetScroll(x, y);

  return EINA_TRUE;
}

Eina_Bool ewk_view_scroll_size_get(const Evas_Object* view, int* width, int* height)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(view, impl, EINA_FALSE);
  impl->GetScrollSize(width,height);
  return EINA_TRUE;
}

void ewk_view_password_confirm_popup_callback_set(Evas_Object* view, Ewk_View_Password_Confirm_Popup_Callback callback, void* user_data)
{
  LOG_EWK_API_MOCKUP();
}

void ewk_view_password_confirm_popup_reply(Evas_Object* ewkView, Ewk_Password_Popup_Option result)
{
  LOG_EWK_API_MOCKUP();
}

void ewk_view_javascript_alert_callback_set(Evas_Object* view, Ewk_View_JavaScript_Alert_Callback callback, void* user_data)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(view, impl);
  if (callback)
    impl->SetJavaScriptAlertCallback(callback, user_data);
}

void ewk_view_javascript_alert_reply(Evas_Object* view)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(view, impl);
  impl->JavaScriptAlertReply();
}

void ewk_view_javascript_confirm_callback_set(Evas_Object* view, Ewk_View_JavaScript_Confirm_Callback callback, void* user_data)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(view, impl);
  if (callback)
    impl->SetJavaScriptConfirmCallback(callback, user_data);
}

void ewk_view_javascript_confirm_reply(Evas_Object* view, Eina_Bool result)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(view, impl);
  impl->JavaScriptConfirmReply(result);
}

void ewk_view_javascript_prompt_callback_set(Evas_Object* view, Ewk_View_JavaScript_Prompt_Callback callback, void* user_data)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(view, impl);
  if (callback)
    impl->SetJavaScriptPromptCallback(callback, user_data);
}

void ewk_view_javascript_prompt_reply(Evas_Object* view, const char* result)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(view, impl);
  impl->JavaScriptPromptReply(result);
}

void ewk_view_before_unload_confirm_panel_callback_set(Evas_Object* ewkView, Ewk_View_Before_Unload_Confirm_Panel_Callback callback, void* userData)
{
  LOG_EWK_API_MOCKUP();
}

void ewk_view_before_unload_confirm_panel_reply(Evas_Object* ewkView, Eina_Bool result)
{
  LOG_EWK_API_MOCKUP();
}

Eina_Bool ewk_view_web_application_capable_get(Evas_Object* ewkView, Ewk_Web_App_Capable_Get_Callback callback, void* userData)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(ewkView, impl, EINA_FALSE);
  EINA_SAFETY_ON_NULL_RETURN_VAL(callback, EINA_FALSE);
  return impl->WebAppCapableGet(callback, userData) ? EINA_TRUE : EINA_FALSE;
}

Eina_Bool ewk_view_web_application_icon_url_get(Evas_Object* ewkView, Ewk_Web_App_Icon_URL_Get_Callback callback, void* userData)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(ewkView, impl, EINA_FALSE);
  EINA_SAFETY_ON_NULL_RETURN_VAL(callback, EINA_FALSE);
  return impl->WebAppIconUrlGet(callback, userData) ? EINA_TRUE : EINA_FALSE;
}

Eina_Bool ewk_view_web_application_icon_urls_get(Evas_Object* ewkView, Ewk_Web_App_Icon_URLs_Get_Callback callback, void* userData)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(ewkView, impl, EINA_FALSE);
  EINA_SAFETY_ON_NULL_RETURN_VAL(callback, EINA_FALSE);
  return impl->WebAppIconUrlsGet(callback, userData) ? EINA_TRUE : EINA_FALSE;
}

Eina_Bool ewk_view_command_execute(Evas_Object* view, const char* command, const char* value)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(view, impl, false);
  EINA_SAFETY_ON_NULL_RETURN_VAL(command, false);
  impl->ExecuteEditCommand(command, value);
  return true;
}

Eina_Bool ewk_view_contents_size_get(Evas_Object* view, Evas_Coord* width, Evas_Coord* height)
{
  if (width)
    *width = 0;
  if (height)
    *height = 0;

  EWK_VIEW_IMPL_GET_OR_RETURN(view, impl, EINA_FALSE);
  Eina_Rectangle contents_size = impl->GetContentsSize();

  if (width)
    *width = contents_size.w;
  if (height)
    *height = contents_size.h;

  return EINA_TRUE;
}

Eina_Bool ewk_view_contents_pdf_get(Evas_Object* view, int width, int height, const char* fileName)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(fileName, EINA_FALSE);
  EWK_VIEW_IMPL_GET_OR_RETURN(view, impl, EINA_FALSE);
  return impl->SaveAsPdf(width, height, fileName);
}

Eina_Bool ewk_view_script_execute(Evas_Object* ewkView, const char* script, Ewk_View_Script_Execute_Callback callback, void* user_data)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(ewkView, impl, false);
  EINA_SAFETY_ON_NULL_RETURN_VAL(script, false);
  // callback can be null, so do not test it for null
  if (0 != strcmp(script, "")) //check for empty string
    return impl->ExecuteJavaScript(script, callback, user_data);
  return false;
}


Eina_Bool ewk_view_plain_text_get(Evas_Object* view, Ewk_View_Plain_Text_Get_Callback callback, void* user_data)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(view, impl, EINA_FALSE);
  EINA_SAFETY_ON_NULL_RETURN_VAL(callback, EINA_FALSE);
  return (impl->PlainTextGet(callback, user_data));
}

Eina_Bool ewk_view_mhtml_data_get(Evas_Object *view, Ewk_View_MHTML_Data_Get_Callback callback, void *user_data)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(callback, EINA_FALSE);
  EWK_VIEW_IMPL_GET_OR_RETURN(view, impl, EINA_FALSE);
  return impl->GetMHTMLData(callback, user_data);
}

Ewk_Hit_Test* ewk_view_hit_test_new(Evas_Object* ewkView, int x, int y, int hit_test_mode)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(ewkView, impl, 0);
  return chromium_glue::from(impl->RequestHitTestDataAt(x, y,
      chromium_glue::to(static_cast<Ewk_Hit_Test_Mode>(hit_test_mode))));
}

Eina_Bool ewk_view_hit_test_request(Evas_Object* o, int x, int y, int hit_test_mode, Ewk_View_Hit_Test_Request_Callback callback, void* user_data)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(o, impl, EINA_FALSE);
  return impl->AsyncRequestHitTestDataAt(x, y, chromium_glue::to(static_cast<Ewk_Hit_Test_Mode>(hit_test_mode)), chromium_glue::to(callback), user_data);
}

Ewk_History* ewk_view_history_get(Evas_Object* ewkView)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(ewkView, impl, NULL);
  return chromium_glue::from(impl->GetBackForwardHistory());
}

Eina_Bool ewk_view_notification_closed(Evas_Object* ewkView, Eina_List* notification_list)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(ewkView, impl, false);

#if !defined(EWK_BRINGUP)
#warning "[M38] Fix and re-enable notifications (edit efl_integration/public/ewk_view.cc)"
  // copied from deleted NotificationControllerEfl::RemoveClosedNotifications()
  if (!eina_list_count(notification_list))
    return false;

  assert(impl->GetWebContext());
  assert(impl->GetWebContext()->browser_context());
  assert(impl->GetWebContext()->browser_context()->GetNotificationController());
  content::NotificationControllerEfl* controller =
      impl->GetWebContext()->browser_context()->GetNotificationController();

  Eina_List* notification_iterator = NULL;
  void* notification_data = NULL;
  EINA_LIST_FOREACH(notification_list, notification_iterator, notification_data) {
    Ewk_Notification* notification =
      static_cast<Ewk_Notification*>(notification_data);

    if (notification) {
      controller->DeleteNotification(notification->GetID(), true);
      delete notification;
    }
  }
#endif
  return true;

}

Eina_Bool ewk_view_popup_menu_select(Evas_Object* ewkView, unsigned int selectedIndex)
{
  LOG_EWK_API_MOCKUP();
  return false;
}

Eina_Bool ewk_view_popup_menu_multiple_select(Evas_Object* ewkView, Eina_Inarray* changeList)
{
  LOG_EWK_API_MOCKUP();
  return false;
}

/*
 * Sends the new orientation of the device.
 *
 * If orientation value is changed, orientationchanged event will occur.
 *
 * @param view object to receive orientation event.
 * @param orientation the new orientation of the device. (degree)
 *
 * orientation will be 0 degrees when the device is oriented to natural position,
 *                     90 degrees when it's left side is at the top,
 *                    -90 degrees when it's right side is at the top,
 *                     180 degrees when it is upside down.
 */
void ewk_view_orientation_send(Evas_Object* ewkView, int orientation)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(ewkView, impl);
  //send to webview
  impl->SendOrientationChangeEventIfNeeded(orientation);
}

void ewk_view_encoding_custom_set(Evas_Object* ewkView, const char* encoding)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(ewkView, impl);
  impl->SetOverrideEncoding(std::string(encoding));
}

Eina_Bool ewk_view_text_selection_range_get(const Evas_Object* view, Eina_Rectangle* left_rect, Eina_Rectangle* right_rect)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(view, impl, EINA_FALSE);
  if (left_rect && right_rect && impl->GetSelectionRange(left_rect, right_rect)) {
    Evas_Coord x, y;
    evas_object_geometry_get(view, &x, &y, 0, 0);
    left_rect->x += x;
    left_rect->y += y;
    right_rect->x += x;
    right_rect->y += y;
    return true;
  }
  return false;
}

Eina_Bool ewk_view_text_selection_clear(Evas_Object *view)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(view, impl, EINA_FALSE);
  return impl->ClearSelection() ? EINA_TRUE : EINA_FALSE;
}

const char* ewk_view_text_selection_text_get(Evas_Object* view)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(view, impl, NULL);
  return impl->GetSelectedText();
}

void ewk_view_focused_input_element_value_set(Evas_Object* ewkView, const char* value)
{
  LOG_EWK_API_MOCKUP();
}

const char* ewk_view_focused_input_element_value_get(Evas_Object* ewkView)
{
  LOG_EWK_API_MOCKUP();
  return NULL;
}

Eina_Bool ewk_view_vertical_panning_hold_get(Evas_Object* ewkView)
{
  LOG_EWK_API_MOCKUP();
  return false;
}

void ewk_view_vertical_panning_hold_set(Evas_Object* view, Eina_Bool hold)
{
  LOG_EWK_API_MOCKUP();
  EWK_VIEW_IMPL_GET_OR_RETURN(view, impl);
  // FIX ME: Currently the pan support is not present.
  // In WK2 panning was implemented with evas directly, here its with chorme gesture reconignation
  // So paning is not present yet
}

void ewk_view_orientation_lock_callback_set(Evas_Object* ewkView, Ewk_Orientation_Lock_Cb func, void* data)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(ewkView, impl);
  EINA_SAFETY_ON_NULL_RETURN(func);
  impl->SetOrientationLockCallback(func, data);
}


void ewk_view_back_forward_list_clear(const Evas_Object *view)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(view, impl);
  impl->BackForwardListClear();
}

Eina_Bool ewk_view_feed_touch_event(Evas_Object *view, Ewk_Touch_Event_Type type, const Eina_List *points, const Evas_Modifier *modifiers)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(view, impl, false);

  impl->HandleTouchEvents(chromium_glue::to(type), points, modifiers);

  return true;
}

Eina_Bool ewk_view_touch_events_enabled_set(Evas_Object *view, Eina_Bool enabled)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(view, impl, false);

  impl->SetTouchEventsEnabled(!!enabled);

  return true;
}

Eina_Bool ewk_view_touch_events_enabled_get(const Evas_Object *view)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(view, impl, false);

  return impl->TouchEventsEnabled();
}

Ewk_Frame_Ref ewk_view_main_frame_get(Evas_Object* o)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(o, impl, NULL);
  return impl->GetMainFrame();
}

Eina_Bool ewk_view_main_frame_scrollbar_visible_set(Evas_Object* ewkView, Eina_Bool visible)
{
  LOG_EWK_API_MOCKUP();
  return false;
}

void ewk_view_content_security_policy_set(Evas_Object* ewkView, const char* policy, Ewk_CSP_Header_Type type)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(ewkView, impl);
  EINA_SAFETY_ON_NULL_RETURN(policy);
  impl->SetContentSecurityPolicy(policy, chromium_glue::to(type));
}

void ewk_view_application_cache_permission_callback_set(Evas_Object* ewkView, Ewk_View_Applicacion_Cache_Permission_Callback callback, void* userData)
{
  // Chromium does not support this feature hence the comment
  LOG_EWK_API_MOCKUP("Not Supported by chromium");
}

void ewk_view_application_cache_permission_reply(Evas_Object* ewkView, Eina_Bool allow)
{
  // Chromium does not support this feature hence the comment
  LOG_EWK_API_MOCKUP("Not Supported by chromium");
}

void ewk_view_exceeded_indexed_database_quota_callback_set(Evas_Object* ewkView, Ewk_View_Exceeded_Indexed_Database_Quota_Callback callback, void* userData)
{
  // Chromium does not support quota for Indexed DB only.
  // IndexedDB uses temporary storage that is shared
  // between other features.
  LOG_EWK_API_MOCKUP("Not Supported by chromium");
}

void ewk_view_exceeded_indexed_database_quota_reply(Evas_Object* ewkView, Eina_Bool allow)
{
  // Chromium does not support quota for Indexed DB only.
  // IndexedDB uses temporary storage that is shared
  // between other features.
  LOG_EWK_API_MOCKUP("Not Supported by chromium");
}

Eina_Bool ewk_view_text_find(Evas_Object *view, const char *text, Ewk_Find_Options options, unsigned int max_match_count)
{
  // FIXME: We need to implement next options in Ewk_Find_Options struct. (refer to ewk_view.h)
  //         - EWK_FIND_OPTIONS_AT_WORD_STARTS
  //         - EWK_FIND_OPTIONS_TREAT_MEDIAL_CAPITAL_AS_WORD_START
  //         - EWK_FIND_OPTIONS_WRAP_AROUND
  //         - EWK_FIND_OPTIONS_SHOW_OVERLAY
  //         - EWK_FIND_OPTIONS_SHOW_FIND_INDICATOR
  //         - EWK_FIND_OPTIONS_SHOW_HIGHLIGHT (Currently there is no way to control this option. so it is always set)

  // FIXME: Updating of max_match_count is not implemented.

  EWK_VIEW_IMPL_GET_OR_RETURN(view, impl, EINA_FALSE);
  EINA_SAFETY_ON_NULL_RETURN_VAL(text, EINA_FALSE);
  impl->Find(text, chromium_glue::to(options));
  return EINA_TRUE;
}

Eina_Bool ewk_view_text_find_highlight_clear(Evas_Object *view)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(view, impl, EINA_FALSE);
  impl->StopFinding();
  return EINA_TRUE;
}

void ewk_view_exceeded_database_quota_callback_set(Evas_Object* ewkView, Ewk_View_Exceeded_Database_Quota_Callback callback, void* userData)
{
  // According to chromium source code:
  // src/third_party/WebKit/Source/modules/webdatabase/SQLTransactionClient.cpp line 67
  // Chromium does not allow users to manually change the quota for an origin (for now, at least).
  // This API is impossible to implement right now
  LOG_EWK_API_MOCKUP("Not Supported by chromium");
}

void ewk_view_exceeded_database_quota_reply(Evas_Object* ewkView, Eina_Bool allow)
{
  // According to chromium source code:
  // src/third_party/WebKit/Source/modules/webdatabase/SQLTransactionClient.cpp line 67
  // Chromium does not allow users to manually change the quota for an origin (for now, at least).
  // This API is impossible to implement right now
  LOG_EWK_API_MOCKUP("Not Supported by chromium");
}

void ewk_view_exceeded_local_file_system_quota_callback_set(Evas_Object* ewkView, Ewk_View_Exceeded_Indexed_Database_Quota_Callback callback, void* userData)
{
  LOG_EWK_API_MOCKUP();
}

void ewk_view_exceeded_local_file_system_quota_reply(Evas_Object* ewkView, Eina_Bool allow)
{
  LOG_EWK_API_MOCKUP();
}

void ewk_view_intercept_request_callback_set (Evas_Object* ewkView, Ewk_View_Intercept_Request_Callback callback, void* user_data)
{
  LOG_EWK_API_MOCKUP();
}

void ewk_view_unfocus_allow_callback_set(Evas_Object* ewkView, Ewk_View_Unfocus_Allow_Callback callback, void* user_data)
{
  LOG_EWK_API_MOCKUP();
}

void ewk_view_use_settings_font(Evas_Object* ewkView)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(ewkView, impl);
  impl->UseSettingsFont();
}

char* ewk_view_get_cookies_for_url(Evas_Object* view, const char* url)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(view, impl, 0);
  EINA_SAFETY_ON_NULL_RETURN_VAL(url, 0);
  std::string cookiesForURL;
  cookiesForURL = impl->GetWebContext()->cookieManager()->GetCookiesForURL(std::string(url));
  if (cookiesForURL.empty())
    return NULL;
  return strndup(cookiesForURL.c_str(), cookiesForURL.length());
}

Eina_Bool ewk_view_fullscreen_exit(Evas_Object* view)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(view, impl, EINA_FALSE);
  impl->ExitFullscreen();
  return EINA_TRUE;
}

Eina_Bool ewk_view_draws_transparent_background_set(Evas_Object *view, Eina_Bool enabled)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(view, impl, EINA_FALSE);
  impl->SetDrawsTransparentBackground(enabled);
  return EINA_TRUE;
}

void ewk_view_browser_font_set(Evas_Object* ewkView)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(ewkView, impl);
  impl->SetBrowserFont();
}

void ewk_view_session_data_get(Evas_Object* ewkView, const char** data, unsigned* length)
{
  EINA_SAFETY_ON_NULL_RETURN(data);
  EINA_SAFETY_ON_NULL_RETURN(length);

  WebView* impl = GetWebViewFromEvasObject(ewkView);
  if (!impl) {
    *data = NULL;
    *length = 0;
    return;
  }

  impl->GetSessionData(data, length);
}

Eina_Bool ewk_view_mode_set(Evas_Object* ewkView, Ewk_View_Mode view_mode)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(ewkView, impl, EINA_FALSE);

  if (view_mode == EWK_VIEW_MODE_WINDOWED || view_mode == EWK_VIEW_MODE_FULLSCREEN) {
    tizen_webview::View_Mode tizen_webview_view_mode;
    tizen_webview_view_mode = chromium_glue::to(view_mode);
    impl->SetViewMode(tizen_webview_view_mode);
    return EINA_TRUE;
  } else {
    return EINA_FALSE;
  }
}

Eina_Bool ewk_view_split_scroll_overflow_enabled_set(Evas_Object* ewkView, const Eina_Bool enabled)
{
  LOG_EWK_API_MOCKUP("for browser");
  return false;
}

Ewk_Back_Forward_List* ewk_view_back_forward_list_get(const Evas_Object* ewkView)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(ewkView, impl, NULL);
  return chromium_glue::from(impl->GetBackForwardList());
}

void ewk_view_draw_focus_ring_enable_set(Evas_Object* ewkView, Eina_Bool enable)
{
#if defined(OS_TIZEN_TV) && !defined(EWK_BRINGUP)
  EWK_VIEW_IMPL_GET_OR_RETURN(ewkView, impl);
  impl->setDrawFocusRing(enable);
#else
   LOG_EWK_API_MOCKUP("Only for Tizen TV Browser");
#endif
}

#if defined(OS_TIZEN_TV)
double ewk_view_page_zoom_get(const Evas_Object* ewkView)
{
  LOG_EWK_API_MOCKUP("for Tizen TV Browser");
  return 0;
}

Eina_Bool ewk_view_page_zoom_set(Evas_Object* ewkView, double zoomFactor)
{
  LOG_EWK_API_MOCKUP("for Tizen TV Browser");
  return false;
}

Evas_Object* ewk_view_smart_add(Evas* canvas, Evas_Smart* smart, Ewk_Context* context, Ewk_Page_Group* pageGroup)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(context, 0);
  return CreateWebViewAsEvasObject(context, canvas, smart);
}
#endif // OS_TIZEN_TV

#if !defined(EWK_BRINGUP)
void ewk_view_quota_permission_request_callback_set(Evas_Object* ewkView, Ewk_Quota_Permission_Request_Callback callback, void* user_data)
{
  EWK_VIEW_IMPL_GET_OR_RETURN(ewkView, impl);
  impl->SetQuotaPermissionRequestCallback((tizen_webview::QuotaPermissionRequest_Cb)callback, user_data);
}

void ewk_view_quota_permission_request_reply(const Ewk_Quota_Permission_Request* request, const Eina_Bool allow)
{
  EINA_SAFETY_ON_NULL_RETURN(request);
  Evas_Object* ewkView = request->getView();
  EWK_VIEW_IMPL_GET_OR_RETURN(ewkView, impl);
  impl->QuotaRequestReply(request, allow == EINA_TRUE);
}

void ewk_view_quota_permission_request_cancel(const Ewk_Quota_Permission_Request* request)
{
  EINA_SAFETY_ON_NULL_RETURN(request);
  Evas_Object* ewkView = request->getView();
  EWK_VIEW_IMPL_GET_OR_RETURN(ewkView, impl);
  impl->QuotaRequestCancel(request);
}
#endif
