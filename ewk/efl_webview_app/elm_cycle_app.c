// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <Elementary.h>

#include <string.h>
#include <stdio.h>
#include <Evas.h>
#include <Eina.h>
#include <Ecore.h>
#include <Ecore_X.h>
#include "efl_integration/public/ewk_context.h"
#include "efl_integration/public/ewk_main.h"
#include "efl_integration/public/ewk_view.h"

static Evas_Object* g_win;
static Evas_Object* g_testObject;
static Ecore_Timer* g_timer;

static int g_updateTime = 10;
static int g_repeatCount = 25;
static const char* g_urls[4] = {"http://www.google.com", "http://www.yahoo.com",
                                "http://msn.com", "http://naver.com"};
static int g_urlCount = 4;

//back forward not working yet
#define BACK_FORWARD_TEST 0

void render_init(int argc, char** argv);
void render_start();
void render_stop();
void render_deinit();

//close button handler
static void
on_done(void *data, Evas_Object *obj, void *event_info);

static Eina_Bool update_timer_callback(void* data)
{
  printf("%s\n", __FUNCTION__);
  render_stop();
  render_start();
  g_timer = NULL;
  return ECORE_CALLBACK_CANCEL;
}
int in_back_test()
{
#if BACK_FORWARD_TEST
  if(g_repeatCount <= 16 && g_repeatCount > 13)
    return 1;
  else
    return 0;
#else
  return 0;
#endif
}
int in_forward_test()
{
#if BACK_FORWARD_TEST
  if(g_repeatCount <= 13 && g_repeatCount > 8)
    return 1;
  else
    return 0;
#else
  return 0;
#endif
}

void render_init(int argc, char** argv)
{
  const char* env;

  if ((env = getenv("WEBVIEW_TEST_INTERVAL")))
      g_updateTime = atoi(env);

  if ((env = getenv("WEBVIEW_TEST_COUNT")))
      g_repeatCount = atoi(env);

  printf("### WebView test (%d)\n", g_repeatCount);

  elm_init(argc, argv);
  elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_CLOSED);

  g_win = elm_win_add(NULL, "webview-profiling", ELM_WIN_BASIC);
  evas_object_smart_callback_add(g_win, "delete,request", on_done, NULL);
  evas_object_size_hint_weight_set(g_win,
         EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

  int w, h;
  ecore_x_window_size_get(ecore_x_window_root_first_get(), &w, &h);
  evas_object_move(g_win, 0, 0);
  evas_object_resize(g_win, w, h);
  evas_object_color_set(g_win, 255, 255, 255, 255);
  evas_object_show(g_win);
  ewk_init();
}

void render_deinit()
{
  printf("%s\n", __FUNCTION__);
  if (g_win) {
      evas_object_del(g_win);
      g_win = NULL;
  }
}


void render_start()
{
  if(g_repeatCount <= 0) {
    evas_object_del(g_win);
     g_win = NULL;
     return;
  }
  if(in_back_test()) {
    if(ewk_view_back_possible(g_testObject)) {
      printf("Test #%d Back possible, going back \n", 26 - g_repeatCount);
      ewk_view_back(g_testObject);
    }
    else {
      printf("back not possible #%d URL: %s \n", 26 - g_repeatCount, g_urls[g_repeatCount%g_urlCount]);
      ewk_view_url_set(g_testObject, g_urls[g_repeatCount%g_urlCount]);
    }
    g_timer = ecore_timer_add(g_updateTime, update_timer_callback, NULL);
    return;
  }
  if(in_forward_test()) {
    if(ewk_view_forward_possible(g_testObject)) {
      printf("Test #%d forward possible, going forward \n", 26 - g_repeatCount );
      ewk_view_forward(g_testObject);
    }
    else {
      printf("forward not possible Test #%d URL: %s \n", 26 - g_repeatCount, g_urls[g_repeatCount%g_urlCount]);
      ewk_view_url_set(g_testObject, g_urls[g_repeatCount%g_urlCount]);
    }

    g_timer = ecore_timer_add(g_updateTime, update_timer_callback, NULL);
    return;
  }

  if (g_testObject) {
    evas_object_del(g_testObject);
    g_testObject = NULL;
  }

  Evas* e = evas_object_evas_get (g_win);

  // create webview
  Ewk_Context* context = ewk_context_default_get();
  g_testObject = ewk_view_add_with_context(e, context);
  int w, h;
  ecore_x_window_size_get(ecore_x_window_root_first_get(), &w, &h);
  evas_object_resize(g_testObject, w, h);

  // load start
  printf("Test #%d URL: %s \n", 26 - g_repeatCount, g_urls[g_repeatCount%g_urlCount]);
  ewk_view_url_set(g_testObject, g_urls[g_repeatCount%g_urlCount]);

  evas_object_focus_set(g_testObject, EINA_TRUE);
  evas_object_show(g_testObject);

  g_timer = ecore_timer_add(g_updateTime, update_timer_callback, NULL);
}

void render_stop()
{
  g_repeatCount--;
  printf("render stop\n");
  if (!g_testObject) {
    return;
  }

  if (g_timer) {
    ecore_timer_del(g_timer);
    g_timer = NULL;
  }
  if(!(in_back_test() || in_forward_test())) {
    printf("delete evas objects\n");
    //we dont delete g_win object because that terminates ecore main loop
    evas_object_del(g_testObject);
    g_testObject = NULL;
  }
}

static void on_done(void *data, Evas_Object *obj, void *event_info)
{
  render_stop();
  elm_exit();
}

int main(int argc, char **argv)
{
  render_init(argc, argv);
  render_start();
  elm_run();
  elm_shutdown();
  return 0;
}
