// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TIZEN_WEBVIEW_PUBLIC_TW_WEBVIEW_DELEGATE_H_
#define TIZEN_WEBVIEW_PUBLIC_TW_WEBVIEW_DELEGATE_H_

#include <Evas.h>
#include <eina_rectangle.h>

namespace tizen_webview {

class WebView;

class WebViewDelegate {
 public:
  // Register the default delegate.
  // Client of WebView should register one before creating a WebView first time.
  // Client has the ownership for |wv_delegate| and should ensure its validity
  // while using WebView.
  // Currently, it is not allowed to re-register another delegate,
  // and if try to do it it is simply ignored.
  static void RegisterDelegate(WebViewDelegate* wv_delegate);

  // Check if a default delegate is registered or not
  static bool IsDelegateRegistered();

  // Get the default delegate registered in RegisterDefaultDelegate()
  static WebViewDelegate* GetInstance();

  // Retrieve WebView from |evas_object|.
  // If |evas_object| is not WebView, return NULL.
  virtual WebView* GetWebViewFromEvasObject(Evas_Object* evas_object) = 0;

  // Check if |evas_object| is WebView or not.
  virtual bool IsWebViewEvasObject(Evas_Object* evas_object) = 0;

  // Get the evas image object holded by WebView evas object.
  virtual Evas_Object* GetContentImageEvasObject(Evas_Object* evas_object) = 0;

  // Get clipped data from WebView evas object which is a clipped smart object.
  virtual Evas_Object_Smart_Clipped_Data* GetSmartClippedData(
      Evas_Object* evas_object) = 0;

  // Get lastly used view port area.
  // If |evas_object| is not a WebView, return Enia_Rectangle with width -1.
  virtual Eina_Rectangle GetLastUsedViewPortArea(Evas_Object* evas_object) = 0;

  // ---- Custom Event handlers
  // Give opportunity for client to process event.
  // [Note] return true if handled, false otherwise.
  virtual bool RequestHandleEvent_FocusIn(WebView* wv) { return false; }
  virtual bool RequestHandleEvent_FocusOut(WebView* wv) { return false; }
  virtual bool RequestHandleEvent_MouseUp(
      WebView* wv, Evas_Event_Mouse_Up* event_info) { return false; }
  virtual bool RequestHandleEvent_MouseDown(
      WebView* wv, Evas_Event_Mouse_Down* event_info) { return false; }
  virtual bool RequestHandleEvent_MouseMove(
      WebView* wv, Evas_Event_Mouse_Move* event_info) { return false; }
  virtual bool RequestHandleEvent_MouseWheel(
      WebView* wv, Evas_Event_Mouse_Wheel* event_info) { return false; }
  virtual bool RequestHandleEvent_KeyUp(
      WebView* wv, Evas_Event_Key_Up* event_info) { return false; }
  virtual bool RequestHandleEvent_KeyDown(
      WebView* wv, Evas_Event_Key_Down* event_info) { return false; }
};

} // namespace tizen_webview

#endif  // TIZEN_WEBVIEW_PUBLIC_TW_WEBVIEW_DELEGATE_H_
