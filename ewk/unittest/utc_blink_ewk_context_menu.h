/*
 * chromium EFL
 *
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */

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
