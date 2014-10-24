/**
 * @file   utc_blink_ewk_base.cpp
 * @author Kamil Klimek <k.klimek@partner.samsung.com>
 * @date   2014-03-03
 * @brief  A Source file to be used by all unit test cases for Chromium EFL API
 *
 * This header file contains a common content for unit test cases, i.e.:
 * it includes useful header files, defines shared object pointers,
 * useful macros, functions and enumeration types.
 */

#include "utc_blink_ewk_base.h"

utc_blink_ewk_base::utc_blink_ewk_base()
    : ::testing::Test()
    , timeout(NULL)
    , main_loop_running(false)
    , main_loop_result(utc_blink_ewk_base::NoOp)
    , log_javascript(true)
    , ewk_window(NULL)
    , ewk_evas(NULL)
    , ewk_background(NULL)
    , ewk_webview(NULL)
    , ewk_context(NULL)
    , resource_dir(NULL)
{
    char *env_dir = getenv("UTC_RESOURCE_PATH");

    if (env_dir) {
        resource_dir = strdup(env_dir);
    } else {
        resource_dir = strdup("/opt/usr/resources");
    }
}

utc_blink_ewk_base::~utc_blink_ewk_base()
{
    if (timeout) {
        ecore_timer_del(timeout);
        timeout = NULL;
    }

    free(resource_dir);
}

std::string utc_blink_ewk_base::GetResourcePath(const char* resource_path) const
{
    std::string retval = resource_dir;
    retval.append("/");
    retval.append(resource_path);

    return retval;
}

std::string utc_blink_ewk_base::GetResourceUrl(const char* resource_path) const
{
    std::string retval("file://");
    retval.append(GetResourcePath(resource_path));
    return retval;
}

utc_blink_ewk_base::MainLoopResult utc_blink_ewk_base::EventLoopStart(double max_time)
{
    utc_blink_ewk_base::MainLoopResult retval = utc_blink_ewk_base::NoOp;

    if (!main_loop_running) {
        main_loop_result = utc_blink_ewk_base::NoOp;

        utc_message("[EventLoopStart] :: timeout: %f", max_time);
        main_loop_running = true;
        timeout = ecore_timer_add(max_time, timeout_cb, this);
        ecore_main_loop_begin();

        if (timeout) {
          ecore_timer_del(timeout);
          timeout = NULL;
        }

        main_loop_running = false;
        retval = main_loop_result;
    }

    return retval;
}

bool utc_blink_ewk_base::EventLoopWait(double time)
{
    if (!main_loop_running) {
        utc_message("[EventLoopWait] :: time %f", time);
        main_loop_running = true;
        timeout = ecore_timer_add(time, timeout_cb_event_loop_wait, this);

        ecore_main_loop_begin();

        if (timeout) {
          ecore_timer_del(timeout);
          timeout = NULL;
        }

        main_loop_running = false;
        return true;
    }

    return false;
}

bool utc_blink_ewk_base::EventLoopStop(utc_blink_ewk_base::MainLoopResult result)
{
    if (main_loop_running && result != utc_blink_ewk_base::NoOp ) {
        utc_message("[EventLoopStop] :: Setting result to: %s", (result == utc_blink_ewk_base::Success ? "Success" : "Failure"));
        main_loop_running = false;
        main_loop_result = result;
        ecore_main_loop_quit();
        return true;
    }

    return false;
}

void utc_blink_ewk_base::SetUp()
{
    PreSetUp();

    EwkInit();

    evas_object_smart_callback_add(ewk_webview, "load,started", load_started_cb, this);
    evas_object_smart_callback_add(ewk_webview, "load,finished", load_finished_cb, this);
    evas_object_smart_callback_add(ewk_webview, "load,error", load_error_cb, this);
    evas_object_smart_callback_add(ewk_webview, "load,progress", load_progress_cb, this);
    evas_object_smart_callback_add(ewk_webview, "load,progress,started", load_progress_started_cb, this);
    evas_object_smart_callback_add(ewk_webview, "load,progress,finished", load_progress_finished_cb, this);
    evas_object_smart_callback_add(ewk_webview, "console,message", console_message_logger_cb, this);

    PostSetUp();
}

void utc_blink_ewk_base::TearDown()
{
    PreTearDown();

    evas_object_smart_callback_del(ewk_webview, "load,started", load_started_cb);
    evas_object_smart_callback_del(ewk_webview, "load,finished", load_finished_cb);
    evas_object_smart_callback_del(ewk_webview, "load,error", load_error_cb);
    evas_object_smart_callback_del(ewk_webview, "load,progress", load_progress_cb);
    evas_object_smart_callback_del(ewk_webview, "load,progress,started", load_progress_started_cb);
    evas_object_smart_callback_del(ewk_webview, "load,progress,finished", load_progress_finished_cb);
    evas_object_smart_callback_del(ewk_webview, "console,message", console_message_logger_cb);

    EwkDeinit();

    PostTearDown();
}

void utc_blink_ewk_base::load_started_cb(void* data, Evas_Object* webview, void* event_info)
{
    utc_message("[load,started] :: data: %p, webview: %p, event_info: %p", data, webview, event_info);
    utc_blink_ewk_base *ut = static_cast<utc_blink_ewk_base*>(data);
    ut->LoadStarted(webview);
}

void utc_blink_ewk_base::load_finished_cb(void* data, Evas_Object* webview, void* event_info)
{
    utc_message("[load,finished] :: data: %p, webview: %p, event_info: %p", data, webview, event_info);
    utc_blink_ewk_base *ut = static_cast<utc_blink_ewk_base*>(data);
    ut->LoadFinished(webview);
}

void utc_blink_ewk_base::load_error_cb(void* data, Evas_Object* webview, void* event_info)
{
    utc_message("[load,error] :: data: %p, webview: %p, event_info: %p", data, webview, event_info);
    utc_blink_ewk_base *ut = static_cast<utc_blink_ewk_base*>(data);
    Ewk_Error *err = static_cast<Ewk_Error *>(event_info);

    if(!ut->LoadError(webview, err)) {
        utc_message("[load,error] :: not handled by test, stopping main loop with Failure");
        ut->EventLoopStop(utc_blink_ewk_base::Failure);
    }
}

void utc_blink_ewk_base::load_progress_cb(void* data, Evas_Object* webview, void* event_info)
{
    utc_message("[load,progress] :: data: %p, webview: %p, event_info: %p", data, webview, event_info);
    double progress = -1.0;
    if (event_info) {
        progress = *((double*)event_info);
    }

    utc_blink_ewk_base *ut = static_cast<utc_blink_ewk_base*>(data);
    ut->LoadProgress(webview, progress);
}

void utc_blink_ewk_base::load_progress_started_cb(void* data, Evas_Object* webview, void* event_info)
{
    utc_message("[load,progress,started] :: data: %p, webview: %p, event_info: %p", data, webview, event_info);
    utc_blink_ewk_base *ut = static_cast<utc_blink_ewk_base*>(data);
    ut->LoadProgressStarted(webview);
}

void utc_blink_ewk_base::load_progress_finished_cb(void* data, Evas_Object* webview, void* event_info)
{
    utc_message("[load,progress,finished] :: data: %p, webview: %p, event_info: %p", data, webview, event_info);
    utc_blink_ewk_base *ut = static_cast<utc_blink_ewk_base*>(data);
    ut->LoadProgressFinished(webview);
}

void utc_blink_ewk_base::console_message_logger_cb(void* data, Evas_Object* webview, void* event_info)
{
  if (data && event_info) {
    utc_blink_ewk_base* owner = static_cast<utc_blink_ewk_base*>(data);

    if (owner->log_javascript && owner->ewk_webview == webview) {
      Ewk_Console_Message* console = static_cast<Ewk_Console_Message*>(event_info);
      const char *msg = ewk_console_message_text_get(console);

      if (msg) {
        utc_message("JavaScript::console (%p): %s", webview, msg);
      } else {
        utc_message("JavaScript::console (%p) ERROR: NULL messsage", webview);
      }
    }
  }
}

Eina_Bool utc_blink_ewk_base::timeout_cb(void *data)
{
    utc_message("[timeout] :: data: %p", data);
    utc_blink_ewk_base *ut = static_cast<utc_blink_ewk_base*>(data);

    if (!ut->TimeOut()) {
        utc_message("[timeout] :: not handled by test, stopping main loop with Failure");
        ut->EventLoopStop(utc_blink_ewk_base::Failure);
    }

    ut->timeout = NULL;
    return ECORE_CALLBACK_CANCEL;
}

Eina_Bool utc_blink_ewk_base::timeout_cb_event_loop_wait(void *data)
{
    utc_blink_ewk_base *ut = static_cast<utc_blink_ewk_base*>(data);
    ecore_main_loop_quit();
    ut->timeout = NULL;
    return ECORE_CALLBACK_CANCEL;
}

void utc_blink_ewk_base::EwkInit()
{
    /* 1. Standard TETware test initialization message */
    utc_message("[[ TET_MSG ]]:: ============ Startup ============");

    ewk_window = elm_win_add(NULL, "TC Launcher", ELM_WIN_BASIC);
    elm_win_title_set(ewk_window, "TC Launcher");
    ewk_evas = evas_object_evas_get(ewk_window);

    ewk_background = evas_object_rectangle_add(ewk_evas);
    evas_object_name_set(ewk_background, "view");
    evas_object_color_set(ewk_background, 255, 0, 255, 255);
    evas_object_move(ewk_background, 0, 0);
    evas_object_resize(ewk_background, DEFAULT_WIDTH_OF_WINDOW, DEFAULT_HEIGHT_OF_WINDOW);
    evas_object_layer_set(ewk_background, EVAS_LAYER_MIN);

    evas_object_show(ewk_background);

    /* 3. Initialization of webview */
    ewk_context = ewk_context_default_get();
    ewk_webview = ewk_view_add_with_context(ewk_evas, ewk_context);
    evas_object_move(ewk_webview, 10, 10);
    evas_object_resize(ewk_webview, DEFAULT_WIDTH_OF_WINDOW - 20, DEFAULT_HEIGHT_OF_WINDOW - 20);

    evas_object_show(ewk_webview);
    evas_object_show(ewk_window);
}

void utc_blink_ewk_base::EwkDeinit()
{
    /* 1. Standard TETware test end/cleanup messages */
    utc_message("[[ TET_MSG ]]:: ============ Cleanup ============");

    /* 2. Freeing resources */
    ewk_context_unref(ewk_context);
    if (ewk_webview)
        evas_object_del(ewk_webview);

    if (ewk_window)
        evas_object_del(ewk_window);

    ewk_window = NULL;
    ewk_evas = NULL;
    ewk_background = NULL;
    ewk_webview = NULL;
}

