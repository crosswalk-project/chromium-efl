// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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

  static ui::TouchEvent toUITouchEvent(const tizen_webview::Touch_Point*, Evas_Object* evas_object, float scale_factor);
  static bool isHardwareBackKey(const Evas_Event_Key_Down* event);
};

}

#endif
