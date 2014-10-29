// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UTC_BLINK_EWK_CONTEXT_MENU_H
#define UTC_BLINK_EWK_CONTEXT_MENU_H

#include "ewk_context_menu.h"
#include <Ecore.h>
#include <Evas.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
    CUSTOM_CONTEXT_MENU_ITEM_BASE_TAG = EWK_CONTEXT_MENU_ITEM_BASE_APPLICATION_TAG,
    CUSTOM_CONTEXT_MENU_ITEM_TEST_TAG
};

static void feed_mouse_click(int button, int x, int y, Evas* evas)
{
  evas_event_feed_mouse_move(evas, x, y, ecore_time_get(), 0);
  evas_event_feed_mouse_down(evas, button, EVAS_BUTTON_NONE, ecore_time_get(), 0);
  evas_event_feed_mouse_up(evas, button, EVAS_BUTTON_NONE, ecore_time_get(), 0);
}

#ifdef __cplusplus
}
#endif

#endif /* UTC_WEBKIT2_EWK_CONTEXT_MENU_H */
