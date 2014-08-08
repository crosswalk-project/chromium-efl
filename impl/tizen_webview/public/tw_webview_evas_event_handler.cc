// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "tw_webview_evas_event_handler.h"

#include "tizen_webview/public/tw_webview_delegate.h"
#include "tizen_webview/public/tw_touch_point.h"

#include "eweb_view.h"
#include "browser/motion/wkext_motion.h"
#include "browser/renderer_host/web_event_factory_efl.h"
#include "browser/renderer_host/render_widget_host_view_efl.h"

#define RETURN_IF_TRUE(val) if((val)) return

#define RETURN_IF_FALSE(val) if(!(val)) return

#define EVAS_EVENT_CALLBACK_ADD(event_type, callback)        \
     evas_object_event_callback_add(evas_object_, event_type,\
        reinterpret_cast<Evas_Object_Event_Cb>(callback),    \
        reinterpret_cast<void*>(webview_))

#define EVAS_EVENT_CALLBACK_DEL(event_type, callback)        \
     evas_object_event_callback_del(evas_object_, event_type,\
        reinterpret_cast<Evas_Object_Event_Cb>(callback))

#define EVAS_SMART_CALLBACK_ADD(event_type, callback)        \
     evas_object_smart_callback_add(evas_object_, event_type,\
        reinterpret_cast<Evas_Smart_Cb>(callback),           \
        reinterpret_cast<void*>(webview_))

#define EVAS_SMART_CALLBACK_DEL(event_type, callback)        \
     evas_object_smart_callback_del(evas_object_, event_type,\
        reinterpret_cast<Evas_Smart_Cb>(callback))

#define EVAS_EVENT_DISPATCH_STANDARD_FLOW_1(our_name)                   \
    WebView* wv = data;                                                 \
    WebViewDelegate* wvd = WebViewDelegate::GetInstance();              \
    if (!wvd) return;                                                   \
    bool handled = wvd->RequestHandleEvent_ ## our_name (wv);           \
    if (!handled && wv->GetEvasEventHandler()) {                        \
      wv->GetEvasEventHandler()->HandleEventDefault_ ## our_name ();    \
    }                                                                   \

#define EVAS_EVENT_DISPATCH_STANDARD_FLOW_2(our_name, evas_event_name)  \
    WebView* wv = data;                                                 \
    WebViewDelegate* wvd = WebViewDelegate::GetInstance();              \
    if (!wvd) return;                                                   \
    evas_event_name * ei =                                              \
        static_cast< evas_event_name *>(event_info);                    \
    bool handled = wvd->RequestHandleEvent_ ## our_name(wv, ei);        \
    if (!handled && wv->GetEvasEventHandler()) {                        \
      wv->GetEvasEventHandler()->HandleEventDefault_ ## our_name (ei);  \
    }                                                                   \


namespace {
const char kSmartCallbackName_MotionEnable[] = "motion,enable";
const char kSmartCallbackName_MotionMove[] = "motion,move";
const char kSmartCallbackName_MotionZoom[] = "motion,zoom";

inline bool isHardwareBackKey(const Evas_Event_Key_Down *event) {
#ifdef OS_TIZEN
  return (strcmp(event->key, "XF86Stop") == 0);
#else
  return (strcmp(event->key, "Escape") == 0);
#endif
}

}
namespace tizen_webview {

WebViewEvasEventHandler::WebViewEvasEventHandler(WebView* wv)
    : webview_ (wv) {
  evas_object_ = webview_->AsEvasObject();
  is_focus_handler_bound_ = false;
  is_key_handler_bound_ = false;
  is_mouse_handler_bound_ = false;
  is_motion_handler_bound_ = false;
  is_touch_handler_bound_ = false;
}

void WebViewEvasEventHandler::BindFocusEventHandlers() {
  RETURN_IF_TRUE(is_focus_handler_bound_);
  EVAS_EVENT_CALLBACK_ADD(EVAS_CALLBACK_FOCUS_IN,  __OnFocusIn);
  EVAS_EVENT_CALLBACK_ADD(EVAS_CALLBACK_FOCUS_OUT, __OnFocusOut);
  is_focus_handler_bound_ = true;
}

void WebViewEvasEventHandler::UnbindFocusEventHandlers() {
  RETURN_IF_FALSE(is_focus_handler_bound_);
  EVAS_EVENT_CALLBACK_DEL(EVAS_CALLBACK_FOCUS_IN,  __OnFocusIn);
  EVAS_EVENT_CALLBACK_DEL(EVAS_CALLBACK_FOCUS_OUT, __OnFocusOut);
  is_focus_handler_bound_ = false;
}

void WebViewEvasEventHandler::BindKeyEventHandlers() {
  RETURN_IF_TRUE(is_key_handler_bound_);
  EVAS_EVENT_CALLBACK_ADD(EVAS_CALLBACK_KEY_DOWN, __OnKeyDown);
  EVAS_EVENT_CALLBACK_ADD(EVAS_CALLBACK_KEY_UP,   __OnKeyUp);
  is_key_handler_bound_ = true;
}

void WebViewEvasEventHandler::UnbindKeyEventHandlers() {
  RETURN_IF_FALSE(is_key_handler_bound_);
  EVAS_EVENT_CALLBACK_DEL(EVAS_CALLBACK_KEY_DOWN, __OnKeyDown);
  EVAS_EVENT_CALLBACK_DEL(EVAS_CALLBACK_KEY_UP,   __OnKeyUp);
  is_key_handler_bound_ = false;
}

void WebViewEvasEventHandler::BindMouseEventHandlers() {
  RETURN_IF_TRUE(is_mouse_handler_bound_);
  EVAS_EVENT_CALLBACK_ADD(EVAS_CALLBACK_MOUSE_DOWN, __OnMouseDown);
  EVAS_EVENT_CALLBACK_ADD(EVAS_CALLBACK_MOUSE_UP, __OnMouseUp);
  EVAS_EVENT_CALLBACK_ADD(EVAS_CALLBACK_MOUSE_MOVE, __OnMouseMove);
  EVAS_EVENT_CALLBACK_ADD(EVAS_CALLBACK_MOUSE_WHEEL, __OnMouseWheel);
  is_mouse_handler_bound_ = true;
}

void WebViewEvasEventHandler::UnbindMouseEventHandlers() {
  RETURN_IF_FALSE(is_mouse_handler_bound_);
  EVAS_EVENT_CALLBACK_DEL(EVAS_CALLBACK_MOUSE_DOWN, __OnMouseDown);
  EVAS_EVENT_CALLBACK_DEL(EVAS_CALLBACK_MOUSE_UP, __OnMouseUp);
  EVAS_EVENT_CALLBACK_DEL(EVAS_CALLBACK_MOUSE_MOVE, __OnMouseMove);
  EVAS_EVENT_CALLBACK_DEL(EVAS_CALLBACK_MOUSE_WHEEL, __OnMouseWheel);
  is_mouse_handler_bound_ = false;
}

void WebViewEvasEventHandler::BindMotionEventHandlers() {
  RETURN_IF_TRUE(is_motion_handler_bound_);
  EVAS_SMART_CALLBACK_ADD(kSmartCallbackName_MotionEnable, __OnMotionEnable);
  EVAS_SMART_CALLBACK_ADD(kSmartCallbackName_MotionMove, __OnMotionMove);
  EVAS_SMART_CALLBACK_ADD(kSmartCallbackName_MotionZoom, __OnMotionZoom);
  is_motion_handler_bound_ = true;
}


void WebViewEvasEventHandler::UnbindMotionEventHandlers() {
  RETURN_IF_FALSE(is_motion_handler_bound_);
  EVAS_SMART_CALLBACK_DEL(kSmartCallbackName_MotionEnable, __OnMotionEnable);
  EVAS_SMART_CALLBACK_DEL(kSmartCallbackName_MotionMove, __OnMotionMove);
  EVAS_SMART_CALLBACK_DEL(kSmartCallbackName_MotionZoom, __OnMotionZoom);
  is_motion_handler_bound_ = false;
}

void WebViewEvasEventHandler::BindTouchEventHandlers() {
  RETURN_IF_TRUE(is_touch_handler_bound_);
  EVAS_EVENT_CALLBACK_ADD(EVAS_CALLBACK_MOUSE_DOWN, __OnTouchDown);
  EVAS_EVENT_CALLBACK_ADD(EVAS_CALLBACK_MOUSE_UP, __OnTouchUp);
  EVAS_EVENT_CALLBACK_ADD(EVAS_CALLBACK_MOUSE_MOVE, __OnTouchMove);
  EVAS_EVENT_CALLBACK_ADD(EVAS_CALLBACK_MULTI_DOWN, __OnTouchDown);
  EVAS_EVENT_CALLBACK_ADD(EVAS_CALLBACK_MULTI_UP, __OnTouchUp);
  EVAS_EVENT_CALLBACK_ADD(EVAS_CALLBACK_MULTI_MOVE, __OnTouchMove);
  is_touch_handler_bound_ = true;
}

void WebViewEvasEventHandler::UnbindTouchEventHandlers() {
  RETURN_IF_FALSE(is_touch_handler_bound_);
  EVAS_EVENT_CALLBACK_DEL(EVAS_CALLBACK_MOUSE_DOWN, __OnTouchDown);
  EVAS_EVENT_CALLBACK_DEL(EVAS_CALLBACK_MOUSE_UP, __OnTouchUp);
  EVAS_EVENT_CALLBACK_DEL(EVAS_CALLBACK_MOUSE_MOVE, __OnTouchMove);
  EVAS_EVENT_CALLBACK_DEL(EVAS_CALLBACK_MULTI_DOWN, __OnTouchDown);
  EVAS_EVENT_CALLBACK_DEL(EVAS_CALLBACK_MULTI_UP, __OnTouchUp);
  EVAS_EVENT_CALLBACK_DEL(EVAS_CALLBACK_MULTI_MOVE, __OnTouchMove);
  is_touch_handler_bound_ = false;
}

// ----- Evas Event Callbacks -------------------------------------------------

void WebViewEvasEventHandler::__OnFocusIn(
    WebView* data, Evas *e, Evas_Object *obj, void *event_info) {
  EVAS_EVENT_DISPATCH_STANDARD_FLOW_1(FocusIn);
}

void WebViewEvasEventHandler::__OnFocusOut(WebView* data, Evas *e, Evas_Object *obj, void *event_info) {
  EVAS_EVENT_DISPATCH_STANDARD_FLOW_1(FocusOut);
}


void WebViewEvasEventHandler::__OnKeyDown(WebView* data, Evas *e, Evas_Object *obj, void *event_info) {
  EVAS_EVENT_DISPATCH_STANDARD_FLOW_2(KeyDown, Evas_Event_Key_Down);
}

void WebViewEvasEventHandler::__OnKeyUp(WebView* data, Evas *e, Evas_Object *obj, void *event_info) {
  EVAS_EVENT_DISPATCH_STANDARD_FLOW_2(KeyUp, Evas_Event_Key_Up);
}

void WebViewEvasEventHandler::__OnMouseDown(WebView* data, Evas *e, Evas_Object *obj, void *event_info) {
  EVAS_EVENT_DISPATCH_STANDARD_FLOW_2(MouseDown, Evas_Event_Mouse_Down);
}

void WebViewEvasEventHandler::__OnMouseUp(WebView* data, Evas *e, Evas_Object *obj, void *event_info) {
  EVAS_EVENT_DISPATCH_STANDARD_FLOW_2(MouseUp, Evas_Event_Mouse_Up);
}

void WebViewEvasEventHandler::__OnMouseMove(WebView* data, Evas *e, Evas_Object *obj, void *event_info) {
  EVAS_EVENT_DISPATCH_STANDARD_FLOW_2(MouseMove, Evas_Event_Mouse_Move);
}

void WebViewEvasEventHandler::__OnMouseWheel(WebView* data, Evas *e, Evas_Object *obj, void *event_info) {
  EVAS_EVENT_DISPATCH_STANDARD_FLOW_2(MouseWheel, Evas_Event_Mouse_Wheel);
}

void WebViewEvasEventHandler::__OnMotionEnable(WebView* data, Evas_Object *eo, void *event_info) {
#if defined(OS_TIZEN)
  bool* enable = static_cast<bool*>(event_info);
  wkext_motion_tilt_enable_set(data->AsEvasObject(), *enable, g_default_tilt_motion_sensitivity);
#else
  DLOG(WARNING) << "Not Supported Event called in " << __PRETTY_FUNCTION__ ;
#endif
}

void WebViewEvasEventHandler::__OnMotionMove(WebView* data, Evas_Object *eo, void *event_info) {
  // Not implemented yet.
}

void WebViewEvasEventHandler::__OnMotionZoom(WebView* data, Evas_Object *eo, void *event_info) {
#if defined(OS_TIZEN)
  data->GetImpl()->rwhv()->makePinchZoom(event_info);
#else
  DLOG(WARNING) << "Not Supported Event called in " << __PRETTY_FUNCTION__ ;
#endif
}

void WebViewEvasEventHandler::__OnTouchDown(WebView* data, Evas *e, Evas_Object *obj, void *event_info) {
  data->GetEvasEventHandler()->HandleEventDefault_TouchEvents(TW_TOUCH_START);
}

void WebViewEvasEventHandler::__OnTouchUp(WebView* data, Evas *e, Evas_Object *obj, void *event_info) {
  data->GetEvasEventHandler()->HandleEventDefault_TouchEvents(TW_TOUCH_END);
}

void WebViewEvasEventHandler::__OnTouchMove(WebView* data, Evas *e, Evas_Object *obj, void *event_info) {
  data->GetEvasEventHandler()->HandleEventDefault_TouchEvents(TW_TOUCH_MOVE);
}


// ----- Default Event Handlers -----------------------------------------------
bool WebViewEvasEventHandler::HandleEventDefault_FocusIn() {
  return webview_->HandleFocusIn();
}

bool WebViewEvasEventHandler::HandleEventDefault_FocusOut() {
  return webview_->HandleFocusOut();
}


bool WebViewEvasEventHandler::HandleEventDefault_KeyDown(const Evas_Event_Key_Down* event_info) {
  bool handled = false;
  if (isHardwareBackKey(event_info)) {
    if (webview_->GetContextMenuController()) {
      DVLOG(1) << "Hiding context menu due to back key press";
      webview_->ResetContextMenuController();
      handled = true;
    }
    if (webview_->GetSelectionController()->IsAnyHandleVisible()) {
      DVLOG(1) << "Clearing text selection due to back key press";
      webview_->ClearSelection();
      handled = true;
    }
#ifndef OS_TIZEN_TV
    if(!handled)
      webview_->GoBack();
#endif
    return handled;
  }
  return webview_->HandleEvasEvent(event_info);
}

bool WebViewEvasEventHandler::HandleEventDefault_KeyUp(const Evas_Event_Key_Up* event_info) {
  return webview_->HandleEvasEvent(event_info);
}

bool WebViewEvasEventHandler::HandleEventDefault_MouseDown(const Evas_Event_Mouse_Down* event_info) {
  webview_->SetFocus(true);
  return webview_->HandleEvasEvent(event_info);
}

bool WebViewEvasEventHandler::HandleEventDefault_MouseUp(const Evas_Event_Mouse_Up* event_info) {
  return webview_->HandleEvasEvent(event_info);
}

bool WebViewEvasEventHandler::HandleEventDefault_MouseMove(const Evas_Event_Mouse_Move* event_info) {
  return webview_->HandleEvasEvent(event_info);
}

bool WebViewEvasEventHandler::HandleEventDefault_MouseWheel(const Evas_Event_Mouse_Wheel* event_info) {
  return webview_->HandleEvasEvent(event_info);
}

bool WebViewEvasEventHandler::HandleEventDefault_TextSelectionDown(int x, int y) {
  if (!webview_->GetSelectionController()) {
    return false;
  }
  return webview_->GetSelectionController()->TextSelectionDown(x, y);
}

bool WebViewEvasEventHandler::HandleEventDefault_TextSelectionUp(int x, int y) {
  if (!webview_->GetSelectionController()) {
    return false;
  }
  return webview_->GetSelectionController()->TextSelectionUp(x, y);
}

bool WebViewEvasEventHandler::HandleEventDefault_TouchEvents(Touch_Event_Type type) {
  // These constants are used to map multi touch's touch id(s).
  // The poorly-written Tizen API document says:
  //  "0 for Mouse Event and device id for Multi Event."
  //  "The point which comes from Mouse Event has id 0 and"
  //  "The point which comes from Multi Event has id that is same as Multi Event's device id."
  static const int kMultiTouchIDMapPart0SingleIndex = 0; // This constant is to map touch id 0 to 0, or [0] -> [0]
  static const int kMultiTouchIDMapPart1StartIndex = 13; // This constant is to map [13, 23] -> [1, 11]
  static const int kMultiTouchIDMapPart1EndIndex = 23; // This constant is to map [13, 23] -> [1, 11]
  static const int kMultiTouchIDMapPart1DiffValue = 12; // 13 - 1 = 12, 23 - 11 = 12

  Evas_Object_Smart_Clipped_Data* clip_data =
    WebViewDelegate::GetInstance()->GetSmartClippedData(evas_object_);
  DCHECK(clip_data);

  unsigned count = evas_touch_point_list_count(clip_data->evas);
  if (!count) {
    return false;
  }

  Evas_Coord_Point pt;
  Eina_List* points = 0;
  for (unsigned i = 0; i < count; ++i) {
    tizen_webview::Touch_Point* point = new tizen_webview::Touch_Point;
    // evas_touch_point_list_nth_id_get returns [0] or [13, )
    // Multi touch's touch id [[0], [13, 23]] should be mapped to [[0], [1, 11]]
    // Internet Blame URL:
    //   https://groups.google.com/d/msg/mailing-enlightenment-devel/-R-ezCzpkTk/HJ0KBCdz6CgJ
    point->id = evas_touch_point_list_nth_id_get(clip_data->evas, i);
    DCHECK(point->id == kMultiTouchIDMapPart0SingleIndex || point->id >= kMultiTouchIDMapPart1StartIndex);
    if (point->id >= kMultiTouchIDMapPart1StartIndex && point->id <= kMultiTouchIDMapPart1EndIndex) {
      point->id -= kMultiTouchIDMapPart1DiffValue;
    } else if (point->id > kMultiTouchIDMapPart1EndIndex) {
      LOG(ERROR) << "evas_touch_point_list_nth_id_get() returned a value greater than ("
                 << kMultiTouchIDMapPart1EndIndex << "). It is ignored.";
    }
    evas_touch_point_list_nth_xy_get(clip_data->evas, i, &point->x, &point->y);
    pt.x = point->x;
    pt.y = point->y;
    point->state = evas_touch_point_list_nth_state_get(clip_data->evas, i);
    if (type == TW_TOUCH_CANCEL)
      point->state = EVAS_TOUCH_POINT_CANCEL;
    points = eina_list_append(points, point);
  }

  if (type == TW_TOUCH_START)
    webview_->SetFocus(EINA_TRUE);

  webview_->HandleTouchEvents(type, points,
      evas_key_modifier_get(clip_data->evas));

#ifdef OS_TIZEN
  if (count >=2) {
    if(type == TW_TOUCH_START) {
      LOG(ERROR) << " wkext_motion_tilt_start";
      wkext_motion_tilt_start(evas_object_, &pt);
    } else if(type == TW_TOUCH_END) {
      wkext_motion_tilt_stop();
    }
  }
#else
  // Silence unused variable warning
  (void)(pt);
#endif

  void* data;
  EINA_LIST_FREE(points, data)
  delete static_cast<Touch_Point*>(data);
  return true;
}

} // namespace tizen_webview
