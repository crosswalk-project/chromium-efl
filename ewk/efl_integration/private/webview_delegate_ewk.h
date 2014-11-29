/*
 * Copyright (C) 2014 Samsung Electronics
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

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
