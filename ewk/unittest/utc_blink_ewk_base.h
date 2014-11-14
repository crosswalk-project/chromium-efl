// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

/**
 * @file   utc_blink_ewk_base.h
 * @author Kamil Klimek <k.klimek@partner.samsung.com>
 * @date   2014-02-28
 * @brief  A header file to be used by all unit test cases for Chromium EFL API
 *
 * This header file contains a common content for unit test cases, i.e.:
 * it includes useful header files, defines shared object pointers,
 * useful macros, functions and enumeration types.
 */


#ifndef UTC_BLINK_EWK_BASE_H
#define UTC_BLINK_EWK_BASE_H

/* google test header */
#include "gtest/gtest.h"
/* Common headers */
#include <Ecore_Evas.h>
#include <Ecore.h>
#include <Edje.h>
#include <Eina.h>
#include <Elementary.h>
#include <glib-object.h>
#include <glib.h>
#include <Evas.h>

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
/* ewk header */
#include "ewk_auth_challenge.h"
#include "ewk_certificate.h"
#include "ewk_console_message.h"
#include "ewk_context.h"
#include "ewk_context_menu.h"
#include "ewk_cookie_manager.h"
#include "ewk_custom_handlers.h"
#include "ewk_enums.h"
#include "ewk_error.h"
#include "ewk_export.h"
#include "ewk_geolocation.h"
#include "ewk_log.h"
#include "ewk_main.h"
#include "ewk_notification.h"
#include "ewk_policy_decision.h"
#include "ewk_settings.h"
#include "ewk_touch.h"
#include "ewk_user_media.h"
#include "ewk_view.h"
#include "ewk_hit_test.h"
#include "ewk_text_style.h"
#include "ewk_web_application_icon_data.h"

/* Definitions and macros */
#define DEFAULT_WIDTH_OF_WINDOW 480
#define DEFAULT_HEIGHT_OF_WINDOW 800
/* User macros */
#define utc_message(msg, ...) fprintf(stderr, msg "\n", ##__VA_ARGS__)

/* Compatibility macros only. Please use gtest macros instead. This will be removed. */
#define utc_check_true(condition) EXPECT_TRUE(condition)
#define utc_check_false(condition) EXPECT_FALSE(condition)
#define utc_check_eq(value, needed) EXPECT_EQ(value, needed)
#define utc_check_ne(value, needed) EXPECT_NE(value, needed)
#define utc_check_gt(value, needed) EXPECT_GT(value, needed)
#define utc_check_ge(value, needed) EXPECT_GE(value, needed)
#define utc_check_lt(value, needed) EXPECT_LT(value, needed)
#define utc_check_le(value, needed) EXPECT_LE(value, needed)
#define utc_check_str_eq(value, needed) EXPECT_STREQ(value, needed)
#define utc_check_str_ne(value, needed) EXPECT_STRNE(value, needed)
#define utc_assert_eq(value, needed) ASSERT_EQ(value, needed)
#define utc_assert_ne(value, needed) ASSERT_NE(value, needed)
#define utc_assert_gt(value, needed) ASSERT_GT(value, needed)
#define utc_assert_ge(value, needed) ASSERT_GE(value, needed)
#define utc_assert_lt(value, needed) ASSERT_LT(value, needed)
#define utc_assert_le(value, needed) ASSERT_LE(value, needed)
#define utc_assert_str_eq(value, needed) ASSERT_STREQ(value, needed)
#define utc_assert_str_ne(value, needed) ASSERT_STRNE(value, needed)
#define utc_pass() SUCCEED()
#define utc_fail() FAIL()

class utc_blink_ewk_base: public ::testing::Test
{
public:
    enum MainLoopResult {
      NoOp = 0,
      Success,
      Failure,
      LoadFailure,
      Timeout
    };
protected:
    class evas_object_smart_callback_auto
    {
    public:
      evas_object_smart_callback_auto(Evas_Object* obj, const char* event, Evas_Smart_Cb func, const void* data)
        : obj_(obj)
        , event_(eina_stringshare_add(event))
        , func_(func)
      {
        evas_object_smart_callback_add(obj, event, func, data);
      }

      ~evas_object_smart_callback_auto()
      {
        evas_object_smart_callback_del(obj_, event_, func_);
        eina_stringshare_del(event_);
      }

    private:
      Evas_Object* obj_;
      Eina_Stringshare* event_;
      Evas_Smart_Cb func_;
    };

    utc_blink_ewk_base();
    virtual ~utc_blink_ewk_base();

    void SetLogJavascript(bool enabled) { log_javascript = enabled; }

    /**
     * Generates proper path for resource file
     *
     * @param [in] resource_path  resource file subpath
     *
     * @retval  full resource path
     */
    std::string GetResourcePath(const char* resource_path) const;

    /**
     * Generates proper url for resource file
     *
     * @param [in] resource_path  resource file subpath
     *
     * @retval  full resource url including file://
     */
    std::string GetResourceUrl(const char* resource_path) const;

    /**
     * Starts main application loop.
     *
     * @param [in] max_time  main loop timeout
     *
     * @retval MainLoopResult::NoOp if event loop is already running.
     *         MainLoopResult::Success if event loop was stopped normally.
     *         MainLoopResult::Failure if stopped abnormally.
     *         MainLoopResult::LoadFailure if "load,error" smart callback was invoked.
     *         MainLoopResult::Timeout if max_time was reached.
     */
    MainLoopResult EventLoopStart(double max_time = 30.0);

    /**
     * Waits given time on main application loop.
     *
     * @params [in] time  time to wait
     */
    bool EventLoopWait(double time);

    /**
     * Stop main application loop.
     *
     * @param [in] result  result to be returned by EventLoopStart.
     *                     if result is MainLoopResult::NoOp this method will do nothing.
     *
     * @retval true if this call stopped event loop, false otherwise.
     *
     * @note This metod may be called multiple times, but only first call will change
     *       event loop result. All other calls will do nothing and return false.
     */
    bool EventLoopStop(MainLoopResult result);

    /**
     * Override this method if you need to peform some extra
     * initialization before initializing ewk.
     */
    virtual void PreSetUp() {}

    /**
     * Override this method if you need to perform some extra
     * initialization after initializing ewk.
     */
    virtual void PostSetUp() {}

    /**
     * Override this method if you need to perform some extra
     * deinitialization before deinitializng ewk.
     */
    virtual void PreTearDown() {}

    /**
     * Override this method if you need to perform some extra
     * deinitialization after deinitialzing ewk.
     */
    virtual void PostTearDown() {}

    /**
     * Called when main application loop timeouted.
     *
     * @retval true if timeout was handled by method, false otherwise.
     *         If method returns false then main application event loop will be stopped with error.
     */
    virtual bool TimeOut() { return false; }

    /**
     * Override this method if you need to handle "load,finished" ewk callback.
     */
    virtual void LoadFinished(Evas_Object* webview) {}

    /**
     * Override this method if you need to handle "load,started" ewk callback.
     */
    virtual void LoadStarted(Evas_Object* webview) {}

    /**
     * Override this method if you need to handle "load,error" ewk callback.
     *
     * @retval true if "load,error" was handled by method, false otherwise.
     *         If method returns false then main application event loop will be stopped with error.
     */
    virtual bool LoadError(Evas_Object* webview, Ewk_Error *error) { EventLoopStop(LoadFailure); return false; }

    /**
     * Override this method if you need to handle "load,progress" ewk callback.
     */
    virtual void LoadProgress(Evas_Object* webview, double progress) {}

    /**
     * Override this method if you need to handle "load,progress,started" ewk callback.
     */
    virtual void LoadProgressStarted(Evas_Object* webview) {}

    /**
     * Override this method if you need to handle "load,progress,finished" ewk callback.
     */
    virtual void LoadProgressFinished(Evas_Object* webview) {}

    /**
     * Performs unit test setup. It calls PreSetUp before default initialization
     * and PostSetUp after default initialization.
     *
     * @note This method should not be overriden.
     */
    void SetUp() final;

    /**
     * Performs unit test deinitialization. It calls PreTearDown before default deinitialization
     * and PostTearDown after default deinitialization.
     *
     * @note This method should not be overriden.
     */
    void TearDown() final;

    template<class T> static void OwnerFromVoid(void *owner, T** out)
    {
      if(out) {
        *out = static_cast<T*>(owner);
      }
    }

    template<typename T> static Evas_Smart_Cb ToSmartCallback(T proc)
    {
      return reinterpret_cast<Evas_Smart_Cb>(proc);
    }

    inline Evas_Object* GetEwkWebView() const { return ewk_webview; }
    inline Evas_Object* GetEwkWindow() const { return ewk_window; }
    inline Evas* GetEwkEvas() const { return ewk_evas; }

private:
    /**
     * Default "load,started" event callback, will call LoadStarted method
     *
     * @see LoadStarted
     */
    static void load_started_cb(void* data, Evas_Object* webview, void* event_info);

    /**
     * Default "load,finished" event callback, will call LoadFinish method
     *
     * @see LoadFinished
     */
    static void load_finished_cb(void* data, Evas_Object* webview, void* event_info);

    /**
     * Default "load,error" event callback, will call LoadError method
     *
     * @see LoadError
     */
    static void load_error_cb(void* data, Evas_Object* webview, void* event_info);

    /**
     * Default "load,progress" event callback, will call LoadProgress method
     *
     * @see LoadProgress
     */
    static void load_progress_cb(void* data, Evas_Object* webview, void* event_info);

    /**
     * Default "load,progress,started" event callback, will call LoadProgressStarted method
     *
     * @see LoadProgressStarted
     */
    static void load_progress_started_cb(void* data, Evas_Object* webview, void* event_info);
    /**
     * Default load,progress,finished" event callback, will call LoadProgressFinished method
     *
     * @see LoadProgressFinished
     */
    static void load_progress_finished_cb(void* data, Evas_Object* webview, void* event_info);

    /**
     * Default "console,message" event callback, logs message to console using utc_message
     */
    static void console_message_logger_cb(void* data, Evas_Object* webview, void* event_info);

    /**
     * Default event loop timeout callback, will call TimeOut method
     *
     * @see TimeOut
     */
    static Eina_Bool timeout_cb(void *data);

    /**
     * EventLoopWait timeotut callback, will stop event loop
     */
    static Eina_Bool timeout_cb_event_loop_wait(void *data);

private:
    /**
     * Initializes objects used in the test
     *
     * This method is always called in SetUp method.
     * @see SetUp
     */
    void EwkInit();

    /**
     * Deinitializes and frees resources used during test.
     *
     * This method is always called in TearDown method.
     * @see TearDown
     */
    void EwkDeinit();

private:
    Ecore_Timer *timeout;
    bool main_loop_running;
    MainLoopResult main_loop_result;

    bool log_javascript;

    Evas_Object* ewk_window;
    Evas* ewk_evas;
    Evas_Object* ewk_background;
    Evas_Object* ewk_webview;
    Ewk_Context* ewk_context;

    char *resource_dir;
};

#endif // UTC_BLINK_EWK_BASE_HUTC_BLINK_EWK_BASE_H

