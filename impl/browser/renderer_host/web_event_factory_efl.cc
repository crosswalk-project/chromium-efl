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

#include "browser/renderer_host/web_event_factory_efl.h"

#include "base/time/time.h"
#include "base/strings/utf_string_conversions.h"
#include "public/ewk_touch.h"
#include "third_party/WebKit/public/web/WebInputEvent.h"
#include "ui/events/keycodes/keyboard_codes.h"
#include "ui/events/keycodes/keyboard_code_conversion_x.h"

#include <Eina.h>
#include <Ecore.h>
#include <Ecore_Input.h>
#include <Ecore_X.h>
#include <Evas.h>

// TODO: Figure out how to avoid this includes.
#include <X11/Xutil.h>
#include <X11/keysym.h>

using namespace blink;

namespace {

inline void TranslateEvasCoordToWebKitCoord(Evas_Object *web_view, int& x, int& y) {
  Evas_Coord tmpX, tmpY;
  evas_object_geometry_get(web_view, &tmpX, &tmpY, 0, 0);
  x -= tmpX;
  y -= tmpY;
}

}
namespace content {

static WebMouseEvent::Button EflToWebMouseButton(unsigned int eflButton) {
  switch(eflButton) {
    case 1:
      return WebMouseEvent::ButtonLeft;
    case 2:
      return WebMouseEvent::ButtonMiddle;
    case 3:
      return WebMouseEvent::ButtonRight;
  }

  return WebMouseEvent::ButtonNone;
}

static int EflToWebModifiers(unsigned int eflmodifiers, unsigned int eflbuttons) {
  int flags = 0;
  if (eflmodifiers & ECORE_EVENT_MODIFIER_CTRL)
    flags |= WebInputEvent::ControlKey;

  if (eflmodifiers & ECORE_EVENT_MODIFIER_SHIFT)
    flags |= WebInputEvent::ShiftKey;

  if (eflmodifiers & ECORE_EVENT_MODIFIER_ALT)
    flags |= WebInputEvent::AltKey;

  if (eflbuttons == 1)
    flags |= WebInputEvent::LeftButtonDown;

  if (eflbuttons == 2)
    flags |= WebInputEvent::MiddleButtonDown;

  if (eflbuttons == 3)
    flags |= WebInputEvent::RightButtonDown;

  return flags;
}

WebMouseEvent WebEventFactoryEfl::toWebMouseEvent(const Ecore_Event_Mouse_Button* ev, bool pressed) {
  WebMouseEvent webKitEvent;
  webKitEvent.timeStampSeconds = (double)ev->timestamp / 1000;
  webKitEvent.button = EflToWebMouseButton(ev->buttons);
  webKitEvent.modifiers = EflToWebModifiers(ev->modifiers, pressed ? ev->buttons : 0);

  webKitEvent.x = webKitEvent.windowX = ev->x;
  webKitEvent.y = webKitEvent.windowY = ev->y;
  webKitEvent.globalX = ev->root.x;
  webKitEvent.globalY = ev->root.y;
  webKitEvent.type = pressed ? WebInputEvent::MouseDown : WebInputEvent::MouseUp;

  webKitEvent.clickCount = 1;

  return webKitEvent;
}

static const float cDefaultScrollStep = 20;

WebMouseWheelEvent WebEventFactoryEfl::toWebMouseEvent(const Ecore_Event_Mouse_Wheel* ev) {
  WebMouseWheelEvent webKitEvent;
  webKitEvent.timeStampSeconds = (double)ev->timestamp / 1000;
  webKitEvent.modifiers = EflToWebModifiers(ev->modifiers, 0);

  webKitEvent.x = webKitEvent.windowX = ev->x;
  webKitEvent.y = webKitEvent.windowY = ev->y;
  webKitEvent.globalX = ev->root.x;
  webKitEvent.globalY = ev->root.y;
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

WebMouseEvent WebEventFactoryEfl::toWebMouseEvent(const Ecore_Event_Mouse_Move* ev) {
  WebMouseEvent webKitEvent;
  webKitEvent.timeStampSeconds = (double)ev->timestamp / 1000;
  webKitEvent.modifiers = EflToWebModifiers(ev->modifiers, 0);

  webKitEvent.x = webKitEvent.windowX = ev->x;
  webKitEvent.y = webKitEvent.windowY = ev->y;
  webKitEvent.globalX = ev->root.x;
  webKitEvent.globalY = ev->root.y;
  webKitEvent.type = WebInputEvent::MouseMove;

  return webKitEvent;
}

ui::KeyboardCode KeyboardCodeFromEflKey(const char* key) {
  return ui::KeyboardCodeFromXKeysym(XStringToKeysym(key));
}

int WindowsKeyCodeFromEflKey(const char* key) {
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

int NativeKeyCodeFromEflKey(const char* key) {
  //return ecore_x_keysym_keycode_get(ev->key);
  return XKeysymToKeycode((Display*)ecore_x_display_get(), XStringToKeysym(key));
}

int CharacterFromEflString(const char* string) {
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

content::NativeWebKeyboardEvent WebEventFactoryEfl::toWebKeyboardEvent(const Ecore_Event_Key* ev, bool pressed) {
  content::NativeWebKeyboardEvent webKitEvent;
  webKitEvent.timeStampSeconds =  (double)ev->timestamp / 1000;
  webKitEvent.modifiers = EflToWebModifiers(ev->modifiers, 0);
  webKitEvent.type = pressed ? WebInputEvent::KeyDown : WebInputEvent::KeyUp;

  webKitEvent.nativeKeyCode = NativeKeyCodeFromEflKey(ev->key);
  webKitEvent.windowsKeyCode = WindowsKeyCodeFromEflKey(ev->key);

  if (webKitEvent.windowsKeyCode == ui::VKEY_RETURN)
      webKitEvent.unmodifiedText[0] = '\r';
  else
    webKitEvent.unmodifiedText[0] = CharacterFromEflString(ev->string);

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

static inline WebInputEvent::Modifiers EvasToWebModifiers(const Evas_Modifier* modifiers) {
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
    case EVAS_TOUCH_POINT_STILL: // No equivalent in chromium M35+
    default:
      return ui::ET_UNKNOWN;
  }

}

ui::TouchEvent WebEventFactoryEfl::toUITouchEvent(Ewk_Touch_Point* p, Evas_Object* web_view, float scale_factor) {
  int x = p->x, y = p->y;

  TranslateEvasCoordToWebKitCoord(web_view, x, y);
  return ui::TouchEvent(EvasTouchEventTypeToUI(p->state),
                        gfx::Point(x / scale_factor, y / scale_factor),
                        p->id, base::TimeDelta::FromMilliseconds(ecore_time_get() * 1000));
}

}
