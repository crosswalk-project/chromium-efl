// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef webview_delegate_ewk_h
#define webview_delegate_ewk_h

#include <Evas.h>
#include <tizen_webview/public/tw_webview.h>
#include <tizen_webview/public/tw_webview_delegate.h>

using tizen_webview::WebView;
using tizen_webview::WebViewDelegate;

struct WebViewDelegateEwk : public WebViewDelegate {
 public:
  static const WebViewDelegateEwk& GetInstance();
 protected:
  WebView* GetWebViewFromEvasObject(Evas_Object* evas_object);
  bool IsWebViewEvasObject(Evas_Object* evas_object);
  Evas_Object* GetContentImageEvasObject(Evas_Object* evas_object);
  Evas_Object_Smart_Clipped_Data* GetSmartClippedData(Evas_Object* evas_object);
  Eina_Rectangle GetLastUsedViewPortArea(Evas_Object* evas_object);

  // Event handlers
  bool RequestHandleEvent_FocusIn(WebView* wv);
  bool RequestHandleEvent_FocusOut(WebView* wv);
  bool RequestHandleEvent_MouseUp(WebView* wv, Evas_Event_Mouse_Up* event_info);
  bool RequestHandleEvent_MouseDown(WebView* wv, Evas_Event_Mouse_Down* event_info);
  bool RequestHandleEvent_MouseMove(WebView* wv, Evas_Event_Mouse_Move* event_info);
  bool RequestHandleEvent_MouseWheel(WebView* wv, Evas_Event_Mouse_Wheel* event_info);
  bool RequestHandleEvent_KeyUp(WebView* wv, Evas_Event_Key_Up* event_info);
  bool RequestHandleEvent_KeyDown(WebView* wv, Evas_Event_Key_Down* event_info);
 private:
  WebViewDelegateEwk() {}
  DISALLOW_COPY_AND_ASSIGN(WebViewDelegateEwk);
};


#endif // webview_delegate_ewk_h
