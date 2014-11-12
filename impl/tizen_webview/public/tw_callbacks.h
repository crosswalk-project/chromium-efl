// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TIZEN_WEBVIEW_PUBLIC_TW_CALLBACKS_H_
#define TIZEN_WEBVIEW_PUBLIC_TW_CALLBACKS_H_

#include <Evas.h>

namespace tizen_webview {
class Hit_Test;
class Security_Origin;

// from ewk_context.h
typedef void (*Local_File_System_Origins_Get_Callback)(Eina_List *origins, void *user_data);
typedef void (*Web_Application_Cache_Origins_Get_Callback)(Eina_List* origins, void* user_data);
typedef void (*Web_Application_Cache_Usage_For_Origin_Get_Callback)(int64_t usage, void* user_data);
typedef void (*Web_Database_Origins_Get_Callback)(Eina_List* origins, void* user_data);
typedef void (*Web_Storage_Origins_Get_Callback)(Eina_List* origins, void* user_data);
typedef void (*Context_Did_Start_Download_Callback)(const char* download_url, void* user_data);
typedef void (*Vibration_Client_Vibrate_Cb)(uint64_t vibration_time, void *user_data);
typedef void (*Vibration_Client_Vibration_Cancel_Cb)(void *user_data);
typedef Eina_Bool (*Mime_Override_Callback)(const char *url, const char *current_mime, char **new_mime);

/**
 * Callback for ewk_view_web_app_capable_get
 *
 * @param capable web application capable
 * @param user_data user_data will be passsed when ewk_view_web_app_capable_get is called
 */
typedef void (*Web_App_Capable_Get_Callback)(Eina_Bool capable, void* user_data);

/**
 * Callback for ewk_view_web_app_icon_get
 *
 * @param icon_url web application icon
 * @param user_data user_data will be passsed when ewk_view_web_app_icon_get is called
 */
typedef void (*Web_App_Icon_URL_Get_Callback)(const char* icon_url, void* user_data);

/**
 * Callback for ewk_view_screenshot_contents_get_async
 *
 * @param image captured screenshot
 * @param user_data user_data will be passsed when ewk_view_screenshot_contents_get_async is called
 */
typedef void (*Web_App_Screenshot_Captured_Callback)(Evas_Object* image, void* user_data);

/**
 * Callback for ewk_view_web_app_icon_urls_get.
 *
 * @param icon_urls list of Ewk_Web_App_Icon_Data for web app
 * @param user_data user_data will be passsed when ewk_view_web_app_icon_urls_get is called
 */
typedef void (*Web_App_Icon_URLs_Get_Callback)(Eina_List *icon_urls, void *user_data);

/**
 * Callback for ewk_view_script_execute
 *
 * @param o the view object
 * @param result_value value returned by script
 * @param user_data user data
 */
typedef void (*View_Script_Execute_Callback)(Evas_Object* o, const char* result_value, void* user_data);

/**
 * Callback for ewk_view_plain_text_get
 *
 * @param o the view object
 * @param plain_text the contents of the given frame converted to plain text
 * @param user_data user data
 */
typedef void (*View_Plain_Text_Get_Callback)(Evas_Object* o, const char* plain_text, void* user_data);

/**
 * Creates a type name for the callback function used to get the page contents.
 *
 * @param o view object
 * @param data mhtml data of the page contents
 * @param user_data user data will be passed when ewk_view_mhtml_data_get is called
 */
typedef void (*View_MHTML_Data_Get_Callback)(Evas_Object *o, const char *data, void *user_data);


//typedef Eina_Bool (*View_Password_Confirm_Popup_Callback)(Evas_Object* o, const char* message, void* user_data);
typedef Eina_Bool (*View_JavaScript_Alert_Callback)(Evas_Object* o, const char* alert_text, void* user_data);
typedef Eina_Bool (*View_JavaScript_Confirm_Callback)(Evas_Object* o, const char* message, void* user_data);
/**
 * Callback for ewk_view_javascript_prompt_callback_set
 *
 * @param o the view object
 * @param message the text to be displayed on the prompt popup
 * @param default_value default text to be entered in the prompt dialog
 * @param user_data user data
 */
typedef Eina_Bool (*View_JavaScript_Prompt_Callback)(Evas_Object* o, const char* message, const char* default_value, void* user_data);
//typedef Eina_Bool (*View_Before_Unload_Confirm_Panel_Callback)(Evas_Object* o, const char* message, void* user_data);
//typedef Eina_Bool (*View_Applicacion_Cache_Permission_Callback)(Evas_Object* o, Security_Origin* origin,  void* user_data);
//typedef Eina_Bool (*View_Exceeded_Indexed_Database_Quota_Callback)(Evas_Object* o, Security_Origin* origin, long long currentQuota, void* user_data);
//typedef Eina_Bool (*View_Exceeded_Database_Quota_Callback)(Evas_Object* o, Security_Origin* origin, const char* database_name, unsigned long long expectedQuota, void* user_data);
//typedef Eina_Bool (*View_Exceeded_Local_File_System_Quota_Callback)(Evas_Object* o, Security_Origin* origin, long long currentQuota, void* user_data);
typedef Eina_Bool (*Orientation_Lock_Cb)(Evas_Object* o, Eina_Bool need_lock, int orientation, void* user_data);
typedef void (*View_Hit_Test_Request_Callback)(Evas_Object* o, int x, int y, int hit_test_mode, Hit_Test* hit_test, void* user_data);
typedef Eina_Bool (*View_Geolocation_Permission_Callback)(Evas_Object* view, void* geolocation_permission_context, void* user_data);
} // namespace tizen_webview

#endif  // TIZEN_WEBVIEW_PUBLIC_TW_CALLBACKS_H_
