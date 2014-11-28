// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "public/ewk_auth_challenge.h"
#include "public/ewk_context.h"
#include "public/ewk_main.h"
#include "public/ewk_view.h"
#include "public/ewk_geolocation.h"
#include "public/ewk_security_origin.h"
#include "public/ewk_error.h"
#include "public/ewk_notification.h"
#include "public/ewk_policy_decision.h"
#include "public/ewk_context_menu.h"
#include "public/ewk_text_style.h"
#include "public/ewk_cookie_manager.h"
#include "public/ewk_console_message.h"

#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Ecore_File.h>
#include <Ecore_Getopt.h>
#include <Ecore_X.h>
#include <Evas.h>
#include <Elementary.h>

#if defined(OS_TIZEN_MOBILE)
#ifndef TIZEN_LEGACY_V_2_2_1
#include <device/haptic.h>
#else
#include <haptic/haptic.h>
#endif
#endif

#include <unistd.h>
#include <stdlib.h>
#include <string.h>

static Ecore_Evas* ee;
static Evas* e;
static Evas_Object* view;
static Evas_Object *popup;

Ewk_Notification *current_notification;

static Ewk_Auth_Challenge* auth_challenge_holder = NULL;
static Evas_Object* credentials[2] = {0};//username and password

static Eina_Bool fullscreen_enabled;
static Eina_Bool clear_cache;
static Eina_Bool disable_cache = EINA_FALSE;
static char* pdf_filename = NULL;
static char* cache_policy = NULL;
static char* content_security_policy = NULL;
static char* content_security_policy_type = NULL;
static char* start_url = NULL;
static int content_text_callback_data = 10;
static int content_text_callback_data_1 = 20;
static Eina_Bool test_cookie;
static Eina_Bool clear_cookie_called;
static Ewk_Cookie_Accept_Policy set_cookie_policy;  
static Eina_List* application_cache_origins = NULL;
static Evas_Object* prompt_entry;
static Eina_List* web_database_origins = NULL;

#if defined(OS_TIZEN_MOBILE)
static haptic_device_h g_haptic_handle;
static Ecore_Timer *g_haptic_timer_id;
static haptic_effect_h g_haptic_effect;
#endif

static void show_userid_password_popup(const char* msg);
static void show_search_popup(void);
static void on_mouse_up(void* data, Evas* canvas, Evas_Object* obj, void* event_info);
static void __search_closed_cb(void *data, Evas_Object *obj, void *event_info);
static void __search_focused_cb(void *data, Evas_Object *obj, void *event_info);
static void __search_unfocused_cb(void *data, Evas_Object *obj, void *event_info);
static void __search_clicked_cb(void *data, Evas_Object *obj, void *event_info);
static void __auth_login_button_cb(void *data, Evas_Object *obj, void *event_info);
static void __auth_close_button_cb(void *data, Evas_Object *obj, void *event_info);
static void __load_finished_cb(void *data, Evas_Object *obj, void *event_info);
static void __load_error_cb(void *data, Evas_Object *obj, void *event_info);
static void __uri_changed_cb(void *data, Evas_Object *obj, void *event_info);
static void __title_changed_cb(void *data, Evas_Object *obj, void *event_info);
static void __load_progress_cb(void *data, Evas_Object *obj, void *event_info);
static void __load_progress_started_cb(void *data, Evas_Object *obj, void *event_info);
static void __load_progress_finished_cb(void *data, Evas_Object *obj, void *event_info);
static void __ime_input_panel_show_cb(void* data, Evas_Object *obj, void* event_info);
static void __ime_input_panel_hide_cb(void* data, Evas_Object *obj, void* event_info);
static void __ime_candidate_panel_show_cb(void* data, Evas_Object *obj, void* event_info);
static void __ime_candidate_panel_hide_cb(void* data, Evas_Object *obj, void* event_info);
static void __create_window_cb(void *data, Evas_Object *obj, void *event_info);
static void __window_closed_cb(void *data, Evas_Object *obj, void *event_info);
static void __imf_input_method_changed_cb(void *data, Evas_Object *obj, void *event_info);
static void __did_start_provisional_load_for_frame_cb(void *data, Evas_Object *obj, void *event_info);
static void __did_commit_load_for_frame(void *data, Evas_Object *obj, void *event_info);
static void __geoLocation_popup_display_cb(void *data, Evas_Object *obj, void *event_info);
void __script_executed_cb(Evas_Object *obj, const char *javascript_result, void *data);
static void __auth_challenge_cb(void* data, Evas_Object *obj, void *event_info);
static void __policy_response_decide_cb(void *data, Evas_Object *obj, void *event_info);
static void __customize_context_menu_cb(void* data, Evas_Object *obj, void *event_info);
static void __customize_context_menu_item_selected_cb(void* data, Evas_Object *obj, void *event_info);
static void __navigation_policy_decide_cb(void *data, Evas_Object *obj, void *event_info);
static void __text_found_cb(void *data, Evas_Object *obj, void *event_info);
static void __newwindow_policy_cb(void *data, Evas_Object *obj, void* policy);
static void __load_nonemptylayout_finished_cb(void* data, Evas_Object *obj, void *event_info);
static void __popup_wait_start_cb(void *data, Evas_Object *obj, void* policy);
static void __popup_wait_finish_cb(void *data, Evas_Object *obj, void* policy);
static void __edge_left_cb(void* data, Evas_Object* obj, void* event_info);
static void __edge_right_cb(void* data, Evas_Object* obj, void* event_info);
static void __edge_top_cb(void* data, Evas_Object* obj, void* event_info);
static void __edge_bottom_cb(void* data, Evas_Object* obj, void* event_info);
static void __scroll_down_cb(void* data, Evas_Object* obj, void* event_info);
static void __scroll_up_cb(void* data, Evas_Object* obj, void* event_info);
static void __scroll_left_cb(void* data, Evas_Object* obj, void* event_info);
static void __scroll_right_cb(void* data, Evas_Object* obj, void* event_info);
static void __webprocess_crashed_cb(void* data, Evas_Object* obj, void* event_info);

static void on_evas_resize(Ecore_Evas*);
static void __notification_show_cb(void *data, Evas_Object *obj, void *event_info);
static void __notification_permission_cb(void *data, Evas_Object *obj, void *event_info);
static void __notification_cancel_cb(void *data, Evas_Object *obj, void *event_info);
static void __notification_cancel_byuser_cb(void *data, Evas_Object *obj, void *event_info);
static void __download_callback(const char*download_url, void* user_data);
static void __frame_rendered_cb(void* data, Evas_Object* obj, void* event_info);
static void __ewk_view_plain_text_get_cb(Evas_Object* o, const char* content_text, void* user_data);
static void __ewk_view_plain_text_get_cb_1(Evas_Object* o, const char* content_text, void* user_data);
static void __load_started_cb(void *data, Evas_Object *obj, void *event_info);
static void __text_style_state_changed_cb(void *data, Evas_Object *obj, void *event_info);
static void __ewk_cookie_manager_async_policy_get_cb(
    Ewk_Cookie_Accept_Policy policy, void *event_info);
static void __ewk_cookie_manager_async_hostnames_get_cb(Eina_List *hostnames, Ewk_Error *error, void *event_info);
static void __back_forward_list_changed_cb(void *data, Evas_Object *obj, void *event_info);
static Eina_Bool __mime_override_cb(const char* url, const char *mime, char **new_mime);
static void __console_message_cb(void *data, Evas_Object *obj, void *event_info);

#if defined(OS_TIZEN_MOBILE)
static Eina_Bool __vibration_timeout_cb(void *data);
#endif
static void __vibration_on_cb(uint64_t vibration_time, void *data);
static void __vibration_off_cb(void *data);
static void __undo_size_cb(void* data, Evas_Object* obj, void* event_info);
static void __redo_size_cb(void* data, Evas_Object* obj, void* event_info);
void __mhtml_save_cb(Evas_Object *obj, const char *mhtml_data, void *data);
static void __clipboard_opened_cb(void* data, Evas_Object* view, void* event_info);
static Eina_Bool __ewk_view_javascript_alert_cb(Evas_Object* o, const char* alert_text, void* user_data);
static Eina_Bool __ewk_view_javascript_confirm_cb(Evas_Object* o, const char* message, void* user_data);
static Eina_Bool __ewk_view_javascript_prompt_cb(Evas_Object* o, const char* message, const char* default_value, void* user_data);

enum {
  FOR_TEST_APPLICATION_CACHE_RUN = EWK_CONTEXT_MENU_ITEM_BASE_APPLICATION_TAG + 1,
  FOR_TEST_APPLICATION_CACHE_DELETE_ALL,
  FOR_TEST_APPLICATION_BACK_FORWAD_CLEAR,
  FOR_TEST_GRAB_SNAPSHOT,
  FOR_TEST_WEB_DATABASE_GET_ORIGINS,
  FOR_TEST_WEB_DATABASE_DELETE_ALL
  // New context menu items can be added here
};

static const Ecore_Getopt options = {
  .prog = "efl_webview_app",
  .usage = "%prog [options] [url]",
  .strict = EINA_FALSE,
  .descs = {
    ECORE_GETOPT_STORE_DEF_BOOL('F', "full-screen", "start in full-screen", EINA_TRUE),
    ECORE_GETOPT_STORE_DEF_BOOL('c', "clear-cache", "clear cache on every page load finished", EINA_TRUE),
    ECORE_GETOPT_STORE_DEF_BOOL('k', "test-cookie", "test cookie on every page load finished", EINA_TRUE),
    ECORE_GETOPT_STORE_DEF_STR('s', "set-cache-policy", "set cache policy", "viewer"),
    ECORE_GETOPT_STORE_DEF_STR('p', "set-content-security-policy", "set content security policy", NULL),
    ECORE_GETOPT_STORE_DEF_STR('t', "content-security-policy-type",
                               "Type of content security policy only applicable if policy is defined",
                               NULL),
    ECORE_GETOPT_STORE_DEF_BOOL('d', "disable-cache", "Disable network cache", EINA_FALSE),
    ECORE_GETOPT_STORE_DEF_STR('w', "save-pdf", "Save as PDF", "webpage.pdf"),
    ECORE_GETOPT_SENTINEL,
  }
};

static Eina_Bool _util_print_hit_test_result_hash_fn(const Eina_Hash *hash, const void *key, void *data, void *fdata)
{
  printf("HitTest attribute (%s): (%s)\n", (const char*)key, (char *)data);
  return EINA_TRUE;
}


static void on_key_down(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
    Evas_Event_Key_Down *ev = (Evas_Event_Key_Down*) event_info;
    static int scroll_x = 0;
    static int scroll_y = 0;

    if (strcmp(ev->key, "XF86Stop") == 0)
    {
      if (ewk_view_text_selection_clear(view))
        return;
    }

    if (!strcmp(ev->key, "F1")) {
        fprintf(stderr, "[on_key_down] (F1) pressed. Go to wikipedia.com\n");
        ewk_view_url_set(view, "http://www.wikipedia.com/");
    }
    else if (!strcmp(ev->key, "F2")) {
        double level = ewk_view_text_zoom_get(view) + 0.1;
        ewk_view_text_zoom_set(view, level);
        printf("current text zoom level %lf\n", ewk_view_text_zoom_get(view));
    }
    else if (!strcmp(ev->key, "F3")) {
        double level = ewk_view_text_zoom_get(view) - 0.1;
        ewk_view_text_zoom_set(view, level);
        printf("current text zoom level %lf\n", ewk_view_text_zoom_get(view));
    }
    else if (!strcmp(ev->key, "F4")) {
      ewk_view_command_execute(view, "Undo", NULL);
    }
    else if (!strcmp(ev->key, "F5")) {
        printf("selection text \"%s\"\n", ewk_view_text_selection_text_get(view));
    }
    else if (!strcmp(ev->key, "F6")) {
        ewk_view_stop(view);
    }
    else if (!strcmp(ev->key, "F7")) {
        static Eina_Bool visibility = EINA_TRUE;
        visibility = !visibility;
        ewk_view_visibility_set(view, visibility);
    }
    else if (!strcmp(ev->key, "F8")) {
        ewk_view_contents_set(view, "ewk Data Set API Test!!!!", 25, "text/plain", NULL, "http://www.naver.com");
    }
    else if (!strcmp(ev->key, "F9")) {
        static int x;
        static int y;
        ewk_view_scroll_pos_get(view, &x, &y);
        printf("scroll_pos_get %d, %d \n",x,y);
    }
    else if (!strcmp(ev->key, "F10")) {
        if (!(ewk_view_plain_text_get(view, __ewk_view_plain_text_get_cb, &content_text_callback_data)))
            printf("\nEWK_VIEW_PLAIN_TEXT_GET FAILURE\n");
        else
            printf("\nEWK_VIEW_PLAIN_TEXT_GET SUCCESS\n");
        if (!(ewk_view_plain_text_get(view, __ewk_view_plain_text_get_cb_1, &content_text_callback_data_1)))
            printf("\nEWK_VIEW_PLAIN_TEXT_GET_1 FAILURE\n");
        else
            printf("\nEWK_VIEW_PLAIN_TEXT_GET_1 SUCCESS\n");
    }
    else if (!strcmp(ev->key, "F11")) {
      ewk_view_command_execute(view, "Redo", NULL);
    }
    else if (evas_key_modifier_is_set(ev->modifiers, "Control")) {
        if (!strcmp(ev->key, "f") || !strcmp(ev->key, "F"))
            show_search_popup();
        else if (!strcmp(ev->key, "s") || !strcmp(ev->key, "S"))
          ewk_view_mhtml_data_get(view, __mhtml_save_cb, NULL);
    }
    else if (!strcmp(ev->key, "w") || !strcmp(ev->key, "W")) {
        if(scroll_y >= 0) {
          scroll_y -= 2;
          ewk_view_scroll_set(view, scroll_x, scroll_y);
          printf("scroll_set  %d, %d \n", scroll_x, scroll_y);
        }
    }
    else if (!strcmp(ev->key, "s") || !strcmp(ev->key, "S")) {
        scroll_y += 2;
        ewk_view_scroll_set(view, scroll_x, scroll_y);
        printf("scroll_set  %d, %d \n", scroll_x, scroll_y);
    }
    else if (!strcmp(ev->key, "d") || !strcmp(ev->key, "D")) {
        scroll_x += 2;
        ewk_view_scroll_set(view, scroll_x, scroll_y);
        printf("scroll_set  %d, %d \n", scroll_x, scroll_y);
    }
    else if (!strcmp(ev->key, "a") || !strcmp(ev->key, "A")) {
        if(scroll_x >= 0) {
          scroll_x -= 2;
          ewk_view_scroll_set(view, scroll_x, scroll_y);
          printf("scroll_set  %d, %d \n", scroll_x, scroll_y);
        }
    }
    else if (!strcmp(ev->key, "BackSpace") || !strcmp(ev->key, "XF86Stop")) {
        ewk_view_back(data);
    }
}

static void on_mouse_wheel(void *data, Evas *e, Evas_Object *obj, void *event_info) 
{
    Evas_Event_Mouse_Wheel* event = (Evas_Event_Mouse_Wheel*)event_info; 
    static double scale_factor = 1.0;
 
    if (evas_key_modifier_is_set(event->modifiers, "Control")) 
    { 
        if (event->z == -1)
            scale_factor += 0.1;
        else 
            scale_factor -= 0.1;

        printf("ewk_view_scale_set() : %lf (%d, %d)\n", scale_factor, event->output.x, event->output.y);

        ewk_view_scale_set(view, scale_factor, event->output.x, event->output.y);
    }
}

static Eina_Strbuf* create_url_with_scheme(const char* base_url, const char* scheme)
{
  Eina_Strbuf* buffer = eina_strbuf_new();
  eina_strbuf_append(buffer, scheme);
  eina_strbuf_append(buffer, base_url);
  return buffer;
}

static void set_url_from_user_input(Evas_Object* view, const char* url)
{
  if (strstr(url, "://")) {
    ewk_view_url_set(view, url);
    return;
  }

  // Add scheme.
  Eina_Strbuf* full_url;
  if (ecore_file_exists(url)) {
    char* realpath = ecore_file_realpath(url);
    full_url = create_url_with_scheme(realpath, "file://");
    free(realpath);
  } else {
    full_url = create_url_with_scheme(url, "http://");
  }

  ewk_view_url_set(view, eina_strbuf_string_get(full_url));

  eina_strbuf_free(full_url);
}

//set cache policy as per command line arguments
static void set_cache_policy(Ewk_Context* context)
{
  if(!cache_policy) {
    ewk_context_cache_model_set(context, EWK_CACHE_MODEL_DOCUMENT_VIEWER);
    return;
  }
  if (!strcmp(cache_policy, "browser"))
    ewk_context_cache_model_set(context, EWK_CACHE_MODEL_DOCUMENT_BROWSER);
  else if (!strcmp(cache_policy, "webbrowser"))
    ewk_context_cache_model_set(context, EWK_CACHE_MODEL_PRIMARY_WEBBROWSER);
  else if(!strcmp(cache_policy, "viewer"))
    ewk_context_cache_model_set(context, EWK_CACHE_MODEL_DOCUMENT_VIEWER);
}

static void set_csp(Evas_Object *ewk_view)
{
  if(!ewk_view || !content_security_policy_type)
    return;

  if(!strcmp(content_security_policy_type, "report"))
    ewk_view_content_security_policy_set(ewk_view, content_security_policy, EWK_REPORT_ONLY);
  else if(!strcmp(content_security_policy_type, "enforce"))
    ewk_view_content_security_policy_set(ewk_view, content_security_policy, EWK_ENFORCE_POLICY);
}

static void disable_cache_for_context(Ewk_Context* context)
{
  ewk_context_cache_disabled_set(context, EINA_TRUE);
}

static void _evas_close_cb(Ecore_Evas* ee)
{
  ecore_main_loop_quit();
}

int main(int argc, char** argv)
{
  // Make sure this happens just once.
  // By default the web process is the same program.
  fprintf(stderr, "efl_webview_app starting...\n");

  ewk_set_arguments(argc, argv);
  elm_init(0,NULL);
  if (!ewk_init())
    return EXIT_FAILURE;
  elm_init(0, NULL);
  Ecore_Getopt_Value values[] = {
    ECORE_GETOPT_VALUE_BOOL(fullscreen_enabled),
    ECORE_GETOPT_VALUE_BOOL(clear_cache),
    ECORE_GETOPT_VALUE_BOOL(test_cookie),
    ECORE_GETOPT_VALUE_STR(cache_policy),
    ECORE_GETOPT_VALUE_STR(content_security_policy),
    ECORE_GETOPT_VALUE_STR(content_security_policy_type),
    ECORE_GETOPT_VALUE_BOOL(disable_cache),
    ECORE_GETOPT_VALUE_STR(pdf_filename),
    ECORE_GETOPT_VALUE_NONE,
  };

  Eina_Bool incognito = EINA_FALSE;
  int idx = ecore_getopt_parse(&options, values, argc, argv);
  for (; idx < argc; ++idx) {
    // Treat first argument not starting with - as the url to load
    if (argv[idx][0] != '-') {
      start_url = strdup(argv[idx]);
    } else if (strcmp(argv[idx], "--incognito") == 0) {
      incognito = EINA_TRUE;
    }
  }

  if (!start_url) {
    start_url = strdup("http://www.google.com");
  }

  int width = 800;
  int height = 600;

  ee = ecore_evas_new("opengl_x11", 0, 0, width, height, 0);
  e = ecore_evas_get(ee);

  ecore_evas_callback_delete_request_set(ee, _evas_close_cb);

#if defined(OS_TIZEN)
  // Ignore fullscreen option, we should always go fullscreen on device.
  Eina_Bool use_fullscreen = EINA_TRUE;
#else
  Eina_Bool use_fullscreen = fullscreen_enabled;
#endif

  if (use_fullscreen) {
    // FIXME: this is wrong with dual screen configuration (desktop).
    // We should use XineramaQueryScreens for that setup.
    ecore_x_window_size_get(ecore_x_window_root_first_get(), &width, &height);
    ecore_evas_size_base_set(ee, width, height);
  }

  Ewk_Context* context = NULL;

  if (incognito == EINA_TRUE) {
    fprintf(stderr, "Creating incognito web view\n");
    view = ewk_view_add_in_incognito_mode(e);
    context = ewk_view_context_get(view);
  } else {
    fprintf(stderr, "Creating normal web view\n");
    context = ewk_context_default_get();
    view = ewk_view_add_with_context(e, context);
  }

  evas_object_resize(view, width, height);

  ecore_evas_callback_resize_set(ee, &on_evas_resize);

  evas_object_event_callback_add(view, EVAS_CALLBACK_KEY_DOWN, on_key_down, view);
  evas_object_event_callback_add(view, EVAS_CALLBACK_MOUSE_WHEEL, on_mouse_wheel, view);
  evas_object_event_callback_add(view, EVAS_CALLBACK_MOUSE_UP , on_mouse_up, 0);
  evas_object_show(view);
  evas_object_focus_set(view, EINA_TRUE);
  set_cache_policy(context);
  if (disable_cache) {
    disable_cache_for_context(context);
  }
  set_csp(view);

  evas_object_smart_callback_add(view, "load,finished", __load_finished_cb, context);
  evas_object_smart_callback_add(view, "load,error", __load_error_cb, 0);
  evas_object_smart_callback_add(view, "url,changed", __uri_changed_cb,0);
  evas_object_smart_callback_add(view, "title,changed", __title_changed_cb,0);
  evas_object_smart_callback_add(view, "load,progress", __load_progress_cb,0);
  evas_object_smart_callback_add(view, "load,progress,started", __load_progress_started_cb,0);
  evas_object_smart_callback_add(view, "load,progress,finished", __load_progress_finished_cb,0);
  evas_object_smart_callback_add(view, "editorclient,ime,opened", __ime_input_panel_show_cb,0);
  evas_object_smart_callback_add(view, "editorclient,ime,closed", __ime_input_panel_hide_cb,0);
  evas_object_smart_callback_add(view, "editorclient,candidate,opened", __ime_candidate_panel_show_cb,0);
  evas_object_smart_callback_add(view, "editorclient,candidate,closed", __ime_candidate_panel_hide_cb,0);
  evas_object_smart_callback_add(view, "create,window", __create_window_cb, 0);
  evas_object_smart_callback_add(view, "inputmethod,changed", __imf_input_method_changed_cb, 0);
  evas_object_smart_callback_add(view, "load,committed", __did_commit_load_for_frame, 0);
  evas_object_smart_callback_add(view, "load,provisional,started", __did_start_provisional_load_for_frame_cb, 0);
  evas_object_smart_callback_add(view, "geolocation,permission,request", __geoLocation_popup_display_cb,0);
  evas_object_smart_callback_add(view, "authentication,challenge", __auth_challenge_cb, 0);
  evas_object_smart_callback_add(view, "notification,show", __notification_show_cb,0);
  evas_object_smart_callback_add(view, "notification,cancel", __notification_cancel_cb,0);
  evas_object_smart_callback_add(view, "notification,permission,request", __notification_permission_cb,0);
  evas_object_smart_callback_add(view, "policy,response,decide", __policy_response_decide_cb, 0);
  evas_object_smart_callback_add(view, "contextmenu,customize", __customize_context_menu_cb, 0);
  evas_object_smart_callback_add(view, "contextmenu,selected", __customize_context_menu_item_selected_cb, 0);
  evas_object_smart_callback_add(view, "policy,navigation,decide", __navigation_policy_decide_cb,0);
  evas_object_smart_callback_add(view, "text,found", __text_found_cb, 0);
  evas_object_smart_callback_add(view, "policy,newwindow,decide", __newwindow_policy_cb,0);
  evas_object_smart_callback_add(view, "load,nonemptylayout,finished", __load_nonemptylayout_finished_cb, 0);
  evas_object_smart_callback_add(view, "popup,reply,wait,start", __popup_wait_start_cb,0);
  evas_object_smart_callback_add(view, "popup,reply,wait,finish", __popup_wait_finish_cb,0);
  evas_object_smart_callback_add(view, "clipboard,opened", __clipboard_opened_cb, 0);
  ewk_context_did_start_download_callback_set(context, __download_callback, NULL);
  evas_object_smart_callback_add(view, "frame,rendered", __frame_rendered_cb, 0);
  evas_object_smart_callback_add(view, "load,started", __load_started_cb, context);
  evas_object_smart_callback_add(view, "edge,left", __edge_left_cb, 0);
  evas_object_smart_callback_add(view, "edge,right", __edge_right_cb, 0);
  evas_object_smart_callback_add(view, "edge,top", __edge_top_cb, 0);
  evas_object_smart_callback_add(view, "edge,bottom", __edge_bottom_cb, 0);
  evas_object_smart_callback_add(view, "text,style,state", __text_style_state_changed_cb, 0);
  evas_object_smart_callback_add(view, "scroll,down", __scroll_down_cb, 0);
  evas_object_smart_callback_add(view, "scroll,up", __scroll_up_cb, 0);
  evas_object_smart_callback_add(view, "scroll,left", __scroll_left_cb, 0);
  evas_object_smart_callback_add(view, "scroll,right", __scroll_right_cb, 0);
  evas_object_smart_callback_add(view, "back,forward,list,changed", __back_forward_list_changed_cb, 0);
  evas_object_smart_callback_add(view, "webprocess,crashed", __webprocess_crashed_cb, 0);
  evas_object_smart_callback_add(view, "undo,size", __undo_size_cb, 0);
  evas_object_smart_callback_add(view, "redo,size", __redo_size_cb, 0);
  
  ewk_view_custom_header_add(view, "X-Test-header", "X-Value-1");

  ewk_context_vibration_client_callbacks_set(context, __vibration_on_cb, __vibration_off_cb, NULL);
  ewk_context_mime_override_callback_set(__mime_override_cb);
  ewk_view_javascript_alert_callback_set(view, __ewk_view_javascript_alert_cb, 0);
  ewk_view_javascript_confirm_callback_set(view, __ewk_view_javascript_confirm_cb, 0);
  ewk_view_javascript_prompt_callback_set(view, __ewk_view_javascript_prompt_cb, 0);
  evas_object_smart_callback_add(view, "console,message", __console_message_cb, 0);

  if(test_cookie) {
    set_cookie_policy = EWK_COOKIE_ACCEPT_POLICY_NEVER;
    ewk_cookie_manager_accept_policy_set(ewk_context_cookie_manager_get(context),
                                         set_cookie_policy);
  }

  set_url_from_user_input(view, start_url);

  ecore_evas_show(ee);
  ewk_view_script_execute(view, "console.log(\"Executed test script\");", __script_executed_cb, 0);
  ecore_main_loop_begin();
  ewk_context_unref(context);
  // deleting view will release context
  evas_object_del(view);
  ewk_shutdown();

  free(start_url);

  return 0;
}

static void on_evas_resize(Ecore_Evas* ee)
{
  int width, height;
  ecore_evas_geometry_get(ee, NULL, NULL, &width, &height);
  evas_object_resize(view, width, height);
}

void __ewk_cookie_manager_async_hostnames_get_cb(Eina_List *hostnames, Ewk_Error *error, void *event_info)
{
  printf ("APP.C callback called __ewk_cookie_manager_async_hostnames_get_cb \n");
  int host_count = eina_list_count(hostnames);
  printf("APP.c total hosts received:%d\n",host_count);
  if (clear_cookie_called) {
    if (host_count) 
      printf("APP.C clear cookie call did not delete cookies\n");
    else
      printf("APP.c all cookies cleared\n");
  }

  Ewk_Context* context = ewk_context_default_get();
  if (host_count) {
    printf("APP.C  calling ewk_cookie_manager_cookies_clear\n");
    if (!clear_cookie_called) {
      clear_cookie_called = EINA_TRUE;
      printf("APP.c calling ewk_view_get_cookies_for_url:%s\n",ewk_view_get_cookies_for_url(view, start_url));
      ewk_cookie_manager_cookies_clear(ewk_context_cookie_manager_get(context));
      printf("APP.c calling ewk_cookie_manager_async_hostnames_with_cookies_get\n");
      ewk_cookie_manager_async_hostnames_with_cookies_get(ewk_context_cookie_manager_get(context),
                                                          __ewk_cookie_manager_async_hostnames_get_cb,
                                                          NULL);
    }
  } else if ( EWK_COOKIE_ACCEPT_POLICY_NEVER == set_cookie_policy) {
    printf("APP.C  cookie policy was set to never now going to set accept except third party\n");
    set_cookie_policy = EWK_COOKIE_ACCEPT_POLICY_NO_THIRD_PARTY;
    ewk_cookie_manager_accept_policy_set(ewk_context_cookie_manager_get(context), set_cookie_policy);
    printf("APP.C Reload called from __ewk_cookie_manager_async_policy_get_cb\n");
    ewk_view_reload(view);
  } else {
    if (!clear_cookie_called)
      printf("APP.cc cookie count is 0 although policy was to accept\n");
  }
}

const char* printCookiePolicy(Ewk_Cookie_Accept_Policy policy)
{
  switch(policy) {
    case EWK_COOKIE_ACCEPT_POLICY_ALWAYS :
      return (" Accept Always ");
    case EWK_COOKIE_ACCEPT_POLICY_NEVER :
      return (" Accept Never ");
    case EWK_COOKIE_ACCEPT_POLICY_NO_THIRD_PARTY :
      return (" No third party ");
  }
  return ("");
}

void __ewk_cookie_manager_async_policy_get_cb(Ewk_Cookie_Accept_Policy policy,
                                              void *event_info) {
  printf ("APP.C callback called __ewk_cookie_manager_async_policy_get_cb \n");
  if (set_cookie_policy != policy) {
    printf("error in setting cookie policy");
  }
  else {
    Ewk_Context* context = ewk_context_default_get();
    printf("APP.C cookie policy is set properly:%s\n", printCookiePolicy(policy));
    printf("APP.c calling ewk_cookie_manager_async_hostnames_with_cookies_get\n");
    ewk_cookie_manager_async_hostnames_with_cookies_get(ewk_context_cookie_manager_get(context),
                                                        __ewk_cookie_manager_async_hostnames_get_cb,
                                                        NULL);
  }
}

void __load_finished_cb(void *data, Evas_Object *obj, void *event_info)
{
  printf ("APP.C callback called __load_finished_cb \n");
  
  if(clear_cache)
    ewk_context_cache_clear((Ewk_Context*)data);

  if(test_cookie) {
    printf("calling get cookie policy async\n");
    Ewk_Context* context = ewk_context_default_get();
    ewk_cookie_manager_accept_policy_async_get(
        ewk_context_cookie_manager_get(context),
        __ewk_cookie_manager_async_policy_get_cb,
        NULL);
  }

}

void __load_error_cb(void *data, Evas_Object *obj, void *event_info)
{
  Ewk_Error* error = (Ewk_Error*)event_info;
  printf("APP.C callback called __load_error_cb\n");
  printf("error url : %s\n", ewk_error_url_get(error));
}

void __uri_changed_cb(void *data, Evas_Object *obj, void *event_info)
{
  printf ("APP.C callback called __uri_changed_cb URI is = %s \n", (const char *)event_info);
}

void __title_changed_cb(void *data, Evas_Object *obj, void *event_info)
{
  printf ("APP.C callback called __title_changed_cb URI is = %s \n", (const char *)event_info);
}

void __load_progress_cb(void *data, Evas_Object *obj, void *event_info)
{
  printf ("APP.C callback called __load_progress_cb value is = %f \n", *((double *)event_info));
}

void __load_progress_started_cb(void *data, Evas_Object *obj, void *event_info)
{
  printf ("APP.C callback called __load_progress_started_cb \n");
}

void __load_progress_finished_cb(void *data, Evas_Object *obj, void *event_info)
{
  printf ("APP.C callback called __load_progress_finsihed_cb \n");
  ewk_view_custom_header_remove(view, "X-Test-header");
  
  int width, height;
  ewk_view_contents_size_get(view, &width, &height);
  printf("APP.c called ewk_view_contents_size_get. width = %d, height = %d\n", width, height);
  ewk_view_scroll_size_get(view, &width, &height);
  printf("APP.c called ewk_view_scroll_size_get. width = %d, height = %d\n", width, height);
  printf("APP.c called ewk_view_load_progress_get : %f3\n", ewk_view_load_progress_get(obj));
  printf("APP.c called ewk_view_title_get   : %s\n", ewk_view_title_get(view));
}

void __ime_input_panel_show_cb(void* data, Evas_Object *obj, void* event_info)
{
  printf ("APP.C callback called __ime_input_panel_show_cb \n");
}

void __ime_input_panel_hide_cb(void* data, Evas_Object *obj, void* event_info)
{
  printf ("APP.C callback called __ime_input_panel_hide_cb \n");
}

void __ime_candidate_panel_show_cb(void* data, Evas_Object *obj, void* event_info)
{
  printf ("APP.C callback called __ime_candidate_panel_show_cb \n");
}

void __ime_candidate_panel_hide_cb(void* data, Evas_Object *obj, void* event_info)
{
  printf ("APP.C callback called __ime_candidate_panel_hide_cb \n");
}

void delete_popup_window(Ecore_Evas* ee)
{
  ecore_evas_free(ee);
}

void __create_window_cb(void *data, Evas_Object *obj, void *event_info)
{
  printf ("APP.C callback called __create_window_cb \n");

  // We have no information about preferred size with the current API.
  const int default_width = 300;
  const int default_height =  400;

  Ecore_Evas* ee = ecore_evas_new("opengl_x11", 0, 0, default_width, default_height, 0);
  Evas* e = ecore_evas_get(ee);

  Evas_Object* view = ewk_view_add(e);
  evas_object_resize(view, default_width, default_height);
  evas_object_show(view);
  evas_object_focus_set(view, EINA_TRUE);
  ecore_evas_show(ee);

  evas_object_smart_callback_add(view, "close,window", __window_closed_cb, 0);

  // Override default handler because it terminates the event loop.
  ecore_evas_callback_delete_request_set(ee, delete_popup_window);

  Evas_Object** result = (Evas_Object**) event_info;
  *result = view;
}

void __window_closed_cb(void *data, Evas_Object *view, void *event_info)
{
  printf ("APP.C callback called __close_window_cb \n");

  Evas* e = evas_object_evas_get(view);
  Ecore_Evas* ee = ecore_evas_ecore_evas_get(e);
  evas_object_hide(view);
  evas_object_focus_set(view, EINA_FALSE);
  evas_object_del(view);

  ecore_evas_ews_delete_request(ee);
}

void __imf_input_method_changed_cb(void *data, Evas_Object *obj, void *event_info)
{
  Eina_Rectangle *rect = (Eina_Rectangle*)event_info;
  printf ("APP.C callback called __imf_input_method_changed_cb Rect values are x = %d, y= %d, height = %d, width = %d\n",
           rect->x, rect->y, rect->h, rect->w);
}

void __script_executed_cb(Evas_Object *obj, const char *javascript_result, void *data)
{
  printf ("APP.C callback called __script_executed_cb called with result : %s\n", javascript_result);
}

void __did_start_provisional_load_for_frame_cb(void *data, Evas_Object *obj, void *event_info)
{
  printf ("APP.C callback called __did_start_provisional_load_for_frame_cb \n");
}

void __did_commit_load_for_frame(void *data, Evas_Object *obj, void *event_info)
{
  printf ("APP.C callback called __did_commit_load_for_frame \n");
}

void __geoLocation_popup_display_cb(void *data, Evas_Object *obj, void *event_info)
{
  Ewk_Geolocation_Permission_Request *request = (Ewk_Geolocation_Permission_Request*)event_info;
  if (request) {
      const Ewk_Security_Origin * sec_origin = ewk_geolocation_permission_request_origin_get((const Ewk_Geolocation_Permission_Request*)request);
      if (sec_origin) {
          printf("APP.C callback called __geoLocation_popup_display_cb URL = %s, protocol = %s, port = %d\n",
          ewk_security_origin_host_get(sec_origin), ewk_security_origin_protocol_get(sec_origin),ewk_security_origin_port_get(sec_origin));
          printf ("APP.C callback called __geoLocation_popup_display_cb Setting permission\n");
          ewk_geolocation_permission_request_set(request, 1);
      }
  }
  printf ("APP.C callback called __geoLocation_popup_display_cb \n");
}

void __navigation_policy_decide_cb(void *data, Evas_Object *obj, void *event_info)
{
  Ewk_Policy_Decision* decision = (Ewk_Policy_Decision*)event_info;
  printf("APP.C callback called __navigation_policy_decide_cb \n");
  printf("URL: %s\n", ewk_policy_decision_url_get(decision));
  printf("Navigation type: %d", ewk_policy_decision_navigation_type_get(decision));
  // we only set USE
  // download needs to be handled and ignore wont have any visible difference.
  printf("Decision: USE\n");
  ewk_policy_decision_use(decision);
}

void __auth_challenge_cb(void* data, Evas_Object *obj, void *event_info)
{
  printf("APP.C callback called __auth_challenge_cb \n");
  Ewk_Auth_Challenge* auth_challenge = (Ewk_Auth_Challenge*)event_info;
  auth_challenge_holder = auth_challenge;
  if (!auth_challenge)
    fprintf(stderr, "auth_challenge NULL\n");

  const char* realm = ewk_auth_challenge_realm_get(auth_challenge);
  const char* url = ewk_auth_challenge_url_get(auth_challenge);
  if (!realm || !url)
    fprintf(stderr, "realm or url NULL\n");

  ewk_auth_challenge_suspend(auth_challenge);
  show_userid_password_popup(url);
}

void show_userid_password_popup(const char* msg)
{
  Evas_Object *popup = elm_win_util_standard_add("auth-window", msg);
  if (!popup) {
    fprintf(stderr,"Pop-up window creation failed");
    return;
  }
  elm_win_modal_set(popup, EINA_TRUE);
  evas_object_size_hint_weight_set(popup , EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_align_set(popup , EVAS_HINT_FILL, EVAS_HINT_FILL);
  evas_object_resize(popup, 400, 300);
  evas_object_show(popup);

  Evas_Object* box = elm_box_add(popup);
  evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, 0.0);
  evas_object_size_hint_align_set(box, EVAS_HINT_FILL, 0.0);
  elm_win_resize_object_add(popup, box);
  evas_object_show(box);

  Evas_Object *user_name_lable = elm_label_add(popup);
  elm_object_text_set(user_name_lable, "User name");
  elm_box_pack_end(box, user_name_lable);
  evas_object_show(user_name_lable);

  Evas_Object *user_name_entry = elm_entry_add(popup);
  elm_entry_single_line_set(user_name_entry, EINA_TRUE);
  elm_object_text_set(user_name_entry, "Delete this and enter user name");
  elm_object_focus_set(user_name_entry, EINA_TRUE);
  elm_box_pack_end(box, user_name_entry);
  evas_object_show(user_name_entry);

  Evas_Object *password_lable = elm_label_add(popup);
  elm_object_text_set(password_lable, "Password");
  elm_box_pack_end(box, password_lable);
  evas_object_show(password_lable);

  Evas_Object *password_entry = elm_entry_add(popup);
  elm_entry_single_line_set(password_entry, EINA_TRUE);
  elm_entry_password_set(password_entry, EINA_TRUE);
  elm_box_pack_end(box, password_entry);
  evas_object_show(password_entry);

  credentials[0] = user_name_entry;
  credentials[1] = password_entry;

  Evas_Object *login_button = elm_button_add(popup);
  elm_object_text_set(login_button, "Login");
  elm_box_pack_end(box, login_button);
  evas_object_show(login_button);

  evas_object_smart_callback_add(popup, "delete,request", __auth_close_button_cb, 0);
  evas_object_smart_callback_add(login_button, "clicked", __auth_login_button_cb, popup);
}

void show_search_popup(void)
{
  Evas_Object *popup = elm_win_util_standard_add("search-text", "Search Text");
  if (!popup) {
    fprintf(stderr,"Pop-up window creation failed");
    return;
  }
  evas_object_size_hint_weight_set(popup , EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_align_set(popup , EVAS_HINT_FILL, EVAS_HINT_FILL);
  evas_object_resize(popup, 400, 300);
  evas_object_show(popup);

  Evas_Object* box = elm_box_add(popup);
  evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, 0.0);
  evas_object_size_hint_align_set(box, EVAS_HINT_FILL, 0.0);
  elm_win_resize_object_add(popup, box);
  evas_object_show(box);

  Evas_Object *search_text_label = elm_label_add(popup);
  elm_object_text_set(search_text_label, "Search Text: ");
  elm_box_pack_end(box, search_text_label);
  evas_object_show(search_text_label);

  Evas_Object *search_text_entry = elm_entry_add(popup);
  elm_entry_single_line_set(search_text_entry, EINA_TRUE);
  elm_object_text_set(search_text_entry, "input search text");
  elm_object_focus_set(search_text_entry, EINA_TRUE);
  elm_box_pack_end(box, search_text_entry);
  evas_object_show(search_text_entry);

  Evas_Object *search_button = elm_button_add(popup);
  elm_object_text_set(search_button, "Search");
  elm_box_pack_end(box, search_button);
  evas_object_show(search_button);


  evas_object_smart_callback_add(popup, "delete,request", __search_closed_cb, 0);
  evas_object_smart_callback_add(search_text_entry, "focused", __search_focused_cb, search_text_entry);
  evas_object_smart_callback_add(search_text_entry, "unfocused", __search_unfocused_cb, search_text_entry);
  evas_object_smart_callback_add(search_text_entry, "activated", __search_clicked_cb, search_text_entry);
  evas_object_smart_callback_add(search_button, "clicked", __search_clicked_cb, search_text_entry);
}

void __search_closed_cb(void *data, Evas_Object *obj, void *event_info)
{
  evas_object_del(obj);
}

void __search_unfocused_cb(void *data, Evas_Object *obj, void *event_info)
{
  Evas_Object* entry = data;
  if (strcmp("", elm_entry_entry_get(entry)) == 0)
    elm_entry_entry_set(entry, "input search text");
}

void __search_focused_cb(void *data, Evas_Object *obj, void *event_info)
{
  Evas_Object* entry = data;
  if (strcmp("input search text", elm_entry_entry_get(entry)) == 0)
    elm_entry_entry_set(entry, "");
}

void __search_clicked_cb(void *data, Evas_Object *obj, void *event_info)
{
  Evas_Object* entry = data;
  if (strcmp("input search text", elm_entry_entry_get(entry)) != 0)
    ewk_view_text_find(view, elm_entry_entry_get(entry), EWK_FIND_OPTIONS_CASE_INSENSITIVE, 0);
}

void __auth_login_button_cb(void *data, Evas_Object *obj, void *event_info)
{
  Evas_Object* popup_window = data;
  const char* username = elm_entry_entry_get(credentials[0]);
  const char* password = elm_entry_entry_get(credentials[1]);
  ewk_auth_challenge_credential_use(auth_challenge_holder, username, password);
  evas_object_del(popup_window);
}

void __auth_close_button_cb(void *data, Evas_Object *obj, void *event_info)
{
  ewk_auth_challenge_credential_cancel(auth_challenge_holder);
  evas_object_del(obj);
}

void __notification_cancel_byuser_cb(void *data, Evas_Object *obj, void *event_info)
{
   Eina_List *list = NULL;
   list =  eina_list_append(list, current_notification);
   ewk_view_notification_closed(view, list);
   evas_object_hide(popup);
   evas_object_del(obj);
   evas_object_del(popup);
}

void __notification_show_cb(void *data, Evas_Object *obj, void *event_info)
{
  current_notification = (Ewk_Notification*)event_info;
  if (current_notification) {
    printf("APP.C callback _notification_show_cb Details: \n");
    printf("APP.C callback Body = %s \n",ewk_notification_body_get(current_notification));
    printf("APP.C callback Icon url = %s\n", ewk_notification_icon_url_get(current_notification));
    printf("APP.C callback Title = %s\n",ewk_notification_title_get(current_notification));
    const Ewk_Security_Origin *org = ewk_notification_security_origin_get((const Ewk_Notification*)current_notification);
    printf("APP.C callback Origin Host = %s\n",ewk_security_origin_host_get(org));
    printf("APP.C callback Origin Protocol = %s\n",ewk_security_origin_protocol_get(org));
    printf("APP.C callback Origin Port = %d\n",ewk_security_origin_port_get(org));
    // Adding pop up code
    popup = elm_popup_add(view);
    elm_popup_allow_events_set(popup, EINA_TRUE);
    elm_object_text_set(popup,ewk_notification_body_get(current_notification));
    elm_popup_content_text_wrap_type_set(popup, ELM_WRAP_CHAR);
    elm_object_part_text_set(popup, "title,text", ewk_notification_title_get(current_notification));
    evas_object_show(popup);

    Evas_Object *btn1 = elm_button_add(popup);
    elm_object_text_set(btn1, "CLOSE");
    elm_object_part_content_set(popup, "button1", btn1);
    evas_object_smart_callback_add(btn1, "clicked", __notification_cancel_byuser_cb, 0);
  }
  printf ("APP.C callback called __notification_show_cb \n");
}

void __notification_permission_cb(void *data, Evas_Object *obj, void *event_info)
{
  Ewk_Notification_Permission_Request *request = (Ewk_Notification_Permission_Request*)event_info;
  if(request) {
    const Ewk_Security_Origin *sec_origin = ewk_notification_permission_request_origin_get((const Ewk_Notification_Permission_Request*)request);
    if (sec_origin) {
        printf("APP.C callback called __notification_permission_cb URL = %s, protocol = %s, port = %d\n",
        ewk_security_origin_host_get(sec_origin), ewk_security_origin_protocol_get(sec_origin),ewk_security_origin_port_get(sec_origin));
    }
    ewk_notification_permission_request_set(request, 1);
  }
}

void __notification_cancel_cb(void *data, Evas_Object *obj, void *event_info)
{
  uint64_t *notification_id = (uint64_t*)event_info;
  evas_object_del(popup);
  printf("APP.C callback  __notification_cancel_cb with notification id = %d\n", (int)*notification_id);
}

void __policy_response_decide_cb(void *data, Evas_Object *obj, void *event_info)
{
  printf("APP.C callback called __policy_response_decide_cb \n");
  Ewk_Policy_Decision *policy_decision = (Ewk_Policy_Decision *)event_info;
  Ewk_Policy_Decision_Type policy_type = ewk_policy_decision_type_get(policy_decision);
  const char *uri = ewk_policy_decision_url_get(policy_decision);
  const char *cookie = ewk_policy_decision_cookie_get(policy_decision);
  const char *mime = ewk_policy_decision_response_mime_get(policy_decision);
  printf("APP.C URL : %s\n", uri);
  printf("APP.C cookie : %s\n", cookie);
  printf("APP.C mime type : %s\n", mime);
  printf("APP.C policy type : %d\n", policy_type);
  ewk_policy_decision_use(policy_decision);
}

void __customize_context_menu_cb(void* data, Evas_Object *obj, void *event_info)
{
  Ewk_Context_Menu *context_menu = (Ewk_Context_Menu*)event_info;
  ewk_context_menu_item_append_as_action(context_menu, EWK_CONTEXT_MENU_ITEM_TAG_GO_BACK, "Back", 1);
  ewk_context_menu_item_append_as_action(context_menu, EWK_CONTEXT_MENU_ITEM_TAG_GO_FORWARD, "Forward", 1);
  ewk_context_menu_item_append_as_action(context_menu, EWK_CONTEXT_MENU_ITEM_TAG_RELOAD, "Reload", 1);
  ewk_context_menu_item_append_as_action(context_menu, FOR_TEST_APPLICATION_BACK_FORWAD_CLEAR, "Bk_fwd_clear", 1);
  ewk_context_menu_item_append_as_action(context_menu, FOR_TEST_APPLICATION_CACHE_RUN, "Run App Cache Test", 1);
  ewk_context_menu_item_append_as_action(context_menu, FOR_TEST_APPLICATION_CACHE_DELETE_ALL, "Delete All App Cache", 1);
  ewk_context_menu_item_append_as_action(context_menu, FOR_TEST_GRAB_SNAPSHOT, "Take Snapshot", 1);
  ewk_context_menu_item_append_as_action(context_menu, FOR_TEST_WEB_DATABASE_GET_ORIGINS, "Get WebDB", 1);
  ewk_context_menu_item_append_as_action(context_menu, FOR_TEST_WEB_DATABASE_DELETE_ALL, "Clear WebDB", 1);
  printf("APP.C callback __customize_context_menu_cb \n");
}

static void get_app_cache_usage_cb(int64_t usage, void* user_data)
{
  static int count_usage_callback = 0;
  Ewk_Security_Origin* origin = (Ewk_Security_Origin*)user_data;
  printf("AppCache usage for %s is %ld\n", ewk_security_origin_host_get(origin), (long int)usage);
  ewk_context_application_cache_delete(ewk_context_default_get(), origin);
  if (eina_list_count(application_cache_origins) == ++count_usage_callback) {
    ewk_context_origins_free(application_cache_origins);
    count_usage_callback = 0;
    application_cache_origins = NULL;
  }
}

static void get_origins_for_cache_cb(Eina_List* origins, void* user_data)
{
  if (!origins)
    return;
  Eina_List* list_iterator = NULL;
  void* data = NULL;
  if (application_cache_origins) {
    fprintf(stderr, "APP/ERROR: First cache usage-delete cycle is still running.\n");
    return;
  }

  application_cache_origins = origins;
  EINA_LIST_FOREACH(origins, list_iterator, data) {
    Ewk_Security_Origin* origin = (Ewk_Security_Origin*)(data);
    ewk_context_application_cache_usage_for_origin_get(ewk_context_default_get(), origin,
                                                       get_app_cache_usage_cb, origin);
  }
}

static void get_origins_for_webDB_cb(Eina_List* origins, void* user_data)
{
  if (!origins)
    return;

  if (web_database_origins)
    return;

  Eina_List* list_iterator = NULL;
  void* data = NULL;
  web_database_origins = origins;
  static int flag = 0;
  EINA_LIST_FOREACH(origins, list_iterator, data) {
    Ewk_Security_Origin* origin = (Ewk_Security_Origin*)(data);
    printf("Web Database origin : %s\n", ewk_security_origin_host_get(origin));
    flag++;
  }
  if(eina_list_count(web_database_origins) == flag) {
    flag = 0;
    ewk_context_origins_free(web_database_origins);
    web_database_origins = NULL;
  }
}

void __customize_context_menu_item_selected_cb(void* data, Evas_Object *obj, void *event_info)
{
  Ewk_Context_Menu_Item* item = (Ewk_Context_Menu_Item*)event_info;
  switch (ewk_context_menu_item_tag_get(item)) {
    case EWK_CONTEXT_MENU_ITEM_TAG_OPEN_LINK_IN_NEW_WINDOW:
      printf("APP.C callback __customize_context_menu_item_selected_cb : Open In new Window Selected\n");

      const int default_width = 300;
      const int default_height =  400;

      Ecore_Evas* ee = ecore_evas_new("software_x11", 0, 0, default_width, default_height, 0);
      Evas* e = ecore_evas_get(ee);

      Evas_Object* new_view = ewk_view_add(e);
      ewk_view_url_set(new_view, ewk_context_menu_item_link_url_get(item));
      evas_object_resize(new_view, default_width, default_height);
      evas_object_show(new_view);
      evas_object_focus_set(new_view, EINA_TRUE);
      ecore_evas_show(ee);
      evas_object_smart_callback_add(new_view, "close,window", __window_closed_cb, 0);
      ecore_evas_callback_delete_request_set(ee, delete_popup_window);
      break;
    case FOR_TEST_APPLICATION_CACHE_RUN:
      if (ewk_context_application_cache_origins_get(ewk_context_default_get(),
                                                    get_origins_for_cache_cb, NULL)) {
        printf("Getting origins for Application Cache\n");
      }
      break;
    case FOR_TEST_APPLICATION_CACHE_DELETE_ALL:
      if (ewk_context_application_cache_delete_all(ewk_context_default_get()))
        printf("All app cache deleted\n");
      break;
    case FOR_TEST_APPLICATION_BACK_FORWAD_CLEAR:
      printf("APP.C callback Bk_fwd_clear callback item selected cb\n");
      ewk_view_back_forward_list_clear(view);
      break;
    case FOR_TEST_GRAB_SNAPSHOT:
      printf("APP.C GRAB_SNAPSHOT selected\n");
      Eina_Rectangle snapshot_rect;
      snapshot_rect.x = snapshot_rect.y = 0;
      evas_object_geometry_get(view, NULL, NULL, &snapshot_rect.w, &snapshot_rect.h);
      Evas_Object* snapshot = ewk_view_screenshot_contents_get(view, snapshot_rect, 1.0, evas_object_evas_get(view));
      if(!snapshot) {
        fprintf(stderr, "Failed to get snapshot\n");
        break;
      }
      static char snapshot_filename[256];
      static int snapshot_count = 1;
      sprintf(snapshot_filename, "snapshot_img%04d.%s", snapshot_count++, "png");
      if (evas_object_image_save(snapshot, snapshot_filename, 0, 0))
        printf("Snapshot image saved in %s\n", snapshot_filename);
      else
        printf("Snapshot image could not be saved\n");
      break;
    case FOR_TEST_WEB_DATABASE_GET_ORIGINS:
      if (ewk_context_web_database_origins_get(ewk_context_default_get(),
                                               get_origins_for_webDB_cb, NULL)) {
        printf("Getting Origins for Web Database\n");
      }
      break;
    case FOR_TEST_WEB_DATABASE_DELETE_ALL:
      if (ewk_context_web_database_delete_all(ewk_context_default_get()))
        printf("All Web Databases deleted\n");
      break;
    default:
      break;
  }
}

void __text_found_cb(void *data, Evas_Object *obj, void *event_info)
{
  printf("text found : %u\n", *((unsigned int*)(event_info)));
}

void __newwindow_policy_cb(void *data, Evas_Object *obj, void* policy)
{
  printf ("APP.C __newwindow_policy_cb called \n");
  Ewk_Policy_Decision *policy_decision = (Ewk_Policy_Decision *)policy;
  const char* url = ewk_policy_decision_url_get(policy_decision);
  printf("APP.C url for new window: %s \n", url);
  ewk_policy_decision_use(policy_decision);
}

void __load_nonemptylayout_finished_cb(void *data, Evas_Object *obj, void *event_info)
{
  printf ("APP.C callback called __load_nonemptylayout_finished_cb \n");
}

void __popup_wait_start_cb(void *data, Evas_Object *obj, void* policy)
{
  printf("Popup wait start\n");
}

void __popup_wait_finish_cb(void *data, Evas_Object *obj, void* policy)
{
  printf("Popup wait finish\n");
}

void __download_callback(const char* download_url, void* user_data)
{
  printf("new download start callback is triggered:%s",download_url);
}

void __frame_rendered_cb(void* data, Evas_Object* obj, void* event_info)
{
  printf ("APP.C callback called __frame_rendered_cb \n");

  if (pdf_filename) {
    int width, height;
    evas_object_geometry_get(obj, NULL, NULL, &width, &height);
    if (ewk_view_contents_pdf_get(obj, width, height, pdf_filename))
      printf("\"Save as PDF\" request sent\n");
    else
      printf("Failed to send \"Save as PDF\" request\n");
  }
}

void __ewk_view_plain_text_get_cb(Evas_Object* o, const char* content_text, void* user_data)
{
  if (content_text)
    printf("__ewk_view_plain_text_get_cb callback success --\n%s\n---user_data--%d\n",content_text,*(int*)(user_data));
}

void __ewk_view_plain_text_get_cb_1(Evas_Object* o, const char* content_text, void* user_data)
{
  if (content_text)
    printf("__ewk_view_plain_text_get_cb_1 callback success --\n%s\n---user_data--%d\n",content_text,*(int*)(user_data));
}

void __load_started_cb(void *data, Evas_Object *obj, void *event_info)
{
  printf("APP.c __load_started_cb called \n");
}

void __edge_left_cb(void* data, Evas_Object* obj, void* event_info)
{
  printf("Edge_Left Notification\n");
}

void __edge_right_cb(void* data, Evas_Object* obj, void* event_info)
{
  printf("Edge_Right Notification\n");
}

void __edge_top_cb(void* data, Evas_Object* obj, void* event_info)
{
  printf("Edge_Top Notification\n");
}

void __edge_bottom_cb(void* data, Evas_Object* obj, void* event_info)
{
  printf("Edge_Bottom Notification\n");
}

#if defined(OS_TIZEN_MOBILE)
Eina_Bool __vibration_timeout_cb(void *data)
{
  g_haptic_timer_id = NULL;
  if (g_haptic_handle) {
#ifndef TIZEN_LEGACY_V_2_2_1
    device_haptic_stop(g_haptic_handle, g_haptic_effect);
    device_haptic_close(g_haptic_handle);
#else
    haptic_stop_effect(g_haptic_handle, g_haptic_effect);
    haptic_close(g_haptic_handle);
#endif
    g_haptic_handle = NULL;
  }

  return ECORE_CALLBACK_CANCEL;
}
#endif

void __vibration_on_cb(uint64_t vibration_time, void *data)
{
  printf("__vibration_on_cb called");
  #if defined(OS_TIZEN_MOBILE)
    uint64_t duration = vibration_time;

    if (g_haptic_timer_id) {
      ecore_timer_del(g_haptic_timer_id);
      g_haptic_timer_id = NULL;
    }

    if (g_haptic_handle) {
#ifndef TIZEN_LEGACY_V_2_2_1
      device_haptic_stop(g_haptic_handle, g_haptic_effect);
      device_haptic_close(g_haptic_handle);
#else
      haptic_stop_effect(g_haptic_handle, g_haptic_effect);
      haptic_close(g_haptic_handle);
#endif
      g_haptic_handle = NULL;
    }

#ifndef TIZEN_LEGACY_V_2_2_1
    if (device_haptic_open(0, &g_haptic_handle) != DEVICE_ERROR_NONE) {
      printf("__vibration_on_cb:device_haptic_open failed");
#else
    if (haptic_open(HAPTIC_DEVICE_0, &g_haptic_handle) != HAPTIC_ERROR_NONE) {
      printf("__vibration_on_cb:haptic_open failed");
#endif
      return;
    }

#ifndef TIZEN_LEGACY_V_2_2_1
    device_haptic_vibrate(g_haptic_handle, duration, 100, &g_haptic_effect);
#else
    haptic_vibrate_monotone(g_haptic_handle, duration, &g_haptic_effect);
#endif
    double in = (double)((double)(duration) / (double)(1000));
    printf("__vibration_on_cb:duration=%f", in);

    g_haptic_timer_id = ecore_timer_add(in, __vibration_timeout_cb, NULL);
  #endif
}

void __vibration_off_cb(void *data)
{
  printf("__vibration_off_cb called");
  #if defined(OS_TIZEN_MOBILE)
    if (g_haptic_timer_id) {
      ecore_timer_del(g_haptic_timer_id);
      g_haptic_timer_id = NULL;
    }

    if (g_haptic_handle) {
#ifndef TIZEN_LEGACY_V_2_2_1
      device_haptic_stop(g_haptic_handle, g_haptic_effect);
      device_haptic_close(g_haptic_handle);
#else
      haptic_stop_effect(g_haptic_handle, g_haptic_effect);
      haptic_close(g_haptic_handle);
#endif
      g_haptic_handle = NULL;
    }
  #endif
}

void __text_style_state_changed_cb(void *data, Evas_Object *obj, void *event_info)
{
  Evas_Point start_point, end_point;
  int r, g, b, a;
  Ewk_Text_Style *style = (Ewk_Text_Style*)event_info;
  printf ("APP.C __text_style_state_changed_cb called \n");
  printf ("APP.c ewk_text_style_underline_get = %d \n",ewk_text_style_underline_get(style));
  printf ("APP.c ewk_text_style_italic_get = %d \n",ewk_text_style_italic_get(style));
  printf ("APP.c ewk_text_style_bold_get = %d \n",ewk_text_style_bold_get(style));
  printf ("APP.c ewk_text_style_ordered_list_get = %d \n",ewk_text_style_ordered_list_get(style));
  printf ("APP.c ewk_text_style_unordered_list_get = %d \n",ewk_text_style_unordered_list_get(style));

  ewk_text_style_position_get(style, &start_point, &end_point);
  printf ("APP.c ewk_text_style_position_get start x = %d, y = %d, end x = %d, y = %d\n",
      start_point.x, start_point.y, end_point.x, end_point.y);

  ewk_text_style_bg_color_get(style, &r, &g, &b, &a);
  printf ("APP.c ewk_text_style_bg_color_get r = %d, g =%d, b = %d, a = %d\n", r, g ,b ,a);
  printf ("APP.c ewk_text_style_has_composition_get = %d \n",ewk_text_style_has_composition_get(style));
  printf ("APP.c ewk_text_style_align_center_get = %d \n",ewk_text_style_align_center_get(style));
  printf ("APP.c ewk_text_style_align_left_get = %d \n",ewk_text_style_align_left_get(style));
  printf ("APP.c ewk_text_style_align_right_get = %d \n",ewk_text_style_align_right_get(style));
  printf ("APP.c ewk_text_style_align_full_get = %d \n",ewk_text_style_align_full_get(style));
}

void __scroll_down_cb(void* data, Evas_Object* obj, void* event_info)
{
  printf("Scroll Down Notification\n");
}

void __scroll_up_cb(void* data, Evas_Object* obj, void* event_info)
{
  printf("Scroll Up Notification\n");
}

void __scroll_left_cb(void* data, Evas_Object* obj, void* event_info)
{
  printf("Scroll Left Notification\n");
}

void __scroll_right_cb(void* data, Evas_Object* obj, void* event_info)
{
  printf("Scroll Right Notification\n");
}

void on_mouse_up(void* data, Evas* canvas, Evas_Object* obj, void* event_info)
{
  Evas_Event_Mouse_Up* event = (Evas_Event_Mouse_Up*)event_info;
  if (!evas_key_modifier_is_set(event->modifiers, "Shift"))
      return;

  Ewk_Hit_Test* ht = ewk_view_hit_test_new(obj, event->canvas.x, event->canvas.y, EWK_HIT_TEST_MODE_ALL);
  printf("HitTest: context %d\n", ewk_hit_test_result_context_get(ht));
  printf("HitTest: link uri %s\n", ewk_hit_test_link_uri_get(ht));
  printf("HitTest: link title %s\n", ewk_hit_test_link_title_get(ht));
  printf("HitTest: link label %s\n",ewk_hit_test_link_label_get(ht));
  printf("HitTest: image uri %s\n", ewk_hit_test_image_uri_get(ht));
  printf("HitTest: media uri %s\n", ewk_hit_test_media_uri_get(ht));
  printf("HitTest: tag name %s\n", ewk_hit_test_tag_name_get(ht));
  printf("HitTest: node value %s\n", ewk_hit_test_node_value_get(ht));
  printf("HitTest: image length %d\n", ewk_hit_test_image_buffer_length_get(ht));
  printf("HitTest: height of image %d\n", FOR_TESTING_ewk_hit_test_image_height_get(ht));
  printf("HitTest: height of image %d\n", FOR_TESTING_ewk_hit_test_image_width_get(ht));
  printf("HitTest: filename extension %s\n", ewk_hit_test_image_file_name_extension_get(ht));
  if (ewk_hit_test_image_buffer_length_get(ht)) {
    // To verify the buffer, we write to file.
    // App might use its own engine to encode, we use evas API.
    Evas_Object* image = evas_object_image_add(e);
    evas_object_image_colorspace_set(image, EVAS_COLORSPACE_ARGB8888);
    evas_object_image_alpha_set(image, EINA_TRUE);
    evas_object_image_size_set(image,
        FOR_TESTING_ewk_hit_test_image_width_get(ht),
        FOR_TESTING_ewk_hit_test_image_height_get(ht));
    evas_object_image_data_copy_set(image, ewk_hit_test_image_buffer_get(ht));
    static char filename_buffer[256];
    static int count = 1;
    sprintf(filename_buffer, "hit_test_img%04d.%s", count++, ewk_hit_test_image_file_name_extension_get(ht));
    if (evas_object_image_save(image, filename_buffer, 0, 0))
      printf("Hit test image saved in %s\n", filename_buffer);
    else
      printf("Hit test image could not be saved\n");
  }

  Eina_Hash *attr_hash = ewk_hit_test_attribute_hash_get(ht);
  if (attr_hash) {
    eina_hash_foreach(attr_hash, _util_print_hit_test_result_hash_fn, NULL);
  }
}

void __undo_size_cb(void* data, Evas_Object* obj, void* event_info)
{
  printf ("APP.C callback called __undo_size_cb\n");
}

void __redo_size_cb(void* data, Evas_Object* obj, void* event_info)
{
  printf ("APP.C callback called __redo_size_cb \n");
}

void __mhtml_save_cb(Evas_Object *obj, const char *mhtml_data, void *data)
{
  printf("APP.C __mhtml_save_cb callback called\n");
  printf("APP.C Data is \n %s \n\n", mhtml_data);
}

void __clipboard_opened_cb(void* data, Evas_Object* view, void* event_info)
{
  printf("APP.C clipboard window opened\n");
}

void __back_forward_list_changed_cb(void *data, Evas_Object *obj, void *event_info)
{
  printf("APP.C Back Forward List Changed Notification\n");
}

Eina_Bool __ewk_view_javascript_alert_cb(Evas_Object* o, const char* alert_text, void* user_data)
{
  printf("APP.C javascript alert opened\n");

  return EINA_TRUE;
}

Eina_Bool __ewk_view_javascript_confirm_cb(Evas_Object* o, const char* message, void* user_data)
{
  printf("APP.C javascript confirm popup opened\n");

  return EINA_TRUE;
}

Eina_Bool __ewk_view_javascript_prompt_cb(Evas_Object* o, const char* message, const char* default_value, void* user_data)
{
  printf("APP.C javascript prompt popup opened\n");

  return EINA_TRUE;
}

void __webprocess_crashed_cb(void* data, Evas_Object* obj, void* event_info)
{
  printf("APP.C callback __webprocess_crashed_cb \n");
  Eina_Bool* handled = (Eina_Bool*)event_info;
  *handled = EINA_TRUE;
  // EWebView (Evas_Object) on which this callback is triggered should be deleted immediately.
  // It is not safe to call any method on it.
  evas_object_del(obj);
  exit(EXIT_FAILURE);
}

Eina_Bool __mime_override_cb(const char* url, const char *mime, char **new_mime)
{
  printf("APP.C callback %s\n", __func__);
  /* Example:
  if (strcmp(url, "file://mytest.abc") == 0 ||
      strcmp(mime, "application/x-php") == 0) {
    *new_mime = strdup("text/html");
    return EINA_TRUE;
  }
  */
  return EINA_FALSE;
}

void __console_message_cb(void *data, Evas_Object *obj, void *event_info)
{
  Ewk_Console_Message* console_message = (Ewk_Console_Message*)event_info;
  printf("APP.C callback called __console_message_cb\n");
  printf("console message:%s:", ewk_console_message_source_get(console_message));
  printf("%d:", ewk_console_message_line_get(console_message));
  printf("%d:", ewk_console_message_level_get(console_message));
  printf("%s\n", ewk_console_message_text_get(console_message));
}
