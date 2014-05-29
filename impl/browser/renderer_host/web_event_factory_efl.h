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

typedef struct _Ecore_Event_Mouse_Button Ecore_Event_Mouse_Button;
typedef struct _Ecore_Event_Mouse_Wheel  Ecore_Event_Mouse_Wheel;
typedef struct _Ecore_Event_Mouse_Move   Ecore_Event_Mouse_Move;
typedef struct _Ecore_Event_Key          Ecore_Event_Key;
typedef struct _Evas Evas;
typedef struct _Evas_Event_Mouse_Down    Evas_Event_Mouse_Down;
typedef struct _Evas_Event_Mouse_Up      Evas_Event_Mouse_Up;
typedef struct _Evas_Event_Mouse_Move    Evas_Event_Mouse_Move;
typedef struct _Evas_Event_Mouse_Wheel   Evas_Event_Mouse_Wheel;
typedef struct _Evas_Event_Key_Down      Evas_Event_Key_Down;
typedef struct _Evas_Event_Key_Up        Evas_Event_Key_Up;
typedef struct _Ewk_Touch_Point          Ewk_Touch_Point;
typedef struct _Evas_Object Evas_Object;

namespace content {

class WebEventFactoryEfl {
 public:
  static blink::WebMouseEvent toWebMouseEvent(const Ecore_Event_Mouse_Button*, bool pressed);
  static blink::WebMouseWheelEvent toWebMouseEvent(const Ecore_Event_Mouse_Wheel*);
  static blink::WebMouseEvent toWebMouseEvent(const Ecore_Event_Mouse_Move*);

  static content::NativeWebKeyboardEvent toWebKeyboardEvent(const Ecore_Event_Key*, bool pressed);

  static blink::WebMouseEvent toWebMouseEvent(Evas*, Evas_Object*, const Evas_Event_Mouse_Down*);
  static blink::WebMouseEvent toWebMouseEvent(Evas*, Evas_Object*,  const Evas_Event_Mouse_Up*);
  static blink::WebMouseEvent toWebMouseEvent(Evas*,  Evas_Object*, const Evas_Event_Mouse_Move*);
  static blink::WebMouseWheelEvent toWebMouseEvent(Evas*,  Evas_Object*, const Evas_Event_Mouse_Wheel*);
  static content::NativeWebKeyboardEvent toWebKeyboardEvent(Evas*, const Evas_Event_Key_Down*);
  static content::NativeWebKeyboardEvent toWebKeyboardEvent(Evas*, const Evas_Event_Key_Up*);

  static ui::TouchEvent toUITouchEvent(Ewk_Touch_Point*, Evas_Object* evas_object);
};

}

#endif
