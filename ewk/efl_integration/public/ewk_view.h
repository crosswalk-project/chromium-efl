/*
 * Copyright (C) 2011 Samsung Electronics. All rights reserved.
 * Copyright (C) 2012 Intel Corporation. All rights reserved.
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

/**
 * @file    ewk_view.h
 * @brief   WebKit main smart object.
 *
 * This object provides view related APIs of WebKit2 to EFL object.
 *
 * The following signals (see evas_object_smart_callback_add()) are emitted:
 *
 * - "back,forward,list,changed", void: reports that the view's back / forward list had changed.
 * - "close,window", void: window is closed.
 * - "create,window", Evas_Object**: a new window is created.
 * - "download,cancelled", Ewk_Download_Job*: reports that a download was effectively cancelled.
 * - "download,failed", Ewk_Download_Job_Error*: reports that a download failed with the given error.
 * - "download,finished", Ewk_Download_Job*: reports that a download completed successfully.
 * - "download,request", Ewk_Download_Job*: reports that a new download has been requested. The client should set the
 *   destination path by calling ewk_download_job_destination_set() or the download will fail.
 * - "form,submission,request", Ewk_Form_Submission_Request*: Reports that a form request is about to be submitted.
 *   The Ewk_Form_Submission_Request passed contains information about the text fields of the form. This
 *   is typically used to store login information that can be used later to pre-fill the form.
 *   The form will not be submitted until ewk_form_submission_request_submit() is called.
 *   It is possible to handle the form submission request asynchronously, by simply calling
 *   ewk_form_submission_request_ref() on the request and calling ewk_form_submission_request_submit()
 *   when done to continue with the form submission. If the last reference is removed on a
 *   #Ewk_Form_Submission_Request and the form has not been submitted yet,
 *   ewk_form_submission_request_submit() will be called automatically.
 * - "icon,changed", void: reports that the view's favicon has changed.
 * - "intent,request,new", Ewk_Intent*: reports new Web intent request.
 * - "intent,service,register", Ewk_Intent_Service*: reports new Web intent service registration.
 * - "load,error", const Ewk_Error*: reports main frame load failed.
 * - "load,finished", void: reports load finished.
 * - "load,progress", double*: load progress has changed (value from 0.0 to 1.0).
 * - "load,provisional,failed", const Ewk_Error*: view provisional load failed.
 * - "load,provisional,redirect", void: view received redirect for provisional load.
 * - "load,provisional,started", void: view started provisional load.
 * - "pageSave,success", void: page save operation was success.
 * - "pageSave,error", void: page save operation has failed.
 * - "policy,decision,navigation", Ewk_Navigation_Policy_Decision*: a navigation policy decision should be taken.
 *   To make a policy decision asynchronously, simply increment the reference count of the
 *   #Ewk_Navigation_Policy_Decision object using ewk_navigation_policy_decision_ref().
 * - "policy,decision,new,window", Ewk_Navigation_Policy_Decision*: a new window policy decision should be taken.
 *   To make a policy decision asynchronously, simply increment the reference count of the
 *   #Ewk_Navigation_Policy_Decision object using ewk_navigation_policy_decision_ref().
 * - "resource,request,failed", const Ewk_Resource_Load_Error*: a resource failed loading.
 * - "resource,request,finished", const Ewk_Resource*: a resource finished loading.
 * - "resource,request,new", const Ewk_Resource_Request*: a resource request was initiated.
 * - "resource,request,response", Ewk_Resource_Load_Response*: a response to a resource request was received.
 * - "resource,request,sent", const Ewk_Resource_Request*: a resource request was sent.
 * - "text,found", unsigned int*: the requested text was found and it gives the number of matches.
 * - "title,changed", const char*: title of the main frame was changed.
 * - "tooltip,text,set", const char*: tooltip was set.
 * - "tooltip,text,unset", void: tooltip was unset.
 * - "url,changed", const char*: url of the main frame was changed.
 * - "webprocess,crashed", Eina_Bool*: expects a @c EINA_TRUE if web process crash is handled; @c EINA_FALSE, otherwise.
 *
 *
 * Tizen specific signals
 * - "magnifier,show", void: magifier of text selection was showed.
 * - "magnifier,hide", void: magifier of text selection was hidden.
 */

#ifndef ewk_view_h
#define ewk_view_h

//#include "ewk_back_forward_list.h"
#include "ewk_context.h"
//#include "ewk_download_job.h"
//#include "ewk_error.h"
//#include "ewk_intent.h"
//#include "ewk_resource.h"
#include "ewk_settings.h"
#include "ewk_touch.h"
//#include "ewk_url_request.h"
//#include "ewk_url_response.h"
#include <Evas.h>

//#include "ewk_frame.h"
#include "ewk_history.h"

//#if ENABLE(TIZEN_WEBKIT2_HIT_TEST)
#include "ewk_export.h"
#include "ewk_frame.h"
#include "ewk_hit_test.h"
#include "ewk_enums.h"

#include "ewk_back_forward_list.h"
#include "ewk_security_origin.h"
//#include "ewk_web_application_icon_data.h"
#include "ewk_intercept_request.h"
#include "ewk_page_group.h"
#include "ewk_window_features.h"
#include "ewk_quota_permission_request.h"
#include "ewk_geolocation.h"

#ifdef __cplusplus
extern "C" {
#endif

/// Enum values containing text directionality values.
typedef enum {
    EWK_TEXT_DIRECTION_RIGHT_TO_LEFT,
    EWK_TEXT_DIRECTION_LEFT_TO_RIGHT
} Ewk_Text_Direction;

enum Ewk_Password_Popup_Option {
    EWK_PASSWORD_POPUP_SAVE,
    EWK_PASSWORD_POPUP_NOT_NOW,
    EWK_PASSWORD_POPUP_NEVER,
    EWK_PASSWORD_POPUP_OK = EWK_PASSWORD_POPUP_SAVE,
    EWK_PASSWORD_POPUP_CANCEL =EWK_PASSWORD_POPUP_NOT_NOW
};
typedef enum Ewk_Password_Popup_Option Ewk_Password_Popup_Option;

typedef struct Ewk_View_Smart_Data Ewk_View_Smart_Data;
typedef struct Ewk_View_Smart_Class Ewk_View_Smart_Class;

// #if PLATFORM(TIZEN)
/// Creates a type name for _Ewk_Event_Gesture.
typedef struct Ewk_Event_Gesture Ewk_Event_Gesture;

/// Represents a gesture event.
struct Ewk_Event_Gesture {
    Ewk_Gesture_Type type; /**< type of the gesture event */
    Evas_Coord_Point position; /**< position of the gesture event */
    Evas_Point velocity; /**< velocity of the gesture event. The unit is pixel per second. */
    double scale; /**< scale of the gesture event */
    int count; /**< count of the gesture */
    unsigned int timestamp; /**< timestamp of the gesture */
};

// #if ENABLE(TIZEN_FOCUS_UI)
enum Ewk_Unfocus_Direction {
    EWK_UNFOCUS_DIRECTION_NONE = 0,
    EWK_UNFOCUS_DIRECTION_FORWARD,
    EWK_UNFOCUS_DIRECTION_BACKWARD,
    EWK_UNFOCUS_DIRECTION_UP,
    EWK_UNFOCUS_DIRECTION_DOWN,
    EWK_UNFOCUS_DIRECTION_LEFT,
    EWK_UNFOCUS_DIRECTION_RIGHT,
};
typedef enum Ewk_Unfocus_Direction Ewk_Unfocus_Direction;
// #endif

// #if ENABLE(TIZEN_INPUT_TAG_EXTENSION)
/**
 * \enum    Ewk_Input_Type
 * @brief   Provides type of focused input element
 */
enum Ewk_Input_Type {
    EWK_INPUT_TYPE_TEXT,
    EWK_INPUT_TYPE_TELEPHONE,
    EWK_INPUT_TYPE_NUMBER,
    EWK_INPUT_TYPE_EMAIL,
    EWK_INPUT_TYPE_URL,
    EWK_INPUT_TYPE_PASSWORD,
    EWK_INPUT_TYPE_COLOR,
    EWK_INPUT_TYPE_DATE,
    EWK_INPUT_TYPE_DATETIME,
    EWK_INPUT_TYPE_DATETIMELOCAL,
    EWK_INPUT_TYPE_MONTH,
    EWK_INPUT_TYPE_TIME,
    EWK_INPUT_TYPE_WEEK
};
typedef enum Ewk_Input_Type Ewk_Input_Type;
// #endif // ENABLE(TIZEN_INPUT_TAG_EXTENSION)

// #if ENABLE(TIZEN_WEBKIT2_TEXT_SELECTION)
/**
 * \enum    Ewk_Selection_Handle_Type
 * @brief   Provides type of selection handle
 */
enum Ewk_Selection_Handle_Type {
    EWK_SELECTION_HANDLE_TYPE_LEFT,
    EWK_SELECTION_HANDLE_TYPE_RIGHT,
    EWK_SELECTION_HANDLE_TYPE_LARGE
};
typedef enum Ewk_Selection_Handle_Type Ewk_Selection_Handle_Type;
// #endif // ENABLE(TIZEN_WEBKIT2_TEXT_SELECTION)
// #endif // #if PLATFORM(TIZEN)

enum Ewk_View_Mode {
    EWK_VIEW_MODE_WINDOWED = 0,
    EWK_VIEW_MODE_FLOATING,
    EWK_VIEW_MODE_FULLSCREEN,
    EWK_VIEW_MODE_MAXIMIZED,
    EWK_VIEW_MODE_MINIMIZED
};
typedef enum Ewk_View_Mode Ewk_View_Mode;

/// Ewk view's class, to be overridden by sub-classes.
struct Ewk_View_Smart_Class {
    Evas_Smart_Class sc; /**< all but 'data' is free to be changed. */
    unsigned long version;

#if defined(OS_TIZEN_TV) && !defined(EWK_BRINGUP)
    Evas_Object* (*window_create)(Ewk_View_Smart_Data *sd, const Ewk_Window_Features *window_features); /**< creates a new window, requested by webkit */
    void (*window_close)(Ewk_View_Smart_Data *sd); /**< closes a window */
#endif // OS_TIZEN_TV

    Eina_Bool (*popup_menu_show)(Ewk_View_Smart_Data *sd, Eina_Rectangle rect, Ewk_Text_Direction text_direction, double page_scale_factor, Eina_List* items, int selected_index);
    Eina_Bool (*popup_menu_hide)(Ewk_View_Smart_Data *sd);
    Eina_Bool (*popup_menu_update)(Ewk_View_Smart_Data *sd, Eina_Rectangle rect, Ewk_Text_Direction text_direction, Eina_List* items, int selected_index);

    Eina_Bool (*text_selection_down)(Ewk_View_Smart_Data *sd, int x, int y);
    Eina_Bool (*text_selection_up)(Ewk_View_Smart_Data *sd, int x, int y);

    Eina_Bool (*input_picker_show)(Ewk_View_Smart_Data *sd, Ewk_Input_Type inputType, const char* inputValue);

    Eina_Bool (*orientation_lock)(Ewk_View_Smart_Data* sd, int orientations);
    void (*orientation_unlock)(Ewk_View_Smart_Data* sd);

    // event handling:
    //  - returns true if handled
    //  - if overridden, have to call parent method if desired
    Eina_Bool (*focus_in)(Ewk_View_Smart_Data *sd);
    Eina_Bool (*focus_out)(Ewk_View_Smart_Data *sd);
    Eina_Bool (*fullscreen_enter)(Ewk_View_Smart_Data *sd, Ewk_Security_Origin *origin);
    Eina_Bool (*fullscreen_exit)(Ewk_View_Smart_Data *sd);
    Eina_Bool (*mouse_wheel)(Ewk_View_Smart_Data *sd, const Evas_Event_Mouse_Wheel *ev);
    Eina_Bool (*mouse_down)(Ewk_View_Smart_Data *sd, const Evas_Event_Mouse_Down *ev);
    Eina_Bool (*mouse_up)(Ewk_View_Smart_Data *sd, const Evas_Event_Mouse_Up *ev);
    Eina_Bool (*mouse_move)(Ewk_View_Smart_Data *sd, const Evas_Event_Mouse_Move *ev);
    Eina_Bool (*key_down)(Ewk_View_Smart_Data *sd, const Evas_Event_Key_Down *ev);
    Eina_Bool (*key_up)(Ewk_View_Smart_Data *sd, const Evas_Event_Key_Up *ev);

    // color picker:
    //   - Shows and hides color picker.
    Eina_Bool (*input_picker_color_request)(Ewk_View_Smart_Data *sd, int r, int g, int b, int a);
    Eina_Bool (*input_picker_color_dismiss)(Ewk_View_Smart_Data *sd);

    // storage:
    //   - Web database.
    unsigned long long (*exceeded_database_quota)(Ewk_View_Smart_Data *sd, const char *databaseName, const char *displayName, unsigned long long currentQuota, unsigned long long currentOriginUsage, unsigned long long currentDatabaseUsage, unsigned long long expectedUsage);

    Eina_Bool (*formdata_candidate_show)(Ewk_View_Smart_Data *sd, int x, int y, int w, int h);
    Eina_Bool (*formdata_candidate_hide)(Ewk_View_Smart_Data *sd);
    Eina_Bool (*formdata_candidate_update_data)(Ewk_View_Smart_Data *sd, Eina_List *dataList);
    Eina_Bool (*formdata_candidate_is_showing)(Ewk_View_Smart_Data *sd);

    Eina_Bool (*gesture_start)(Ewk_View_Smart_Data *sd, const Ewk_Event_Gesture *ev);
    Eina_Bool (*gesture_end)(Ewk_View_Smart_Data *sd, const Ewk_Event_Gesture *ev);
    Eina_Bool (*gesture_move)(Ewk_View_Smart_Data *sd, const Ewk_Event_Gesture *ev);

    void (*selection_handle_down)(Ewk_View_Smart_Data *sd, Ewk_Selection_Handle_Type handleType, int x, int y);
    void (*selection_handle_move)(Ewk_View_Smart_Data *sd, Ewk_Selection_Handle_Type handleType, int x, int y);
    void (*selection_handle_up)(Ewk_View_Smart_Data *sd, Ewk_Selection_Handle_Type handleType, int x, int y);

    Eina_Bool (*window_geometry_set)(Ewk_View_Smart_Data *sd, Evas_Coord x, Evas_Coord y, Evas_Coord width, Evas_Coord height);
    Eina_Bool (*window_geometry_get)(Ewk_View_Smart_Data *sd, Evas_Coord *x, Evas_Coord *y, Evas_Coord *width, Evas_Coord *height);
};

// #if PLATFORM(TIZEN)
/**
 * Callback for ewk_view_web_app_capable_get
 *
 * @param capable web application capable
 * @param user_data user_data will be passsed when ewk_view_web_app_capable_get is called
 */
typedef void (*Ewk_Web_App_Capable_Get_Callback)(Eina_Bool capable, void* user_data);

/**
 * Callback for ewk_view_web_app_icon_get
 *
 * @param icon_url web application icon
 * @param user_data user_data will be passsed when ewk_view_web_app_icon_get is called
 */
typedef void (*Ewk_Web_App_Icon_URL_Get_Callback)(const char* icon_url, void* user_data);

/**
 * Callback for ewk_view_screenshot_contents_get_async
 *
 * @param image captured screenshot
 * @param user_data user_data will be passsed when ewk_view_screenshot_contents_get_async is called
 */
typedef void (*Ewk_Web_App_Screenshot_Captured_Callback)(Evas_Object* image, void* user_data);

/**
 * Callback for ewk_view_web_app_icon_urls_get.
 *
 * @param icon_urls list of Ewk_Web_App_Icon_Data for web app
 * @param user_data user_data will be passsed when ewk_view_web_app_icon_urls_get is called
 */
typedef void (*Ewk_Web_App_Icon_URLs_Get_Callback)(Eina_List *icon_urls, void *user_data);
// #endif

/**
 * The version you have to put into the version field
 * in the @a Ewk_View_Smart_Class structure.
 */
#define EWK_VIEW_SMART_CLASS_VERSION 1UL

/**
 * Initializer for whole Ewk_View_Smart_Class structure.
 *
 * @param smart_class_init initializer to use for the "base" field
 * (Evas_Smart_Class).
 *
 * @see EWK_VIEW_SMART_CLASS_INIT_NULL
 * @see EWK_VIEW_SMART_CLASS_INIT_VERSION
 * @see EWK_VIEW_SMART_CLASS_INIT_NAME_VERSION
 */
#define EWK_VIEW_SMART_CLASS_INIT(smart_class_init) {smart_class_init, EWK_VIEW_SMART_CLASS_VERSION}

/**
* Initializer to zero a whole Ewk_View_Smart_Class structure.
*
* @see EWK_VIEW_SMART_CLASS_INIT_VERSION
* @see EWK_VIEW_SMART_CLASS_INIT_NAME_VERSION
* @see EWK_VIEW_SMART_CLASS_INIT
*/
#define EWK_VIEW_SMART_CLASS_INIT_NULL EWK_VIEW_SMART_CLASS_INIT(EVAS_SMART_CLASS_INIT_NULL)

/**
 * Initializer to zero a whole Ewk_View_Smart_Class structure and set
 * name and version.
 *
 * Similar to EWK_VIEW_SMART_CLASS_INIT_NULL, but will set version field of
 * Evas_Smart_Class (base field) to latest EVAS_SMART_CLASS_VERSION and name
 * to the specific value.
 *
 * It will keep a reference to name field as a "const char *", that is,
 * name must be available while the structure is used (hint: static or global!)
 * and will not be modified.
 *
 * @see EWK_VIEW_SMART_CLASS_INIT_NULL
 * @see EWK_VIEW_SMART_CLASS_INIT_VERSION
 * @see EWK_VIEW_SMART_CLASS_INIT
 */
#define EWK_VIEW_SMART_CLASS_INIT_NAME_VERSION(name) EWK_VIEW_SMART_CLASS_INIT(EVAS_SMART_CLASS_INIT_NAME_VERSION(name))

typedef struct EwkViewImpl EwkViewImpl;
/**
 * @brief Contains an internal View data.
 *
 * It is to be considered private by users, but may be extended or
 * changed by sub-classes (that's why it's in public header file).
 */
struct Ewk_View_Smart_Data {
    Evas_Object_Smart_Clipped_Data base;
    const Ewk_View_Smart_Class* api; /**< reference to casted class instance */
    Evas_Object* self; /**< reference to owner object */
    Evas_Object* image; /**< reference to evas_object_image for drawing web contents */
    EwkViewImpl* priv; /**< should never be accessed, c++ stuff */
    struct {
        Evas_Coord x, y, w, h; /**< last used viewport */
    } view;
    struct { /**< what changed since last smart_calculate */
        Eina_Bool any:1;

        // WebKit use these but we don't. We should remove these if we are sure
        // we do it right.
        Eina_Bool size:1;
        Eina_Bool position:1;
    } changed;
};

/**
 * Sets the smart class APIs, enabling view to be inherited.
 *
 * @param api class definition to set, all members with the
 *        exception of @a Evas_Smart_Class->data may be overridden, must
 *        @b not be @c NULL
 *
 * @note @a Evas_Smart_Class->data is used to implement type checking and
 *       is not supposed to be changed/overridden. If you need extra
 *       data for your smart class to work, just extend
 *       Ewk_View_Smart_Class instead.
 *       The Evas_Object which inherits the ewk_view should use
 *       ewk_view_smart_add() to create Evas_Object instead of
 *       evas_object_smart_add() because it performs additional initialization
 *       for the ewk_view.
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE on failure (probably
 *         version mismatch)
 *
 * @see ewk_view_smart_add()
 */
EAPI Eina_Bool ewk_view_smart_class_set(Ewk_View_Smart_Class *api);

/**
 * Creates a new EFL WebKit view object with Evas_Smart and Ewk_Context.
 *
 * @note The Evas_Object which inherits the ewk_view should create its
 *       Evas_Object using this API instead of evas_object_smart_add()
 *       because the default initialization for ewk_view is done in this API.
 *
 * @param e canvas object where to create the view object
 * @param smart Evas_Smart object. Its type should be EWK_VIEW_TYPE_STR
 * @param context Ewk_Context object which is used for initializing
 * @param pageGroup Ewk_Page_Group object which is used for initializing
 *
 * @return view object on success or @c NULL on failure
 */
EAPI Evas_Object *ewk_view_smart_add(Evas *e, Evas_Smart *smart, Ewk_Context *context, Ewk_Page_Group *pageGroup);

/**
 * Enum values used to specify search options.
 * @brief   Provides option to find text
 * @info    Keep this in sync with WKFindOptions.h
 */
enum _Ewk_Find_Options {
  EWK_FIND_OPTIONS_NONE, /**< no search flags, this means a case sensitive, no wrap, forward only search. */
  EWK_FIND_OPTIONS_CASE_INSENSITIVE = 1 << 0, /**< case insensitive search. */
  EWK_FIND_OPTIONS_BACKWARDS = 1 << 3, /**< search backwards. */
  EWK_FIND_OPTIONS_AT_WORD_STARTS = 1 << 1, /**< search text only at the beginning of the words. */
  EWK_FIND_OPTIONS_TREAT_MEDIAL_CAPITAL_AS_WORD_START = 1 << 2, /**< treat capital letters in the middle of words as word start. */
  EWK_FIND_OPTIONS_WRAP_AROUND = 1 << 4, /**< if not present search will stop at the end of the document. */
  EWK_FIND_OPTIONS_SHOW_OVERLAY = 1 << 5, /**< show overlay */
  EWK_FIND_OPTIONS_SHOW_FIND_INDICATOR = 1 << 6, /**< show indicator */
  EWK_FIND_OPTIONS_SHOW_HIGHLIGHT = 1 << 7 /**< show highlight */
};
typedef enum _Ewk_Find_Options Ewk_Find_Options;

/**
 * Creates a new EFL WebKit view object.
 *
 * @param e canvas object where to create the view object
 * @param data a pointer to data to restore session data
 * @param length length of session data to restore session data
 *
 * @return view object on success or @c NULL on failure
 */
EAPI Evas_Object* ewk_view_add_with_session_data(Evas *e, const char *data, unsigned length);

/**
 * Creates a new EFL WebKit view object.
 *
 * @param e canvas object where to create the view object
 *
 * @return view object on success or @c NULL on failure
 */
EAPI Evas_Object *ewk_view_add(Evas *e);

/**
 * Creates a new EFL web view object in incognito mode.
 *
 * @param e canvas object where to create the view object
 *
 * @return view object on success or @c NULL on failure
 */
EAPI Evas_Object *ewk_view_add_in_incognito_mode(Evas *e);

/**
 * Creates a new EFL WebKit view object based on specific Ewk_Context.
 *
 * @param e canvas object where to create the view object
 * @param context Ewk_Context object to declare process model
 *
 * @return view object on success or @c NULL on failure
 */
EAPI Evas_Object *ewk_view_add_with_context(Evas *e, Ewk_Context *context);

/**
 * Gets the Ewk_Context of this view.
 *
 * @param o the view object to get the Ewk_Context
 *
 * @return the Ewk_Context of this view or @c NULL on failure
 */
EAPI Ewk_Context *ewk_view_context_get(const Evas_Object *o);

/**
 * Asks the object to load the given URL.
 *
 * @param o view object to load @a URL
 * @param url uniform resource identifier to load
 *
 * @return @c EINA_TRUE is returned if @a o is valid, irrespective of load,
 *         or @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_view_url_set(Evas_Object *o, const char *url);

/**
 * Returns the current URL string of view object.
 *
 * It returns an internal string and should not
 * be modified. The string is guaranteed to be stringshared.
 *
 * @param o view object to get current URL
 *
 * @return current URL on success or @c NULL on failure
 */
EAPI const char *ewk_view_url_get(const Evas_Object *o);

/**
 * Asks the main frame to reload the current document.
 *
 * @param o view object to reload current document
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE otherwise
 *
 * @see ewk_view_reload_bypass_cache()
 */
EAPI Eina_Bool ewk_view_reload(Evas_Object *o);

/**
 * Reloads the current page's document without cache.
 *
 * @param o view object to reload current document
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE otherwise
 */
EAPI Eina_Bool ewk_view_reload_bypass_cache(Evas_Object *o);

/**
 * Asks the main frame to stop loading.
 *
 * @param o view object to stop loading
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE otherwise.
 */
EAPI Eina_Bool ewk_view_stop(Evas_Object* o);


enum Ewk_Page_Visibility_State {
    EWK_PAGE_VISIBILITY_STATE_VISIBLE,
    EWK_PAGE_VISIBILITY_STATE_HIDDEN,
    EWK_PAGE_VISIBILITY_STATE_PRERENDER
};
typedef enum Ewk_Page_Visibility_State Ewk_Page_Visibility_State;

enum Ewk_Http_Method {
    EWK_HTTP_METHOD_GET,
    EWK_HTTP_METHOD_HEAD,
    EWK_HTTP_METHOD_POST,
    EWK_HTTP_METHOD_PUT,
    EWK_HTTP_METHOD_DELETE,
};
typedef enum Ewk_Http_Method Ewk_Http_Method;

/**
 * Callback for ewk_view_script_execute
 *
 * @param o the view object
 * @param result_value value returned by script
 * @param user_data user data
 */
typedef void (*Ewk_View_Script_Execute_Callback)(Evas_Object* o, const char* result_value, void* user_data);

/**
 * Callback for ewk_view_plain_text_get
 *
 * @param o the view object
 * @param plain_text the contents of the given frame converted to plain text
 * @param user_data user data
 */
typedef void (*Ewk_View_Plain_Text_Get_Callback)(Evas_Object* o, const char* plain_text, void* user_data);

/**
 * Creates a type name for the callback function used to get the page contents.
 *
 * @param o view object
 * @param data mhtml data of the page contents
 * @param user_data user data will be passed when ewk_view_mhtml_data_get is called
 */
typedef void (*Ewk_View_MHTML_Data_Get_Callback)(Evas_Object *o, const char *data, void *user_data);


EAPI Eina_Bool ewk_view_vertical_panning_hold_get(Evas_Object* o);

/**
  * Block/Release the vertical pan
  *
  * @param o view object on which pan is to be blocked/release
  * @param hold status of pan
  */
EAPI void ewk_view_vertical_panning_hold_set(Evas_Object* o, Eina_Bool hold);

/**
 * Gets the minimum and maximum value of the scale range or -1 on failure
 *
 * @param o view object to get the minimum and maximum value of the scale range
 * @param min_scale Pointer to an double in which to store the minimum scale factor of the object.
 * @param max_scale Pointer to an double in which to store the maximum scale factor of the object.
 *
 * @note Use @c NULL pointers on the scale components you're not
 * interested in: they'll be ignored by the function.
 */
EAPI void ewk_view_scale_range_get(Evas_Object* o, double* min_scale, double* max_scale);

/**
 * Gets the current text zoom level.
 *
 * @param o view object to get the zoom level
 *
 * @return current zoom level in use on success or @c -1.0 on failure
 */
EAPI double ewk_view_text_zoom_get(const Evas_Object* o);

/**
 * Sets the current text zoom level.
 *
 * @param o view object to set the zoom level
 * @param textZoomFactor a new level to set
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE otherwise
 */
EAPI Eina_Bool ewk_view_text_zoom_set(Evas_Object* o, double text_zoom_factor);

EAPI void ewk_view_suspend(Evas_Object* o);
EAPI void ewk_view_resume(Evas_Object* o);

/**
 * @typedef Ewk_View_Script_Execute_Cb Ewk_View_Script_Execute_Cb
 * @brief Callback type for use with ewk_view_script_execute()
 */
typedef void (*Ewk_View_Script_Execute_Cb)(Evas_Object *o, const char *return_value, void *user_data);

typedef Eina_Bool (*Ewk_View_Password_Confirm_Popup_Callback)(Evas_Object* o, const char* message, void* user_data);
EAPI void ewk_view_password_confirm_popup_callback_set(Evas_Object* o, Ewk_View_Password_Confirm_Popup_Callback callback, void* user_data);
EAPI void ewk_view_password_confirm_popup_reply(Evas_Object* o, Ewk_Password_Popup_Option result);

typedef Eina_Bool (*Ewk_View_JavaScript_Alert_Callback)(Evas_Object* o, const char* alert_text, void* user_data);
EAPI void ewk_view_javascript_alert_callback_set(Evas_Object* o, Ewk_View_JavaScript_Alert_Callback callback, void* user_data);
EAPI void ewk_view_javascript_alert_reply(Evas_Object* o);


typedef Eina_Bool (*Ewk_View_JavaScript_Confirm_Callback)(Evas_Object* o, const char* message, void* user_data);
EAPI void ewk_view_javascript_confirm_callback_set(Evas_Object* o, Ewk_View_JavaScript_Confirm_Callback callback, void* user_data);
/**
 * Handle javascript confirm dialog reply
 *
 * @param o view object
 * @param result boolean result contains true when ok is pressed false when cancel is pressed on the dialog
 */
EAPI void ewk_view_javascript_confirm_reply(Evas_Object* o, Eina_Bool result);

/**
 * Callback for ewk_view_javascript_prompt_callback_set
 *
 * @param o the view object
 * @param message the text to be displayed on the prompt popup
 * @param default_value default text to be entered in the prompt dialog
 * @param user_data user data
 */
typedef Eina_Bool (*Ewk_View_JavaScript_Prompt_Callback)(Evas_Object* o, const char* message, const char* default_value, void* user_data);

/**
 * Display javascript prompt popup
 *
 * @param o view object
 * @param callback callback function to be called when the prompt popup is to be opened
 * @param user_data user data
 *
 */
EAPI void ewk_view_javascript_prompt_callback_set(Evas_Object* o, Ewk_View_JavaScript_Prompt_Callback callback, void* user_data);

/**
 * Handle javascript prompt dialog reply
 *
 * @param o view object
 * @param result value entered in the prompt dialog
 */
EAPI void ewk_view_javascript_prompt_reply(Evas_Object* o, const char* result);


//#if ENABLE(TIZEN_SUPPORT_BEFORE_UNLOAD_CONFIRM_PANEL)
typedef Eina_Bool (*Ewk_View_Before_Unload_Confirm_Panel_Callback)(Evas_Object* o, const char* message, void* user_data);
EAPI void ewk_view_before_unload_confirm_panel_callback_set(Evas_Object* o, Ewk_View_Before_Unload_Confirm_Panel_Callback callback, void* user_data);
EAPI void ewk_view_before_unload_confirm_panel_reply(Evas_Object* o, Eina_Bool result);
//#endif

//#if ENABLE(TIZEN_APPLICATION_CACHE)
typedef Eina_Bool (*Ewk_View_Applicacion_Cache_Permission_Callback)(Evas_Object* o, Ewk_Security_Origin* origin,  void* user_data);
EAPI void ewk_view_application_cache_permission_callback_set(Evas_Object* o, Ewk_View_Applicacion_Cache_Permission_Callback callback, void* user_data);
EAPI void ewk_view_application_cache_permission_reply(Evas_Object* o, Eina_Bool allow);
//#endif

typedef Eina_Bool (*Ewk_View_Exceeded_Indexed_Database_Quota_Callback)(Evas_Object* o, Ewk_Security_Origin* origin, long long currentQuota, void* user_data);
EAPI void ewk_view_exceeded_indexed_database_quota_callback_set(Evas_Object* o, Ewk_View_Exceeded_Indexed_Database_Quota_Callback callback, void* user_data);
EAPI void ewk_view_exceeded_indexed_database_quota_reply(Evas_Object* o, Eina_Bool allow);


typedef Eina_Bool (*Ewk_View_Exceeded_Database_Quota_Callback)(Evas_Object* o, Ewk_Security_Origin* origin, const char* database_name, unsigned long long expectedQuota, void* user_data);
EAPI void ewk_view_exceeded_database_quota_callback_set(Evas_Object* o, Ewk_View_Exceeded_Database_Quota_Callback callback, void* user_data);
EAPI void ewk_view_exceeded_database_quota_reply(Evas_Object* o, Eina_Bool allow);

typedef Eina_Bool (*Ewk_View_Exceeded_Local_File_System_Quota_Callback)(Evas_Object* o, Ewk_Security_Origin* origin, long long currentQuota, void* user_data);
EAPI void ewk_view_exceeded_local_file_system_quota_callback_set(Evas_Object* o, Ewk_View_Exceeded_Local_File_System_Quota_Callback callback, void* user_data);
EAPI void ewk_view_exceeded_local_file_system_quota_reply(Evas_Object* o, Eina_Bool allow);

//#if ENABLE(TIZEN_INTERCEPT_REQUEST)
typedef void (*Ewk_View_Intercept_Request_Callback)(Evas_Object* o, Ewk_Intercept_Request* interceptRequest, void* user_data);
EAPI void ewk_view_intercept_request_callback_set (Evas_Object *o, Ewk_View_Intercept_Request_Callback callback, void* user_data);
//#endif

//#if ENABLE(TIZEN_FOCUS_UI)
typedef Eina_Bool (*Ewk_View_Unfocus_Allow_Callback)(Evas_Object* o, Ewk_Unfocus_Direction direction, void* user_data);
EAPI void ewk_view_unfocus_allow_callback_set (Evas_Object *o, Ewk_View_Unfocus_Allow_Callback callback, void* user_data);
//#endif

typedef Eina_Bool (*Ewk_View_Geolocation_Permission_Callback)(Evas_Object* ewk_view, Ewk_Geolocation_Permission_Request* geolocation_permission_request, void* user_data);
EAPI void ewk_view_geolocation_permission_callback_set(Evas_Object* ewk_view, Ewk_View_Geolocation_Permission_Callback callback, void* user_data);

EAPI Eina_Bool ewk_view_mode_set(Evas_Object* ewkView, Ewk_View_Mode view_mode);

/**
 * Gets the Ewk_Settings of this view.
 *
 * @param o view object to get Ewk_Settings
 *
 * @return the Ewk_Settings of this view or @c NULL on failure
 */
EAPI Ewk_Settings *ewk_view_settings_get(const Evas_Object *o);

/**
 * Asks the main frame to navigate back in the history.
 *
 * @param o view object to navigate back
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE otherwise
 *
 * @see ewk_frame_back()
 */
EAPI Eina_Bool ewk_view_back(Evas_Object *o);

/**
 * Asks the main frame to navigate forward in the history.
 *
 * @param o view object to navigate forward
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE otherwise
 *
 * @see ewk_frame_forward()
 */
EAPI Eina_Bool ewk_view_forward(Evas_Object *o);

/**
 * Queries if it is possible to navigate backwards one item in the history.
 *
 * @param o view object to query if backward navigation is possible
 *
 * @return @c EINA_TRUE if it is possible to navigate backwards in the history, @c EINA_FALSE otherwise
 */
EAPI Eina_Bool ewk_view_back_possible(Evas_Object *o);

/**
 * Queries if it is possible to navigate forwards one item in the history.
 *
 * @param o view object to query if forward navigation is possible
 *
 * @return @c EINA_TRUE if it is possible to navigate forwards in the history, @c EINA_FALSE otherwise
 */
EAPI Eina_Bool ewk_view_forward_possible(Evas_Object *o);

/**
 * Gets the current title of the main frame.
 *
 * It returns an internal string and should not
 * be modified. The string is guaranteed to be stringshared.
 *
 * @param o view object to get current title
 *
 * @return current title on success or @c NULL on failure
 */
EAPI const char *ewk_view_title_get(const Evas_Object *o);

/**
 * Gets the current load progress of page.
 *
 * The progress estimation from 0.0 to 1.0.
 *
 * @param o view object to get the current progress
 *
 * @return the load progress of page, value from 0.0 to 1.0,
 *         or @c -1.0 on failure
 */
EAPI double ewk_view_load_progress_get(const Evas_Object *o);

/*
 * Requests loading of the given request data.
 *
 * @param o view object to load
 * @param url uniform resource identifier to load
 * @param method http method
 * @param headers http headers
 * @param body http body data
 *
 * @return @c EINA_TRUE on successful request or @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_view_url_request_set(Evas_Object* o, const char* url, Ewk_Http_Method method, Eina_Hash* headers, const char* body);

/**
 * Requests the specified plain text string into the view object
 *
 * @note The mime type of document will be "text/plain".
 *
 * @return @c EINA_TRUE on successful request, @c EINA_FALSE on errors
 */
EAPI Eina_Bool ewk_view_plain_text_set(Evas_Object* o, const char* plain_text);

/**
 * Requests loading the given contents by mime type into the view object.
 *
 * @param o view object to load
 * @param contents what to load
 * @param contents_size size of @a contents (in bytes),
 * @param mime_type type of @a contents data, if @c 0 is given "text/html" is assumed
 * @param encoding encoding for @a contents data, if @c 0 is given "UTF-8" is assumed
 * @param base_uri base uri to use for relative resources, may be @c 0,
 *        if provided @b must be an absolute uri
 *
 * @return @c EINA_TRUE on successful request, @c EINA_FALSE on errors
 */
EAPI Eina_Bool ewk_view_contents_set(Evas_Object* o, const char* contents, size_t contents_size, char* mime_type, char* encoding, char* base_uri);

/**
 * Requests loading the given contents.
 *
 * @param o view object to load document
 * @param html HTML data to load
 * @param base_uri Base URI used for relative paths to external objects (optional)
 *
 * @return @c EINA_TRUE on successful request, @c EINA_FALSE on errors
 */
EAPI Eina_Bool ewk_view_html_contents_set(Evas_Object* o, const char* html, const char* base_uri);

/**
 * Requests for setting page visibility state.
 *
 * @param o view object to set the page visibility
 * @param page_visibility_state visible state of the page to set
 * @param initial_state @c EINA_TRUE if this function is called at page initialization time,
 *                     @c EINA_FALSE otherwise
 *
 * @return @c EINA_TRUE on successful request or @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_view_page_visibility_state_set(Evas_Object* o, Ewk_Page_Visibility_State page_visibility_state, Eina_Bool initial_state);

/**
* Request to set the user agent string.
*
* @param o view object to set the user agent string
*
* @return @c EINA_TRUE on success or @c EINA_FALSE on failure
*/
EAPI Eina_Bool ewk_view_user_agent_set(Evas_Object* o, const char* user_agent);

/**
* Returns user agent string.
*
* @param o view object to get the user agent string
*
* @return @c user agent string
*/
EAPI const char* ewk_view_user_agent_get(const Evas_Object* o);

/**
* Request to set the user agent with application name.
*
* @param o view object to set the user agent with application name
*
* @param application_name string to set the user agent
*
* @return @c EINA_TRUE on success or @c EINA_FALSE on failure
*/
EAPI Eina_Bool ewk_view_application_name_for_user_agent_set(Evas_Object* o, const char* application_name);

/**
* Returns application name string.
*
* @param o view object to get the application name
*
* @return @c application name. The returned string @b should be freed by
*         eina_stringshare_del() after use.
*/
EAPI const char* ewk_view_application_name_for_user_agent_get(const Evas_Object* o);

/*
* add custom header
*
* @param o view object to add custom header
*
* @param name custom header name to add the custom header
*
* @param value custom header value to add the custom header
*
* @return @c EINA_TRUE on success or @c EINA_FALSE on failure
*/
EAPI Eina_Bool ewk_view_custom_header_add(const Evas_Object* o, const char* name, const char* value);
/**
* remove custom header
*
* @param o view object to remove custom header
*
* @param name custom header name to remove the custom header
*
* @return @c EINA_TRUE on success or @c EINA_FALSE on failure
*/
EAPI Eina_Bool ewk_view_custom_header_remove(const Evas_Object* o, const char* name);
/**
* clears all custom headers
*
* @param o view object to clear custom headers
*
* @return @c EINA_TRUE on success or @c EINA_FALSE on failure
*/
EAPI Eina_Bool ewk_view_custom_header_clear(const Evas_Object* o);

/**
 * Request to set the current page's visibility.
 *
 * @param o view object to set the visibility.
 * @param enable EINA_TRUE to set on the visibility of the page, EINA_FALSE otherwise.
 *
 * @return @c EINA_TRUE on successful request, @c EINA_FALSE on errors
 */
EAPI Eina_Bool ewk_view_visibility_set(const Evas_Object* o, Eina_Bool enable);

/**
 * Returns the evas image object of the specified viewArea of page
 *
 * The returned evas image object @b should be freed after use.
 *
 * @param o view object to get specified rectangle of cairo surface.
 * @param viewArea rectangle of cairo surface.
 * @param scaleFactor scale factor of cairo surface.
 * @param canvas canvas for creating evas image.
 *
 * @return newly allocated evas image object on sucess or @c 0 on failure.
 */
EAPI Evas_Object* ewk_view_screenshot_contents_get(const Evas_Object* o, Eina_Rectangle viewArea, float scaleFactor, Evas* canvas);

/**
 * Makes request of evas image object of the specified viewArea of page asynchronously
 *
 * The returned evas image object through async callback @b should be freed after use.
 *
 * @param o view object to get specified rectangle of cairo surface.
 * @param viewArea rectangle of cairo surface.
 * @param scaleFactor scale factor of cairo surface.
 * @param canvas canvas for creating evas image.
 * @param callback result callback to get captured screenshot.
 * @param user_data will be passed when result_callback is called
 *    -I.e., user data will be kept until callback is called.
 *
 * @return @c EINA_TRUE on successful request, @c EINA_FALSE on errors.
 */
EAPI Eina_Bool ewk_view_screenshot_contents_get_async(const Evas_Object* o, Eina_Rectangle viewArea,
         float scaleFactor, Evas* canvas, Ewk_Web_App_Screenshot_Captured_Callback callback, void* user_data);

/**
 * Returns the evas image object for the cache image specified in url.
 *
 * The returned evas image object @b should be freed after use.
 *
 * @param o view object to get specified rectangle of cairo surface.
 * @param imageUrl url of the image in the cache.
 * @param canvas canvas for creating evas image.
 *
 * @return newly allocated evas image object on sucess or @c 0 on failure.
 */
EAPI Evas_Object* ewk_view_cache_image_get(const Evas_Object* o, const char* imageUrl, Evas* canvas);

/**
 * Start a server for inspecting web pages
 * This server will be used by Remote Web Browser to transfer messages over network
 *
 * @param [o] view object to debug
 * @param [in] port It is a port number for the server. A free port on system will be allocated if port is 0
 *
 * @return @c assigned port number on success or @c 0 on failure
 */
EAPI unsigned int ewk_view_inspector_server_start(Evas_Object* o, unsigned int port);

/**
 * Stop a server for inspecting web pages
 *
 * @param [o] view object to debug
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE otherwise
 */
EAPI Eina_Bool ewk_view_inspector_server_stop(Evas_Object* o);

/**
 * Scrolls webpage of view by dx and dy.
 *
 * @param o view object to scroll
 * @param dx horizontal offset to scroll
 * @param dy vertical offset to scroll
 */
EAPI void ewk_view_scroll_by(Evas_Object* o, int dx, int dy);

/**
 * Gets the current scroll position of given view.
 *
 * @param o view object to get the current scroll position
 * @param x the pointer to store the horizontal position, may be @c NULL
 * @param y the pointer to store the vertical position, may be @c NULL
 *
 * @return @c EINA_TRUE on success, @c EINA_FALSE otherwise and
 *         values are zeroed.
 */
EAPI Eina_Bool ewk_view_scroll_pos_get(Evas_Object* o, int* x, int* y);

/**
 * Sets an absolute scroll of the given view.
 *
 * Both values are from zero to the contents size minus the viewport
 * size.
 *
 * @param o view object to scroll
 * @param x horizontal position to scroll
 * @param y vertical position to scroll
 *
 * @return @c EINA_TRUE on success, @c EINA_FALSE otherwise
 */
EAPI Eina_Bool ewk_view_scroll_set(Evas_Object* o, int x, int y);

/**
 * Gets the possible scroll size of the given view.
 *
 * Possible scroll size is contents size minus the viewport size.
 *
 * @param o view object to get scroll size
 * @param w the pointer to store the horizontal size that is possible to scroll,
 *        may be @c 0
 * @param h the pointer to store the vertical size that is possible to scroll,
 *        may be @c 0
 *
 * @return @c EINA_TRUE on success, @c EINA_FALSE otherwise and
 *         values are zeroed
 */
EAPI Eina_Bool ewk_view_scroll_size_get(const Evas_Object* o, int* w, int* h);

/**
 * Requests for getting web application capable.
 *
 * @param callback result callback to get web application capable
 * @param user_data user_data will be passed when result_callback is called
 *    -I.e., user data will be kept until callback is called
 *
 * @return @c EINA_TRUE on successful request or @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_view_web_application_capable_get(Evas_Object* o, Ewk_Web_App_Capable_Get_Callback callback, void* user_data);

/**
 * Requests for getting web application icon string.
 *
 * @param callback result callback to get web application icon url
 * @param user_data user_data will be passed when result_callback is called
 *    -I.e., user data will be kept until callback is called
 *
 * @return @c EINA_TRUE on successful request or @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_view_web_application_icon_url_get(Evas_Object* o, Ewk_Web_App_Icon_URL_Get_Callback callback, void* user_data);

/**
 * Requests for getting web application icon list of Ewk_Web_App_Icon_Data.
 *
 * @param callback result callback to get web application icon urls
 * @param user_data user_data will be passed when result_callback is called
 *    -I.e., user data will be kept until callback is called
 *
 * @return @c EINA_TRUE on successful request or @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_view_web_application_icon_urls_get(Evas_Object *o, Ewk_Web_App_Icon_URLs_Get_Callback callback, void *user_data);

/**
 * Executes editor command.
 *
 * @param o view object to execute command
 * @param command editor command to execute
 * @param value the value to be passed into command
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_view_command_execute(Evas_Object* o, const char* command, const char* value);

/**
 * Gets last known contents size.
 *
 * @param o view object to get contents size
 * @param width pointer to store contents size width, may be @c 0
 * @param height pointer to store contents size height, may be @c 0
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE on failure and
 *         @a width and @a height will be zeroed
 */
EAPI Eina_Bool ewk_view_contents_size_get(Evas_Object* o, Evas_Coord* width, Evas_Coord* height);

/**
 * Create PDF file of page contents
 *
 * @param o view object to get page contents.
 * @param width the suface width of PDF file.
 * @param height the suface height of PDF file.
 * @param fileName the file name for creating PDF file.
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE on failure
 */
/* This return value is status of the request not the status of actual operation.
 * There should be some callback to get the actual status or reason of failure.
 */
EAPI Eina_Bool ewk_view_contents_pdf_get(Evas_Object* o, int width, int height, const char* fileName);

/**
 * Requests execution of the given script.
 *
 * @note This allows to use NULL for the callback parameter.
 *       So, if the result data from the script is not required, NULL might be used for the callback parameter.
 *
 * @param o view object to execute script
 * @param script Java Script to execute
 * @param callback result callback
 * @param user_data user data
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE otherwise
 */
EAPI Eina_Bool ewk_view_script_execute(Evas_Object* o, const char* script, Ewk_View_Script_Execute_Callback callback, void* user_data);

/**
 * Retrieve the contents in plain text.
 *
 * @param o view object whose contents to retrieve.
 * @param callback result callback
 * @param user_data user data
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE otherwise
 */
EAPI Eina_Bool ewk_view_plain_text_get(Evas_Object* o, Ewk_View_Plain_Text_Get_Callback callback, void* user_data);

/**
 * Get page contents as MHTML data
 *
 * @param o view object to get the page contents
 * @param callback callback function to be called when the operation is finished
 * @param user_data user data to be passed to the callback function
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE otherwise
 */
EAPI Eina_Bool ewk_view_mhtml_data_get(Evas_Object *o, Ewk_View_MHTML_Data_Get_Callback callback, void *user_data);


/**
 * Deprecated - use async hit tests instead
 * Creates a new hit test for the given veiw object and point.
 *
 * The returned object should be freed by ewk_hit_test_free().
 *
 * @param o view object to do hit test on
 * @param x the horizontal position to query
 * @param y the vertical position to query
 * @param hit_test_mode the Ewk_Hit_Test_Mode enum value to query
 *
 * @return a newly allocated hit test on success, @c 0 otherwise
 */
EINA_DEPRECATED EAPI Ewk_Hit_Test* ewk_view_hit_test_new(Evas_Object* o, int x, int y, int hit_test_mode);

typedef void (*Ewk_View_Hit_Test_Request_Callback)(Evas_Object* o, int x, int y, int hit_test_mode, Ewk_Hit_Test*, void* user_data);

/**
 * Requests new hit test for given view object and point.
 *
 * @param o              view object to do hit test on
 * @param x              the horizontal position to query
 * @param y              the vertical position to query
 * @param hit_test_mode  the Ewk_Hit_Test_Mode enum value to query
 * @param callback       callback to be executed when hit test request was finished
 *
 * @return EINA_TRUE if hit test request was queued, otherwise EINA_FALSE
 */
EAPI Eina_Bool ewk_view_hit_test_request(Evas_Object* o, int x, int y, int hit_test_mode, Ewk_View_Hit_Test_Request_Callback, void* user_data);

/**
 * Get the whole history(whole back & forward list) associated with this view.
 *
 * @param o view object to get the history(whole back & forward list)
 *
 * @return a newly allocated history of @b newly allocated item
 *         instance. This memory of each item must be released with
 *         ewk_history_free() after use.
 *
 * @see ewk_history_free()
 */
#if !defined(EWK_BRINGUP)
EAPI Ewk_History* ewk_view_history_get(Evas_Object* o);
#endif

/*
 * Notify that notification is closed.
 *
 * @param notification_list list of Ewk_Notification pointer
 *        notification_list is freed in this function.
 *
 * @return @c EINA_TRUE on successful request or @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_view_notification_closed(Evas_Object* o, Eina_List* notification_list);

/*
 * Sends the orientation of the device.
 *
 * If orientation value is changed, orientationchanged event will occur.
 *
 * @param o view object to receive orientation event.
 * @param orientation the new orientation of the device. (degree)
 *
 * orientation will be 0 degrees when the device is oriented to natural position,
 *                     90 degrees when it's left side is at the top,
 *                    -90 degrees when it's right side is at the top,
 *                     180 degrees when it is upside down.
 */
EAPI void ewk_view_orientation_send(Evas_Object *o, int orientation);

/**
 * Sets the encoding and reloads the page.
 *
 * @param ewkView view to set the encoding
 * @param encoding the new encoding to set or @c 0 to restore the default one
 */
EAPI void ewk_view_encoding_custom_set(Evas_Object* ewkView, const char* encoding);

/**
 * Gets the Range of contained selection.
 *
 * @param o view object with selection in progress
 * @param left_rect Refers to left ie start of selection
 * @param right_rect Refers to right ie end of selection
 *
 * @return @c EINA_TRUE on success, @c EINA_FALSE otherwise and
 *         values are zeroed
 */
EAPI Eina_Bool ewk_view_text_selection_range_get(const Evas_Object* o, Eina_Rectangle* left_rect, Eina_Rectangle* right_rect);

/**
 * Returns the selection text.
 *
 * @param o view object to get selection text.
 *
 * @return selection text or @c NULL on failure
 */
EAPI const char* ewk_view_text_selection_text_get(Evas_Object* o);

/**
 * Clears the current selection
 *
 * @param o view object with selection in progress
 *
 * @return @c EINA_TRUE on success, @c EINA_FALSE on errors
 */
EAPI Eina_Bool ewk_view_text_selection_clear(Evas_Object *o);

/**
 * Sets the focused input element value
 *
 * @param o view object to send the value
 * @param value the string value to be set
 */
EAPI void ewk_view_focused_input_element_value_set(Evas_Object* o, const char* value);
// #endif // ENABLE(TIZEN_INPUT_TAG_EXTENSION)

/**
 * Gets the focused input element's value
 *
 * @param o view object to get the value
 *
 * @return focused input element's value on success or NULL on failure.
 */
EAPI const char* ewk_view_focused_input_element_value_get(Evas_Object* o);


/**
 * Selects index of current popup menu.
 *
 * @param o view object contains popup menu.
 * @param index index of item to select
 *
 * @return @c EINA_TRUE on success, @c EINA_FALSE on failure (probably
 *         popup menu is not selected or index is out of range)
 */
EAPI Eina_Bool ewk_view_popup_menu_select(Evas_Object *o, unsigned int index);

/**
 * Selects Multiple indexes  of current popup menu.
 *
 * @param o view object contains popup menu.
 * @param changedlist  list of item selected and deselected
 *
 * @return @c EINA_TRUE on success, @c EINA_FALSE on failure (probably
 *         popup menu is not selected or index is out of range)
 */
EAPI Eina_Bool ewk_view_popup_menu_multiple_select(Evas_Object *o, Eina_Inarray* changedlist);

/**
 * Sets whether the ewk_view supports the mouse events or not.
 *
 * The ewk_view will support the mouse events if EINA_TRUE or not support the
 * mouse events otherwise. The default value is EINA_TRUE.
 *
 * @param o view object to enable/disable the mouse events
 * @param enabled a state to set
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_view_mouse_events_enabled_set(Evas_Object *o, Eina_Bool enabled);

/**
 * Queries if the ewk_view supports the mouse events.
 *
 * @param o view object to query if the mouse events are enabled
 *
 * @return @c EINA_TRUE if the mouse events are enabled or @c EINA_FALSE otherwise
 */
EAPI Eina_Bool ewk_view_mouse_events_enabled_get(const Evas_Object *o);

typedef Eina_Bool (*Ewk_Orientation_Lock_Cb)(Evas_Object* o, Eina_Bool need_lock, int orientation, void* user_data);

 /**
 * Deprecated
 * Sets callback of orientation lock function
 *
 * func will be called when screen lock is called or unlock is called.
 * When screen.lockOrientation is called, need_lock will be true and orientation
 * will be the flags which should be locked.
 * For example, when contents called 'screen.lockOrientation("portrait"), orientation
 * will be EWK_SCREEN_ORIENTATION_PORTRAIT_PRIMARY | EWK_SCREEN_ORIENTATION_PORTRAIT_SECONDARY
 * When screen.unlockOrientation is called, need_lock will be false.
 *
 * @param o view object to set the callback of orientation
 * @param func callback function to be called when screen orientation is locked or unlocked.
 * @param use_data user_data will be passsed when ewk_view_web_app_icon_get is called
 *
 * @return current URI on success or @c 0 on failure
 */
EAPI void ewk_view_orientation_lock_callback_set(Evas_Object *o, Ewk_Orientation_Lock_Cb func, void* user_data);

/**
 * Loads the specified @a html string as the content of the view.
 *
 * External objects such as stylesheets or images referenced in the HTML
 * document are located relative to @a baseUrl.
 *
 * If an @a unreachableUrl is passed it is used as the url for the loaded
 * content. This is typically used to display error pages for a failed
 * load.
 *
 * @param o view object to load document
 * @param html HTML data to load
 * @param base_uri Base URI used for relative paths to external objects (optional)
 * @param unreachable_uri URI that could not be reached (optional)
 *
 * @return @c EINA_TRUE on successful request, @c EINA_FALSE on errors
 */
EAPI Eina_Bool ewk_view_html_string_load(Evas_Object *o, const char *html, const char *base_uri, const char *unreachable_uri);

/**
 * Scales the current page, centered at the given point.
 *
 * @param o view object to set the zoom level
 * @param scale_factor a new level to set
 * @param cx x of center coordinate
 * @param cy y of center coordinate
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE otherwise
 */
EAPI Eina_Bool ewk_view_scale_set(const Evas_Object* o, double scaleFactor, int x, int y);

/**
 * Queries the current scale factor of the page.
 *
 * @param o view object to get the scale factor
 *
 * @return current scale factor in use on success or @c -1.0 on failure
 */
EAPI double ewk_view_scale_get(const Evas_Object *o);

/**
 * Searches and hightlights the given string in the document.
 *
 * @param o view object to find text
 * @param text text to find
 * @param options options to find
 * @param max_match_count maximum match count to find, unlimited if 0
 *
 * @return @c EINA_TRUE on success, @c EINA_FALSE on errors
 */
EAPI Eina_Bool ewk_view_text_find(Evas_Object* o, const char* text, Ewk_Find_Options options, unsigned int max_match_count);

/**
 * Clears the highlight of searched text.
 *
 * @param o view object to find text
 *
 * @return @c EINA_TRUE on success, @c EINA_FALSE on errors
 */
EAPI Eina_Bool ewk_view_text_find_highlight_clear(Evas_Object *o);

/*
 * Sets the user chosen color. To be used when implementing a color picker.
 *
 * The function should only be called when a color has been requested by the document.
 * If called when this is not the case or when the input picker has been dismissed, this
 * function will fail and return EINA_FALSE.
 *
 * @param o view object contains color picker
 * @param r red channel value to be set
 * @param g green channel value to be set
 * @param b blue channel value to be set
 * @param a alpha channel value to be set
 *
 * @return @c EINA_TRUE on success @c EINA_FALSE otherwise
 */
EAPI Eina_Bool ewk_view_color_picker_color_set(Evas_Object *o, int r, int g, int b, int a);

/**
 * Feeds the touch event to the view.
 *
 * @param o view object to feed touch event
 * @param type the type of touch event
 * @param points a list of points (Ewk_Touch_Point) to process
 * @param modifiers an Evas_Modifier handle to the list of modifier keys
 *        registered in the Evas. Users can get the Evas_Modifier from the Evas
 *        using evas_key_modifier_get() and can set each modifier key using
 *        evas_key_modifier_on() and evas_key_modifier_off()
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_view_feed_touch_event(Evas_Object *o, Ewk_Touch_Event_Type type, const Eina_List *points, const Evas_Modifier *modifiers);

/**
 * Sets whether the ewk_view supports the touch events or not.
 *
 * The ewk_view will support the touch events if @c EINA_TRUE or not support the
 * touch events otherwise. The default value is @c EINA_FALSE.
 *
 * @param o view object to enable/disable the touch events
 * @param enabled a state to set
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_view_touch_events_enabled_set(Evas_Object *o, Eina_Bool enabled);

/**
 * Queries if the ewk_view supports the touch events.
 *
 * @param o view object to query if the touch events are enabled
 *
 * @return @c EINA_TRUE if the touch events are enabled or @c EINA_FALSE otherwise
 */
EAPI Eina_Bool ewk_view_touch_events_enabled_get(const Evas_Object *o);

/**
 * Get main frame from given view
 *
 * @param o view object to retrieve main frame from
 *
 * @return main frame
 */
EAPI Ewk_Frame_Ref ewk_view_main_frame_get(Evas_Object* o);

/**
 * Sets the visibility of main frame scrollbar.
 *
 * @param o view object
 * @param visible visibility of main frame scrollbar
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_view_main_frame_scrollbar_visible_set(Evas_Object *o, Eina_Bool visible);

/**
 * Clear back forward list of a page.
 *
 * @param o view object to clear back forward list
 */
EAPI void ewk_view_back_forward_list_clear(const Evas_Object *o);

/// Enum values containing Content Security Policy header types.
enum _Ewk_CSP_Header_Type {
  EWK_REPORT_ONLY,
  EWK_ENFORCE_POLICY,
  EWK_DEFAULT_POLICY
};
typedef enum _Ewk_CSP_Header_Type Ewk_CSP_Header_Type;

/**
 * Set received Content Security Policy data from web app
 *
 * @param o view object
 * @param policy Content Security Policy data
 * @param type Content Security Policy header type
 *
 */
EAPI void ewk_view_content_security_policy_set(Evas_Object* o, const char* policy, Ewk_CSP_Header_Type type);

/**
 * When font-family is "Tizen", use system's Settings font as default font-family
 *
 * @param o view object
 *
 */
EAPI void ewk_view_use_settings_font(Evas_Object* o);

/*
 * Get cookies associated with an URL.
 *
 * @param o view object in which URL is opened.
 * @param url the url for which cookies needs to be obtained.
 *
 * @return @c character array containing cookies, @c NULL if no cookies are found.
 *
 * The return character array has to be owned by the application and freed when not required.
 */
EAPI char* ewk_view_get_cookies_for_url(Evas_Object* o, const char* url);

/*
 * Exit fullscreen when the back key is pressed.
 *
 * @param o view object to exit fullscreen mode
 *
 * @return @c EINA_TRUE if successful, @c EINA_FALSE otherwise
 */
EAPI Eina_Bool ewk_view_fullscreen_exit(Evas_Object* o);

/**
 * Sets whether to draw transparent background or not.
 *
 * @param o view object to enable/disable transparent background
 * @param enabled a state to set
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_view_draws_transparent_background_set(Evas_Object *o, Eina_Bool enabled);

/**
 * set a font for browser application
 *
 * @param o view object
 *
 */
EAPI void ewk_view_browser_font_set(Evas_Object* o);

/**
 * Gets the session data to be saved in a persistent store on browser exit
 *
 * @param ewkView view object whose session needs to be stored.
 * @param data out parameter session data
 * @param length out parameter length of session data
 *
 * @return void
 */
EAPI void ewk_view_session_data_get(Evas_Object* ewkView, const char** data, unsigned* length);

/**
 * Load the error page which web page is not found.
 *
 * @param ewkView view object whose session needs to be stored.
 * @param ErrorUrl that could not be found.
 *
 * @return void
 */
EAPI void ewk_view_not_found_error_page_load(Evas_Object* ewkView, const char* ErrorUrl);

/**
 * Enable or disable supporting of the split scrolling for overflow scroll.
 *
 * @param ewkView view object to set the support of the split scrolling for overflow scroll
 * @param enable @c EINA_TRUE to support split scrolling, @c EINA_FALSE not to support
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_view_split_scroll_overflow_enabled_set(Evas_Object* ewkView, const Eina_Bool enabled);

/**
 * Gets the back-forward list associated with this view.
 *
 * The returned instance is unique for this view and thus multiple calls
 * to this function with the same view as parameter returns the same
 * handle. This handle is alive while view is alive, thus one
 * might want to listen for EVAS_CALLBACK_DEL on given view
 * (@a o) to know when to stop using returned handle.
 *
 * @param o view object to get navigation back-forward list
 *
 * @return the back-forward list instance handle associated with this view
 */
EAPI Ewk_Back_Forward_List *ewk_view_back_forward_list_get(const Evas_Object *o);

/**
 * Set Draw Focus Ring for CacheBuilder
 *
 * @param ewkView view object
 * @@param enabled a state to set
 *
 */
EAPI void ewk_view_draw_focus_ring_enable_set(Evas_Object* ewkView, Eina_Bool enable);

/**
 * Queries the current zoom factor of the page.
 *
 * It returns previous zoom factor after ewk_view_page_zoom_factor_set is called immediately
 * until zoom factor of page is really changed.
 *
 * @param o view object to get the zoom factor
 *
 * @return current zoom factor in use on success or @c -1.0 on failure
 */
EAPI double ewk_view_page_zoom_get(const Evas_Object *o);

/**
 * Sets zoom factor of the current page.
 *
 * @param o view object to set the zoom level
 * @param zoom_factor a new level to set
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE otherwise
 */
EAPI Eina_Bool ewk_view_page_zoom_set(Evas_Object *o, double zoom_factor);

/**
 * Creates a new EFL WebKit view object with Evas_Smart and Ewk_Context.
 *
 * @note The Evas_Object which inherits the ewk_view should create its
 *       Evas_Object using this API instead of evas_object_smart_add()
 *       because the default initialization for ewk_view is done in this API.
 *
 * @param e canvas object where to create the view object
 * @param smart Evas_Smart object. Its type should be EWK_VIEW_TYPE_STR
 * @param context Ewk_Context object which is used for initializing
 * @param pageGroup Ewk_Page_Group object which is used for initializing
 *
 * @return view object on success or @c NULL on failure
 */
EAPI Evas_Object *ewk_view_smart_add(Evas *e, Evas_Smart *smart, Ewk_Context *context, Ewk_Page_Group *pageGroup);

#if !defined(EWK_BRINGUP)
/**
 * Callback for quota permission request feature.
 *
 * @param ewkView view object where quota permission was requested
 * @param request quota permission request object
 * @param user_data user_data passed to ewk_view_quota_permission_request_callback_set
 */
typedef void (*Ewk_Quota_Permission_Request_Callback)(Evas_Object* ewkView, const Ewk_Quota_Permission_Request* request, void* user_data);

/**
 * Sets callback quota permission request.
 *
 * func will be called when page requests increased storage quota.
 *
 * @param ewkView view object to set the callback
 * @param func callback function to be called when quota permission is requested
 * @param use_data user_data will be passsed to callback function
 *
 * @return void
 */
EAPI void ewk_view_quota_permission_request_callback_set(Evas_Object* ewkView, Ewk_Quota_Permission_Request_Callback callback, void* user_data);

/**
 * Set reply to quota permission request.
 *
 * Set @c EINA_TRUE if user accepts new quota size for origin
 * or @c EINA_FALSE if user doesn't accept new quota for origin.
 *
 * @param request view object to set the support of the split scrolling for overflow scroll
 * @param enable @c EINA_TRUE allow quota size, @c EINA_FALSE
 *
 * @return void
 */
EAPI void ewk_view_quota_permission_request_reply(const Ewk_Quota_Permission_Request* request, const Eina_Bool allow);

/**
 * Cancels quota permission request.
 *
 * @param request view object to set the support of the split scrolling for overflow scroll
 *
 * @return void
 */
EAPI void ewk_view_quota_permission_request_cancel(const Ewk_Quota_Permission_Request* request);
#endif // EWK_BRINGUP

/*
 * Requests web login using password database.
 *
 * @param o view object
 *
 * @return void
 */
EAPI Eina_Bool ewk_view_web_login_request(Evas_Object* ewkView);

#ifdef __cplusplus
}
#endif

#endif // ewk_view_h
