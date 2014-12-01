// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <app.h>
#include <dlog.h>
#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Ecore_File.h>
#include <Ecore_Getopt.h>
#include <Ecore_X.h>
#include <Evas.h>
#include <Elementary.h>
#include <glib/gprintf.h>
#if defined(OS_TIZEN_TV)
#include <Ecore_IMF_Evas.h>
#endif

#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#if defined(OS_TIZEN_MOBILE)
#ifndef TIZEN_LEGACY_V_2_2_1
#include <device/haptic.h>
#else
#include <haptic/haptic.h>
#endif
#endif

#include "public/ewk_context.h"
#include "public/ewk_context_menu.h"
#include "public/ewk_error.h"
#include "public/ewk_main.h"
#include "public/ewk_policy_decision.h"
#include "public/ewk_geolocation.h"
#include "public/ewk_view.h"
#include "public/ewk_user_media.h"
#include "browser-string.h"
#include "browser-object.h"

#ifdef OS_TIZEN_MOBILE
#include <efl_assist.h>
#endif

#undef LOG_TAG
#define LOG_TAG "CHROMIUMEFL"
#define TOOL_BOX_HEIGHT 150
#define URL_BOX_HEIGHT 75
// SIP height in TV
#if defined(OS_TIZEN_TV)
#define IMF_START_POINT_Y 635
#endif

// To support vibration
#if defined(OS_TIZEN_MOBILE)
static haptic_device_h s_haptic_handle; // haptic handle.
static Ecore_Timer* s_haptic_timer_id; // Timer ID.
static haptic_effect_h s_haptic_effect; // haptic_effect struct.
#endif

static const char default_start_url[] = "http://www.google.com/";
static const char* start_url = NULL;
static int is_imf_shown = 0;
//Added for SIP status
static int im_context_status = 0;
#if defined(OS_TIZEN_TV)
static Evas_Object * curr_focus = NULL;
#endif
// We are just following Tizen platform application pattern.
// It will be required to pass data across lifecycle callbacks.
typedef struct
{
  Evas* e;

  /*let's explain those Evas_Object's hierarchy:
  +main_window
    -webview
    +tool_box
      +url_box
        -url_entry
      +button_box
        -backbtn
        -nextbtn
        -refreshbtn
        -gobtn
        -stopbtn
    -bg_tool_box
    -progress_bar
  */
  Evas_Object *main_window;

  Evas_Object* webview;     //an ewk_view which's app demo is

  Evas_Object *tool_box;    //box containig url_box and button_box

  Evas_Object *url_box;     //box containing only url_entry
  Evas_Object *url_entry;

  Evas_Object *button_box;  //box containing navigation buttons
  Evas_Object *backbtn;
  Evas_Object *nextbtn;
  Evas_Object *refreshbtn;
  Evas_Object *gobtn;
  Evas_Object *stopbtn;

  Evas_Object *bg_tool_box; //background under the toolbox

  Evas_Object *progress_bar;

  Evas_Object *popup; // popup window
  Eina_Stringshare *popup_url;
  Ewk_Geolocation_Permission_Request* geolRequest;
  Ewk_User_Media_Permission_Request* userMediaRequest;
} app_data;

// Added to Support customized context menu
typedef enum _context_menu_type {
	TEXT_ONLY = 0,
	INPUT_FIELD,
	TEXT_LINK,
	IMAGE_ONLY,
	IMAGE_LINK,
	EMAIL_SCHEME,
	TEL_SCHEME,
	UNKNOWN_MENU
} context_menu_type;

typedef enum _custom_context_menu_item_tag {
	CUSTOM_CONTEXT_MENU_ITEM_BASE_TAG = EWK_CONTEXT_MENU_ITEM_BASE_APPLICATION_TAG,
	CUSTOM_CONTEXT_MENU_ITEM_FIND_ON_PAGE,
	CUSTOM_CONTEXT_MENU_ITEM_SHARE,
	CUSTOM_CONTEXT_MENU_ITEM_SEARCH,
	CUSTOM_CONTEXT_MENU_ITEM_SAVE_TO_KEEPIT,
	CUSTOM_CONTEXT_MENU_ITEM_CALL,
	CUSTOM_CONTEXT_MENU_ITEM_SEND_MESSAGE,
	CUSTOM_CONTEXT_MENU_ITEM_SEND_EMAIL,
	CUSTOM_CONTEXT_MENU_ITEM_SEND_ADD_TO_CONTACT,
} custom_context_menu_item_tag;

static void __load_started_cb          (void *data, Evas_Object *obj, void *event_info);
static void __load_finished_cb         (void *data, Evas_Object *obj, void *event_info);
static void __load_progress_finished_cb(void *data, Evas_Object *obj, void *event_info);
static void __load_error_cb            (void *data, Evas_Object *obj, void *event_info);
static void __uri_changed_cb           (void *data, Evas_Object *obj, void *event_info);
static void __title_changed_cb         (void *data, Evas_Object *obj, void *event_info);
static void __load_progress_cb         (void *data, Evas_Object *obj, void *event_info);

#if defined(OS_TIZEN_TV)
static void _url_clicked_cb             (void *data, Evas_Object *obj, void *event_info);
static void _webview_mouse_down_cb      (void *data, Evas *e, Evas_Object *obj, void *event_info);
#endif

static void __policy_newwindow_decide_cb  (void *data, Evas_Object *obj, void *event_info);
static void __navigation_policy_decide_cb(void *data, Evas_Object *obj, void *event_info);
static void __policy_response_decide_cb(void *data, Evas_Object *obj, void *event_info);
static void __geolocation_permission_request_cb(void *data, Evas_Object *obj, void *event_info);
static void __usermedia_permission_request_cb(void *data, Evas_Object *obj, void *event_info);
//Added for load previous page, when keyboard backspace key is pressed
static void webview_key_down_cb       (void *data, Evas *e, Evas_Object *obj, void *event_info);
static void __im_context_open_cb      (void *data, Evas_Object *obj, void *event_info);
static void __im_context_close_cb     (void *data, Evas_Object *obj, void *event_info);
static Evas_Object* _create_view      (Evas_Object *parent, app_data *data);
static Evas_Object* _create_button_bar(Evas_Object *parent, app_data *data);
static Evas_Object* _create_url_bar   (Evas_Object *parent, int width, const char *start_url , app_data *data);
static Evas_Object* _create_tool_bar  (Evas_Object *parent, const char *start_url , app_data *data);

static void __popup_close(app_data *data);
static void __popup_open_url_cb(void *data, Evas_Object *obj, void *event_info);
static void __popup_cancel_url_cb(void *data, Evas_Object *obj, void *event_info);

static void url_hit_cb(void *data, Evas_Object *obj, void *event_info);
static void _imf_panel_state_cb (void *data, Ecore_IMF_Context *ctx, int value);
#if defined(OS_TIZEN_MOBILE)
static void _imf_panel_changed_cb(void *data, Evas_Object *obj, Eina_Rectangle *rect);
#endif

static void _geol_cancel_cb (void *data, Evas_Object *obj, void *ev);
static void _geol_ok_cb     (void *data, Evas_Object *obj, void *ev);
static void _back_btn_cb    (void *data, Evas_Object *btn, void *ev);
static void _next_btn_cb    (void *data, Evas_Object *btn, void *ev);
static void _reload_btn_cb  (void *data, Evas_Object *btn, void *ev);
static void _stop_btn_cb    (void *data, Evas_Object *btn, void *ev);
static void hide_imf_panel(app_data * data);
#ifdef OS_TIZEN
static void _back_hard_key_cb(void *data, Evas_Object *btn, void *ev);
#endif

// Added to Support cusomized context menu
static void __customize_context_menu_cb(void *data, Evas_Object *obj, void *event_info);
static void __customize_context_menu_item_selected_cb(void *data, Evas_Object *obj, void *event_info);

// To Support customized context menu
static void enable_customize_contextmenu(app_data *data, Eina_Bool enable);
static void show_context_menu_text_only(app_data *data, Ewk_Context_Menu *menu);
static void show_context_menu_text_link(app_data *data, Ewk_Context_Menu *menu);
static void show_context_menu_image_link(app_data *data, Ewk_Context_Menu *menu);
static void show_context_menu_call_number(app_data *data, Ewk_Context_Menu *menu);
static void show_context_menu_email_address(app_data *data, Ewk_Context_Menu *menu);
static void show_context_menu_image_only(app_data *data, Ewk_Context_Menu *menu);
context_menu_type get_menu_type(app_data *data, Ewk_Context_Menu *menu);

//  Added to Support Javascript popup
static Eina_Bool __javascript_alert_cb(Evas_Object* o, const char* alert_text, void* user_data);
static Eina_Bool __javascript_confirm_cb(Evas_Object* o, const char* message, void* user_data);
static Eina_Bool __javascript_prompt_cb(Evas_Object* o, const char* message, const char* default_value, void* user_data);

// To support vibration
#if defined(OS_TIZEN_MOBILE)
static Eina_Bool __vibration_timeout_cb(void *data);
static void __vibration_on_cb(uint64_t vibration_time, void *data);
static void __vibration_off_cb(void *data);
#endif

static void get_main_window_size(int *changed_ang, int *width, int *height);

enum
{
  FOR_TEST_APPLICATION_CACHE_RUN = EWK_CONTEXT_MENU_ITEM_BASE_APPLICATION_TAG + 1,
  FOR_TEST_APPLICATION_CACHE_DELETE_ALL,
  FOR_TEST_APPLICATION_BACK_FORWAD_CLEAR,
  // New context menu items can be added here
};

static const Ecore_Getopt options =
{
  .prog = "mini_browser",
  .usage = "%prog [options] [url]",
  .strict = EINA_TRUE,
  .descs =
  {
    ECORE_GETOPT_STORE_DEF_STR('u', "url", "go to url", default_start_url),
    ECORE_GETOPT_SENTINEL,
  }
};

static Eina_Strbuf* create_url_with_scheme(const char* base_url, const char* scheme)
{
  Eina_Strbuf* buffer = eina_strbuf_new();
  eina_strbuf_append(buffer, scheme);
  eina_strbuf_append(buffer, base_url);
  return buffer;
}

static void set_url_from_user_input(Evas_Object* webview, const char* url)
{
  if (strstr(url, "://"))
  {
    ewk_view_url_set(webview, url);
    return;
  }

  // Add scheme.
  Eina_Strbuf* full_url;
  if (ecore_file_exists(url))
  {
    char* realpath = ecore_file_realpath(url);
    full_url = create_url_with_scheme(realpath, "file://");
    free(realpath);
  }
  else
  {
    full_url = create_url_with_scheme(url, "http://");
  }

  ewk_view_url_set(webview, eina_strbuf_string_get(full_url));

  eina_strbuf_free(full_url);
}

static Evas_Object* _create_main_window(void *data)
{
  Evas_Object *window = elm_win_add(NULL, "ChromiumEfl", ELM_WIN_BASIC);
  if (window) {
    int width, height;
    elm_win_title_set(window, "ChromiumEfl");
    evas_object_size_hint_weight_set(window, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(window, EVAS_HINT_FILL, EVAS_HINT_FILL);
    elm_win_borderless_set(window, EINA_TRUE);
    elm_win_conformant_set(window, EINA_TRUE);
    ecore_x_window_size_get(ecore_x_window_root_first_get(), &width, &height);
    evas_object_resize(window, width, height);
    elm_win_indicator_mode_set(window, ELM_WIN_INDICATOR_SHOW);

    /* add a standard bg */
    Evas_Object *bg = elm_bg_add(window);
    /* not allow bg to expand. let's limit dialog size to contents */
    evas_object_size_hint_weight_set(bg, 1.0, 1.0);
    /* add object as a resize object for the window (controls window minimum
     * size as well as gets resized if window is resized) */
    elm_win_resize_object_add(window, bg);
    elm_bg_color_set(bg, 225, 225, 225);
    evas_object_show(bg);
  }
  return window;
}

Evas_Object* _create_view(Evas_Object *parent, app_data *data)
{
  if (start_url == NULL)
  {
    start_url = default_start_url;
  }

  int width, height;
  evas_object_geometry_get(parent, NULL, NULL, &width, &height);

  Evas_Object *webview = ewk_view_add_with_context(evas_object_evas_get(parent), ewk_context_default_get() );
  evas_object_resize(webview, width, height - TOOL_BOX_HEIGHT);

  evas_object_show(webview);
  evas_object_focus_set(webview, EINA_TRUE);

  data->webview = webview;

  evas_object_smart_callback_add(webview, "load,finished"         , __load_finished_cb         , data);
  evas_object_smart_callback_add(webview, "load,error"            , __load_error_cb            , data);
  evas_object_smart_callback_add(webview, "url,changed"           , __uri_changed_cb           , data);
  evas_object_smart_callback_add(webview, "title,changed"         , __title_changed_cb         , data);
  evas_object_smart_callback_add(webview, "load,progress"         , __load_progress_cb         , data);
  evas_object_smart_callback_add(webview, "load,progress,finished", __load_progress_finished_cb, data);
  evas_object_smart_callback_add(webview, "load,started"          , __load_started_cb          , data);
  evas_object_smart_callback_add(webview, "policy,newwindow,decide", __policy_newwindow_decide_cb, data);
  #if defined(OS_TIZEN_TV)
  evas_object_event_callback_add(webview, EVAS_CALLBACK_MOUSE_DOWN, _webview_mouse_down_cb       , data);
  #endif
  evas_object_smart_callback_add(webview, "policy,response,decide", __policy_response_decide_cb, data);
  evas_object_smart_callback_add(webview, "policy,navigation,decide", __navigation_policy_decide_cb, data);
  evas_object_smart_callback_add(webview, "geolocation,permission,request", __geolocation_permission_request_cb, data);
  evas_object_smart_callback_add(webview, "usermedia,permission,request", __usermedia_permission_request_cb, data);
  //  Added to Support Javascript popup
  ewk_view_javascript_alert_callback_set(webview, __javascript_alert_cb, data);
  ewk_view_javascript_confirm_callback_set(webview, __javascript_confirm_cb, data);
  ewk_view_javascript_prompt_callback_set(webview, __javascript_prompt_cb, data);

  // To support vibration
#if defined(OS_TIZEN_MOBILE)
  printf("[%s][%d][%s] calling ewk_context_vibration_client_callbacks_set...\n", __FUNCTION__, __LINE__, "vibration");
  ewk_context_vibration_client_callbacks_set(webview, __vibration_on_cb, __vibration_off_cb, NULL);
#endif

  //Added to support keyboard backspace key
  evas_object_smart_callback_add(webview, "editorclient,ime,opened", __im_context_open_cb  , data);
  evas_object_smart_callback_add(webview, "editorclient,ime,closed", __im_context_close_cb , data);
  evas_object_event_callback_add(webview, EVAS_CALLBACK_KEY_DOWN, webview_key_down_cb , data);

  // To Enable customized context menu
  enable_customize_contextmenu(data, EINA_TRUE);

#if defined(OS_TIZEN_MOBILE)
  evas_object_smart_callback_add(webview, "inputmethod,changed", _imf_panel_changed_cb , data);
#endif

  // Show Ime only on user action
  ewk_settings_uses_keypad_without_user_action_set(ewk_view_settings_get(webview), false);

  return webview;
}

#if defined(OS_TIZEN_TV)
// To bring back toolbar to original position when SID_DONE/SID_CANCEL is pressed
static void _SIP_close_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
  Evas_Event_Key_Down *ev = (Evas_Event_Key_Down*) event_info;
  app_data *ad = (app_data*) data;
  if (!strcmp(ev->key, "Select"))
  {
    const char* url = elm_entry_entry_get( ad->url_entry );
    set_url_from_user_input(ad->webview, url);
    hide_imf_panel(ad);
    evas_object_focus_set(ad->webview, EINA_TRUE);
  }
  if (!strcmp(ev->key, "Cancel"))
  {
    hide_imf_panel(ad);
  }
}

/* On Mouse Event over webview should bring back
 * Tool bar to its original position
 */
void _webview_mouse_down_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
  app_data *ad = (app_data*) data;
  hide_imf_panel(ad);
}
#endif

//Added to load previous page, when keyboard backspace key is pressed
void webview_key_down_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
  app_data *ad = (app_data*) data;
  Evas_Event_Key_Down *ev = (Evas_Event_Key_Down*) event_info;

  if (strcmp(ev->key, "XF86Stop") == 0)
  {
    if (ewk_view_text_selection_clear(ad->webview))
      return;
  }
  if ( (!strcmp(ev->key, "BackSpace") || !strcmp(ev->key, "XF86Stop")) && (!im_context_status) )
  {
    ewk_view_back(ad->webview);
  }
}

Evas_Object* _create_button_bar(Evas_Object *parent , app_data *data)
{
  Evas_Object *ic;
  data->button_box = elm_box_add(parent);
  elm_box_horizontal_set(data->button_box, EINA_TRUE);
  elm_box_padding_set(data->button_box,2,2);

  data->backbtn=elm_button_add(data->button_box);
  elm_box_pack_end(data->button_box, data->backbtn);
  evas_object_smart_callback_add(data->backbtn, "clicked", _back_btn_cb, data);

  ic=elm_icon_add(parent);
  elm_icon_standard_set(ic, "arrow_left");
  elm_object_part_content_set(data->backbtn, "icon", ic);
  evas_object_show(data->backbtn);

  data->nextbtn=elm_button_add(data->button_box);
  elm_box_pack_end(data->button_box, data->nextbtn);
  evas_object_smart_callback_add(data->nextbtn, "clicked", _next_btn_cb, data);

  ic=elm_icon_add(parent);
  elm_icon_standard_set(ic, "arrow_right");
  elm_object_part_content_set(data->nextbtn, "icon", ic);
  evas_object_show(data->nextbtn);

  data->refreshbtn=elm_button_add(data->button_box);
  elm_box_pack_end(data->button_box, data->refreshbtn);

  ic=elm_icon_add(parent);
  elm_icon_standard_set(ic, "refresh");
  elm_object_part_content_set(data->refreshbtn, "icon", ic);
  evas_object_show(data->refreshbtn);
  evas_object_smart_callback_add(data->refreshbtn, "clicked", _reload_btn_cb, data);

  data->stopbtn=elm_button_add(data->button_box);
  elm_box_pack_end(data->button_box, data->stopbtn);

  ic=elm_icon_add(parent);
  elm_icon_standard_set(ic, "close");
  elm_object_part_content_set(data->stopbtn, "icon", ic);
  evas_object_show(data->stopbtn);
  evas_object_smart_callback_add(data->stopbtn, "clicked", _stop_btn_cb, data);

  data->gobtn=elm_button_add(data->button_box);
  elm_object_text_set(data->gobtn, "GO");
  elm_box_pack_end(data->button_box,data->gobtn);
  evas_object_show(data->gobtn);

  evas_object_smart_callback_add(data->gobtn, "clicked", url_hit_cb, data);
  return data->button_box;
}

Evas_Object* _create_url_bar(Evas_Object *parent, int width, const char *start_url, app_data *data )
{
  data->url_box = elm_box_add(parent);
  elm_box_horizontal_set(data->url_box, EINA_TRUE);
  evas_object_size_hint_weight_set(data->url_box, EVAS_HINT_EXPAND, 0.0);
  evas_object_size_hint_align_set(data->url_box, EVAS_HINT_FILL, EVAS_HINT_FILL);
  evas_object_resize(data->url_box, width, URL_BOX_HEIGHT);
  evas_object_show(data->url_box);

  data->url_entry = elm_entry_add(data->url_box);
  elm_entry_single_line_set(data->url_entry, EINA_TRUE);
  elm_entry_scrollable_set(data->url_entry, EINA_TRUE);
  evas_object_size_hint_weight_set(data->url_entry, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_align_set(data->url_entry, EVAS_HINT_FILL, EVAS_HINT_FILL);
  elm_entry_text_style_user_push(data->url_entry,
     "DEFAULT='font=Sans font_size=35 color=#000000 ' hilight=' + font_weight=Bold'");
  elm_object_text_set(data->url_entry, start_url);
  elm_entry_input_panel_layout_set(data->url_entry, ELM_INPUT_PANEL_LAYOUT_URL);
  elm_entry_autocapital_type_set  (data->url_entry, ELM_AUTOCAPITAL_TYPE_NONE );
  elm_entry_editable_set(data->url_entry, EINA_TRUE);
  evas_object_color_set(data->url_entry, 255, 255, 255, 255);
  elm_box_padding_set(data->url_box, 5,5);
  elm_box_pack_end(data->url_box, data->url_entry);
  evas_object_show(data->url_entry);
  evas_object_smart_callback_add(data->url_entry, "activated", url_hit_cb, data);
  #if defined(OS_TIZEN_TV)
  evas_object_event_callback_add(data->url_entry, EVAS_CALLBACK_KEY_DOWN, _SIP_close_cb, data);
  evas_object_smart_callback_add(data->url_entry, "clicked", _url_clicked_cb, data);
  #endif
  elm_entry_prediction_allow_set(data->url_entry, EINA_FALSE);
#ifdef OS_TIZEN_MOBILE
  ea_object_event_callback_add(data->url_entry, EA_CALLBACK_BACK, _back_hard_key_cb, data);
#endif
  // Add imf event
  Ecore_IMF_Context *imf_context = elm_entry_imf_context_get(data->url_entry);
  if (imf_context) {
    ecore_imf_context_prediction_allow_set(imf_context, EINA_FALSE);
    ecore_imf_context_input_panel_event_callback_add (
        imf_context, ECORE_IMF_INPUT_PANEL_STATE_SHOW, _imf_panel_state_cb, data);
    ecore_imf_context_input_panel_event_callback_add (
        imf_context, ECORE_IMF_INPUT_PANEL_STATE_HIDE, _imf_panel_state_cb, data);
  }
  evas_object_show(data->url_entry);

  return data->url_box;
}

Evas_Object* _create_tool_bar(Evas_Object *parent, const char *start_url, app_data *data)
{
  //these pointers are local. data _create_url_bar and _create_button_bar
  //puts pointers to the right field in app_data structure
  Evas_Object *button_box, *url_box;

  int width, height;
  evas_object_geometry_get (parent, NULL, NULL, &width, &height);

  /* add a standard bg */
  data->bg_tool_box = evas_object_rectangle_add( evas_object_evas_get(parent) );
  //Here we set the rectangles red, green, blue and opacity levels
  evas_object_color_set(data->bg_tool_box, 225, 225, 225, 255); // opaque white background
  evas_object_resize(data->bg_tool_box, width, TOOL_BOX_HEIGHT);     // covers full canvas

  data->tool_box = elm_box_add(parent);
  evas_object_move(data->tool_box, 0, height - TOOL_BOX_HEIGHT);
  evas_object_resize(data->tool_box, width, TOOL_BOX_HEIGHT);
  evas_object_show(data->tool_box);

  url_box = _create_url_bar(data->tool_box, width, start_url, data);
  button_box = _create_button_bar(data->tool_box, data);

  elm_box_pack_end(data->tool_box,url_box);
  evas_object_propagate_events_set(data->tool_box, EINA_FALSE);
  evas_object_size_hint_align_set(button_box, 0.0, 0.0);
  elm_box_pack_end(data->tool_box,button_box);
  evas_object_show(button_box);
  evas_object_show(url_box);

  Elm_Theme *theme = elm_theme_new();
  elm_theme_set(theme, "tizen-HD-light");
  elm_object_theme_set(data->tool_box, theme);
  evas_object_show(data->tool_box);

  return data->tool_box;
}

//FIXME: The commented line below is for demo.
//#if defined(OS_TIZEN_MOBILE)
static void __br_rotate_cb(void *data, Evas_Object *obj, void *event)
{
  if (!data)
    return;

  app_data *ad = (app_data *)data;
  int changed_ang = elm_win_rotation_get(ad->main_window);
  int main_window_width  = 0, main_window_height = 0;
  get_main_window_size(&changed_ang, &main_window_width, &main_window_height);

  // ewk_view_orientation_send expects angles: 0, 90, -90, 180.
  if (changed_ang == 270)
    changed_ang = -90;
  ewk_view_orientation_send(ad->webview, changed_ang);

  evas_object_resize(ad->webview, main_window_width, main_window_height - TOOL_BOX_HEIGHT);
  evas_object_resize(ad->tool_box, main_window_width, TOOL_BOX_HEIGHT);
  evas_object_move(ad->tool_box, 0, main_window_height - TOOL_BOX_HEIGHT);
}

static void _register_rotate_callback(Evas_Object *main_window, void *user_data)
{
  if (elm_win_wm_rotation_supported_get(main_window)) {
    const int rots[] = { APP_DEVICE_ORIENTATION_0, APP_DEVICE_ORIENTATION_90, APP_DEVICE_ORIENTATION_270 };
    elm_win_wm_rotation_available_rotations_set(main_window, rots, (sizeof(rots) / sizeof(int)));
  }
  evas_object_smart_callback_add(main_window, "wm,rotation,changed", __br_rotate_cb, user_data);
}
//#endif //#if defined(OS_TIZEN_MOBILE)

#if defined(OS_TIZEN_MOBILE)
static void _on_focus_in_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
  app_data *ad = (app_data *) data;
  set_url_from_user_input(ad->webview, start_url);
  evas_object_event_callback_del(ad->main_window, EVAS_CALLBACK_FOCUS_IN, _on_focus_in_cb);
}
#endif

static bool __br_app_create(void *data)
{
  elm_init(0, NULL);
  if (!ewk_init()) {
    LOGE("elm_init Failed. App not started");
    return false;
  }

  elm_config_preferred_engine_set("opengl_x11");
  app_data *ad = (app_data *) data;

  ad->main_window = _create_main_window(ad);

  // All ewk things go here
  ad->e = evas_object_evas_get (ad->main_window);


  // Create view and tool bar
  _create_view(ad->main_window, ad);

  _create_tool_bar(ad->main_window, start_url, ad);

  // Add progress bar
  ad->progress_bar = evas_object_rectangle_add(ad->e);
  evas_object_color_set(ad->progress_bar, 87, 44, 230, 200);
  evas_object_resize(ad->progress_bar, 0, 4);
  evas_object_show(ad->progress_bar);

  evas_object_show(ad->main_window);
#if defined(OS_TIZEN_MOBILE)
  evas_object_event_callback_add(ad->main_window, EVAS_CALLBACK_FOCUS_IN, _on_focus_in_cb, ad);
#else
  set_url_from_user_input(ad->webview, start_url);
#endif
//FIXME: The commented line below is for demo.
//#if defined(OS_TIZEN_MOBILE)
  _register_rotate_callback(ad->main_window, ad);
//#endif
  return true;
}
static bool __br_app_terminate(void* data)
{
  #if defined(OS_TIZEN_TV)
  curr_focus = NULL;
  hide_imf_panel(data);
  #endif
  return true;
}

static void __br_app_pause(void* data)
{
}

static void __br_app_resume(void* data)
{
}

int main(int argc, char** argv)
{
  LOGD(" Minibrowser main\n");

  //FIXME: this is temporal code.
  ewk_set_arguments(argc, argv);

  Ecore_Getopt_Value values[] =
  {
    ECORE_GETOPT_VALUE_STR(start_url),
    ECORE_GETOPT_VALUE_NONE,
  };

  printf("MINIB.C main \n");
  if (ecore_getopt_parse(&options, values, argc, argv) < 0)
  {
    LOGE("ERROR: could not parse options.\n");
    return EXIT_FAILURE;
  }

  app_event_callback_s ops;
  memset(&ops, 0x00, sizeof(app_event_callback_s));
  ops.create = __br_app_create;
  ops.terminate = __br_app_terminate;
  ops.pause = __br_app_pause;
  ops.resume = __br_app_resume;

#ifdef TIZEN_MOBILE_LEGACY_V_2_1
  ops.service = NULL;
#endif
  ops.low_memory = NULL;
  ops.low_battery = NULL;
  ops.device_orientation = NULL;
  ops.language_changed = NULL;
  ops.region_format_changed = NULL;

  app_data ad;
  memset(&ad, 0x00, sizeof(app_data));
  int ret = app_efl_main(&argc, &argv, &ops, &ad);
  return ret;
}

void get_main_window_size(int *changed_ang, int *width, int *height)
{
  switch (*changed_ang) {
    case APP_DEVICE_ORIENTATION_0:
      ecore_x_window_size_get(ecore_x_window_root_first_get(), width, height);
      break;
    case APP_DEVICE_ORIENTATION_90:
    case APP_DEVICE_ORIENTATION_270:
      ecore_x_window_size_get(ecore_x_window_root_first_get(), height, width);
      break;
    default:
      return;
  }
}

void __load_started_cb(void *data, Evas_Object *obj, void *event_info)
{
  LOGD("Start Loading \n");
  app_data *ad = (app_data *) data;
  evas_object_show(ad->progress_bar);
}

//To check SIP status
void __im_context_open_cb(void *data, Evas_Object *obj, void *event_info)
{
  LOGD("IM_CONTEXT_EFL_ON \n");
  im_context_status = 1;
}

void __im_context_close_cb(void *data, Evas_Object *obj, void *event_info)
{
  LOGD("IM_CONTEXT_EFL_OFF \n");

#if defined(OS_TIZEN_MOBILE)
  app_data *ad = (app_data *)data;
  int changed_ang = elm_win_rotation_get(ad->main_window);
  int main_window_width  = 0, main_window_height = 0;
  get_main_window_size(&changed_ang, &main_window_width, &main_window_height);
  evas_object_resize(ad->webview, main_window_width, main_window_height - TOOL_BOX_HEIGHT);
#endif

  im_context_status = 0;
}

#if defined(OS_TIZEN_MOBILE)
void _imf_panel_changed_cb(void *data, Evas_Object *obj, Eina_Rectangle *rect)
{
  printf("__imf_panel_changed_cb\n");
  LOGD("IM_CONTEXT_EFL_OFF \n");

  app_data *ad = (app_data *)data;
  int changed_ang = elm_win_rotation_get(ad->main_window);
  int main_window_width  = 0, main_window_height = 0;
  get_main_window_size(&changed_ang, &main_window_width, &main_window_height);

  if (rect->h < TOOL_BOX_HEIGHT)
    evas_object_resize(ad->webview, main_window_width, main_window_height - TOOL_BOX_HEIGHT);
  else
    evas_object_resize(ad->webview, main_window_width, main_window_height - rect->h);
}
#endif

void __load_progress_finished_cb(void *data, Evas_Object *obj, void *event_info)
{
  LOGD("Ending Loading \n");
  app_data *ad = (app_data *) data;
  evas_object_resize(ad->progress_bar, 0, 10);
  evas_object_hide(ad->progress_bar);
}

void __load_finished_cb(void *data, Evas_Object *obj, void *event_info)
{
  LOGD("Load Done \n");
  app_data *ad = (app_data *) data;
  evas_object_focus_set(ad->webview, EINA_TRUE);

}

void __load_progress_cb(void *data, Evas_Object *obj, void *event_info)
{
  LOGD("Progress = %f \n", *((double *)event_info));
  app_data *ad = (app_data *) data;
  int width;
  evas_object_geometry_get (ad->main_window, NULL, NULL, &width, NULL);
  evas_object_resize(ad->progress_bar, (*((double *)event_info))*width, 10);
}

void __load_error_cb(void *data, Evas_Object *obj, void *event_info)
{
  Ewk_Error* error = (Ewk_Error*)event_info;
  LOGD("ERROR URL : %s\n", ewk_error_url_get(error));
  LOGD("ERROR DESCRIPTION : %s\n", ewk_error_description_get(error));

  /*do nothing if user action aborts loading page which is not
  considered as a fault, else display error description in webview*/
  const char* errorDescription=ewk_error_description_get(error);
  if( strcmp(errorDescription,"net::ERR_ABORTED") != 0 )
  {
    ewk_view_not_found_error_page_load(obj, ewk_error_url_get(error));
  }
}

void __uri_changed_cb(void *data, Evas_Object *obj, void *event_info)
{
  LOGD("URI is = %s \n", (const char *)event_info);
  app_data *ad = (app_data*) data;
  elm_object_text_set(ad->url_entry, (const char *)event_info);
}

void __title_changed_cb(void *data, Evas_Object *obj, void *event_info)
{
  LOGD("Title is = %s \n", (const char *)event_info);
}

void __popup_close(app_data* ad)
{
  if (ad->popup) {
    evas_object_focus_set(ad->webview, EINA_TRUE);
    evas_object_hide(ad->popup);
    evas_object_del(ad->popup);
    ad->popup = NULL;
    eina_stringshare_del(ad->popup_url);
    ad->popup_url = NULL;
  }
}

void __popup_open_url_cb(void *data, Evas_Object *obj, void *event_info)
{
  app_data* ad = (app_data*)data;
  ewk_view_url_set(ad->webview, ad->popup_url);
  __popup_close(ad);
}

void __popup_cancel_url_cb(void *data, Evas_Object *obj, void *event_info)
{
  app_data* ad = (app_data*)data;
  __popup_close(ad);
}

void __policy_newwindow_decide_cb(void *data, Evas_Object *obj, void *event_info)
{
  app_data* ad = (app_data*)data;

  if (ad->popup) {
    __popup_close(ad);
  }

  Ewk_Policy_Decision* policy_decision = (Ewk_Policy_Decision*)event_info;
  Evas_Object* popup = elm_popup_add(ad->main_window);
  ad->popup_url = eina_stringshare_add(ewk_policy_decision_url_get(policy_decision));
  gchar* title = g_strdup_printf("Do you want to open \"%s\" in current window", ad->popup_url);
  elm_object_part_text_set(popup, "title,text", "New window request");
  elm_object_text_set(popup, title);
  g_free(title);

  Evas_Object *open = elm_button_add(popup);
  elm_object_text_set(open, "Open");
  elm_object_part_content_set(popup, "button1", open);
  evas_object_smart_callback_add(open, "clicked", __popup_open_url_cb, ad);

  Evas_Object *cancel = elm_button_add(popup);
  elm_object_text_set(cancel, "Cancel");
  elm_object_part_content_set(popup, "button2", cancel);
  evas_object_smart_callback_add(cancel, "clicked", __popup_cancel_url_cb, ad);

  ad->popup = popup;
  evas_object_show(popup);
}

void _geol_ok_cb (void *data, Evas_Object *obj, void *event_info)
{
  LOGD("geolocation permission granted");
  app_data* ad = (app_data*)data;
  ewk_geolocation_permission_request_set( ad->geolRequest, EINA_TRUE);
  __popup_close(ad);
}


void _geol_cancel_cb (void *data, Evas_Object *obj, void *event_info)
{
  LOGD("geolocation restricted");
  app_data* ad = (app_data*)data;
  ewk_geolocation_permission_request_set( ad->geolRequest, EINA_FALSE);
  __popup_close(ad);
}

void __geolocation_permission_request_cb(void *data, Evas_Object *obj, void *event_info)
{
  LOGD("geolocation request recieved");
  app_data* ad = (app_data*)data;

  if (ad->popup) {
    __popup_close(ad);
  }
  Ewk_Geolocation_Permission_Request* permissionRequest=
      (Ewk_Geolocation_Permission_Request*)event_info;
  Evas_Object* popup = elm_popup_add(ad->main_window);
  ewk_geolocation_permission_request_suspend(permissionRequest);
  ad->geolRequest=permissionRequest;

  elm_object_part_text_set(popup, "title,text", "Geolocation permission request");
  elm_object_text_set(popup,"Do you want to allow this page to access geolocation data?");

  Evas_Object *ok = elm_button_add(popup);
  elm_object_text_set(ok, "ok");
  elm_object_part_content_set(popup, "button1", ok);
  evas_object_smart_callback_add(ok, "clicked", _geol_ok_cb, ad);

  Evas_Object *cancel = elm_button_add(popup);
  elm_object_text_set(cancel, "Cancel");
  elm_object_part_content_set(popup, "button2", cancel);
  evas_object_smart_callback_add(cancel, "clicked", _geol_cancel_cb, ad);
  ad->popup = popup;
  evas_object_show(popup);
}

// Callback for Allow button press for media request
void _usermedia_ok_cb (void *data, Evas_Object *obj, void *event_info)
{
  Ewk_User_Media_Permission_Request* permissionRequest=
      (Ewk_User_Media_Permission_Request*)event_info;
  app_data* ad = (app_data*)data;
  ewk_user_media_permission_request_set(ad->userMediaRequest,EINA_TRUE);
    __popup_close(ad);
}

// Callback for Deny button press for media request
void _usermedia_cancel_cb (void *data, Evas_Object *obj, void *event_info)
{
  app_data* ad = (app_data*)data;
  Ewk_User_Media_Permission_Request* permissionRequest=
      (Ewk_User_Media_Permission_Request*)event_info;
  ewk_user_media_permission_request_set(ad->userMediaRequest,EINA_FALSE);
  __popup_close(ad);
}

// Callback for showing permission request for media request
void __usermedia_permission_request_cb(void *data, Evas_Object *obj, void *event_info)
{
  app_data* ad = (app_data*)data;
  if (ad->popup) {
    __popup_close(ad);
  }
  Ewk_User_Media_Permission_Request* permissionRequest=
      (Ewk_User_Media_Permission_Request*)event_info;
  Evas_Object* popup = elm_popup_add(ad->main_window);
  ewk_user_media_permission_request_suspend(permissionRequest);

  ad->userMediaRequest=permissionRequest;
  elm_object_part_text_set(popup, "title,text", "Media permission request");
  elm_object_text_set(popup,"Do you want to allow this page to access Media Resources?");

  Evas_Object *ok = elm_button_add(popup);
  elm_object_text_set(ok, "ok");
  elm_object_part_content_set(popup, "button1", ok);
  evas_object_smart_callback_add(ok, "clicked", _usermedia_ok_cb, ad);

  Evas_Object *cancel = elm_button_add(popup);
  elm_object_text_set(cancel, "Cancel");
  elm_object_part_content_set(popup, "button2", cancel);
  evas_object_smart_callback_add(cancel, "clicked", _usermedia_cancel_cb, ad);
  ad->popup = popup;
  evas_object_show(popup);
}

void url_hit_cb(void *data, Evas_Object *obj, void *event_info)
{
  LOGD("Send load request\n");
  app_data *ad = (app_data*) data;
  const char* url = elm_entry_entry_get( ad->url_entry );
  set_url_from_user_input(ad->webview, url);
  hide_imf_panel(ad);
  evas_object_focus_set(ad->webview, EINA_TRUE);
}

#if defined(OS_TIZEN_TV)
/* On Click to Urlbar need to move Toolbar top of SIP
 * so that SIP and toolbar should not overlap
 */
void _url_clicked_cb(void *data, Evas_Object *obj, void *event_info)
{
  app_data *ad = (app_data*) data;
  elm_entry_input_panel_enabled_set(ad->url_entry, EINA_TRUE);
  if( curr_focus == NULL )
    curr_focus = evas_focus_get(ad->e);
  evas_object_focus_set(curr_focus, EINA_TRUE);
  Ecore_IMF_Context *imf_context = elm_entry_imf_context_get(ad->url_entry);
  if (imf_context)
  {
    if(is_imf_shown == 0 && ecore_imf_context_input_panel_state_get(imf_context) != ECORE_IMF_INPUT_PANEL_STATE_SHOW)
    {
      is_imf_shown = 1;
      int x, y, w, h;
      int width, height;
      evas_object_geometry_get (ad->bg_tool_box, NULL, NULL, &width, &height);
      ecore_imf_context_input_panel_geometry_get (imf_context, &x, &y, &w, &h);
      //evas_object_resize(ad->webview, width, IMF_START_POINT_Y  - TOOL_BOX_HEIGHT); //To resize webview size above toolbar
      evas_object_move(ad->bg_tool_box, 0, IMF_START_POINT_Y - TOOL_BOX_HEIGHT);
      evas_object_show(ad->bg_tool_box);
      evas_object_move(ad->tool_box, 0, IMF_START_POINT_Y - TOOL_BOX_HEIGHT);
      elm_entry_input_panel_show(ad->url_entry);
    }
  }
}
#endif

void _imf_panel_state_cb (void *data, Ecore_IMF_Context *ctx, int value)
{
  LOGD("_imf_panel_state_cb show\n");
  app_data *ad = (app_data*) data;
  int x=0, y=0, w=0, h=0;
  if (value == ECORE_IMF_INPUT_PANEL_STATE_SHOW)
  {
    LOGD("Keypad Up\n");
    ecore_imf_context_input_panel_geometry_get (ctx, &x, &y, &w, &h);
    evas_object_move(ad->bg_tool_box, 0, y - TOOL_BOX_HEIGHT);
    evas_object_show(ad->bg_tool_box);
    evas_object_move(ad->tool_box, 0, y - TOOL_BOX_HEIGHT);
    is_imf_shown = 1;
  }
  else if (value == ECORE_IMF_INPUT_PANEL_STATE_HIDE)
  {
    LOGD("Keypad down\n");
    evas_object_geometry_get(ad->main_window, NULL, NULL, &w, &h);
    evas_object_move(ad->tool_box, 0, h - TOOL_BOX_HEIGHT);
    evas_object_hide(ad->bg_tool_box);
  }
}

void hide_imf_panel(app_data* data)
{
  #if defined(OS_TIZEN_TV)
/* On SIP close, Toolbar bring back to original position*/
  if (is_imf_shown == 1)
  {
    int width, height;
    ecore_x_window_size_get(ecore_x_window_root_first_get(), &width, &height);
    //evas_object_resize(data->webview, width, height - TOOL_BOX_HEIGHT);
    evas_object_move(data->bg_tool_box, 0, height - TOOL_BOX_HEIGHT);
    evas_object_show(data->bg_tool_box);
    evas_object_move(data->tool_box, 0, height - TOOL_BOX_HEIGHT);
    evas_object_focus_set(curr_focus, EINA_FALSE);
    elm_entry_input_panel_hide(data->url_entry);
    evas_object_focus_set(data->webview, EINA_TRUE);
    is_imf_shown = 0;
  }
  #else
  evas_object_focus_set(data->url_entry, EINA_FALSE);
  elm_entry_input_panel_hide(data->url_entry);
  is_imf_shown = 0;
  #endif
}

void _back_btn_cb(void *data, Evas_Object *btn, void *ev)
{
  LOGD("Previous Page\n");
  app_data *ad = (app_data*) data;
  ewk_view_back(ad->webview);
  hide_imf_panel(ad);
}

void _next_btn_cb(void *data, Evas_Object *btn, void *ev)
{
  LOGD("Next Page\n");
  app_data *ad = (app_data*) data;
  ewk_view_forward(ad->webview);
  hide_imf_panel(ad);
}

void _reload_btn_cb(void *data, Evas_Object *btn, void *ev)
{
  LOGD("Reoading\n");
  app_data *ad = (app_data*) data;
  ewk_view_reload(ad->webview);
  hide_imf_panel(ad);
}

void _stop_btn_cb(void *data, Evas_Object *btn, void *ev)
{
  LOGD("Stopping\n");
  app_data *ad = (app_data*) data;
  ewk_view_stop(ad->webview);
  hide_imf_panel(ad);
}

void __policy_response_decide_cb(void *data, Evas_Object *obj, void *ev)
{
  LOGD(" callback called __policy_response_decide_cb \n");
  Ewk_Policy_Decision *policy_decision = (Ewk_Policy_Decision *)ev;
  Ewk_Policy_Decision_Type policy_type = ewk_policy_decision_type_get(policy_decision);
  const char *uri = ewk_policy_decision_url_get(policy_decision);
  const char *cookie = ewk_policy_decision_cookie_get(policy_decision);
  const char *mime = ewk_policy_decision_response_mime_get(policy_decision);
  LOGD(" URL : %s\n", uri);
  LOGD(" cookie : %s\n", cookie);
  LOGD(" mime type : %s\n", mime);
  LOGD(" policy type : %d\n", policy_type);
  ewk_policy_decision_use(policy_decision);
}

void __navigation_policy_decide_cb(void *data, Evas_Object *obj, void *ev)
{
  Ewk_Policy_Decision* decision = (Ewk_Policy_Decision*)ev;
  LOGD(" callback called __navigation_policy_decide_cb \n");
  LOGD("URL: %s\n", ewk_policy_decision_url_get(decision));
  LOGD("Navigation type: %d", ewk_policy_decision_navigation_type_get(decision));
  // we only set USE
  // download needs to be handled and ignore wont have any visible difference.
  LOGD("Decision: USE\n");
  ewk_policy_decision_use(decision);
}

#ifdef OS_TIZEN
void _back_hard_key_cb(void *data, Evas_Object *btn, void *ev)
{
  app_data *ad = (app_data*) data;

  if (ad->popup) {
    __popup_close(ad);
    return;
  }
#if defined(OS_TIZEN_TV)
  /* SIP in TV has dedicated Close button but in Mobile Backkey is used as Close
   * For TV, On SIP Close Press only Hide IMF, Don't allow Navigation Back
   * Navigation back is handled seperatly for Backkey Event, where as
   * For Mobile, Backkey is used as SIP Close button and Navigation Back
   */
  hide_imf_panel(ad);
#else
  if (is_imf_shown)
    evas_object_focus_set(ad->webview, EINA_TRUE);
#endif
}
#endif


context_menu_type get_menu_type(app_data *data, Ewk_Context_Menu *menu)
{
  //RETV_MSG_IF(!menu, UNKNOWN_MENU, "menu is NULL");

  unsigned short count = ewk_context_menu_item_count(menu);
  Eina_Bool text = EINA_FALSE;
  Eina_Bool link = EINA_FALSE;
  Eina_Bool image = EINA_FALSE;
  Eina_Bool email_address = EINA_FALSE;
  Eina_Bool call_number = EINA_FALSE;
  Eina_Bool selection_mode = EINA_FALSE;

  unsigned short i = 0;
  for (i = 0 ; i < count ; i++) {
    Ewk_Context_Menu_Item *item = ewk_context_menu_nth_item_get(menu, i);
    Ewk_Context_Menu_Item_Tag tag = ewk_context_menu_item_tag_get(item);
    const char *link_url = ewk_context_menu_item_link_url_get(item);
//BROWSER_LOGD("tag=%d", tag);

    if (link_url && !strncmp(mailto_scheme, link_url, strlen(mailto_scheme)))
      email_address = EINA_TRUE;
    if (link_url && !strncmp(tel_scheme, link_url, strlen(tel_scheme)))
      call_number = EINA_TRUE;
    if (tag == EWK_CONTEXT_MENU_ITEM_TAG_TEXT_SELECTION_MODE)
      selection_mode = EINA_TRUE;
    if (tag == EWK_CONTEXT_MENU_ITEM_TAG_CLIPBOARD)
      return INPUT_FIELD;
    if (tag == EWK_CONTEXT_MENU_ITEM_TAG_SEARCH_WEB)
      text = EINA_TRUE;
    if (tag == EWK_CONTEXT_MENU_ITEM_TAG_OPEN_LINK_IN_NEW_WINDOW)
      link = EINA_TRUE;
    if (tag == EWK_CONTEXT_MENU_ITEM_TAG_COPY_IMAGE_TO_CLIPBOARD)
      image = EINA_TRUE;
  }

  if (email_address && selection_mode)
          return EMAIL_SCHEME;
  if (call_number && selection_mode)
          return TEL_SCHEME;
  if (text && !link)
          return TEXT_ONLY;
  if (link && !image)
          return TEXT_LINK;
  if (image && !link)
          return IMAGE_ONLY;
  if (image && link)
          return IMAGE_LINK;

  return UNKNOWN_MENU;
}


void __customize_context_menu_cb(void *data, Evas_Object *obj, void *event_info)
{
  app_data * ad = (app_data*)data;
  Ewk_Context_Menu *menu = (Ewk_Context_Menu*)event_info;
  context_menu_type menu_type = get_menu_type(ad, menu);

  if (menu_type == UNKNOWN_MENU || menu_type == INPUT_FIELD)
    return;

  switch (menu_type) {
    case TEXT_ONLY:
      show_context_menu_text_only(ad, menu);
    break;

    case TEXT_LINK:
      show_context_menu_text_link(ad, menu);
    break;

    case IMAGE_ONLY:
      show_context_menu_image_only(ad, menu);
    break;

    case IMAGE_LINK:
      show_context_menu_image_link(ad, menu);
    break;

    case EMAIL_SCHEME:
      show_context_menu_email_address(ad, menu);
    break;

    case TEL_SCHEME:
      show_context_menu_call_number(ad, menu);
    break;

    default:
      return;
    break;
  }
}

void __customize_context_menu_item_selected_cb(void *data, Evas_Object *obj, void *event_info)
{
  Ewk_Context_Menu_Item* item = (Ewk_Context_Menu_Item*)event_info;

  switch (ewk_context_menu_item_tag_get(item)) {
    case EWK_CONTEXT_MENU_ITEM_TAG_OPEN_LINK_IN_NEW_WINDOW:
      break;
    default:
      break;
  }
}

void show_context_menu_text_only(app_data *data, Ewk_Context_Menu *menu)
{
  unsigned short count = ewk_context_menu_item_count(menu);
  //RET_MSG_IF(count == 0, "There is no context menu for text only");

  app_data * ad = (app_data*)data;
  const char *selected_text = ewk_view_text_selection_text_get(ad->webview);
  Eina_Bool text_selected = EINA_FALSE;
  if (selected_text && strlen(selected_text) > 0)
    text_selected = EINA_TRUE;

  Ewk_Context_Menu_Item_Tag *original_ctx_item = NULL;
  original_ctx_item = (Ewk_Context_Menu_Item_Tag *)malloc(sizeof(Ewk_Context_Menu_Item_Tag) * count);
  //RET_MSG_IF(original_ctx_item == NULL, "Failed to allocate memory for ctx menu reordering");

  memset(original_ctx_item, 0x00, sizeof(Ewk_Context_Menu_Item_Tag) * count);

  unsigned short i=0;
  for (i = 0 ; i < count ; i++) {
    Ewk_Context_Menu_Item *item = ewk_context_menu_nth_item_get(menu, 0);
    Ewk_Context_Menu_Item_Tag tag = ewk_context_menu_item_tag_get(item);

    original_ctx_item[i] = tag;
    ewk_context_menu_item_remove(menu, item);
  }

  /* Select all */
  for (i = 0 ; i < count ; i++) {
    if (original_ctx_item[i] == EWK_CONTEXT_MENU_ITEM_TAG_SELECT_ALL) {
            ewk_context_menu_item_append_as_action(menu, EWK_CONTEXT_MENU_ITEM_TAG_SELECT_ALL, BR_STRING_CTXMENU_SELECT_ALL, true);
            break;
    }
  }

  /* Copy */
  if (text_selected == EINA_TRUE) {
    for (i = 0 ; i < count ; i++) {
            if (original_ctx_item[i] == EWK_CONTEXT_MENU_ITEM_TAG_COPY) {
                    ewk_context_menu_item_append_as_action(menu, EWK_CONTEXT_MENU_ITEM_TAG_COPY, BR_STRING_CTXMENU_COPY, true);
                    break;
            }
    }
  }

  /*
  // Share 
  if (text_selected == EINA_TRUE)
    ewk_context_menu_item_append(menu, CUSTOM_CONTEXT_MENU_ITEM_SHARE, BR_STRING_SHARE, browser_img_dir"/I01_ctx_popup_icon_share.png", true);


  // Search
  if (text_selected == EINA_TRUE) {
    for (i = 0 ; i < count ; i++) {
            if (original_ctx_item[i] == EWK_CONTEXT_MENU_ITEM_TAG_SEARCH_WEB) {
                    ewk_context_menu_item_append(menu, CUSTOM_CONTEXT_MENU_ITEM_SEARCH, BR_STRING_CTXMENU_WEB_SEARCH, browser_img_dir"/I01_ctx_popup_icon_web_search.png", true);
                    break;
            }
    }
  }

  // Translator
  if (text_selected == EINA_TRUE) {
    for (i = 0 ; i < count ; i++) {
            if (original_ctx_item[i] == EWK_CONTEXT_MENU_ITEM_TAG_TRANSLATE) {
                    ewk_context_menu_item_append(menu, EWK_CONTEXT_MENU_ITEM_TAG_TRANSLATE, BR_STRING_CTXMENU_TRANSLATE, browser_img_dir"/I01_ctx_popup_icon_translate.png", true);
                    break;
            }
    }
  }
  */

  free(original_ctx_item);
}

void show_context_menu_text_link(app_data *data, Ewk_Context_Menu *menu)
{
  /*
  ewk_context_menu_item_append_as_action(context_menu, EWK_CONTEXT_MENU_ITEM_TAG_GO_BACK, "Back", 1);
  ewk_context_menu_item_append_as_action(context_menu, EWK_CONTEXT_MENU_ITEM_TAG_GO_FORWARD, "Forward", 1);
  ewk_context_menu_item_append_as_action(context_menu, EWK_CONTEXT_MENU_ITEM_TAG_RELOAD, "Reload", 1);
  ewk_context_menu_item_append_as_action(context_menu, FOR_TEST_APPLICATION_BACK_FORWAD_CLEAR, "Bk_fwd_clear", 1);
  ewk_context_menu_item_append_as_action(context_menu, FOR_TEST_APPLICATION_CACHE_RUN, "Run App Cache Test", 1);
  ewk_context_menu_item_append_as_action(context_menu, FOR_TEST_APPLICATION_CACHE_DELETE_ALL, "Delete All App Cache", 1);
  ewk_context_menu_item_append_as_action(context_menu, FOR_TEST_GRAB_SNAPSHOT, "Take Snapshot", 1);
  ewk_context_menu_item_append_as_action(context_menu, FOR_TEST_WEB_DATABASE_GET_ORIGINS, "Get WebDB", 1);
  ewk_context_menu_item_append_as_action(context_menu, FOR_TEST_WEB_DATABASE_DELETE_ALL, "Clear WebDB", 1);
  */

}

void show_context_menu_image_only(app_data *data, Ewk_Context_Menu *menu)
{


}

void show_context_menu_image_link(app_data *data, Ewk_Context_Menu *menu)
{

}

void show_context_menu_email_address(app_data *data, Ewk_Context_Menu *menu)
{
}

void show_context_menu_call_number(app_data *data, Ewk_Context_Menu *menu)
{
}


void enable_customize_contextmenu(app_data *data, Eina_Bool enable)
{
  if (enable) {
    evas_object_smart_callback_add(data->webview, "contextmenu,customize", __customize_context_menu_cb, data);
    evas_object_smart_callback_add(data->webview, "contextmenu,selected", __customize_context_menu_item_selected_cb, data);
  } else {
    evas_object_smart_callback_del(data->webview, "contextmenu,customize", __customize_context_menu_cb);
    evas_object_smart_callback_del(data->webview, "contextmenu,selected", __customize_context_menu_item_selected_cb);
  }
}

//  Added to Support Javascript popup
Eina_Bool __javascript_alert_cb(Evas_Object* o, const char* alert_text, void* user_data)
{
  return EINA_TRUE;
}

Eina_Bool __javascript_confirm_cb(Evas_Object* o, const char* message, void* user_data)
{
  return EINA_TRUE;
}

Eina_Bool __javascript_prompt_cb(Evas_Object* o, const char* message, const char* default_value, void* user_data)
{
  return EINA_TRUE;
}

#if defined(OS_TIZEN_MOBILE)
// To support vibration: this is a timeout callback.
// The original source code author is kinipk.
Eina_Bool __vibration_timeout_cb(void *data)
{
  s_haptic_timer_id = NULL;
  if (s_haptic_handle) {
#ifndef TIZEN_LEGACY_V_2_2_1
    device_haptic_stop(s_haptic_handle, s_haptic_effect);
    device_haptic_close(s_haptic_handle);
#else
    haptic_stop_effect(s_haptic_handle, s_haptic_effect);
    haptic_close(s_haptic_handle);
#endif
    s_haptic_handle = NULL;
  }

  return ECORE_CALLBACK_CANCEL;
}

// To support vibration: @see |ewk_context_vibration_client_callbacks_set|, also.
// The original source code author is kinipk.
void __vibration_on_cb(uint64_t vibration_time, void *data)
{
  printf("[%s][%d][%s] called\n", __FUNCTION__, __LINE__, "vibration");
  uint64_t duration = vibration_time;

  if (s_haptic_timer_id) {
    ecore_timer_del(s_haptic_timer_id);
    s_haptic_timer_id = NULL;
  }

  if (s_haptic_handle) {
#ifndef TIZEN_LEGACY_V_2_2_1
    device_haptic_stop(s_haptic_handle, s_haptic_effect);
    device_haptic_close(s_haptic_handle);
#else
    haptic_stop_effect(s_haptic_handle, s_haptic_effect);
    haptic_close(s_haptic_handle);
#endif
    s_haptic_handle = NULL;
  }

#ifndef TIZEN_LEGACY_V_2_2_1
  if (device_haptic_open(0, &s_haptic_handle) != DEVICE_ERROR_NONE) {
    printf("[%s][%d][%s] ERROR: __vibration_on_cb:device_haptic_open failed\n", __FUNCTION__, __LINE__, "vibration");
#else
  if (haptic_open(HAPTIC_DEVICE_0, &s_haptic_handle) != HAPTIC_ERROR_NONE) {
    printf("[%s][%d][%s] ERROR: __vibration_on_cb:haptic_open failed\n", __FUNCTION__, __LINE__, "vibration");
#endif
    return;
  }

#ifndef TIZEN_LEGACY_V_2_2_1
  device_haptic_vibrate(s_haptic_handle, duration, 100, &s_haptic_effect);
#else
  haptic_vibrate_monotone(s_haptic_handle, duration, &s_haptic_effect);
#endif
  double in = (double)((double)(duration) / (double)(1000));
  printf("[%s][%d][%s] __vibration_on_cb:duration=%f\n", __FUNCTION__, __LINE__, "vibration", in);

  s_haptic_timer_id = ecore_timer_add(in, __vibration_timeout_cb, NULL);
}

// To support vibration: @see |ewk_context_vibration_client_callbacks_set|, also.
// The original source code author is kinipk.
void __vibration_off_cb(void *data)
{
  printf("[%s][%d][%s] called\n", __FUNCTION__, __LINE__, "vibration");
  if (s_haptic_timer_id) {
    ecore_timer_del(s_haptic_timer_id);
    s_haptic_timer_id = NULL;
  }

  if (s_haptic_handle) {
#ifndef TIZEN_LEGACY_V_2_2_1
    device_haptic_stop(s_haptic_handle, s_haptic_effect);
    device_haptic_close(s_haptic_handle);
#else
    haptic_stop_effect(s_haptic_handle, s_haptic_effect);
    haptic_close(s_haptic_handle);
#endif
    s_haptic_handle = NULL;
  }
}

#endif // #if caluse of #if defined(OS_TIZEN_MOBILE)...
