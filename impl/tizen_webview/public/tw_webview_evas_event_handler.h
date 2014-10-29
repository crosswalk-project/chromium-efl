// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TIZEN_WEBVIEW_EVENT_TW_WEBVIEW_EVAS_EVENT_HANDLER_H_
#define TIZEN_WEBVIEW_EVENT_TW_WEBVIEW_EVAS_EVENT_HANDLER_H_

#include <Evas.h>
#include "tizen_webview/public/tw_touch_event.h"

namespace tizen_webview {

class WebView;
class WebViewDelegate;
class WebViewEvasEventHandler {
 public:
  explicit WebViewEvasEventHandler(WebView*);

  // Default event handlers of WebView.
  void BindKeyEventHandlers();
  void UnbindKeyEventHandlers();
  void BindMouseEventHandlers();
  void UnbindMouseEventHandlers();
  void BindFocusEventHandlers();
  void UnbindFocusEventHandlers();
  void BindMotionEventHandlers();
  void UnbindMotionEventHandlers();
  void BindTouchEventHandlers();
  void UnbindTouchEventHandlers();

  // ---- Evas Event callbacks
  // Each callback will be registered to Evas Object to handle a specified
  // evas event. It should be EFL's Evas_Event_Cb type because it will be
  // passed in as an arguement of evas_event_callback_add().
  // Evas will trigger corresponding callback given evas object and event.
  // Each callback basically does followings:
  //  - firstly, ask WebViewDelegate to handle the event
  //  - If WebViewDelegate return true, the event is not processed any more
  //  - Otherwise, forward the event to corresponding default event handler
  typedef void (*Evas_Event_Callback)(WebView* data, Evas *e,
                                      Evas_Object* obj, void* event_info);
  static void __OnFocusIn   (WebView*, Evas*, Evas_Object*, void*);
  static void __OnFocusOut  (WebView*, Evas*, Evas_Object*, void*);
  static void __OnKeyDown   (WebView*, Evas*, Evas_Object*, void*);
  static void __OnKeyUp     (WebView*, Evas*, Evas_Object*, void*);
  static void __OnMouseDown (WebView*, Evas*, Evas_Object*, void*);
  static void __OnMouseUp   (WebView*, Evas*, Evas_Object*, void*);
  static void __OnMouseMove (WebView*, Evas*, Evas_Object*, void*);
  static void __OnMouseWheel(WebView*, Evas*, Evas_Object*, void*);
  static void __OnTouchDown (WebView*, Evas*, Evas_Object*, void*);
  static void __OnTouchUp   (WebView*, Evas*, Evas_Object*, void*);
  static void __OnTouchMove (WebView*, Evas*, Evas_Object*, void*);

  // --- Evas smart object event callbacks
  // Each callback will be registered to Evas Smart Object to handle a evas
  // smart event. It should be EFL's Evas_Smart_Cb type because it will be
  // passed in as an arguement of evas_event_callback_add().
  typedef void (*Evas_Smart_Callback)(WebView* data, Evas_Object* obj,
                                      void* event_info);
  static void __OnMotionEnable(WebView*, Evas_Object*, void*);
  static void __OnMotionMove  (WebView*, Evas_Object*, void*);
  static void __OnMotionZoom  (WebView*, Evas_Object*, void*);

  // ---- Default event handlers
  // It is usually (but not always) invoked when the given event is not handled
  // by WebViewDelegate.
  // It's made public to allow a WebView Client to invoke it directly if
  // necessary (e.g, to implement Ewk webview's smart api).
  bool HandleEventDefault_FocusIn();
  bool HandleEventDefault_FocusOut();
  bool HandleEventDefault_KeyDown(const Evas_Event_Key_Down*);
  bool HandleEventDefault_KeyUp  (const Evas_Event_Key_Up*);
  bool HandleEventDefault_MouseDown (const Evas_Event_Mouse_Down*);
  bool HandleEventDefault_MouseUp   (const Evas_Event_Mouse_Up*);
  bool HandleEventDefault_MouseMove (const Evas_Event_Mouse_Move*);
  bool HandleEventDefault_MouseWheel(const Evas_Event_Mouse_Wheel*);
  bool HandleEventDefault_TextSelectionDown(int x, int y);
  bool HandleEventDefault_TextSelectionUp  (int x, int y);
  bool HandleEventDefault_TouchEvents(tizen_webview::Touch_Event_Type type);

 private:
  WebView* webview_;
  Evas_Object* evas_object_;
  bool is_focus_handler_bound_;
  bool is_key_handler_bound_;
  bool is_mouse_handler_bound_;
  bool is_motion_handler_bound_;
  bool is_touch_handler_bound_;
};

} // namespace tizen_webview

#endif  // TIZEN_WEBVIEW_PUBLIC_TW_WEBVIEW_EVAS_EVENT_HANDLER_H_
