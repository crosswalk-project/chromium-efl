/*
 * Copyright (C) 2014 Samsung Electronics
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
*/

#ifndef WEB_EVENT_FACTORY_EFL
#define WEB_EVENT_FACTORY_EFL

#include "content/browser/renderer_host/render_widget_host_impl.h"
#include "content/public/browser/native_web_keyboard_event.h"
#include "third_party/WebKit/public/web/WebInputEvent.h"
#include "ui/events/event.h"

#include <Evas.h>

namespace tizen_webview {
struct Touch_Point;
}

namespace content {

class WebEventFactoryEfl {
 public:
  static blink::WebMouseEvent toWebMouseEvent(Evas*, Evas_Object*, const Evas_Event_Mouse_Down*, float scale_factor);
  static blink::WebMouseEvent toWebMouseEvent(Evas*, Evas_Object*,  const Evas_Event_Mouse_Up*, float scale_factor);
  static blink::WebMouseEvent toWebMouseEvent(Evas*,  Evas_Object*, const Evas_Event_Mouse_Move*, float scale_factor);
  static blink::WebMouseWheelEvent toWebMouseEvent(Evas*,  Evas_Object*, const Evas_Event_Mouse_Wheel*, float scale_factor);
  static content::NativeWebKeyboardEvent toWebKeyboardEvent(Evas*, const Evas_Event_Key_Down*);
  static content::NativeWebKeyboardEvent toWebKeyboardEvent(Evas*, const Evas_Event_Key_Up*);

  static ui::TouchEvent toUITouchEvent(tizen_webview::Touch_Point*, Evas_Object* evas_object, float scale_factor);
};

}

#endif
