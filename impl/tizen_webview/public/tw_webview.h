// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TIZEN_WEBVIEW_PUBLIC_TW_WEBVIEW_H_
#define TIZEN_WEBVIEW_PUBLIC_TW_WEBVIEW_H_

#include <Evas.h>
#include <string>
#include <map>
#include "content/public/browser/navigation_controller.h"
#include "tizen_webview/public/tw_back_forward_history.h"
#include "tizen_webview/public/tw_back_forward_list_impl.h"
#include "tizen_webview/public/tw_callbacks.h"
#include "tizen_webview/public/tw_content_security_policy.h"
#include "tizen_webview/public/tw_hit_test.h"
#include "tizen_webview/public/tw_find_options.h"
#include "tizen_webview/public/tw_macro.h"
#include "tizen_webview/public/tw_touch_event.h"
#include "tizen_webview/public/tw_view_mode.h"

// TODO: Remove this dependency
#include "EdgeEffect.h"
class Ewk_Settings;
class WebContentDelegateEfl;
struct SelectionStylePrams;
// TODO END

class EWebView;

namespace gfx {
class Point;
}

// TODO: remove ui event
namespace ui {
class GestureEvent;
class TouchEvent;
}

namespace tizen_webview {

class Frame;
class ContextMenuController;
class SelectionController;
class WebContext;
class WebViewDelegate;
class WebViewEvasEventHandler;

class WebView {
 public:
  // ---- create/delete
  static WebView* Create(WebContext* wc, Evas_Object* eo) WARN_UNUSED_RESULT;
  static void Delete(WebView* wv);

  // initialize data members and activate event handlers.
  // call this once after Create() and before using EWebView
  void Initialize();

  // ---- Get various objects related with this webview
  Evas_Object* AsEvasObject();
  static WebView* FromEvasObject(Evas_Object* eo);
  WebContext* GetWebContext();
  Ewk_Settings* GetSettings();
  SelectionController* GetSelectionController();
  WebViewEvasEventHandler* GetEvasEventHandler();
  ContextMenuController* GetContextMenuController();
  void ResetContextMenuController();

  Frame* GetMainFrame();

  // ---- User Agent
  bool SetUserAgent(const char* userAgent);
  const char* GetUserAgent() const;
  bool SetUserAgentAppName(const char* application_name);
  const char* GetUserAgentAppName() const;

  // ---- Load
  void SetURL(const char* url_string);
  const char* GetURL() const;
  void Reload();
  void ReloadIgnoringCache();
  double GetProgressValue(); // GetLoadProgressPercentage
  void LoadHTMLString(const char* html, const char* base_uri, const char* unreachable_uri);
  void Suspend();
  void Resume();
  void Stop();
  void LoadPlainTextString(const char* plain_text);
  void LoadData(const char* data, size_t size, const char* mime_type, const char* encoding, const char* base_uri, const char* unreachable_uri = NULL); // SetContent
  void UrlRequestSet(const char* url,
      content::NavigationController::LoadURLType loadtype,
      Eina_Hash* headers,
      const char* body);
  void LoadNotFoundErrorPage(const std::string& invalidUrl);

  // ---- Navigation
  bool GoBack();
  bool GoForward();
  bool CanGoBack();
  bool CanGoForward();
  void BackForwardListClear();
  BackForwardList* GetBackForwardList() const;
  BackForwardHistory* GetBackForwardHistory() const;

  // ---- Contents
  const char* GetTitle();
  const Eina_Rectangle GetContentsSize() const;
  bool PlainTextGet(View_Plain_Text_Get_Callback callback, void* user_data);
  void InvokePlainTextGetCallback(const std::string& content_text, int plain_text_get_callback_id);
  int SetEwkViewPlainTextGetCallback(View_Plain_Text_Get_Callback callback, void* user_data);
  bool GetMHTMLData(View_MHTML_Data_Get_Callback callback, void* user_data);
  void OnMHTMLContentGet(const std::string& mhtml_content, int callback_id);
  void SetOverrideEncoding(const std::string& encoding);
  void ExecuteEditCommand(const char* command, const char* value);
  void Find(const char* text, Find_Options options);
  void StopFinding();
  void SetContentSecurityPolicy(const char* policy, ContentSecurityPolicyType type);

  void SetViewGeolocationPermissionCallback(View_Geolocation_Permission_Callback callback, void* user_data);

  // ---- View
  // > View Visibility
  void Show();
  void Hide();
  // > View Focus
  void SetFocus(Eina_Bool focus);
  bool HasFocus() const;
  // > View Scale
  void SetScale(double scale_factor, int x, int y);
  double GetScale();
  void DidChangePageScaleFactor(double scale_factor);
  void GetPageScaleRange(double *min_scale, double *max_scale);
  void DidChangePageScaleRange(double min_scale, double max_scale);
  // > View Scroll
  bool GetScrollPosition(int* x, int* y) const;
  void SetScroll(int x, int y);       // TODO: rename to ScrollTo (x,y) x,y is left top (see GetScrollSize)
  void GetScrollSize(int* w, int* h); // TODO: rename to GetPossibleScrollSize
  // > View Orientation
  void SendOrientationChangeEventIfNeeded(int orientation);
  void SetOrientation(int orientation);
  void SetOrientationLockCallback(Orientation_Lock_Cb func, void* data);
  // > View mode
  void SetViewMode(tizen_webview::View_Mode view_mode);
  bool IsFullscreen();
  void ExitFullscreen();
  // > View Snapshot
  // Get snapshot for given |rect|. Return a new created Evas_Object.
  Evas_Object* GetSnapshot(Eina_Rectangle rect);
  bool GetSnapshotAsync(Eina_Rectangle rect, Evas* canvas, Web_App_Screenshot_Captured_Callback callback, void* user_data);
  // > View Misc.
  void SetDrawsTransparentBackground(bool enabled);

  // ---- Touch & Hit Test
  bool TouchEventsEnabled() const;
  void SetTouchEventsEnabled(bool enabled);
  void HandleTouchEvents(Touch_Event_Type type, const Eina_List *points, const Evas_Modifier *modifiers);
  Hit_Test* RequestHitTestDataAt(int x, int y, Hit_Test_Mode mode);
  Eina_Bool AsyncRequestHitTestDataAt(int x, int y, tizen_webview::Hit_Test_Mode mode, tizen_webview::View_Hit_Test_Request_Callback, void* user_data);

  // ---- Selection
  void QuerySelectionStyle();
  void OnQuerySelectionStyleReply(const SelectionStylePrams& params);
  void SelectClosestWord(const gfx::Point& touch_point);
  void SelectLinkText(const gfx::Point& touch_point);
  bool GetSelectionRange(Eina_Rectangle* left_rect, Eina_Rectangle* right_rect);
  bool ClearSelection();
  const char* GetSelectedText() const;

  // ---- Session
  void GetSessionData(const char **data, unsigned *length) const;
  bool RestoreFromSessionData(const char *data, unsigned length);

  // ---- Settings
  void UseSettingsFont();
  void SetMouseEventsEnabled(bool enabled);
  bool MouseEventsEnabled() const;
  double GetTextZoomFactor() const;
  void SetTextZoomFactor(double text_zoom_factor);
  bool GetLinkMagnifierEnabled() const;
  void SetLinkMagnifierEnabled(bool enabled);
  void SetBrowserFont(); // browser specific? make static?
  void UpdateWebKitPreferences(); // global? make static

  //---- JavaScript
  bool ExecuteJavaScript(const char* script, View_Script_Execute_Callback callback, void* userdata);
  void SetJavaScriptAlertCallback(View_JavaScript_Alert_Callback callback, void* user_data);
  void JavaScriptAlertReply();
  void SetJavaScriptConfirmCallback(View_JavaScript_Confirm_Callback callback, void* user_data);
  void JavaScriptConfirmReply(bool result);
  void SetJavaScriptPromptCallback(View_JavaScript_Prompt_Callback callback, void* user_data);
  void JavaScriptPromptReply(const char* result);

  // ---- Web App
  bool WebAppCapableGet(Web_App_Capable_Get_Callback callback, void *userData);
  bool WebAppIconUrlGet(Web_App_Icon_URL_Get_Callback callback, void *userData);
  bool WebAppIconUrlsGet(Web_App_Icon_URLs_Get_Callback callback, void *userData);
  void InvokeWebAppCapableGetCallback(bool capable, int callbackId);
  void InvokeWebAppIconUrlGetCallback(const std::string &iconUrl, int callbackId);
  void InvokeWebAppIconUrlsGetCallback(const std::map<std::string, std::string> &iconUrls, int callbackId);

  // ---- Notification Permission Request
  void SetNotificationPermissionCallback(
      tizen_webview::View_Notification_Permission_Callback callback,
      void* user_data);

  // ---- Misc.
  bool SaveAsPdf(int width, int height, const std::string& file_name);
  // Returns TCP port number with Inspector, or 0 if error.
  int StartInspectorServer(int port = 0);
  bool StopInspectorServer();


  /// ---- Event handling
  bool HandleShow();
  bool HandleHide();
  bool HandleMove(int x, int y);
  bool HandleResize(int width, int height);
  bool HandleFocusIn();
  bool HandleFocusOut();
  bool HandleEvasEvent(const Evas_Event_Mouse_Down* event);
  bool HandleEvasEvent(const Evas_Event_Mouse_Up* event);
  bool HandleEvasEvent(const Evas_Event_Mouse_Move* event);
  bool HandleEvasEvent(const Evas_Event_Mouse_Wheel* event);
  bool HandleEvasEvent(const Evas_Event_Key_Down* event);
  bool HandleEvasEvent(const Evas_Event_Key_Up* event);
  bool HandleGesture(ui::GestureEvent* event);
  bool HandleTouchEvent(ui::TouchEvent* event);

  // TODO: move to private
  typedef ::EWebView Impl;
  Impl* GetImpl() { return impl_; }
  const Impl* GetImpl() const { return impl_; }

 private:
  Impl* impl_;
  void SetImpl(Impl* impl) { impl_ = impl; }

  WebView();
  ~WebView();
  DISALLOW_COPY_AND_ASSIGN(WebView);
  friend class ::WebContentDelegateEfl;
};

} // namespace tizen_webview

#endif  // TIZEN_WEBVIEW_PUBLIC_TW_WEBVIEW_H_
