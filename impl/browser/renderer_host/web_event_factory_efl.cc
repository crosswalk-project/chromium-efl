// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "browser/renderer_host/web_event_factory_efl.h"

#include "base/time/time.h"
#include "base/strings/utf_string_conversions.h"
#include "third_party/WebKit/public/web/WebInputEvent.h"
#include "ui/events/keycodes/keyboard_codes.h"
#include "ui/events/keycodes/keyboard_code_conversion_x.h"

#include <Ecore.h>
#include <Ecore_X.h>

// TODO: Figure out how to avoid this includes.
#include <X11/Xutil.h>
#include <X11/keysym.h>

#include "tizen_webview/public/tw_touch_point.h"

using namespace blink;
using namespace tizen_webview;

namespace {

static void TranslateEvasCoordToWebKitCoord(Evas_Object *web_view, int& x, int& y) {
  Evas_Coord tmpX, tmpY;
  evas_object_geometry_get(web_view, &tmpX, &tmpY, 0, 0);
  x -= tmpX;
  y -= tmpY;
}

}

namespace content {

static const float cDefaultScrollStep = 20;

static ui::KeyboardCode KeyboardCodeFromEflKey(const char* key) {
  return ui::KeyboardCodeFromXKeysym(XStringToKeysym(key));
}

static int WindowsKeyCodeFromEflKey(const char* key) {
  int windows_key_code = KeyboardCodeFromEflKey(key);
  if (windows_key_code == ui::VKEY_SHIFT ||
      windows_key_code == ui::VKEY_CONTROL ||
      windows_key_code == ui::VKEY_MENU) {
    // To support DOM3 'location' attribute, we need to lookup an X KeySym and
    // set ui::VKEY_[LR]XXX instead of ui::VKEY_XXX.
    KeySym keysym = XStringToKeysym(key);
    switch (keysym) {
      case XK_Shift_L:
        return ui::VKEY_LSHIFT;
      case XK_Shift_R:
        return ui::VKEY_RSHIFT;
      case XK_Control_L:
        return ui::VKEY_LCONTROL;
      case XK_Control_R:
        return ui::VKEY_RCONTROL;
      case XK_Meta_L:
      case XK_Alt_L:
        return ui::VKEY_LMENU;
      case XK_Meta_R:
      case XK_Alt_R:
        return ui::VKEY_RMENU;
    }
  }
  return windows_key_code;
}

static int NativeKeyCodeFromEflKey(const char* key) {
  //return ecore_x_keysym_keycode_get(ev->key);
  return XKeysymToKeycode((Display*)ecore_x_display_get(), XStringToKeysym(key));
}

static int CharacterFromEflString(const char* string) {
  if (string) {
    base::string16 result;
    base::UTF8ToUTF16(string, strlen(string), &result);
    return result.length() == 1 ? result[0] : 0;
  }

  return 0;
}

// From
// third_party/blink/Source/blink/chromium/src/gtk/WebInputEventFactory.cpp:
static blink::WebUChar GetControlCharacter(int windows_key_code, bool shift) {
  if (windows_key_code >= ui::VKEY_A &&
    windows_key_code <= ui::VKEY_Z) {
    // ctrl-A ~ ctrl-Z map to \x01 ~ \x1A
    return windows_key_code - ui::VKEY_A + 1;
  }
  if (shift) {
    // following graphics chars require shift key to input.
    switch (windows_key_code) {
      // ctrl-@ maps to \x00 (Null byte)
      case ui::VKEY_2:
        return 0;
      // ctrl-^ maps to \x1E (Record separator, Information separator two)
      case ui::VKEY_6:
        return 0x1E;
      // ctrl-_ maps to \x1F (Unit separator, Information separator one)
      case ui::VKEY_OEM_MINUS:
        return 0x1F;
      // Returns 0 for all other keys to avoid inputting unexpected chars.
      default:
        break;
    }
  } else {
    switch (windows_key_code) {
      // ctrl-[ maps to \x1B (Escape)
      case ui::VKEY_OEM_4:
        return 0x1B;
      // ctrl-\ maps to \x1C (File separator, Information separator four)
      case ui::VKEY_OEM_5:
        return 0x1C;
      // ctrl-] maps to \x1D (Group separator, Information separator three)
      case ui::VKEY_OEM_6:
        return 0x1D;
      // ctrl-Enter maps to \x0A (Line feed)
      case ui::VKEY_RETURN:
        return 0x0A;
      // Returns 0 for all other keys to avoid inputting unexpected chars.
      default:
        break;
    }
  }
  return 0;
}

enum {
  LeftButton = 1,
  MiddleButton = 2,
  RightButton = 3
};

static  WebMouseEvent::Button EvasToWebMouseButton(int button) {
  if (button == LeftButton)
    return WebMouseEvent::ButtonLeft;
  if (button == MiddleButton)
    return WebMouseEvent::ButtonMiddle;
  if (button == RightButton)
    return WebMouseEvent::ButtonRight;

  return WebMouseEvent::ButtonNone;
}

static WebInputEvent::Modifiers EvasToWebModifiers(const Evas_Modifier* modifiers) {
  unsigned result = 0;

  if (evas_key_modifier_is_set(modifiers, "Shift"))
    result |= WebInputEvent::ShiftKey;
  if (evas_key_modifier_is_set(modifiers, "Control"))
    result |= WebInputEvent::ControlKey;
  if (evas_key_modifier_is_set(modifiers, "Alt"))
    result |= WebInputEvent::AltKey;
  if (evas_key_modifier_is_set(modifiers, "Meta"))
    result |= WebInputEvent::MetaKey;

  return static_cast<WebInputEvent::Modifiers>(result);
}

blink::WebMouseEvent WebEventFactoryEfl::toWebMouseEvent(Evas* evas, Evas_Object* web_view, const Evas_Event_Mouse_Down* ev, float scale_factor) {
  WebMouseEvent webKitEvent;
  webKitEvent.timeStampSeconds = (double)ev->timestamp / 1000;
  webKitEvent.button = EvasToWebMouseButton(ev->button);
  webKitEvent.modifiers = EvasToWebModifiers(ev->modifiers);

  webKitEvent.windowX = (ev->canvas.x) / scale_factor;
  webKitEvent.windowY = (ev->canvas.y) / scale_factor;

  TranslateEvasCoordToWebKitCoord(web_view, webKitEvent.windowX, webKitEvent.windowY);

  webKitEvent.x = webKitEvent.windowX;
  webKitEvent.y = webKitEvent.windowY;

  webKitEvent.globalX = (evas_coord_world_x_to_screen(evas, ev->canvas.x)) / scale_factor;
  webKitEvent.globalY = (evas_coord_world_y_to_screen(evas, ev->canvas.y)) / scale_factor;
  webKitEvent.type = WebInputEvent::MouseDown;

  webKitEvent.clickCount = 1;

  return webKitEvent;
}

blink::WebMouseEvent WebEventFactoryEfl::toWebMouseEvent(Evas* evas, Evas_Object* web_view, const Evas_Event_Mouse_Up* ev, float scale_factor) {
  WebMouseEvent webKitEvent;
  webKitEvent.timeStampSeconds = (double)ev->timestamp / 1000;
  webKitEvent.button = EvasToWebMouseButton(ev->button);
  webKitEvent.modifiers = EvasToWebModifiers(ev->modifiers);

  webKitEvent.windowX = (ev->canvas.x) / scale_factor;
  webKitEvent.windowY = (ev->canvas.y) / scale_factor;

  TranslateEvasCoordToWebKitCoord(web_view, webKitEvent.windowX, webKitEvent.windowY);

  webKitEvent.x = webKitEvent.windowX;
  webKitEvent.y = webKitEvent.windowY;

  webKitEvent.globalX = (evas_coord_world_x_to_screen(evas, ev->canvas.x)) / scale_factor;
  webKitEvent.globalY = (evas_coord_world_y_to_screen(evas, ev->canvas.y)) / scale_factor;
  webKitEvent.type = WebInputEvent::MouseUp;

  webKitEvent.clickCount = 1;

  return webKitEvent;
}

blink::WebMouseEvent WebEventFactoryEfl::toWebMouseEvent(Evas* evas, Evas_Object* web_view, const Evas_Event_Mouse_Move* ev, float scale_factor) {
  WebMouseEvent webKitEvent;
  webKitEvent.timeStampSeconds = (double)ev->timestamp / 1000;
  webKitEvent.modifiers = EvasToWebModifiers(ev->modifiers);

  webKitEvent.windowX = (ev->cur.canvas.x) / scale_factor;
  webKitEvent.windowY = (ev->cur.canvas.y) / scale_factor;

  TranslateEvasCoordToWebKitCoord(web_view, webKitEvent.windowX, webKitEvent.windowY);

  webKitEvent.x = webKitEvent.windowX;
  webKitEvent.y = webKitEvent.windowY;

  webKitEvent.globalX = (evas_coord_world_x_to_screen(evas, ev->cur.canvas.x)) / scale_factor;
  webKitEvent.globalY = (evas_coord_world_y_to_screen(evas, ev->cur.canvas.y)) / scale_factor;
  webKitEvent.type = WebInputEvent::MouseMove;

  return webKitEvent;
}

blink::WebMouseWheelEvent WebEventFactoryEfl::toWebMouseEvent(Evas* evas, Evas_Object* web_view, const Evas_Event_Mouse_Wheel* ev, float scale_factor) {
  WebMouseWheelEvent webKitEvent;
  webKitEvent.timeStampSeconds = (double)ev->timestamp / 1000;
  webKitEvent.modifiers = EvasToWebModifiers(ev->modifiers);

  webKitEvent.windowX = (ev->canvas.x) / scale_factor;
  webKitEvent.windowY = (ev->canvas.y) / scale_factor;

  TranslateEvasCoordToWebKitCoord(web_view, webKitEvent.windowX, webKitEvent.windowY);

  webKitEvent.x = webKitEvent.windowX;
  webKitEvent.y = webKitEvent.windowY;

  webKitEvent.globalX = (evas_coord_world_x_to_screen(evas, ev->canvas.x)) / scale_factor;
  webKitEvent.globalY = (evas_coord_world_y_to_screen(evas, ev->canvas.y)) / scale_factor;
  webKitEvent.type = WebInputEvent::MouseWheel;

  if (ev->direction) {
    webKitEvent.wheelTicksX = ev->z;
    webKitEvent.deltaX = ev->z * cDefaultScrollStep;
  } else {
    webKitEvent.wheelTicksY = -(ev->z);
    webKitEvent.deltaY = -(ev->z * cDefaultScrollStep);
  }

  return webKitEvent;
}

static content::NativeWebKeyboardEvent WebKeyEvent(unsigned int timestamp, const Evas_Modifier* modifiers, const char* key,
                                                   const char* string, bool pressed) {
  content::NativeWebKeyboardEvent webKitEvent;
  webKitEvent.timeStampSeconds =  (double)timestamp / 1000;
  webKitEvent.modifiers = EvasToWebModifiers(modifiers);
  webKitEvent.type = pressed ? WebInputEvent::KeyDown : WebInputEvent::KeyUp;

  webKitEvent.nativeKeyCode = NativeKeyCodeFromEflKey(key);
  webKitEvent.windowsKeyCode = WindowsKeyCodeFromEflKey(key);

  if (webKitEvent.windowsKeyCode == ui::VKEY_RETURN)
      webKitEvent.unmodifiedText[0] = '\r';
  else
    webKitEvent.unmodifiedText[0] = CharacterFromEflString(string);

  if (webKitEvent.modifiers & blink::WebInputEvent::ControlKey) {
    webKitEvent.text[0] =
        GetControlCharacter(
            webKitEvent.windowsKeyCode,
            webKitEvent.modifiers & blink::WebInputEvent::ShiftKey);
  } else {
    webKitEvent.text[0] = webKitEvent.unmodifiedText[0];
  }

  webKitEvent.setKeyIdentifierFromWindowsKeyCode();

  return webKitEvent;
}

content::NativeWebKeyboardEvent WebEventFactoryEfl::toWebKeyboardEvent(Evas*, const Evas_Event_Key_Down* ev) {
  return WebKeyEvent(ev->timestamp, ev->modifiers, ev->key, ev->string, true);
}

content::NativeWebKeyboardEvent WebEventFactoryEfl::toWebKeyboardEvent(Evas*, const Evas_Event_Key_Up* ev) {
  return WebKeyEvent(ev->timestamp, ev->modifiers, ev->key, ev->string, false);
}

static ui::EventType EvasTouchEventTypeToUI(Evas_Touch_Point_State evas_touch) {
  switch(evas_touch) {
    case EVAS_TOUCH_POINT_DOWN:
      return ui::ET_TOUCH_PRESSED;
    case EVAS_TOUCH_POINT_MOVE:
      return ui::ET_TOUCH_MOVED;
    case EVAS_TOUCH_POINT_UP:
      return ui::ET_TOUCH_RELEASED;
    case EVAS_TOUCH_POINT_CANCEL:
      return ui::ET_TOUCH_CANCELLED;
    case EVAS_TOUCH_POINT_STILL:
      // Not handled by chromium, should not be passed here.
    default:
      NOTREACHED();
      return ui::ET_UNKNOWN;
  }
}

ui::TouchEvent WebEventFactoryEfl::toUITouchEvent(const tizen_webview::Touch_Point* p, Evas_Object* web_view, float scale_factor) {
  int x = p->x, y = p->y;

  TranslateEvasCoordToWebKitCoord(web_view, x, y);
  return ui::TouchEvent(EvasTouchEventTypeToUI(p->state),
                        gfx::Point(x / scale_factor, y / scale_factor),
                        p->id, base::TimeDelta::FromMilliseconds(ecore_time_get() * 1000));
}

}
