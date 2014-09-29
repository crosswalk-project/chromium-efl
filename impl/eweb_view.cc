/*
    Copyright (C) 2014 Samsung Electronics

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "eweb_view.h"
#include <config.h>

#include "base/pickle.h"
#include "base/threading/thread_restrictions.h"
#include "browser/navigation_policy_handler_efl.h"
#include "browser/renderer_host/render_widget_host_view_efl.h"
#include "browser/renderer_host/web_event_factory_efl.h"
#include "common/render_messages_efl.h"
#include "common/version_info.h"
#include "components/sessions/serialized_navigation_entry.h"
#include "API/ewk_policy_decision_private.h"
#include "API/ewk_settings_private.h"
#include "API/ewk_text_style_private.h"
#include "web_contents_delegate_efl.h"
#include "public/platform/WebString.h"
#include "base/command_line.h"
#include "base/files/file_path.h"
#include "base/logging.h"
#include "base/strings/utf_string_conversions.h"
#include "content/common/view_messages.h"
#include "content/browser/renderer_host/ui_events_helper.h"
#include "content/browser/renderer_host/render_view_host_impl.h"
#include "content/browser/web_contents/web_contents_view.h"
#include "content/public/browser/browser_message_filter.h"
#include "content/public/browser/navigation_controller.h"
#include "content/public/browser/navigation_entry.h"
#include "content/public/browser/resource_dispatcher_host.h"
#include "content/public/common/content_client.h"
#include "content/public/common/user_agent.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/host_zoom_map.h"
#include "skia/ext/platform_canvas.h"
#include "third_party/WebKit/public/web/WebFindOptions.h"
#include "ui/events/event_switches.h"
#include "browser/motion/wkext_motion.h"
#include "ui/base/l10n/l10n_util.h"
#include "devtools_delegate_efl.h"

#include "tizen_webview/public/tw_hit_test.h"
#include "tizen_webview/public/tw_touch_point.h"
#include "tizen_webview/public/tw_web_context.h"
#include "tizen_webview/public/tw_webview.h"
#include "tizen_webview/public/tw_webview_delegate.h"
#include "tizen_webview/public/tw_webview_evas_event_handler.h"

#ifdef OS_TIZEN
#include <vconf.h>
#include "browser/selectpicker/popup_menu_item.h"
#include "browser/selectpicker/popup_menu_item_private.h"
#include "browser/selectpicker/WebPopupItem.h"
#endif
#include <Ecore_Evas.h>
#include <Elementary.h>
#include <Eina.h>

#include <iostream>

//this constant is not defined in efl headers so we have to do it here
#ifndef GL_BGRA
#define GL_BGRA 0x80E1
#endif

using namespace content;
using namespace tizen_webview;

// GetContentClient() is defined in content_client.cc, but in content_client.h
// it is hidden by CONTENT_IMPLEMENTATION ifdef. We don't want to define
// CONTENT_IMPLEMENTATION because it may bring a lot of things we don't need.
namespace content {
  ContentClient* GetContentClient();
}

namespace {

inline void SetDefaultStringIfNull(const char*& variable,
                                   const char* default_string) {
  if (!variable) {
    variable = default_string;
  }
}

#ifdef OS_TIZEN
bool GetTiltZoomEnabled()
{
  int motion_enabled = 0;
  vconf_get_bool(VCONFKEY_SETAPPL_MOTION_ACTIVATION, &motion_enabled);
  if (motion_enabled) {
    int tilt_enabled = 0;
    vconf_get_bool(VCONFKEY_SETAPPL_USE_TILT, &tilt_enabled);
    //BROWSER_LOGD("******* motion_enabled=[%d], tilt_enabled=[%d]", motion_enabled, tilt_enabled);
    if (tilt_enabled) {
      return true;
    }
  }
  return false;
}
#endif // OS_TIZEN

void GetEinaRectFromGfxRect(const gfx::Rect& gfx_rect, Eina_Rectangle* eina_rect)
{
  eina_rect->x = gfx_rect.x();
  eina_rect->y = gfx_rect.y();
  eina_rect->w = gfx_rect.width();
  eina_rect->h = gfx_rect.height();
}

} // namespace

class WebViewBrowserMessageFilter: public content::BrowserMessageFilter {
 public:
  WebViewBrowserMessageFilter(EWebView* web_view)
    : BrowserMessageFilter(ChromeMsgStart)
    , web_view_(web_view) {
    WebContents* web_contents = GetWebContents();
    DCHECK(web_contents);

    if (web_contents && web_contents->GetRenderProcessHost())
      web_contents->GetRenderProcessHost()->AddFilter(this);
  }

  bool OnMessageReceived(const IPC::Message& message) {
    bool handled = true;
    IPC_BEGIN_MESSAGE_MAP(WebViewBrowserMessageFilter, message)
      IPC_MESSAGE_HANDLER(EwkViewHostMsg_HitTestReply, OnReceivedHitTestData)
      IPC_MESSAGE_UNHANDLED(handled = false)
    IPC_END_MESSAGE_MAP()
    return handled;
  }

 private:
  void OnReceivedHitTestData(int render_view, const _Ewk_Hit_Test& hit_test_data,
      const NodeAttributesMap& node_attributes) {
    DCHECK(BrowserThread::CurrentlyOn(BrowserThread::IO));
    WebContents* contents = GetWebContents();
    DCHECK(contents);

    if (contents) {
      RenderViewHost* render_view_host= contents->GetRenderViewHost();
      DCHECK(render_view_host);

      if (render_view_host && render_view_host->GetRoutingID() == render_view)
        web_view_->UpdateHitTestData(hit_test_data, node_attributes);
    }
  }

  WebContents* GetWebContents() const {
    if (web_view_ && web_view_->web_contents_delegate())
      return web_view_->web_contents_delegate()->web_contents();

    return NULL;
  }

private:
  EWebView* web_view_;
};

WebContents* EWebView::contents_for_new_window_ = NULL;

int EWebView::find_request_id_counter_ = 0;

EWebView* EWebView::FromEvasObject(Evas_Object* eo) {
  WebView *wv = WebView::FromEvasObject(eo);
  if (!wv) {
    DLOG(ERROR) << "Trying to get WebView from non-WebView Evas_Object";
    return NULL;
  }
  return wv->GetImpl();
}

tizen_webview::WebView* EWebView::GetPublicWebView() {
  DCHECK(public_webview_);
  if (public_webview_ == NULL) {
     DLOG(ERROR) << "WebView is not set. Something wrong on construction";
  }
  return public_webview_;
}

Evas_Object* EWebView::GetContentImageObject() const
{
  return WebViewDelegate::GetInstance()->GetContentImageEvasObject(evas_object_);
}

RenderWidgetHostViewEfl* EWebView::rwhv() const {
  return static_cast<RenderWidgetHostViewEfl*>(web_contents_delegate_->web_contents()->GetRenderWidgetHostView());
}

void EWebView::set_renderer_crashed() {
#ifndef NDEBUG
  DCHECK(!renderer_crashed_);
  renderer_crashed_ = true;
#endif
}

EWebView::EWebView(tizen_webview::WebView* owner, tizen_webview::WebContext* context, Evas_Object* object)
    : public_webview_(owner),
      evas_event_handler_(NULL),
      context_(context),
      evas_object_(object),
      touch_events_enabled_(false),
      mouse_events_enabled_(false),
      text_zoom_factor_(1.0),
      current_find_request_id_(find_request_id_counter_++),
      progress_(0.0),
      hit_test_completion_(false, false),
      page_scale_factor_(1.0),
      min_page_scale_factor_(-1.0),
      max_page_scale_factor_(-1.0),
      inspector_server_(NULL),
      message_filter_(NULL),
#ifndef NDEBUG
      renderer_crashed_(false),
#endif
      is_initialized_(false) {
  context->GetImpl()->Initialize(object);
}

void EWebView::Initialize() {
  if (is_initialized_) {
    return;
  }

  evas_event_handler_ = new tizen_webview::WebViewEvasEventHandler(public_webview_);
  selection_controller_.reset(new content::SelectionControllerEfl(this));
#ifdef TIZEN_EDGE_EFFECT
  edge_effect_ = EdgeEffect::create(evas_object_);
#endif
#if defined(TIZEN_POPUPZOOMER_SUPPORT)
  disambiguation_popup_controller_.reset(new content::DisambiguationPopupControllerEfl(this));
#endif

  if (contents_for_new_window_) {
    web_contents_delegate_.reset(new WebContentsDelegateEfl(this, contents_for_new_window_));

    // Null it out as soon as possible. This way it does not cause harm if the embedder creates
    // a new view in the "create,window" callback as long as the result of the callback is the
    // first one that has been created.
    contents_for_new_window_ = NULL;
  } else {
    web_contents_delegate_.reset(new WebContentsDelegateEfl(this))  ;
  }

  // Make sure a RenderWidgetHostViewEfl is created and set up properly.
  WebContentsImpl* web_contents =
      static_cast<WebContentsImpl*>(web_contents_delegate_->web_contents());
  WebContentsView* contents_view =
      static_cast<WebContentsView*>((web_contents)->GetView());
  contents_view->CreateViewForWidget(web_contents->GetRenderViewHost());
  DCHECK(rwhv());

  // Activate Event handler
  evas_event_handler_->BindFocusEventHandlers();
  evas_event_handler_->BindKeyEventHandlers();

#if defined(OS_TIZEN)
  bool enable = GetTiltZoomEnabled();
  if (enable) {
    evas_event_handler_->BindMotionEventHandlers();
  } else {
    evas_event_handler_->UnbindMotionEventHandlers();
  }
  //evas_object_smart_callback_call(evas_object(), "motion,enable", (void*)&enable);
  wkext_motion_tilt_enable_set(evas_object_, static_cast<int>(enable),
      g_default_tilt_motion_sensitivity);
#endif

  CommandLine *cmdline = CommandLine::ForCurrentProcess();
  if (cmdline->HasSwitch(switches::kTouchEvents))
    SetTouchEventsEnabled(true);
  else
    SetMouseEventsEnabled(true);

#if defined(OS_TIZEN)
  popupMenuItems_ = 0;
  popupPicker_ = 0;
#endif
  //allow this object and its children to get a focus
  elm_object_tree_focus_allow_set (evas_object_, EINA_TRUE);
  is_initialized_ = true;
}

EWebView::~EWebView()
{
  if (!is_initialized_) {
    return;
  }
  StopInspectorServer(); // inside is check to Inspector is running
  GetEvasEventHandler()->UnbindFocusEventHandlers();
  GetEvasEventHandler()->UnbindKeyEventHandlers();
  GetEvasEventHandler()->UnbindTouchEventHandlers();
  GetEvasEventHandler()->UnbindMouseEventHandlers();

  context_menu_.reset();
  mhtml_callback_map_.Clear();

#if defined(OS_TIZEN)
  void* item;
  EINA_LIST_FREE(popupMenuItems_, item);
    delete static_cast<Popup_Menu_Item*>(item);

  if (popupPicker_)
    popup_picker_del(popupPicker_);
#endif
//  evas_object_del(evas_object());
  delete evas_event_handler_;
  public_webview_ = NULL;
}

void EWebView::SetFocus(Eina_Bool focus)
{
  if (HasFocus() != focus) {
    elm_object_focus_set(evas_object_, focus);
  }
}

void EWebView::CreateNewWindow(WebContents* new_contents) {
  DCHECK(!contents_for_new_window_);
  contents_for_new_window_ = new_contents;

  Evas_Object* new_object = NULL; // We do not actually need it but this is the API.
  SmartCallback<EWebViewCallbacks::CreateNewWindow>().call(&new_object);

  contents_for_new_window_ = NULL;

  // If no view was created by the embedder than we have a dangling WebContents.
  // This should not really happen though since we negotiate window creation via
  // the "policy,decision,new,window" smart callback.
}

void EWebView::SetURL(const char* url_string) {
  GURL url(url_string);
  NavigationController::LoadURLParams params(url);
  WebContents* web_contents = web_contents_delegate_->web_contents();
  web_contents->GetController().LoadURLWithParams(params);
}

const char* EWebView::GetURL() const {
  WebContents* web_contents = web_contents_delegate_->web_contents();
  return web_contents->GetVisibleURL().possibly_invalid_spec().c_str();
}

void EWebView::Reload() {
  WebContents* web_contents = web_contents_delegate_->web_contents();
  web_contents->GetController().Reload(true);
}

void EWebView::ReloadIgnoringCache() {
  WebContents* web_contents = web_contents_delegate_->web_contents();
  web_contents->GetController().ReloadIgnoringCache(true);
}

Eina_Bool EWebView::CanGoBack() {
  WebContents* web_contents = web_contents_delegate_->web_contents();
  return web_contents->GetController().CanGoBack();
}

Eina_Bool EWebView::CanGoForward() {
  WebContents* web_contents = web_contents_delegate_->web_contents();
  return web_contents->GetController().CanGoForward();
}

Eina_Bool EWebView::HasFocus() const {
  return elm_object_focus_get(evas_object_);
}

Eina_Bool EWebView::GoBack() {
  WebContents* web_contents = web_contents_delegate_->web_contents();
  if (!web_contents->GetController().CanGoBack())
    return EINA_FALSE;

  web_contents->GetController().GoBack();
  return EINA_TRUE;
}

Eina_Bool EWebView::GoForward() {
  WebContents* web_contents = web_contents_delegate_->web_contents();
  if (!web_contents->GetController().CanGoForward())
    return EINA_FALSE;

  web_contents->GetController().GoForward();
  return EINA_TRUE;
}

void EWebView::Stop() {
  WebContents* web_contents = web_contents_delegate_->web_contents();
  if (web_contents->IsLoading())
    web_contents->Stop();
}

void EWebView::Suspend() {
  CHECK(web_contents_delegate_);

  WebContents* contents = web_contents_delegate_->web_contents();
  CHECK(contents);

  RenderViewHost *rvh = contents->GetRenderViewHost();
  CHECK(rvh);

  content::ResourceDispatcherHost* rdh = content::ResourceDispatcherHost::Get();
  CHECK(rdh);

  content::BrowserThread::PostTask(
    content::BrowserThread::IO, FROM_HERE,
    base::Bind(&content::ResourceDispatcherHost::BlockRequestsForRoute,
      base::Unretained(rdh),
      rvh->GetProcess()->GetID(), rvh->GetRoutingID()));
}

void EWebView::Resume() {
  CHECK(web_contents_delegate_);

  WebContents* contents = web_contents_delegate_->web_contents();
  CHECK(contents);

  RenderViewHost *rvh = contents->GetRenderViewHost();
  CHECK(rvh);

  content::ResourceDispatcherHost* rdh = content::ResourceDispatcherHost::Get();
  CHECK(rdh);

  content::BrowserThread::PostTask(
    content::BrowserThread::IO, FROM_HERE,
    base::Bind(&content::ResourceDispatcherHost::ResumeBlockedRequestsForRoute,
      base::Unretained(rdh),
      rvh->GetProcess()->GetID(), rvh->GetRoutingID()));
}

double EWebView::GetTextZoomFactor() const {
  if (text_zoom_factor_ < 0.0)
    return -1.0;

  return text_zoom_factor_;
}

void EWebView::SetTextZoomFactor(double text_zoom_factor) {
  if (text_zoom_factor_ == text_zoom_factor || text_zoom_factor < 0.0)
    return;

  text_zoom_factor_ = text_zoom_factor;
  double zoom_level = log(text_zoom_factor) / log(1.2);
  WebContents* web_contents = web_contents_delegate_->web_contents();
  content::HostZoomMap::SetZoomLevel(web_contents, zoom_level);
}

void EWebView::ExecuteEditCommand(const char* command, const char* value) {
  EINA_SAFETY_ON_NULL_RETURN(command);

  value = (value == NULL) ? "" : value;

  WebContents* web_contents = web_contents_delegate_->web_contents();
  RenderViewHostImpl* rvhi = static_cast<RenderViewHostImpl*>(web_contents->GetRenderViewHost());

  rvhi->ExecuteEditCommand(command, value);

  // This is workaround for rich text toolbar buttons in email application
  if ( !strcmp(command, "InsertOrderedList")
    || !strcmp(command, "InsertUnorderedList")
    || !strcmp(command, "AlignCenter")
    || !strcmp(command, "AlignJustified")
    || !strcmp(command, "AlignLeft")
    || !strcmp(command, "AlignRight") ) {
    QuerySelectionStyle();
  }
}

void EWebView::SendOrientationChangeEventIfNeeded(int orientation) {
  WebContents* web_contents = web_contents_delegate_->web_contents();
  RenderViewHostImpl* rvhi = static_cast<RenderViewHostImpl*>(
                             web_contents->GetRenderViewHost());
#warning "[M37] Fix screen orientation"
#if 0
  //send new orientation value to RenderView Host to pass to renderer
  if(rvhi && rvhi->GetOrientation() != orientation) {
    switch(orientation) {
    case -90:
    case 0:
    case 90:
    case 180:
      rvhi->SendOrientationChangeEvent(orientation);
      break;

    default:
      NOTREACHED();
      break;
    }
  }
#endif
}

void EWebView::SetOrientationLockCallback(tizen_webview::Orientation_Lock_Cb func, void* data) {
  orientation_lock_callback_.reset(new OrientationLockCallback(func, data));
}

void EWebView::Show() {
  web_contents_delegate_->web_contents()->WasShown();
}

void EWebView::Hide() {
  web_contents_delegate_->web_contents()->WasHidden();
}

void EWebView::InvokeAuthCallback(LoginDelegateEfl* login_delegate,
                                  const GURL& url,
                                  const std::string& realm) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));

  auth_challenge_.reset(new _Ewk_Auth_Challenge(login_delegate, url, realm));
  SmartCallback<EWebViewCallbacks::AuthChallenge>().call(auth_challenge_.get());

  if (!auth_challenge_->is_decided && !auth_challenge_->is_suspended) {
    auth_challenge_->is_decided = true;
    auth_challenge_->login_delegate->Cancel();
  }
}

void EWebView::InvokePolicyResponseCallback(tizen_webview::PolicyDecision* policy_decision) {
  set_policy_decision(policy_decision);
  SmartCallback<EWebViewCallbacks::PolicyResponseDecide>().call(policy_decision_.get());

  // if app has not decided nor suspended, we act as if it was accepted.
  if (!policy_decision_->isDecided() && !policy_decision_->isSuspended())
    policy_decision_->Use();
}

void EWebView::InvokePolicyNavigationCallback(RenderViewHost* rvh,
    const NavigationPolicyParams params, bool* handled) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  policy_decision_.reset(new tizen_webview::PolicyDecision(params, rvh));

  SmartCallback<EWebViewCallbacks::NavigationPolicyDecision>().call(policy_decision_.get());

  // if app has not decided nor suspended, we act as if it was accepted.
  if (!policy_decision_->isDecided() && !policy_decision_->isSuspended())
    policy_decision_->Use();

  *handled = policy_decision_->GetImpl()->GetNavigationPolicyHandler()->GetDecision() == NavigationPolicyHandlerEfl::Handled;
}

void EWebView::HandleTouchEvents(tizen_webview::Touch_Event_Type type, const Eina_List *points, const Evas_Modifier *modifiers)
{
  const Eina_List* l;
  void* data;
  EINA_LIST_FOREACH(points, l, data) {
    const tizen_webview::Touch_Point* point = reinterpret_cast<tizen_webview::Touch_Point*>(data);
    if (point->state == EVAS_TOUCH_POINT_STILL) {
      // Chromium doesn't expect (and doesn't like) these events.
      continue;
    }
    ui::TouchEvent touch_event = WebEventFactoryEfl::toUITouchEvent(point, evas_object(), rwhv()->device_scale_factor());
    rwhv()->HandleTouchEvent(&touch_event);
  }
}

/* FIXME: Figure out wher this code should be placed.
void EWebView::DispatchPostponedGestureEvent(ui::GestureEvent* event) {
  Ewk_Settings* settings = GetSettings();
  LOG(INFO) << "DispatchPostponedGestureEvent :: " << event->details().type();
  if (event->details().type() == ui::ET_GESTURE_LONG_PRESS) {
    if (selection_controller_->GetSelectionEditable())
      ClearSelection();

    if (settings && settings->textSelectionEnabled()) {
      tizen_webview::Hit_Test* hit_test = RequestHitTestDataAtBlinkCoords(event->x(), event->y(), TW_HIT_TEST_MODE_DEFAULT);
      _Ewk_Hit_Test* hit_test_data = hit_test->impl;
      if (hit_test_data && hit_test_data->context & TW_HIT_TEST_RESULT_CONTEXT_EDITABLE) {
        selection_controller_->SetSelectionStatus(true);
        selection_controller_->SetCaretSelectionStatus(true);
        selection_controller_->SetSelectionEditable(true);
        selection_controller_->HandleLongPressEvent(rwhv()->ConvertPointInViewPix(gfx::Point(event->x(), event->y())));
      } else if (hit_test_data
          && !(hit_test_data->context & TW_HIT_TEST_RESULT_CONTEXT_LINK)
          && !(hit_test_data->context & TW_HIT_TEST_RESULT_CONTEXT_IMAGE)
          && !(hit_test_data->context & TW_HIT_TEST_RESULT_CONTEXT_MEDIA)
          && hit_test_data->context & TW_HIT_TEST_RESULT_CONTEXT_TEXT) {
        selection_controller_->SetSelectionStatus(true);
        selection_controller_->HandleLongPressEvent(rwhv()->ConvertPointInViewPix(gfx::Point(event->x(), event->y())));
        LOG(INFO) << __PRETTY_FUNCTION__ << ":: link, !image, !media, text";
      } else if (hit_test_data && hit_test_data->context & TW_HIT_TEST_RESULT_CONTEXT_DOCUMENT) {
        LOG(INFO) << __PRETTY_FUNCTION__ << ":: TW_HIT_TEST_RESULT_CONTEXT_DOCUMENT";
      } else if (hit_test_data && hit_test_data->context & TW_HIT_TEST_RESULT_CONTEXT_IMAGE) {
        LOG(INFO) << __PRETTY_FUNCTION__ << ":: TW_HIT_TEST_RESULT_CONTEXT_IMAGE";
      } else if (hit_test_data && hit_test_data->context & TW_HIT_TEST_RESULT_CONTEXT_LINK) {
        ClearSelection();
        LOG(INFO) << __PRETTY_FUNCTION__ << ":: TW_HIT_TEST_RESULT_CONTEXT_LINK";
      } else {
        LOG(INFO) << __PRETTY_FUNCTION__ << ":: hit_test = " << hit_test_data->context;
      }
      delete hit_test;
      rwhv()->HandleGesture(event);
    }
  } else if ((event->details().type() == ui::ET_GESTURE_TAP) || (event->details().type() == ui::ET_GESTURE_SHOW_PRESS))  {
    tizen_webview::Hit_Test* hit_test = RequestHitTestDataAtBlinkCoords(event->x(), event->y(), TW_HIT_TEST_MODE_DEFAULT);
   _Ewk_Hit_Test* hit_test_data = hit_test->impl;
    LOG(INFO) << __PRETTY_FUNCTION__ << " hit_test = " << hit_test_data;
    if (hit_test_data && hit_test_data->context & TW_HIT_TEST_RESULT_CONTEXT_EDITABLE) {
      LOG(INFO) << "DispatchPostponedGestureEvent :: TW_HIT_TEST_RESULT_CONTEXT_EDITABLE";
      selection_controller_->SetSelectionStatus(true);
      if (selection_controller_->GetSelectionEditable()) {
        selection_controller_->HideHandle();
        selection_controller_->SetCaretSelectionStatus(true);
      } else
        selection_controller_->SetSelectionEditable(true);
    } else {
      if (hit_test_data && hit_test_data->context & TW_HIT_TEST_RESULT_CONTEXT_DOCUMENT)
        LOG(INFO) << __PRETTY_FUNCTION__ << " DOCUMENT";
      if (hit_test_data && hit_test_data->context & TW_HIT_TEST_RESULT_CONTEXT_TEXT)
        LOG(INFO) << __PRETTY_FUNCTION__ << " TEXT";

      selection_controller_->SetSelectionEditable(false);
      //ClearSelection();
    }
    delete hit_test;
    rwhv()->HandleGesture(event);
  } else {
    ClearSelection();
    rwhv()->HandleGesture(event);
  }
  // FIXME:As there is only single window currently, and even there are no
  // other evas objects, adding focus in event here for display of key board.
  //Once added build with proper apps to be removed
  rwhv()->HandleFocusIn();
}
*/

bool EWebView::TouchEventsEnabled() const {
  return touch_events_enabled_;
}

// TODO: Touch events use the same mouse events in EFL API.
// Figure out how to distinguish touch and mouse events on touch&mice devices.
// Currently mouse and touch support is mutually exclusive.
void EWebView::SetTouchEventsEnabled(bool enabled) {
  if (touch_events_enabled_ == enabled)
    return;

  touch_events_enabled_ = enabled;

  if (enabled) {
    GetEvasEventHandler()->UnbindMouseEventHandlers();
    GetEvasEventHandler()->BindTouchEventHandlers();
  } else {
    // TODO(sns.park): Why not call BindMouseEventHandlers()?
    // I think it should be called to be symmetric with "enabled" case
    GetEvasEventHandler()->UnbindTouchEventHandlers();
  }
}

bool EWebView::MouseEventsEnabled() const {
  return mouse_events_enabled_;
}

void EWebView::SetMouseEventsEnabled(bool enabled) {
  if (mouse_events_enabled_ == enabled)
    return;

  mouse_events_enabled_ = enabled;

  if (enabled) {
    GetEvasEventHandler()->UnbindTouchEventHandlers();
    GetEvasEventHandler()->BindMouseEventHandlers();
  } else {
    // TODO(sns.park): Why not call BindTouchEventHandlers()?
    // I think it should be called to be symmetric with "enabled" case
    GetEvasEventHandler()->UnbindMouseEventHandlers();
  }
}

namespace {

class JavaScriptCallbackDetails {
 public:
  JavaScriptCallbackDetails(tizen_webview::View_Script_Execute_Callback callback_func, void *user_data, Evas_Object* view)
    : callback_func_(callback_func)
    , user_data_(user_data)
    , view_(view) {}

  tizen_webview::View_Script_Execute_Callback callback_func_;
  void *user_data_;
  Evas_Object* view_;
};

void JavaScriptComplete(JavaScriptCallbackDetails* script_callback_data, const base::Value* result) {
  if (!script_callback_data->callback_func_)
    return;

  std::string return_string;
  result->GetAsString(&return_string);
  script_callback_data->callback_func_(script_callback_data->view_, return_string.c_str(), script_callback_data->user_data_);
}

} //namespace

bool EWebView::ExecuteJavaScript(const char* script, tizen_webview::View_Script_Execute_Callback callback, void* userdata) {
  if (!script)
    return false;

  if (!web_contents_delegate_)
    return false;

  WebContents* web_contents = web_contents_delegate_->web_contents();
  if (!web_contents)
    return false;

  RenderFrameHost* render_frame_host = web_contents->GetMainFrame();
  if (!render_frame_host)
    return false;

  // Note: M37. Execute JavaScript, |script| with |RenderFrameHost::ExecuteJavaScript|.
  // @see also https://codereview.chromium.org/188893005 for more details.
  base::string16 js_script;
  base::UTF8ToUTF16(script, strlen(script), &js_script);
  if (callback) {
    JavaScriptCallbackDetails* script_callback_data = new JavaScriptCallbackDetails(callback, userdata, evas_object_);
    render_frame_host->ExecuteJavaScript(js_script, base::Bind(&JavaScriptComplete, base::Owned(script_callback_data)));
  } else {
    render_frame_host->ExecuteJavaScript(js_script);
  }

  return true;
}

bool EWebView::SetUserAgent(const char* userAgent) {
  web_contents_delegate()->web_contents()->SetUserAgentOverride(std::string(userAgent));
  const content::NavigationController& controller =
      web_contents_delegate()->web_contents()->GetController();
  for (int i = 0; i < controller.GetEntryCount(); ++i)
    controller.GetEntryAtIndex(i)->SetIsOverridingUserAgent(true);
  overridden_user_agent_ = std::string(userAgent);
  web_contents_delegate_->web_contents()->SetUserAgentOverride(userAgent);
  return true;
}

bool EWebView::SetUserAgentAppName(const char* application_name) {
  EflWebView::VersionInfo::GetInstance()->
    SetProductName(std::string(application_name));
  return true;
}

void EWebView::set_magnifier(bool status) {
  rwhv()->set_magnifier(status);
}

const char* EWebView::GetUserAgent() const {
  if (overridden_user_agent_.empty()) {
    return (GetContentClient()->GetUserAgent()).c_str();
  } else {
    return overridden_user_agent_.c_str();
  }
}

const char* EWebView::GetUserAgentAppName() const {
  return EflWebView::VersionInfo::GetInstance()->Name().c_str();
}

const char* EWebView::GetSelectedText() const {
  selected_text_ = UTF16ToUTF8(rwhv()->GetSelectedText());
  if (selected_text_.empty())
    return NULL;
  return selected_text_.c_str();
}

Ewk_Settings* EWebView::GetSettings() {
  RenderViewHost* render_view_host = web_contents_delegate_->web_contents()->GetRenderViewHost();
  if (!render_view_host)
    return NULL;

  if (!settings_)
    settings_.reset(new Ewk_Settings(evas_object_, render_view_host->GetWebkitPreferences()));

  return settings_.get();
}

void EWebView::UpdateWebKitPreferences() {
  RenderViewHost* render_view_host = web_contents_delegate_->web_contents()->GetRenderViewHost();
  if (!render_view_host)
    return;

  render_view_host->UpdateWebkitPreferences(settings_->getPreferences());
}

void EWebView::SetContentSecurityPolicy(const char* policy, tizen_webview::ContentSecurityPolicyType type) {
  web_contents_delegate_->SetContentSecurityPolicy((policy ? policy : std::string()), type);
}

void EWebView::LoadHTMLString(const char* html, const char* base_uri, const char* unreachable_uri) {
  LoadData(html, std::string::npos, NULL, NULL, base_uri, unreachable_uri);
}

void EWebView::LoadPlainTextString(const char* plain_text) {
  LoadData(plain_text, std::string::npos, "text/plain", NULL, NULL, NULL);
}

void EWebView::LoadData(const char* data, size_t size, const char* mime_type, const char* encoding, const char* base_uri, const char* unreachable_uri)
{
  SetDefaultStringIfNull(mime_type, "text/html");
  SetDefaultStringIfNull(encoding, "utf-8");
  SetDefaultStringIfNull(base_uri, "about:blank");  // Webkit2 compatible
  SetDefaultStringIfNull(unreachable_uri, "");

  std::string str_data = data;

  if (size < str_data.length())
    str_data = str_data.substr(0, size);

  std::string url_str("data:");
  url_str.append(mime_type);
  url_str.append(";charset=");
  url_str.append(encoding);
  url_str.append(",");
  url_str.append(str_data);

  NavigationController::LoadURLParams data_params(GURL(url_str.c_str()));

  data_params.base_url_for_data_url = GURL(base_uri);
  data_params.virtual_url_for_data_url = GURL(unreachable_uri);

  data_params.load_type = NavigationController::LOAD_TYPE_DATA;
  data_params.should_replace_current_entry = false;
  WebContents* web_contents = web_contents_delegate_->web_contents();
  web_contents->GetController().LoadURLWithParams(data_params);
}

void EWebView::ShowPopupMenu(const gfx::Rect& rect, blink::TextDirection textDirection, double pageScaleFactor, const std::vector<content::MenuItem>& items, int data, int selectedIndex, bool multiple) {
#if defined(OS_TIZEN)
  Eina_List* popupItems = 0;
  const size_t size = items.size();
  for (size_t i = 0; i < size; ++i) {
    popupItems = eina_list_append(popupItems, Popup_Menu_Item::create(blink::WebPopupItem(blink::WebPopupItem::Type(items[i].type), base::UTF16ToUTF8(items[i].label), blink::TextDirection(items[i].rtl), items[i].has_directional_override, base::UTF16ToUTF8(items[i].tool_tip), base::UTF16ToUTF8(items[i].label), items[i].enabled, true, items[i].checked)).leakPtr());
  }
  popupMenuItems_ = popupItems;

  // DJKim : FIXME
#if 0 //ENABLE(TIZEN_WEBKIT2_FORM_NAVIGATION)
  if (FormIsNavigating()) {
#if 0 //ENABLE(TIZEN_MULTIPLE_SELECT
    popupPicker_->multiSelect = true;
#endif
    PopupMenuUpdate(popupMenuItems_, 0);
    SetFormIsNavigating(false);
    return;
  }
#endif
  if (popupPicker_)
    popup_picker_del(popupPicker_);
  popupPicker_ = 0;

  if (multiple)
    popupPicker_ = popup_picker_new(this, evas_object(), popupMenuItems_, 0, multiple);
  else
    popupPicker_ = popup_picker_new(this, evas_object(), popupMenuItems_, selectedIndex, multiple);
  // DJKim : FIXME
  //popup_picker_buttons_update(popupPicker_, formNavigation.position, formNavigation.count, false);
#endif
}

Eina_Bool EWebView::HidePopupMenu() {
#if defined(OS_TIZEN)
  if (!popupPicker_)
    return false;

  // DJKim : FIXME
  //if (impl->pageProxy->formIsNavigating())
  //if (FormIsNavigating())
    //return true;

  popup_picker_del(popupPicker_);
  popupPicker_ = 0;
#endif
  return true;
}

Eina_Bool EWebView::PopupMenuClose() {
#if defined(OS_TIZEN)
// DJKim : FIXME
#if 0
  if (!impl->popupMenuProxy)
    return false;

  impl->popupMenuProxy = 0;
#endif
  HidePopupMenu();

// DJKim : FIXME
#if 1//ENABLE(TIZEN_WEBKIT2_POPUP_INTERNAL)
  //ewk_view_touch_events_enabled_set(ewkView, true);
  //releasePopupMenuList(popupMenuItems_);
  if (!popupMenuItems_)
    return false;

  void* item;
  EINA_LIST_FREE(popupMenuItems_, item)
  delete static_cast<Popup_Menu_Item*>(item);
  popupMenuItems_ = 0;
#else
  void* item;
  EINA_LIST_FREE(popupMenuItems_, item)
  delete static_cast<Popup_Menu_Item*>(item);
#endif

  RenderViewHostImpl* render_view_host = static_cast<RenderViewHostImpl*>(web_contents_delegate_->web_contents()->GetRenderViewHost());
  if (!render_view_host)
    return false;

#if !defined(EWK_BRINGUP)
  render_view_host->PopupMenuClose();
#endif
#endif
  return true;
}

void EWebView::SetFormIsNavigating(bool formIsNavigating) {
#if defined(OS_TIZEN)
  formIsNavigating_ = formIsNavigating;
#endif
}

Eina_Bool EWebView::PopupMenuUpdate(Eina_List* items, int selectedIndex) {
#if defined(OS_TIZEN)
  if (popupPicker_)
    return false;

  popup_picker_update(evas_object(), popupPicker_, items, selectedIndex);
  // DJKim : FIXME
  //popup_picker_buttons_update(popupPicker_, formIsNavigating_.position, formIsNavigating_.count, false);
#endif
  return true;
}

Eina_Bool EWebView::DidSelectPopupMenuItem(int selectedindex) {
#if defined(OS_TIZEN)
  RenderViewHostImpl* render_view_host = static_cast<RenderViewHostImpl*>(web_contents_delegate_->web_contents()->GetRenderViewHost());
  if (!render_view_host)
    return false;

  if (!popupMenuItems_)
    return false;

  //When user select empty space then no index is selected, so selectedIndex value is -1
  //In that case we should call valueChanged() with -1 index.That in turn call popupDidHide()
  //in didChangeSelectedIndex() for reseting the value of m_popupIsVisible in RenderMenuList.
  if (selectedindex != -1 && selectedindex >= (int)eina_list_count(popupMenuItems_))
    return false;

#if !defined(EWK_BRINGUP)
  render_view_host->DidSelectPopupMenuItem(selectedindex);
#endif
#endif
  return true;
}

Eina_Bool EWebView::DidMultipleSelectPopupMenuItem(std::vector<int>& selectedindex) {
#if defined(OS_TIZEN)
  RenderViewHostImpl* render_view_host = static_cast<RenderViewHostImpl*>(web_contents_delegate_->web_contents()->GetRenderViewHost());
  if (!render_view_host)
    return false;

  if (!popupMenuItems_)
    return false;

#if !defined(EWK_BRINGUP)
  render_view_host->DidMultipleSelectPopupMenuItem(selectedindex);
#endif
#endif
  return true;
}

void EWebView::ShowContextMenu(const content::ContextMenuParams& params, content::ContextMenuType type) {
  // fix for context menu coordinates type: MENU_TYPE_LINK (introduced by CBGRAPHICS-235),
  // this menu is created in renderer process and it does not now anything about
  // view scaling factor and it has another calling sequence, so coordinates is not updated
  content::ContextMenuParams convertedParams = params;
  if (type == MENU_TYPE_LINK) {
    gfx::Point convertedPoint = rwhv()->ConvertPointInViewPix(gfx::Point(params.x, params.y));
    convertedParams.x = convertedPoint.x();
    convertedParams.y = convertedPoint.y();
  }

  context_menu_.reset(new content::ContextMenuControllerEfl(GetPublicWebView(), type, web_contents_delegate()));

  if(!selection_controller_->IsShowingMagnifier()) {
    if(!context_menu_->PopulateAndShowContextMenu(convertedParams))
      context_menu_.reset();
  }
}

void EWebView::CancelContextMenu(int request_id) {
  context_menu_.reset();
}

void EWebView::Find(const char* text, tizen_webview::Find_Options find_options) {
  base::string16 find_text = base::UTF8ToUTF16(text);
  bool find_next = (previous_text_ == find_text);

  if (!find_next) {
    current_find_request_id_ = find_request_id_counter_++;
    previous_text_ = find_text;
  }

  blink::WebFindOptions web_find_options;
  web_find_options.forward = !(find_options & TW_FIND_OPTIONS_BACKWARDS);
  web_find_options.matchCase = !(find_options & TW_FIND_OPTIONS_CASE_INSENSITIVE);
  web_find_options.findNext = find_next;

  web_contents_delegate_->web_contents()->Find(current_find_request_id_, find_text, web_find_options);
}

void EWebView::SetScale(double scale_factor, int x, int y) {
  RenderViewHost* render_view_host = web_contents_delegate_->web_contents()->GetRenderViewHost();
  render_view_host->Send(new EwkViewMsg_Scale(render_view_host->GetRoutingID(), scale_factor, x, y));
}

void EWebView::GetScrollPosition(int* x, int* y) const {
  DCHECK(x);
  DCHECK(y);
  const gfx::Vector2dF scroll_position = rwhv()->GetLastScrollOffset();
  *x = scroll_position.x();
  *y = scroll_position.y();
}

void EWebView::SetScroll(int x, int y) {
  RenderViewHost* render_view_host = web_contents_delegate()->web_contents()->GetRenderViewHost();
  if (!render_view_host)
    return;

  render_view_host->Send(new EwkViewMsg_SetScroll(render_view_host->GetRoutingID(), x, y));
}

void EWebView::UseSettingsFont() {
  RenderViewHost* render_view_host = web_contents_delegate()->web_contents()->GetRenderViewHost();
  if (render_view_host)
    render_view_host->Send(new EwkViewMsg_UseSettingsFont(render_view_host->GetRoutingID()));
}

void EWebView::DidChangeContentsArea(int width, int height) {
  contents_area_ = gfx::Size(width, height);
}

void EWebView::DidChangeContentsSize(int width, int height) {
  contents_size_ = gfx::Size(width, height);
}

const Eina_Rectangle EWebView::GetContentsSize() const {
  Eina_Rectangle rect;
  EINA_RECTANGLE_SET(&rect, 0, 0, contents_size_.width(), contents_size_.height());
  return rect;
}

void EWebView::GetScrollSize(int* width, int* height) {
  if (width)
    *width = 0;
  if (height)
    *height = 0;

  Eina_Rectangle last_view_port =
      WebViewDelegate::GetInstance()->GetLastUsedViewPortArea(evas_object());

#ifdef OS_TIZEN
  if (width && contents_area_.width() > last_view_port.w)
    *width = contents_area_.width() - last_view_port.w;
  if (height && contents_area_.height() > last_view_port.h)
    *height = contents_area_.height() - last_view_port.h;
#else
  if (width && contents_size_.width() > last_view_port.w)
    *width = contents_size_.width() - last_view_port.w;
  if (height && contents_size_.height() > last_view_port.h)
    *height = contents_size_.height() - last_view_port.h;
#endif
}

void EWebView::SelectRange(const gfx::Point& start, const gfx::Point& end) {
   rwhv()->SelectRange(start, end);
}

void EWebView::MoveCaret(const gfx::Point& point) {
   rwhv()->MoveCaret(point);
}

void EWebView::QuerySelectionStyle() {
  Ewk_Settings* settings = GetSettings();
  if (settings->textStyleStateState()) {
    RenderViewHost* render_view_host = web_contents_delegate_->web_contents()->GetRenderViewHost();
    render_view_host->Send(new EwkViewMsg_GetSelectionStyle(render_view_host->GetRoutingID()));
  }
}

void EWebView::OnQuerySelectionStyleReply(const SelectionStylePrams& params) {
  gfx::Rect left_rect, right_rect;
  selection_controller_->GetSelectionBounds(&left_rect, &right_rect);
  _Ewk_Text_Style style_data(params, left_rect.origin(), right_rect.bottom_right());
  SmartCallback<EWebViewCallbacks::TextStyleState>().call(&style_data);
}

void EWebView::SelectClosestWord(const gfx::Point& touch_point) {
  float device_scale_factor = rwhv()->device_scale_factor();
  RenderViewHost* render_view_host = web_contents_delegate_->web_contents()->GetRenderViewHost();
  render_view_host->Send(new EwkViewMsg_SelectClosestWord(render_view_host->GetRoutingID(), touch_point.x() / device_scale_factor, touch_point.y() / device_scale_factor));
}

void EWebView::SelectLinkText(const gfx::Point& touch_point) {
  float device_scale_factor = rwhv()->device_scale_factor();
  RenderViewHost* render_view_host = web_contents_delegate_->web_contents()->GetRenderViewHost();
  render_view_host->Send(new ViewMsg_SelectLinkText(render_view_host->GetRoutingID(), gfx::Point(touch_point.x() / device_scale_factor, touch_point.y() / device_scale_factor)));
}

bool EWebView::GetSelectionRange(Eina_Rectangle* left_rect, Eina_Rectangle* right_rect) {
  if (left_rect && right_rect) {
    gfx::Rect left, right;
    selection_controller_->GetSelectionBounds(&left, &right);
    GetEinaRectFromGfxRect(left, left_rect);
    GetEinaRectFromGfxRect(right, right_rect);
    return true;
  }
  return false;
}

// TODO(sns.park) : better to move this method to SelectionController
bool EWebView::ClearSelection()
{
    LOG(INFO) << "EWebView::ClearSelection";
    bool retval = false;
    if (selection_controller_->GetSelectionStatus()) {
      selection_controller_->ClearSelection();
      retval = true;
    }
    selection_controller_->SetSelectionEditable(false);
    selection_controller_->SetCaretSelectionStatus(false);

    ExecuteEditCommand("Unselect", NULL);
    return retval;
}

tizen_webview::Hit_Test* EWebView::RequestHitTestDataAt(int x, int y, tizen_webview::Hit_Test_Mode mode) {
  // TODO: this calculations should be moved outside and reused everywhere it's required
  Evas_Coord tmpX, tmpY;
  evas_object_geometry_get(evas_object_, &tmpX, &tmpY, NULL, NULL);

  x -= tmpX;
  y -= tmpY;
  x /= rwhv()->device_scale_factor();
  y /= rwhv()->device_scale_factor();

  return RequestHitTestDataAtBlinkCoords(x, y, mode);
}

tizen_webview::Hit_Test* EWebView::RequestHitTestDataAtBlinkCoords(int x, int y, tizen_webview::Hit_Test_Mode mode) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));

  // WebViewBrowserMessageFilter requires RenderProcessHost to be already created.
  // In EWebView constructor we have no guarantee that related RenderProcessHost is already created
  // We do not destroy message_filter_ manualy as it is managed by RenderProcessHost after setting it as filter
  // TODO: this pointer should be set to NULL when WebProcess crash/quits
  if (!message_filter_) {
    message_filter_ = new WebViewBrowserMessageFilter(this);
  }

  RenderViewHost* render_view_host = web_contents_delegate()->web_contents()->GetRenderViewHost();
  content::RenderProcessHost* render_process_host = web_contents_delegate()->web_contents()->GetRenderProcessHost();
  DCHECK(render_view_host);
  DCHECK(render_process_host);

  if (render_view_host && render_process_host) {
    // We wait on UI thread till hit test data is updated.
#if !defined(EWK_BRINGUP)
    base::ThreadRestrictions::ScopedAllowWait allow_wait;
#endif
    render_view_host->Send(new EwkViewMsg_DoHitTest(render_view_host->GetRoutingID(), x, y, mode));
    hit_test_completion_.Wait();
    return new tizen_webview::Hit_Test(hit_test_data_);
  }

  return NULL;
}

void EWebView::UpdateHitTestData(const _Ewk_Hit_Test& hit_test_data, const NodeAttributesMap& node_attributes) {
  DCHECK(content::BrowserThread::CurrentlyOn(content::BrowserThread::IO));
  hit_test_data_ = hit_test_data;
  hit_test_data_.nodeData.PopulateNodeAtributes(node_attributes);
  hit_test_completion_.Signal();
}

void EWebView::OnCopyFromBackingStore(bool success, const SkBitmap& bitmap) {
  if (selection_controller_->GetSelectionStatus() ||
      selection_controller_->GetCaretSelectionStatus())
    selection_controller_->UpdateMagnifierScreen(bitmap);
}

void EWebView::UpdateMagnifierScreen(const SkBitmap& bitmap) {
  selection_controller_->UpdateMagnifierScreen(bitmap);
}

void EWebView::SetOverrideEncoding(const std::string& encoding) {
  web_contents_delegate_->web_contents()->SetOverrideEncoding(encoding);
}

bool EWebView::GetLinkMagnifierEnabled() const {
  WebContents* webContents = web_contents_delegate()->web_contents();
  return webContents->GetMutableRendererPrefs()->tap_multiple_targets_strategy == TAP_MULTIPLE_TARGETS_STRATEGY_POPUP;
}

void EWebView::SetLinkMagnifierEnabled(bool enabled) {
  WebContents* webContents = web_contents_delegate()->web_contents();

  webContents->GetMutableRendererPrefs()->tap_multiple_targets_strategy =
      enabled ? TAP_MULTIPLE_TARGETS_STRATEGY_POPUP
              : TAP_MULTIPLE_TARGETS_STRATEGY_NONE;
  webContents->GetRenderViewHost()->SyncRendererPrefs();
}

void EWebView::GetSnapShotForRect(gfx::Rect& rect) {
#ifdef OS_TIZEN
  rwhv()->GetSnapshotForRect(rect);
#endif
}

Evas_Object* EWebView::GetSnapshot(Eina_Rectangle rect) {
  Evas_Object* image = NULL;
#ifdef OS_TIZEN
  int width = rect.w;
  int height = rect.h;

  if (width > rwhv()->GetViewBoundsInPix().width() - rect.x)
    width = rwhv()->GetViewBoundsInPix().width() - rect.x;
  if (height > rwhv()->GetViewBoundsInPix().height() - rect.y)
    height = rwhv()->GetViewBoundsInPix().height() - rect.y;

  int x = rect.x;
  int y = rwhv()->GetViewBoundsInPix().height() - height + rect.y;

  Evas_GL_API* gl_api = rwhv()->evasGlApi();
  DCHECK(gl_api);
  int size = width * height;

  GLubyte tmp[size*4];
  GLubyte bits[size*4];
  gl_api->glReadPixels(x, y, width, height, GL_BGRA, GL_UNSIGNED_BYTE, bits);

  //flip data after reading
  for (int i=0; i < height; i++)
    memcpy(&tmp[i*width*4], &bits[(height-i-1)*width*4], width*4*sizeof(unsigned char));
  image = evas_object_image_filled_add(rwhv()->evas());
  if (image) {
    evas_object_image_size_set(image, width, height);
    evas_object_image_alpha_set(image, EINA_TRUE);
    evas_object_image_data_copy_set(image, tmp);
    evas_object_resize(image, width, height);
  }
#endif
  return image;
}

void EWebView::BackForwardListClear() {
  content::NavigationController& controller =
      web_contents_delegate()->web_contents()->GetController();

  int entry_count = controller.GetEntryCount();
  bool entry_removed = false;

  for (int i = 0; i < entry_count; i++) {
    if (controller.RemoveEntryAtIndex(i)) {
      entry_removed = true;
      entry_count = controller.GetEntryCount();
      i--;
    }
  }

  if (entry_removed)
    InvokeBackForwardListChangedCallback();
}

void EWebView::InvokeBackForwardListChangedCallback() {
  SmartCallback<EWebViewCallbacks::BackForwardListChange>().call();
}

bool EWebView::WebAppCapableGet(tizen_webview::Web_App_Capable_Get_Callback callback, void *userData) {
  RenderViewHost *renderViewHost = web_contents_delegate()->web_contents()->GetRenderViewHost();
  if (!renderViewHost) {
    return false;
  }
  WebApplicationCapableGetCallback *cb = new WebApplicationCapableGetCallback(callback, userData);
  int callbackId = web_app_capable_get_callback_map_.Add(cb);
  return renderViewHost->Send(new EwkViewMsg_WebAppCapableGet(renderViewHost->GetRoutingID(), callbackId));
}

bool EWebView::WebAppIconUrlGet(tizen_webview::Web_App_Icon_URL_Get_Callback callback, void *userData) {
  RenderViewHost* renderViewHost = web_contents_delegate()->web_contents()->GetRenderViewHost();
  if (!renderViewHost) {
    return false;
  }
  WebApplicationIconUrlGetCallback *cb = new WebApplicationIconUrlGetCallback(callback, userData);
  int callbackId = web_app_icon_url_get_callback_map_.Add(cb);
  return renderViewHost->Send(new EwkViewMsg_WebAppIconUrlGet(renderViewHost->GetRoutingID(), callbackId));
}

bool EWebView::WebAppIconUrlsGet(tizen_webview::Web_App_Icon_URLs_Get_Callback callback, void *userData) {
  RenderViewHost* renderViewHost = web_contents_delegate()->web_contents()->GetRenderViewHost();
  if (!renderViewHost) {
    return false;
  }
  WebApplicationIconUrlsGetCallback *cb = new WebApplicationIconUrlsGetCallback(callback, userData);
  int callbackId = web_app_icon_urls_get_callback_map_.Add(cb);
  return renderViewHost->Send(new EwkViewMsg_WebAppIconUrlsGet(renderViewHost->GetRoutingID(), callbackId));
}

void EWebView::InvokeWebAppCapableGetCallback(bool capable, int callbackId) {
  WebApplicationCapableGetCallback *callback = web_app_capable_get_callback_map_.Lookup(callbackId);
  if (!callback)
    return;
  callback->Run(capable);
}

void EWebView::InvokeWebAppIconUrlGetCallback(const std::string& iconUrl, int callbackId) {
  WebApplicationIconUrlGetCallback *callback = web_app_icon_url_get_callback_map_.Lookup(callbackId);
  if (!callback)
    return;
  callback->Run(iconUrl);
}

void EWebView::InvokeWebAppIconUrlsGetCallback(const StringMap &iconUrls, int callbackId) {
  WebApplicationIconUrlsGetCallback *callback = web_app_icon_urls_get_callback_map_.Lookup(callbackId);
  if (!callback) {
    return;
  }
  callback->Run(iconUrls);
}

void EwkViewPlainTextGetCallback::TriggerCallback(Evas_Object* obj, const std::string& content_text)
{
  if(callback_)
    (callback_)(obj, content_text.c_str(), user_data_);
}

int EWebView::SetEwkViewPlainTextGetCallback(tizen_webview::View_Plain_Text_Get_Callback callback, void* user_data) {
  EwkViewPlainTextGetCallback* view_plain_text_callback_ptr = new EwkViewPlainTextGetCallback(callback, user_data);
  return plain_text_get_callback_map_.Add(view_plain_text_callback_ptr);
}

bool EWebView::PlainTextGet(tizen_webview::View_Plain_Text_Get_Callback callback, void* user_data) {
  RenderViewHost* render_view_host = web_contents_delegate()->web_contents()->GetRenderViewHost();
  if (!render_view_host)
    return false;
  int plain_text_get_callback_id = SetEwkViewPlainTextGetCallback(callback, user_data);
  return render_view_host->Send(new EwkViewMsg_PlainTextGet(render_view_host->GetRoutingID(), plain_text_get_callback_id));
}

void EWebView::InvokePlainTextGetCallback(const std::string& content_text, int plain_text_get_callback_id) {
  EwkViewPlainTextGetCallback* view_plain_text_callback_invoke_ptr = plain_text_get_callback_map_.Lookup(plain_text_get_callback_id);
  view_plain_text_callback_invoke_ptr->TriggerCallback(evas_object(), content_text);
  plain_text_get_callback_map_.Remove(plain_text_get_callback_id);
}

void EWebView::StopFinding() {
  web_contents_delegate_->web_contents()->StopFinding(content::STOP_FIND_ACTION_CLEAR_SELECTION);
}

void EWebView::SetProgressValue(double progress) {
  progress_ = progress;
}

double EWebView::GetProgressValue() {
  return progress_;
}

const char* EWebView::GetTitle() {
  title_ = UTF16ToUTF8(web_contents_delegate_->web_contents()->GetTitle());
  return title_.c_str();
}

bool EWebView::SaveAsPdf(int width, int height, const std::string& filename) {
  RenderViewHost* render_view_host = web_contents_delegate()->web_contents()->GetRenderViewHost();
  if (!render_view_host)
    return false;

  return render_view_host->Send(new EwkViewMsg_PrintToPdf(render_view_host->GetRoutingID(),
      width, height, base::FilePath(filename)));
}

bool EWebView::GetMHTMLData(tizen_webview::View_MHTML_Data_Get_Callback callback, void* user_data) {
  RenderViewHost* render_view_host = web_contents_delegate()->web_contents()->GetRenderViewHost();
  if (!render_view_host)
    return false;

  MHTMLCallbackDetails* callback_details = new MHTMLCallbackDetails(callback, user_data);
  int mhtml_callback_id = mhtml_callback_map_.Add(callback_details);
  return render_view_host->Send(new EwkViewMsg_GetMHTMLData(render_view_host->GetRoutingID(), mhtml_callback_id));
}

void EWebView::OnMHTMLContentGet(const std::string& mhtml_content, int callback_id) {
  MHTMLCallbackDetails* callback_details = mhtml_callback_map_.Lookup(callback_id);
  callback_details->Run(evas_object(), mhtml_content);
  mhtml_callback_map_.Remove(callback_id);
}

void MHTMLCallbackDetails::Run(Evas_Object* obj, const std::string& mhtml_content) {
  if (callback_func_)
    callback_func_(obj, mhtml_content.c_str(), user_data_);
}

bool EWebView::IsFullscreen() {
  WebContents* web_contents = web_contents_delegate_->web_contents();
  return web_contents_delegate_->IsFullscreenForTabOrPending(web_contents);
}

void EWebView::ExitFullscreen() {
  RenderViewHost* rvh = web_contents_delegate_->web_contents()->GetRenderViewHost();
  if (!rvh)
    return;

  rvh->ExitFullscreen();
}

double EWebView::GetScale() {
  return page_scale_factor_;
}

void EWebView::DidChangePageScaleFactor(double scale_factor) {
  page_scale_factor_ = scale_factor;
}

inline JavaScriptDialogManagerEfl* EWebView::GetJavaScriptDialogManagerEfl() {
  return static_cast<JavaScriptDialogManagerEfl*>(web_contents_delegate()->GetJavaScriptDialogManager());
}

void EWebView::SetJavaScriptAlertCallback(tizen_webview::View_JavaScript_Alert_Callback callback, void* user_data) {
  GetJavaScriptDialogManagerEfl()->SetAlertCallback(callback, user_data);
}

void EWebView::JavaScriptAlertReply() {
  GetJavaScriptDialogManagerEfl()->ExecuteDialogClosedCallBack(true, std::string());
  SmartCallback<EWebViewCallbacks::PopupReplyWaitFinish>().call(0);
}

void EWebView::SetJavaScriptConfirmCallback(tizen_webview::View_JavaScript_Confirm_Callback callback, void* user_data) {
  GetJavaScriptDialogManagerEfl()->SetConfirmCallback(callback, user_data);
}

void EWebView::JavaScriptConfirmReply(bool result) {
  GetJavaScriptDialogManagerEfl()->ExecuteDialogClosedCallBack(result, std::string());
  SmartCallback<EWebViewCallbacks::PopupReplyWaitFinish>().call(0);
}

void EWebView::SetJavaScriptPromptCallback(tizen_webview::View_JavaScript_Prompt_Callback callback, void* user_data) {
  GetJavaScriptDialogManagerEfl()->SetPromptCallback(callback, user_data);
}

void EWebView::JavaScriptPromptReply(const char* result) {
  GetJavaScriptDialogManagerEfl()->ExecuteDialogClosedCallBack(true, (std::string(result)));
  SmartCallback<EWebViewCallbacks::PopupReplyWaitFinish>().call(0);
}

void EWebView::GetPageScaleRange(double *min_scale, double *max_scale) {
  if (min_scale)
    *min_scale = min_page_scale_factor_;
  if (max_scale)
    *max_scale = max_page_scale_factor_;
}

void EWebView::DidChangePageScaleRange(double min_scale, double max_scale) {
  min_page_scale_factor_ = min_scale;
  max_page_scale_factor_ = max_scale;
}

void EWebView::SetDrawsTransparentBackground(bool enabled) {
  RenderViewHost* render_view_host = web_contents_delegate()->web_contents()->GetRenderViewHost();
  if (!render_view_host)
    return;

  render_view_host->Send(new EwkViewMsg_SetDrawsTransparentBackground(render_view_host->GetRoutingID(), enabled));
}

void EWebView::GetSessionData(const char **data, unsigned *length) const {
  static const int MAX_SESSION_ENTRY_SIZE = std::numeric_limits<int>::max();

  WebContents* contents = web_contents_delegate_->web_contents();
  NavigationController &navigationController = contents->GetController();
  Pickle sessionPickle;
  const int itemCount = navigationController.GetEntryCount();

  sessionPickle.WriteInt(itemCount);
  sessionPickle.WriteInt(navigationController.GetCurrentEntryIndex());

  for (int i = 0; i < itemCount; i++) {
    NavigationEntry *navigationEntry = navigationController.GetEntryAtIndex(i);
    sessions::SerializedNavigationEntry serializedEntry =
      sessions::SerializedNavigationEntry::FromNavigationEntry(i, *navigationEntry);
    serializedEntry.WriteToPickle(MAX_SESSION_ENTRY_SIZE, &sessionPickle);
  }

  *data = static_cast<char *>(malloc(sizeof(char) * sessionPickle.size()));
  memcpy(const_cast<char *>(*data), sessionPickle.data(), sessionPickle.size());
  *length = sessionPickle.size();
}

bool EWebView::RestoreFromSessionData(const char *data, unsigned length) {
  Pickle sessionPickle(data, length);
  PickleIterator pickleIterator(sessionPickle);
  int entryCount;
  int currentEntry;

  if (!pickleIterator.ReadInt(&entryCount))
    return false;
  if (!pickleIterator.ReadInt(&currentEntry))
    return false;

  std::vector<sessions::SerializedNavigationEntry> serializedEntries;
  serializedEntries.resize(entryCount);
  for (int i = 0; i < entryCount; ++i) {
    if (!serializedEntries.at(i).ReadFromPickle(&pickleIterator))
      return false;
  }

  if (!entryCount)
    return true;

  std::vector<NavigationEntry *> navigationEntries =
    sessions::SerializedNavigationEntry::ToNavigationEntries(serializedEntries, context()->browser_context());
  WebContents* contents = web_contents_delegate_->web_contents();
  NavigationController &navigationController = contents->GetController();

  if (currentEntry < 0)
    currentEntry = 0;

  if (currentEntry >= static_cast<int>(navigationEntries.size()))
    currentEntry = navigationEntries.size() - 1;

  navigationController.Restore(currentEntry,
                               NavigationController::RESTORE_LAST_SESSION_EXITED_CLEANLY,
                               &navigationEntries);
  return true;
}

void EWebView::SetBrowserFont() {
  RenderViewHost* render_view_host = web_contents_delegate()->web_contents()->GetRenderViewHost();
  if (render_view_host)
    render_view_host->Send(new EwkViewMsg_SetBrowserFont(render_view_host->GetRoutingID()));
}

void EWebView::ShowFileChooser(const content::FileChooserParams& params) {
  RenderViewHost* render_view_host = web_contents_delegate()->web_contents()->GetRenderViewHost();
  if (!render_view_host)
    return;
#if defined(OS_TIZEN_MOBILE)
#if !defined(EWK_BRINGUP)
  if (params.capture) {
    const std::string capture_types[] = {"video/*", "audio/*", "image/*"};
    unsigned int capture_types_num = sizeof(capture_types)/sizeof(*capture_types);
    for (unsigned int i = 0; i < capture_types_num; ++i) {
      for (unsigned int j = 0; j < params.accept_types.size(); ++j) {
        if (UTF16ToUTF8(params.accept_types[j]) == capture_types[i]) {
          filechooser_mode_ = params.mode;
          LaunchCamera(params.accept_types[j]);
          return;
        }
      }
    }
  }
#endif
#endif
  file_chooser_.reset(new content::FileChooserControllerEfl(render_view_host, &params));
  file_chooser_->open();
}

#ifdef TIZEN_CONTENTS_DETECTION
void EWebView::ShowContentsDetectedPopup(const char* message) {
  popup_controller_.reset(new PopupControllerEfl(this));
  popup_controller_->openPopup(message);
}
#endif

void EWebView::RequestColorPicker(int r, int g, int b, int a) {
  inputPicker_.reset(new InputPicker(web_contents_delegate()->web_contents()));
  inputPicker_->showColorPicker(r, g, b, a);
}

void EWebView::DismissColorPicker() {
  inputPicker_->hideColorPicker();
}

bool EWebView::SetColorPickerColor(int r, int g, int b, int a) {
  web_contents_delegate()->web_contents()->DidChooseColorInColorChooser(SkColorSetARGB(a, r, g, b));
  return true;
}

void EWebView::InputPickerShow(tizen_webview::Input_Type inputType, const char* inputValue) {
  inputPicker_.reset(new InputPicker(web_contents_delegate()->web_contents()));
  inputPicker_->show(inputType, inputValue);
}

bool EWebView::IsIMEShow() {
  if (rwhv()->im_context())
    return rwhv()->im_context()->IsShow();

  return false;
}

gfx::Rect EWebView::GetIMERect() {
  if (rwhv()->im_context())
    return rwhv()->im_context()->GetIMERect();

  return gfx::Rect();
}

std::string EWebView::GetErrorPage(const std::string& invalidUrl) {
  base::string16 url16;
  url16.assign(invalidUrl.begin(), invalidUrl.end());


#if defined(EWK_BRINGUP)
  std::string errorHead = "This webpage is not available";
  std::string errorMessage = "The server at <ph name=\"" + invalidUrl + "\">&lt;strong&gt;$1&lt;/strong&gt;</ph> can't be found, because the DNS lookup failed.";
#else
  // Those strings were removed form webkit_strings.grd
  // They exist in chrome/app/generated_resources.grd, but it is part of chrome that we don't use currently.
  std::string errorHead = l10n_util::GetStringUTF8(IDS_ERRORPAGES_HEADING_NOT_AVAILABLE);
  std::string errorMessage = l10n_util::GetStringFUTF8(IDS_ERRORPAGES_SUMMARY_NAME_NOT_RESOLVED, url16);
#endif

  std::string html =
    "<html>"
      "<head>"
        "<meta name='viewport' content='width=device-width, initial-scale=1.0, user-scalable=no'>"
        "<meta http-equiv='Content-Type' content='text/html; charset=UTF-8'>"
        "<title>";
  html += invalidUrl;
  html +=
        "</title>"
        "<style type=text/css>"
        "#body"
        "{"
        " background-color: #fff;"
        " margin: 0;"
        " padding: 0;"
        "}"
        "#Box"
        "{"
        " background: #fff;"
        " width: 80%%;"
        " min-width: 150px;"
        " max-width: 750px;"
        " margin: auto;"
        " padding: 5px;"
        " border: 1px solid #BFA3A3;"
        " border-radius: 1px;"
        " word-wrap:break-word"
        "}"
        "</style>"
      "</head>"
      "<body bgcolor=\"#CFCFCF\">"
      "<div id=Box>"
      "<h2 align=\"center\">";
    html += errorHead;
    html += "</h2></br>";
    html += errorMessage;
    html +=
      "</div>"
      "</body>"
    "</html>"
    ;
   return html;
}

std::string EWebView::GetPlatformLocale() {
  char* local_default = setlocale(LC_CTYPE, 0);
  if (!local_default)
    return std::string("en-US");
  std::string locale = std::string(local_default);
  locale.replace(locale.find('_'),1,"-");
  size_t position = locale.find('.');
  if (position != std::string::npos)
    locale = locale.substr(0,position);
  return locale;
}

int EWebView::StartInspectorServer(int port) {
  if (inspector_server_) {
    inspector_server_->Stop(); // Asynchronous releas inside Stop()
  }
  inspector_server_ = new content::DevToolsDelegateEfl(port);
  return inspector_server_ ? inspector_server_->port() : 0;
}

bool EWebView::StopInspectorServer() {
  if (!inspector_server_) {
    return false;
  }
  inspector_server_->Stop(); // Asynchronous releas inside Stop()
  inspector_server_ = NULL;
  return true;
}

#if defined(OS_TIZEN_MOBILE) && !defined(EWK_BRINGUP)
void EWebView::cameraResultCb(service_h request,
                              service_h reply,
                              service_result_e result,
		              void* data)
{
  EWebView* webview = static_cast<EWebView*>(data);
  if (result == SERVICE_RESULT_SUCCEEDED) {
    int ret = -1;
    char** filesarray;
    int number;
    ret =service_get_extra_data_array(reply, SERVICE_DATA_SELECTED,
      &filesarray,&number);
    if (filesarray) {
      for(int i =0; i< number;i++) {
        std::vector<ui::SelectedFileInfo> files;
        RenderViewHost* render_view_host = webview->web_contents_delegate()->
          web_contents()->GetRenderViewHost();
        if (!render_view_host) {
          return;
        }
        if (filesarray[i]) {
          GURL url(filesarray[i]);
          if (!url.is_valid()) {
            base::FilePath path(url.SchemeIsFile() ? url.path() :
              filesarray[i]);
            files.push_back(ui::SelectedFileInfo(path, base::FilePath()));
          }
        }
        render_view_host->FilesSelectedInChooser(files,
          webview->filechooser_mode_);
      }
    }
  } else {
    RenderViewHost* render_view_host = webview->web_contents_delegate()->
      web_contents()->GetRenderViewHost();
    std::vector<ui::SelectedFileInfo> files;
    if (render_view_host) {
      render_view_host->FilesSelectedInChooser(files,
        webview->filechooser_mode_);
    }
  }
}

bool EWebView::LaunchCamera(base::string16 mimetype)
{
  service_h svcHandle = 0;
  if (service_create(&svcHandle) < 0 || !svcHandle) {
    LOG(ERROR) << __FUNCTION__ << " Service Creation Failed ";
    return false;
  }
  service_set_operation(svcHandle, SERVICE_OPERATION_CREATE_CONTENT);
  service_set_mime(svcHandle, UTF16ToUTF8(mimetype).c_str());
  service_add_extra_data(svcHandle, "CALLER", "Browser");

  int ret = service_send_launch_request(svcHandle, cameraResultCb, this);
  if (ret != SERVICE_ERROR_NONE) {
    LOG(ERROR) << __FUNCTION__ << " Service Launch Failed ";
    service_destroy(svcHandle);
    return false;
  }
  service_destroy(svcHandle);
  return true;
}
#endif

void EWebView::UrlRequestSet(const char* url, std::string method, Eina_Hash* headers, const char* body) {
  net::URLRequestContext context;
  scoped_ptr<net::URLRequest> request(context.CreateRequest(
      GURL(url), net::DEFAULT_PRIORITY, NULL, NULL));
  request->set_method(method);

  if (headers) {
    net::HttpRequestHeaders* header;
    Eina_Iterator* it = eina_hash_iterator_tuple_new(headers);
    void* data;
    while (eina_iterator_next(it, &data)) {
      Eina_Hash_Tuple* t = static_cast<Eina_Hash_Tuple*>(data);
      const char* name = static_cast<const char*>(t->key);
      const char* value = static_cast<const char*>(t->data);
      header->SetHeader(base::StringPiece(name), base::StringPiece(value));
      request->SetExtraRequestHeaders(*header);
    }
    eina_iterator_free(it);
  }

  if (body) {
    std::string str = body;
    request->EnableChunkedUpload();
    request->AppendChunkToUpload(str.c_str(), str.length(), true);
  }
}

bool EWebView::HandleShow() {
  if (rwhv()) {
    rwhv()->HandleShow();
    return true;
  }
  return false;
}

bool EWebView::HandleHide() {
  if (rwhv()) {
    rwhv()->HandleHide();
    return true;
  }
  return false;
}

bool EWebView::HandleMove(int x, int y) {
  if (rwhv()) {
    rwhv()->HandleMove(x, y);
    return true;
  }
  return false;
}

bool EWebView::HandleResize(int width, int height) {
  if (rwhv()) {
    rwhv()->HandleResize(width, height);
    return true;
  }
  return false;
}

bool EWebView::HandleFocusIn() {
  if (rwhv()) {
    rwhv()->HandleFocusIn();
    return true;
  }
  return false;
}

bool EWebView::HandleFocusOut() {
  if (rwhv()) {
    rwhv()->HandleFocusOut();
    return true;
  }
  return false;
}

bool EWebView::HandleEvasEvent(const Evas_Event_Mouse_Down* event) {
  if (rwhv()) {
    rwhv()->HandleEvasEvent(event);
    return true;
  }
  return false;
}

bool EWebView::HandleEvasEvent(const Evas_Event_Mouse_Up* event) {
  if (rwhv()) {
    rwhv()->HandleEvasEvent(event);
    return true;
  }
  return false;
}

bool EWebView::HandleEvasEvent(const Evas_Event_Mouse_Move* event) {
  if (rwhv()) {
    rwhv()->HandleEvasEvent(event);
    return true;
  }
  return false;
}

bool EWebView::HandleEvasEvent(const Evas_Event_Mouse_Wheel* event) {
  if (rwhv()) {
    rwhv()->HandleEvasEvent(event);
    return true;
  }
  return false;
}

bool EWebView::HandleEvasEvent(const Evas_Event_Key_Down* event) {
  if (rwhv()) {
    rwhv()->HandleEvasEvent(event);
    return true;
  }
  return false;
}

bool EWebView::HandleEvasEvent(const Evas_Event_Key_Up* event) {
  if (rwhv()) {
    rwhv()->HandleEvasEvent(event);
    return true;
  }
  return false;
}

bool EWebView::HandleGesture(ui::GestureEvent* event) {
  if (rwhv()) {
    rwhv()->HandleGesture(event);
    return true;
  }
  return false;
}

bool EWebView::HandleTouchEvent(ui::TouchEvent* event) {
  if (rwhv()) {
    rwhv()->HandleTouchEvent(event);
    return true;
  }
  return false;
}
