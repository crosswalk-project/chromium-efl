// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "tw_webview.h"
#include "eweb_view.h"

#include <base/logging.h>
#include <base/memory/scoped_ptr.h>

#include "browser/renderer_host/render_widget_host_view_efl.h"
#include <tizen_webview/public/tw_webview_delegate.h>

namespace {

}

namespace tizen_webview {

WebView* WebView::Create(WebContext* wc, Evas_Object* eo) {
  WebView* wv = new WebView();
  DCHECK(wv);

  Impl* impl = new Impl(wv, wc, eo);
  if (!impl) {
    DLOG(ERROR) << "[tizen_webview] Cannot create webview";
    delete wv;
    return NULL;
  }
  wv->SetImpl(impl);
  DCHECK(wv == impl->GetPublicWebView());
  return wv;
}

void WebView::Delete(WebView* wv) {
  delete wv;
}

void WebView::Initialize() {
  impl_->Initialize();
}

WebView::WebView()
    : impl_(NULL) {
}

WebView::~WebView() {
  delete impl_;
}

Evas_Object* WebView::AsEvasObject() {
  return impl_->evas_object();
}

WebView* WebView::FromEvasObject(Evas_Object* eo) {
  return WebViewDelegate::GetInstance()->GetWebViewFromEvasObject(eo);
}

WebContext* WebView::GetWebContext() {
  return impl_->context();
}

Ewk_Settings* WebView::GetSettings() {
  return impl_->GetSettings();
}

SelectionController* WebView::GetSelectionController() {
  return impl_->selection_controller_.get();
}

WebViewEvasEventHandler* WebView::GetEvasEventHandler() {
  return impl_->GetEvasEventHandler();
}

ContextMenuController* WebView::GetContextMenuController() {
  return impl_->context_menu_.get();
}

void WebView::ResetContextMenuController() {
  return impl_->context_menu_.reset();
}


bool WebView::SetUserAgent(const char* userAgent) {
  return impl_->SetUserAgent(userAgent);
}

const char* WebView::GetUserAgent() const {
  return impl_->GetUserAgent();
}

bool WebView::SetUserAgentAppName(const char* application_name) {
  return impl_->SetUserAgentAppName(application_name);
}

const char* WebView::GetUserAgentAppName() const {
  return impl_->GetUserAgentAppName();
}

void WebView::SetURL(const char* url_string) {
  return impl_->SetURL(url_string);
}

const char* WebView::GetURL() const {
  return impl_->GetURL();
}

void WebView::Reload() {
  return impl_->Reload();
}

void WebView::ReloadIgnoringCache() {
  return impl_->ReloadIgnoringCache();
}

double WebView::GetProgressValue() {
  return impl_->GetProgressValue();
}

void WebView::LoadHTMLString(const char* html, const char* base_uri, const char* unreachable_uri) {
  return impl_->LoadHTMLString(html, base_uri, unreachable_uri);
}

void WebView::Suspend() {
  return impl_->Suspend();
}

void WebView::Resume() {
  return impl_->Resume();
}

void WebView::Stop() {
  return impl_->Stop();
}

void WebView::LoadPlainTextString(const char* plain_text) {
  return impl_->LoadPlainTextString(plain_text);
}

void WebView::LoadData(const char* data, size_t size, const char* mime_type, const char* encoding, const char* base_uri, const char* unreachable_uri /*= NULL*/) {
  return impl_->LoadData(data, size, mime_type, encoding, base_uri, unreachable_uri);
}

void WebView::UrlRequestSet(const char* url, std::string method, Eina_Hash* headers, const char* body) {
  return impl_->UrlRequestSet(url, method, headers, body);
}

std::string WebView::GetErrorPage(const std::string& invalidUrl) {
  return impl_->GetErrorPage(invalidUrl);
}

bool WebView::GoBack() {
  return impl_->GoBack();
}

bool WebView::GoForward() {
  return impl_->GoForward();
}

bool WebView::CanGoBack() {
  return impl_->CanGoBack();
}

bool WebView::CanGoForward() {
  return impl_->CanGoForward();
}

void WebView::BackForwardListClear() {
  return impl_->BackForwardListClear();
}

const char* WebView::GetTitle() {
  return impl_->GetTitle();
}

const Eina_Rectangle WebView::GetContentsSize() const {
  return impl_->GetContentsSize();
}

bool WebView::PlainTextGet(View_Plain_Text_Get_Callback callback, void* user_data) {
  return impl_->PlainTextGet(callback, user_data);
}

void WebView::InvokePlainTextGetCallback(const std::string& content_text, int plain_text_get_callback_id) {
  return impl_->InvokePlainTextGetCallback(content_text, plain_text_get_callback_id);
}

int WebView::SetEwkViewPlainTextGetCallback(View_Plain_Text_Get_Callback callback, void* user_data) {
  return impl_->SetEwkViewPlainTextGetCallback(callback, user_data);
}

bool WebView::GetMHTMLData(View_MHTML_Data_Get_Callback callback, void* user_data) {
  return impl_->GetMHTMLData(callback, user_data);
}

void WebView::OnMHTMLContentGet(const std::string& mhtml_content, int callback_id) {
  return impl_->OnMHTMLContentGet(mhtml_content, callback_id);
}

void WebView::SetOverrideEncoding(const std::string& encoding) {
  return impl_->SetOverrideEncoding(encoding);
}

void WebView::ExecuteEditCommand(const char* command, const char* value) {
  return impl_->ExecuteEditCommand(command, value);
}

void WebView::Find(const char* text, Find_Options options) {
  return impl_->Find(text, options);
}

void WebView::StopFinding() {
  return impl_->StopFinding();
}

void WebView::SetContentSecurityPolicy(const char* policy, ContentSecurityPolicyType type) {
  return impl_->SetContentSecurityPolicy(policy, type);
}

void WebView::Show() {
  return impl_->Show();
}

void WebView::Hide() {
  return impl_->Hide();
}

void WebView::SetFocus(Eina_Bool focus) {
  return impl_->SetFocus(focus);
}

bool WebView::HasFocus() const {
  return impl_->HasFocus();
}

void WebView::SetScale(double scale_factor, int x, int y) {
  return impl_->SetScale(scale_factor, x, y);
}

double WebView::GetScale() {
  return impl_->GetScale();
}

void WebView::DidChangePageScaleFactor(double scale_factor) {
  return impl_->DidChangePageScaleFactor(scale_factor);
}

void WebView::GetPageScaleRange(double *min_scale, double *max_scale) {
  return impl_->GetPageScaleRange(min_scale, max_scale);
}

void WebView::DidChangePageScaleRange(double min_scale, double max_scale) {
  return impl_->DidChangePageScaleRange(min_scale, max_scale);
}

void WebView::GetScrollPosition(int* x, int* y) const {
  return impl_->GetScrollPosition(x, y);
}

void WebView::SetScroll(int x, int y) {
  return impl_->SetScroll(x, y);
}

void WebView::GetScrollSize(int* w, int* h) {
  return impl_->GetScrollSize(w, h);
}

void WebView::SendOrientationChangeEventIfNeeded(int orientation) {
  return impl_->SendOrientationChangeEventIfNeeded(orientation);
}

void WebView::SetOrientationLockCallback(Orientation_Lock_Cb func, void* data) {
  return impl_->SetOrientationLockCallback(func, data);
}

void WebView::SetViewMode(int view_mode) {
#warning "[M37] View Mode is not supported by engine any more"
#if 0
  return impl_->SetViewMode(view_mode);
#endif
}

bool WebView::IsFullscreen() {
  return impl_->IsFullscreen();
}

void WebView::ExitFullscreen() {
  return impl_->ExitFullscreen();
}

Evas_Object* WebView::GetSnapshot(Eina_Rectangle rect) {
  return impl_->GetSnapshot(rect);
}

void WebView::SetDrawsTransparentBackground(bool enabled) {
  return impl_->SetDrawsTransparentBackground(enabled);
}

bool WebView::TouchEventsEnabled() const {
  return impl_->TouchEventsEnabled();
}

void WebView::SetTouchEventsEnabled(bool enabled) {
  return impl_->SetTouchEventsEnabled(enabled);
}

void WebView::HandleTouchEvents(Touch_Event_Type type, const Eina_List *points, const Evas_Modifier *modifiers) {
  return impl_->HandleTouchEvents(type, points, modifiers);
}

Hit_Test* WebView::RequestHitTestDataAt(int x, int y, Hit_Test_Mode mode) {
  return impl_->RequestHitTestDataAt(x, y, mode);
}

Eina_Bool WebView::AsyncRequestHitTestDataAt(int x, int y, tizen_webview::Hit_Test_Mode mode, tizen_webview::View_Hit_Test_Request_Callback callback, void* user_data)
{
  return impl_->AsyncRequestHitTestDataAt(x, y, mode, callback, user_data);
}
//
//void WebView::UpdateHitTestData(const Hit_Test& hit_test_data, const NodeAttributesMap& node_attributes) {
//  return impl_->UpdateHitTestData(hit_test_data, node_attributes);
//}

void WebView::QuerySelectionStyle() {
  return impl_->QuerySelectionStyle();
}

void WebView::OnQuerySelectionStyleReply(const SelectionStylePrams& params) {
  return impl_->OnQuerySelectionStyleReply(params);
}

void WebView::SelectClosestWord(const gfx::Point& touch_point) {
  return impl_->SelectClosestWord(touch_point);
}

void WebView::SelectLinkText(const gfx::Point& touch_point) {
  return impl_->SelectLinkText(touch_point);
}

bool WebView::GetSelectionRange(Eina_Rectangle* left_rect, Eina_Rectangle* right_rect) {
  return impl_->GetSelectionRange(left_rect, right_rect);
}

bool WebView::ClearSelection() {
  return impl_->ClearSelection();
}

const char* WebView::GetSelectedText() const {
  return impl_->GetSelectedText();
}

void WebView::GetSessionData(const char **data, unsigned *length) const {
  return impl_->GetSessionData(data, length);
}

bool WebView::RestoreFromSessionData(const char *data, unsigned length) {
  return impl_->RestoreFromSessionData(data, length);
}

void WebView::UseSettingsFont() {
  return impl_->UseSettingsFont();
}

void WebView::SetMouseEventsEnabled(bool enabled) {
  return impl_->SetMouseEventsEnabled(enabled);
}

bool WebView::MouseEventsEnabled() const {
  return impl_->MouseEventsEnabled();
}

double WebView::GetTextZoomFactor() const {
  return impl_->GetTextZoomFactor();
}

void WebView::SetTextZoomFactor(double text_zoom_factor) {
  return impl_->SetTextZoomFactor(text_zoom_factor);
}

bool WebView::GetLinkMagnifierEnabled() const {
  return impl_->GetLinkMagnifierEnabled();
}

void WebView::SetLinkMagnifierEnabled(bool enabled) {
  return impl_->SetLinkMagnifierEnabled(enabled);
}

void WebView::SetBrowserFont() {
  return impl_->SetBrowserFont();
}

void WebView::UpdateWebKitPreferences() {
  return impl_->UpdateWebKitPreferences();
}

bool WebView::ExecuteJavaScript(const char* script, View_Script_Execute_Callback callback, void* userdata) {
  return impl_->ExecuteJavaScript(script, callback, userdata);
}

void WebView::SetJavaScriptAlertCallback(View_JavaScript_Alert_Callback callback, void* user_data) {
  return impl_->SetJavaScriptAlertCallback(callback, user_data);
}

void WebView::JavaScriptAlertReply() {
  return impl_->JavaScriptAlertReply();
}

void WebView::SetJavaScriptConfirmCallback(View_JavaScript_Confirm_Callback callback, void* user_data) {
  return impl_->SetJavaScriptConfirmCallback(callback, user_data);
}

void WebView::JavaScriptConfirmReply(bool result) {
  return impl_->JavaScriptConfirmReply(result);
}

void WebView::SetJavaScriptPromptCallback(View_JavaScript_Prompt_Callback callback, void* user_data) {
  return impl_->SetJavaScriptPromptCallback(callback, user_data);
}

void WebView::JavaScriptPromptReply(const char* result) {
  return impl_->JavaScriptPromptReply(result);
}

bool WebView::WebAppCapableGet(Web_App_Capable_Get_Callback callback, void *userData) {
  return impl_->WebAppCapableGet(callback, userData);
}

bool WebView::WebAppIconUrlGet(Web_App_Icon_URL_Get_Callback callback, void *userData) {
  return impl_->WebAppIconUrlGet(callback, userData);
}

bool WebView::WebAppIconUrlsGet(Web_App_Icon_URLs_Get_Callback callback, void *userData) {
  return impl_->WebAppIconUrlsGet(callback, userData);
}

void WebView::InvokeWebAppCapableGetCallback(bool capable, int callbackId) {
  return impl_->InvokeWebAppCapableGetCallback(capable, callbackId);
}

void WebView::InvokeWebAppIconUrlGetCallback(const std::string &iconUrl, int callbackId) {
  return impl_->InvokeWebAppIconUrlGetCallback(iconUrl, callbackId);
}

void WebView::InvokeWebAppIconUrlsGetCallback(const std::map<std::string, std::string> &iconUrls, int callbackId) {
  return impl_->InvokeWebAppIconUrlsGetCallback(iconUrls, callbackId);
}

bool WebView::SaveAsPdf(int width, int height, const std::string& file_name) {
  return impl_->SaveAsPdf(width, height, file_name);
}

int WebView::StartInspectorServer(int port ) {
  return impl_->StartInspectorServer(port);
}

bool WebView::StopInspectorServer() {
  return impl_->StopInspectorServer();
}

bool WebView::HandleShow() {
  return impl_->HandleShow();
}

bool WebView::HandleHide() {
  return impl_->HandleHide();
}

bool WebView::HandleMove(int x, int y) {
  return impl_->HandleMove(x, y);
}

bool WebView::HandleResize(int width, int height) {
  return impl_->HandleResize(width, height);
}

bool WebView::HandleFocusIn() {
  return impl_->HandleFocusIn();
}

bool WebView::HandleFocusOut() {
  return impl_->HandleFocusOut();
}

bool WebView::HandleEvasEvent(const Evas_Event_Mouse_Down* event) {
  return impl_->HandleEvasEvent(event);
}

bool WebView::HandleEvasEvent(const Evas_Event_Mouse_Up* event) {
  return impl_->HandleEvasEvent(event);
}

bool WebView::HandleEvasEvent(const Evas_Event_Mouse_Move* event) {
  return impl_->HandleEvasEvent(event);
}

bool WebView::HandleEvasEvent(const Evas_Event_Mouse_Wheel* event) {
  return impl_->HandleEvasEvent(event);
}

bool WebView::HandleEvasEvent(const Evas_Event_Key_Down* event) {
  return impl_->HandleEvasEvent(event);
}

bool WebView::HandleEvasEvent(const Evas_Event_Key_Up* event) {
  return impl_->HandleEvasEvent(event);
}

bool WebView::HandleGesture(ui::GestureEvent* event) {
  return impl_->HandleGesture(event);
}

bool WebView::HandleTouchEvent(ui::TouchEvent* event) {
  return impl_->HandleTouchEvent(event);
}

} // namespace tizen_webview
