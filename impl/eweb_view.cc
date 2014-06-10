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

#include "base/threading/thread_restrictions.h"
#include "browser/navigation_policy_handler_efl.h"
#include "browser/renderer_host/render_widget_host_view_efl.h"
#include "browser/renderer_host/web_event_factory_efl.h"
#include "common/render_messages_efl.h"
#include "common/version_info.h"
#include "API/ewk_policy_decision_private.h"
#include "API/ewk_settings_private.h"
#include "API/ewk_text_style_private.h"
#include "API/ewk_view_private.h"
#include "public/ewk_private.h"
#include "web_contents_delegate_efl.h"

#include "base/files/file_path.h"
#include "base/logging.h"
#include "base/strings/utf_string_conversions.h"
#include "content/common/view_messages.h"
#include "content/browser/renderer_host/ui_events_helper.h"
#include "content/browser/renderer_host/render_view_host_impl.h"
#include "content/browser/web_contents/web_contents_view.h"
#include "content/public/browser/navigation_controller.h"
#include "content/public/browser/navigation_entry.h"
#include "content/public/browser/resource_dispatcher_host.h"
#include "content/public/common/content_client.h"
#include "content/public/common/user_agent.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/host_zoom_map.h"
#include "skia/ext/platform_canvas.h"
#include "third_party/WebKit/public/web/WebFindOptions.h"
#include "ui/events/gestures/gesture_recognizer.h"
#include "browser/motion/wkext_motion.h"

#ifdef OS_TIZEN
#include <vconf.h>
#endif
#include <Ecore_Evas.h>
#include <Elementary.h>
#include <Eina.h>

using namespace content;

namespace {

#ifndef SET_EMPTY_STRING_IF_NULL
static const char* EMPTY_STRING = "";
#define SET_STRING_IF_NULL(variable, string) (variable) = ((variable) ? (variable) : (string))
#define SET_EMPTY_STRING_IF_NULL(variable) SET_STRING_IF_NULL(variable, EMPTY_STRING)
#endif

static const char SmartClassName[] = "EWebView";

inline EWebView* ToEWebView(const Ewk_View_Smart_Data* smartData) {
  DCHECK(smartData);
  DCHECK(smartData->priv);
  return smartData->priv;
}

inline Ewk_View_Smart_Data* ToSmartData(const Evas_Object* evas_object) {
  DCHECK(evas_object);
  DCHECK(IsEWebViewObject(evas_object));
  return static_cast<Ewk_View_Smart_Data*>(evas_object_smart_data_get(evas_object));
}

void SmartDataChanged(Ewk_View_Smart_Data* d) {
  DCHECK(d);
  if (d->changed.any)
    return;
  d->changed.any = true;
  evas_object_smart_changed(d->self);
}

template <Evas_Callback_Type EventType>
class EWebEventHandler {
 public:
  static void Subscribe(Evas_Object* evas_object) {
    evas_object_event_callback_add(evas_object, EventType, func_ptr(), ToSmartData(evas_object));
  }
  static void Unsubscribe(Evas_Object* evas_object) {
    evas_object_event_callback_del(evas_object, EventType, func_ptr());
  }
  static void HandleEvent(Ewk_View_Smart_Data*, Evas*, Evas_Object*, void* event_info) {}

 private:
  // Convenient automatic cast of the data pointer.
  typedef void (*HandleEventFuncPtr)(Ewk_View_Smart_Data*, Evas*, Evas_Object*, void* event_info);
  typedef void (*CallbackFuncPtr)(void*, Evas*, Evas_Object*, void* event_info);
  static CallbackFuncPtr func_ptr()
  { return reinterpret_cast<CallbackFuncPtr>(&HandleEvent); }
};

template <>
void EWebEventHandler<EVAS_CALLBACK_FOCUS_IN>::HandleEvent(Ewk_View_Smart_Data* d, Evas*, Evas_Object*, void*) {
  if (d->api->focus_in)
    d->api->focus_in(d);
}

template <>
void EWebEventHandler<EVAS_CALLBACK_FOCUS_OUT>::HandleEvent(Ewk_View_Smart_Data* d, Evas*, Evas_Object*, void*) {
  if (d->api->focus_out)
    d->api->focus_out(d);
}

#define DEFINE_EVENT_HANDLER(EVENT_TYPE_ID, EVENT_TYPE, handler_func_ptr) \
  template <> \
  void EWebEventHandler< EVENT_TYPE_ID >::HandleEvent(Ewk_View_Smart_Data* d, Evas* e, Evas_Object*, void* event) \
  { \
    if (d->api->handler_func_ptr) \
      d->api->handler_func_ptr(d, static_cast<EVENT_TYPE*>(event)); \
  } \
  typedef int ForceSemicolon

DEFINE_EVENT_HANDLER(EVAS_CALLBACK_MOUSE_DOWN, Evas_Event_Mouse_Down, mouse_down);
DEFINE_EVENT_HANDLER(EVAS_CALLBACK_MOUSE_UP, Evas_Event_Mouse_Up, mouse_up);
DEFINE_EVENT_HANDLER(EVAS_CALLBACK_MOUSE_WHEEL, Evas_Event_Mouse_Wheel, mouse_wheel);
DEFINE_EVENT_HANDLER(EVAS_CALLBACK_MOUSE_MOVE, Evas_Event_Mouse_Move, mouse_move);
DEFINE_EVENT_HANDLER(EVAS_CALLBACK_KEY_DOWN, Evas_Event_Key_Down, key_down);
DEFINE_EVENT_HANDLER(EVAS_CALLBACK_KEY_UP, Evas_Event_Key_Up, key_up);

} // namespace

Evas_Smart_Class EWebView::parent_smart_class_ = EVAS_SMART_CLASS_INIT_NULL;

WebContents* EWebView::contents_for_new_window_ = NULL;

int EWebView::find_request_id_counter_ = 0;

bool EWebView::InitSmartClassInterface(Ewk_View_Smart_Class& api) {
  if (api.version != EWK_VIEW_SMART_CLASS_VERSION) {
    EINA_LOG_CRIT("Ewk_View_Smart_Class %p is version %lu while %lu was expected.",
                  &api, api.version, EWK_VIEW_SMART_CLASS_VERSION);
    return false;
  }

  if (!parent_smart_class_.add)
    evas_object_smart_clipped_smart_set(&parent_smart_class_);

  evas_object_smart_clipped_smart_set(&api.sc);

  // Set Evas_Smart_Class callbacks.
  api.sc.add = &handleEvasObjectAdd;
  api.sc.del = &handleEvasObjectDelete;
  api.sc.move = &handleEvasObjectMove;
  api.sc.resize = &handleEvasObjectResize;
  api.sc.show = &handleEvasObjectShow;
  api.sc.hide = &handleEvasObjectHide;
  api.sc.calculate = &handleEvasObjectCalculate;
  api.sc.color_set = &handleEvasObjectColorSet;

  // Set Ewk_View_Smart_Class callbacks.
  api.focus_in = &handleFocusIn;
  api.focus_out = &handleFocusOut;
  api.mouse_down = &handleMouseDown;
  api.mouse_up = &handleMouseUp;
  api.mouse_wheel = &handleMouseWheel;
  api.mouse_move = &handleMouseMove;
  api.key_down = &handleKeyDown;
  api.key_up = &handleKeyUp;

  // Type identifier.
  api.sc.data = SmartClassName;

  return true;
}

static Evas_Smart* DefaultSmartClassInstance() {
  static Ewk_View_Smart_Class api = EWK_VIEW_SMART_CLASS_INIT_NAME_VERSION(SmartClassName);
  static Evas_Smart* smart = 0;
  if (!smart) {
    EWebView::InitSmartClassInterface(api);
    smart = evas_smart_class_new(&api.sc);
  }

  return smart;
}

static void GetEinaRectFromGfxRect(const gfx::Rect& gfx_rect, Eina_Rectangle* eina_rect) {
  eina_rect->x = gfx_rect.x();
  eina_rect->y = gfx_rect.y();
  eina_rect->w = gfx_rect.width();
  eina_rect->h = gfx_rect.height();
}

EWebView* EWebView::Create(EWebContext* context, Evas* canvas, Evas_Smart* smart) {
  EINA_SAFETY_ON_NULL_RETURN_VAL(canvas, 0);

  Evas_Object* evasObject = evas_object_smart_add(canvas, smart ? smart : DefaultSmartClassInstance());
  EINA_SAFETY_ON_NULL_RETURN_VAL(evasObject, 0);

  Ewk_View_Smart_Data* smartData = ToSmartData(evasObject);
  if (!smartData) {
    evas_object_del(evasObject);
    return 0;
  }

  DCHECK(!smartData->priv);
  smartData->priv = new EWebView(context, evasObject);
  return smartData->priv;
}

Evas_Object* EWebView::GetContentImageObject() const {
  Ewk_View_Smart_Data* smart_data = ToSmartData(evas_object_);
  DCHECK(smart_data);
  return smart_data->image;
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

EWebView::EWebView(EWebContext* context, Evas_Object* object)
    : context_(context),
      evas_object_(object),
      orientation_(EWK_SCREEN_ORIENTATION_NATURAL),
      touch_events_enabled_(false),
      mouse_events_enabled_(false),
      gesture_recognizer_(ui::GestureRecognizer::Create()),
      text_zoom_factor_(1.0),
      selection_controller_(new content::SelectionControllerEfl(this)),
      current_find_request_id_(find_request_id_counter_++),
      progress_(0.0),
      hit_test_completion_(false, false),
      page_scale_factor_(1.0),
      min_page_scale_factor_(-1.0),
      max_page_scale_factor_(-1.0)
#ifndef NDEBUG
      ,renderer_crashed_(false)
#endif
{
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

  gesture_recognizer_->AddGestureEventHelper(this);

  EWebEventHandler<EVAS_CALLBACK_FOCUS_IN>::Subscribe(object);
  EWebEventHandler<EVAS_CALLBACK_FOCUS_OUT>::Subscribe(object);
  EWebEventHandler<EVAS_CALLBACK_KEY_DOWN>::Subscribe(object);
  EWebEventHandler<EVAS_CALLBACK_KEY_UP>::Subscribe(object);

#if defined(OS_TIZEN)
  bool enable = GetTiltZoomEnabled();
  if (enable) {
    SubscribeMotionEvents();
  } else {
    UnsubscribeMotionEvents();
  }
  evas_object_smart_callback_call(evas_object(), "motion,enable", (void*)&enable);

  SetTouchEventsEnabled(true);
#else
  SetMouseEventsEnabled(true);
#endif

  //allow this object and its children to get a focus
  elm_object_tree_focus_allow_set (object, EINA_TRUE);
}

EWebView::~EWebView() {
  EWebEventHandler<EVAS_CALLBACK_FOCUS_IN>::Unsubscribe(evas_object());
  EWebEventHandler<EVAS_CALLBACK_FOCUS_OUT>::Unsubscribe(evas_object());
  EWebEventHandler<EVAS_CALLBACK_KEY_DOWN>::Unsubscribe(evas_object());
  EWebEventHandler<EVAS_CALLBACK_KEY_UP>::Unsubscribe(evas_object());

  if (touch_events_enabled_)
    UnsubscribeTouchEvents();

  if (mouse_events_enabled_)
    UnsubscribeMouseEvents();
  context_menu_.reset();

  mhtml_callback_map_.Clear();
}

void EWebView::SubscribeMouseEvents() {
  EWebEventHandler<EVAS_CALLBACK_MOUSE_DOWN>::Subscribe(evas_object());
  EWebEventHandler<EVAS_CALLBACK_MOUSE_UP>::Subscribe(evas_object());
  EWebEventHandler<EVAS_CALLBACK_MOUSE_WHEEL>::Subscribe(evas_object());
  EWebEventHandler<EVAS_CALLBACK_MOUSE_MOVE>::Subscribe(evas_object());
}

void EWebView::UnsubscribeMouseEvents() {
  EWebEventHandler<EVAS_CALLBACK_MOUSE_DOWN>::Unsubscribe(evas_object());
  EWebEventHandler<EVAS_CALLBACK_MOUSE_UP>::Unsubscribe(evas_object());
  EWebEventHandler<EVAS_CALLBACK_MOUSE_WHEEL>::Unsubscribe(evas_object());
  EWebEventHandler<EVAS_CALLBACK_MOUSE_MOVE>::Unsubscribe(evas_object());
}

void EWebView::SubscribeTouchEvents() {
  Ewk_View_Smart_Data* sd = ToSmartData(evas_object_);
  evas_object_event_callback_add(evas_object(), EVAS_CALLBACK_MOUSE_DOWN, OnTouchDown, sd);
  evas_object_event_callback_add(evas_object(), EVAS_CALLBACK_MOUSE_UP, OnTouchUp, sd);
  evas_object_event_callback_add(evas_object(), EVAS_CALLBACK_MOUSE_MOVE, OnTouchMove, sd);
  evas_object_event_callback_add(evas_object(), EVAS_CALLBACK_MULTI_DOWN, OnTouchDown, sd);
  evas_object_event_callback_add(evas_object(), EVAS_CALLBACK_MULTI_UP, OnTouchUp, sd);
  evas_object_event_callback_add(evas_object(), EVAS_CALLBACK_MULTI_MOVE, OnTouchMove, sd);
}

void EWebView::UnsubscribeTouchEvents() {
  evas_object_event_callback_del(evas_object(), EVAS_CALLBACK_MOUSE_DOWN, OnTouchDown);
  evas_object_event_callback_del(evas_object(), EVAS_CALLBACK_MOUSE_UP, OnTouchUp);
  evas_object_event_callback_del(evas_object(), EVAS_CALLBACK_MOUSE_MOVE, OnTouchMove);
  evas_object_event_callback_del(evas_object(), EVAS_CALLBACK_MULTI_DOWN, OnTouchDown);
  evas_object_event_callback_del(evas_object(), EVAS_CALLBACK_MULTI_UP, OnTouchUp);
  evas_object_event_callback_del(evas_object(), EVAS_CALLBACK_MULTI_MOVE, OnTouchMove);
}

#ifdef OS_TIZEN
void EWebView::SubscribeMotionEvents()
{
  Ewk_View_Smart_Data* sd = ToSmartData(evas_object_);
  evas_object_smart_callback_add(evas_object(), "motion,enable", OnMotionEnable, sd);
  evas_object_smart_callback_add(evas_object(), "motion,move", OnMotionMove,sd);
  evas_object_smart_callback_add(evas_object(), "motion,zoom", OnMotionZoom,sd);
}

void EWebView::UnsubscribeMotionEvents()
{
  evas_object_smart_callback_del(evas_object(), "motion,enable", OnMotionEnable);
  evas_object_smart_callback_del(evas_object(), "motion,move", OnMotionMove);
  evas_object_smart_callback_del(evas_object(), "motion,zoom", OnMotionZoom);
}
#endif

void EWebView::SetFocus(Eina_Bool focus) {
  if (HasFocus() != focus)
    elm_object_focus_set(evas_object_, focus);
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
}

void EWebView::SendOrientationChangeEventIfNeeded(int orientation) {
  if ((orientation != EWK_SCREEN_ORIENTATION_NATURAL)
      && (orientation != EWK_SCREEN_ORIENTATION_90)
      && (orientation != EWK_SCREEN_ORIENTATION_MINUS_90)
      && (orientation != EWK_SCREEN_ORIENTATION_180)
      && (orientation != EWK_SCREEN_ORIENTATION_270))
    return;
  //return if no change in orientation from previous state
  if (GetCurrentOrientation() == orientation)
    return;
  //set the new orientation value
  orientation_ = static_cast<Ewk_Screen_Orientation_Degrees>(orientation);
  WebContents* web_contents = web_contents_delegate_->web_contents();
  RenderViewHostImpl* rvhi = static_cast<RenderViewHostImpl*>(
                             web_contents->GetRenderViewHost());
#warning "[M37] Fix screen orientation"
#if 0
  //send new orientation value to RenderView Host to pass to renderer
  if (rvhi)
    rvhi->SendOrientationChangeEvent(orientation);
#endif
}

void EWebView::SetOrientationLockCallback(Ewk_Orientation_Lock_Cb func, void* data) {
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

void EWebView::InvokePolicyResponseCallback(Ewk_Policy_Decision* policy_decision) {
  set_policy_decision(policy_decision);
  SmartCallback<EWebViewCallbacks::PolicyResponseDecide>().call(policy_decision_.get());

  // if app has not decided nor suspended, we act as if it was accepted.
  if (!policy_decision_->isDecided && !policy_decision_->isSuspended)
    policy_decision_->Use();
}

void EWebView::InvokePolicyNavigationCallback(RenderViewHost* rvh,
    const NavigationPolicyParams params, bool* handled) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  policy_decision_.reset(new Ewk_Policy_Decision(params.url,
                                                 static_cast<Ewk_Policy_Navigation_Type>(params.type),
                                                 params.is_main_frame,
                                                 new NavigationPolicyHandlerEfl(rvh, params)));

  SmartCallback<EWebViewCallbacks::NavigationPolicyDecision>().call(policy_decision_.get());

  // if app has not decided nor suspended, we act as if it was accepted.
  if (!policy_decision_->isDecided && !policy_decision_->isSuspended)
    policy_decision_->Use();

  *handled = policy_decision_.get()->navigation_policy_handler->GetDecision() == NavigationPolicyHandlerEfl::Handled;
}

void EWebView::handleEvasObjectAdd(Evas_Object* evas_object) {
  const Evas_Smart* smart = evas_object_smart_smart_get(evas_object);
  const Evas_Smart_Class* smart_class = evas_smart_class_get(smart);
  const Ewk_View_Smart_Class* api = reinterpret_cast<const Ewk_View_Smart_Class*>(smart_class);
  DCHECK(api);

  Ewk_View_Smart_Data* d = ToSmartData(evas_object);

  if (!d) {
    // Allocating with 'calloc' as the API contract is that it should be deleted with 'free()'.
    d = static_cast<Ewk_View_Smart_Data*>(calloc(1, sizeof(Ewk_View_Smart_Data)));
    evas_object_smart_data_set(evas_object, d);
  }

  d->self = evas_object;
  d->api = api;

  parent_smart_class_.add(evas_object);

  d->priv = 0; // Will be initialized later.

  // Create evas_object_image to draw web contents.
  d->image = evas_object_image_add(d->base.evas);
  evas_object_image_alpha_set(d->image, false);
  evas_object_image_filled_set(d->image, true);
  evas_object_smart_member_add(d->image, evas_object);
  evas_object_show(d->image);
}

void EWebView::handleEvasObjectDelete(Evas_Object* evas_object) {
  Ewk_View_Smart_Data* smart_data = ToSmartData(evas_object);
  if (smart_data) {
    DCHECK(smart_data->priv);
    delete smart_data->priv;
  }

  parent_smart_class_.del(evas_object);
}

void EWebView::handleEvasObjectShow(Evas_Object* o) {
  Ewk_View_Smart_Data* d = ToSmartData(o);
  // WebKit bails here if widget accelerated compositing is used.
  // TODO: consider this when we will have AC support.
  if (evas_object_clipees_get(d->base.clipper))
    evas_object_show(d->base.clipper);
  evas_object_show(d->image);

  ToEWebView(d)->rwhv()->HandleShow();
}

void EWebView::handleEvasObjectHide(Evas_Object* o) {
  Ewk_View_Smart_Data* d = ToSmartData(o);
  evas_object_hide(d->base.clipper);
  evas_object_hide(d->image);
  // Deleting view by app results in calling hide method.
  // We assert that, RWHV is null only when renderer has crashed.
  if (!ToEWebView(d)->rwhv()) {
    // DLOG_ASSERT is used because it is controlled by NDEBUG
    DLOG_ASSERT(ToEWebView(d)->renderer_crashed_);
    return;
  }
  ToEWebView(d)->rwhv()->HandleHide();
}

void EWebView::handleEvasObjectMove(Evas_Object* o, Evas_Coord x, Evas_Coord y) {
  Ewk_View_Smart_Data* d = ToSmartData(o);
  evas_object_move(d->image, x, y);

  ToEWebView(d)->rwhv()->HandleMove(x, y);

  SmartDataChanged(d);
}

void EWebView::handleEvasObjectResize(Evas_Object* o, Evas_Coord width, Evas_Coord height) {
  Ewk_View_Smart_Data* d = ToSmartData(o);
  evas_object_resize(d->image, width, height);
  evas_object_image_size_set(d->image, width, height);
  evas_object_image_fill_set(d->image, 0, 0, width, height);
  d->view.w = width;
  d->view.h = height;
  ToEWebView(d)->rwhv()->HandleResize(width, height);

  SmartDataChanged(d);
}

void EWebView::handleEvasObjectCalculate(Evas_Object* o) {
  Ewk_View_Smart_Data* d = ToSmartData(o);
  Evas_Coord x, y, width, height;
  evas_object_geometry_get(o, &x, &y, &width, &height);
  d->view.x = x;
  d->view.y = y;
  d->view.w = width;
  d->view.h = height;
}

void EWebView::handleEvasObjectColorSet(Evas_Object*, int red, int green, int blue, int alpha) {
  // FIXME: implement.
}

Eina_Bool EWebView::handleFocusIn(Ewk_View_Smart_Data* d) {
  ToEWebView(d)->rwhv()->HandleFocusIn();
  return true;
}

Eina_Bool EWebView::handleFocusOut(Ewk_View_Smart_Data* d) {
  // Deleting view by app results in calling focus out method.
  // We assert that, RWHV is null only when renderer has crashed.
  if (!ToEWebView(d)->rwhv()) {
    // DLOG_ASSERT is used because it is controlled by NDEBUG
   DLOG_ASSERT(ToEWebView(d)->renderer_crashed_);
   return false;
  }

  ToEWebView(d)->rwhv()->HandleFocusOut();
  return true;
}

Eina_Bool EWebView::handleMouseWheel(Ewk_View_Smart_Data* d, const Evas_Event_Mouse_Wheel* event) {
  ToEWebView(d)->rwhv()->HandleEvasEvent(event);
  return true;
}

Eina_Bool EWebView::handleMouseDown(Ewk_View_Smart_Data* d, const Evas_Event_Mouse_Down* event) {
  ToEWebView(d)->SetFocus(EINA_TRUE);
  ToEWebView(d)->rwhv()->HandleEvasEvent(event);
  return true;
}

Eina_Bool EWebView::handleMouseUp(Ewk_View_Smart_Data* d, const Evas_Event_Mouse_Up* event) {
  ToEWebView(d)->rwhv()->HandleEvasEvent(event);
  return true;
}

Eina_Bool EWebView::handleMouseMove(Ewk_View_Smart_Data* d, const Evas_Event_Mouse_Move* event) {
  ToEWebView(d)->rwhv()->HandleEvasEvent(event);
  return true;
}

Eina_Bool EWebView::handleKeyDown(Ewk_View_Smart_Data* d, const Evas_Event_Key_Down* event) {
  ToEWebView(d)->rwhv()->HandleEvasEvent(event);
  return true;
}

Eina_Bool EWebView::handleKeyUp(Ewk_View_Smart_Data* d, const Evas_Event_Key_Up* event) {
  ToEWebView(d)->rwhv()->HandleEvasEvent(event);
  return true;
}

void EWebView::OnTouchDown(void* sd, Evas*, Evas_Object*, void*) {
  ToEWebView(static_cast<Ewk_View_Smart_Data*>(sd))->HandleTouchEvents(EWK_TOUCH_START);
}

void EWebView::OnTouchUp(void* sd, Evas*, Evas_Object*, void*) {
  ToEWebView(static_cast<Ewk_View_Smart_Data*>(sd))->HandleTouchEvents(EWK_TOUCH_END);
}

void EWebView::OnTouchMove(void* sd, Evas*, Evas_Object*, void*) {
  ToEWebView(static_cast<Ewk_View_Smart_Data*>(sd))->HandleTouchEvents(EWK_TOUCH_MOVE);
}

void EWebView::HandleTouchEvents(Ewk_Touch_Event_Type type, const Eina_List *points, const Evas_Modifier *modifiers) {
  const Eina_List* l;
  void* data;
  EINA_LIST_FOREACH(points, l, data) {
    ui::TouchEvent touch_event = WebEventFactoryEfl::toUITouchEvent(static_cast<Ewk_Touch_Point*>(data), evas_object(), rwhv()->device_scale_factor());
    rwhv()->HandleTouchEvent(&touch_event);
  }
}

void EWebView::ProcessAckedTouchEvent(const TouchEventWithLatencyInfo& touch,
                                      InputEventAckState ack_result) {
  ScopedVector<ui::TouchEvent> events;
  if (!MakeUITouchEventsFromWebTouchEvents(touch, &events, LOCAL_COORDINATES))
    return;

  ui::EventResult result = (ack_result == INPUT_EVENT_ACK_STATE_CONSUMED) ?
	  ui::ER_HANDLED : ui::ER_UNHANDLED;
  for (ScopedVector<ui::TouchEvent>::const_iterator iter = events.begin(),
      end = events.end(); iter != end; ++iter)  {
    scoped_ptr<ui::GestureRecognizer::Gestures> gestures(
	gesture_recognizer_->ProcessTouchEventForGesture(*(*iter), result, this));
    if (gestures) {
      for (size_t j = 0; j < gestures->size(); ++j) {
        ui::GestureEvent* event = gestures->get().at(j);
        rwhv()->HandleGesture(event);
      }
    }
  }
}

void EWebView::HandleTouchEvents(Ewk_Touch_Event_Type type) {
  // These constants are used to map multi touch's touch id(s).
  // The poorly-written Tizen API document says:
  //  "0 for Mouse Event and device id for Multi Event."
  //  "The point which comes from Mouse Event has id 0 and"
  //  "The point which comes from Multi Event has id that is same as Multi Event's device id."
  static const int kMultiTouchIDMapPart0SingleIndex = 0; // This constant is to map touch id 0 to 0, or [0] -> [0]
  static const int kMultiTouchIDMapPart1StartIndex = 13; // This constant is to map [13, 23] -> [1, 11]
  static const int kMultiTouchIDMapPart1EndIndex = 23; // This constant is to map [13, 23] -> [1, 11]
  static const int kMultiTouchIDMapPart1DiffValue = 12; // 13 - 1 = 12, 23 - 11 = 12

  Ewk_View_Smart_Data* sd = ToSmartData(evas_object_);

  unsigned count = evas_touch_point_list_count(sd->base.evas);
  if (!count)
    return;

  Evas_Coord_Point pt;
  Eina_List* points = 0;
  for (unsigned i = 0; i < count; ++i) {
    Ewk_Touch_Point* point = new Ewk_Touch_Point;
    // evas_touch_point_list_nth_id_get returns [0] or [13, )
    // Multi touch's touch id [[0], [13, 23]] should be mapped to [[0], [1, 11]]
    // Internet Blame URL:
    //   https://groups.google.com/d/msg/mailing-enlightenment-devel/-R-ezCzpkTk/HJ0KBCdz6CgJ
    point->id = evas_touch_point_list_nth_id_get(sd->base.evas, i);
    DCHECK(point->id == kMultiTouchIDMapPart0SingleIndex || point->id >= kMultiTouchIDMapPart1StartIndex);
    if (point->id >= kMultiTouchIDMapPart1StartIndex && point->id <= kMultiTouchIDMapPart1EndIndex) {
      point->id -= kMultiTouchIDMapPart1DiffValue;
    } else if (point->id > kMultiTouchIDMapPart1EndIndex) {
      LOG(ERROR) << "evas_touch_point_list_nth_id_get() returned a value greater than ("
                 << kMultiTouchIDMapPart1EndIndex << "). It is ignored.";
    }
    evas_touch_point_list_nth_xy_get(sd->base.evas, i, &point->x, &point->y);
    pt.x = point->x;
    pt.y = point->y;
    point->state = evas_touch_point_list_nth_state_get(sd->base.evas, i);
    if (type == EWK_TOUCH_CANCEL)
      point->state = EVAS_TOUCH_POINT_CANCEL;
    points = eina_list_append(points, point);
  }

  if (type == EWK_TOUCH_START)
    SetFocus(EINA_TRUE);

  HandleTouchEvents(type, points, evas_key_modifier_get(sd->base.evas));

#ifdef OS_TIZEN
  if (count >= 2) {
    if (type == EWK_TOUCH_START) {
      LOG(ERROR) << " wkext_motion_tilt_start";
      wkext_motion_tilt_start(evas_object(), &pt);
    } else if (type == EWK_TOUCH_END) {
      wkext_motion_tilt_stop();
    }
  }
#else
  // Silence unused variable warning
  (void)(pt);
#endif

  void* data;
  EINA_LIST_FREE(points, data)
  delete static_cast<Ewk_Touch_Point*>(data);
}

bool EWebView::CanDispatchToConsumer(ui::GestureConsumer* consumer) {
  return true;
}

void EWebView::DispatchPostponedGestureEvent(ui::GestureEvent* event) {
  Ewk_Settings* settings = GetSettings();
  LOG(INFO) << "DispatchPostponedGestureEvent :: " << event->details().type();
  if (event->details().type() == ui::ET_GESTURE_LONG_PRESS) {
    LOG(INFO) << "DispatchPostponedGestureEvent :: ET_GESTURE_LONG_PRESS";
    if (selection_controller_->GetSelectionEditable())
      ClearSelection();

    if (settings && settings->textSelectionEnabled()) {
      _Ewk_Hit_Test* hit_test_data = RequestHitTestDataAt(event->x(), event->y(), EWK_HIT_TEST_MODE_DEFAULT);
      if (hit_test_data && hit_test_data->context & EWK_HIT_TEST_RESULT_CONTEXT_EDITABLE) {
        LOG(INFO) << "DispatchPostponedGestureEvent :: hit_test EWK_HIT_TEST_RESULT_CONTEXT_EDITABLE";
        selection_controller_->SetSelectionStatus(true);
        selection_controller_->SetCaretSelectionStatus(true);
        selection_controller_->SetSelectionEditable(true);
        selection_controller_->HandleLongPressEvent(gfx::Point(event->x(), event->y()));
      } else if (hit_test_data
          && !(hit_test_data->context & EWK_HIT_TEST_RESULT_CONTEXT_LINK)
          && !(hit_test_data->context & EWK_HIT_TEST_RESULT_CONTEXT_IMAGE)
          && !(hit_test_data->context & EWK_HIT_TEST_RESULT_CONTEXT_MEDIA)
          && hit_test_data->context & EWK_HIT_TEST_RESULT_CONTEXT_TEXT) {
        selection_controller_->SetSelectionStatus(true);
        selection_controller_->HandleLongPressEvent(gfx::Point(event->x(), event->y()));
        delete hit_test_data;
        LOG(INFO) << __PRETTY_FUNCTION__ << ":: link, !image, !media, text";
      } else if (hit_test_data && hit_test_data->context & EWK_HIT_TEST_RESULT_CONTEXT_DOCUMENT) {
        LOG(INFO) << __PRETTY_FUNCTION__ << ":: EWK_HIT_TEST_RESULT_CONTEXT_DOCUMENT";
      } else if (hit_test_data && hit_test_data->context & EWK_HIT_TEST_RESULT_CONTEXT_IMAGE) {
        LOG(INFO) << __PRETTY_FUNCTION__ << ":: EWK_HIT_TEST_RESULT_CONTEXT_IMAGE";
      } else if (hit_test_data && hit_test_data->context & EWK_HIT_TEST_RESULT_CONTEXT_LINK) {
        ClearSelection();
        LOG(INFO) << __PRETTY_FUNCTION__ << ":: EWK_HIT_TEST_RESULT_CONTEXT_LINK";
      } else {
        LOG(INFO) << __PRETTY_FUNCTION__ << ":: hit_test = " << hit_test_data->context;
      }
      rwhv()->HandleGesture(event);
    }
  } else if ((event->details().type() == ui::ET_GESTURE_TAP) || (event->details().type() == ui::ET_GESTURE_SHOW_PRESS))  {
    _Ewk_Hit_Test* hit_test_data = RequestHitTestDataAt(event->x(), event->y(), EWK_HIT_TEST_MODE_DEFAULT);
    LOG(INFO) << __PRETTY_FUNCTION__ << " hit_test = " << hit_test_data;
    if (hit_test_data && hit_test_data->context & EWK_HIT_TEST_RESULT_CONTEXT_EDITABLE) {
      LOG(INFO) << "DispatchPostponedGestureEvent :: EWK_HIT_TEST_RESULT_CONTEXT_EDITABLE";
      selection_controller_->SetSelectionStatus(true);
      if (selection_controller_->GetSelectionEditable()) {
        selection_controller_->HideHandle();
        selection_controller_->SetCaretSelectionStatus(true);
      } else
        selection_controller_->SetSelectionEditable(true);
      delete hit_test_data;
    } else {
      if (hit_test_data && hit_test_data->context & EWK_HIT_TEST_RESULT_CONTEXT_DOCUMENT)
        LOG(INFO) << __PRETTY_FUNCTION__ << " DOCUMENT";
      if (hit_test_data && hit_test_data->context & EWK_HIT_TEST_RESULT_CONTEXT_TEXT)
        LOG(INFO) << __PRETTY_FUNCTION__ << " TEXT";

      selection_controller_->SetSelectionEditable(false);
      ClearSelection();
    }
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

void EWebView::DispatchCancelTouchEvent(ui::TouchEvent* event) {
  NOTIMPLEMENTED();
  rwhv()->HandleTouchEvent(event);
}

void EWebView::DispatchGestureEvent(ui::GestureEvent*) {
  NOTIMPLEMENTED();
}

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
    UnsubscribeMouseEvents();
    SubscribeTouchEvents();
  } else
    UnsubscribeTouchEvents();
}

bool EWebView::MouseEventsEnabled() const {
  return mouse_events_enabled_;
}

void EWebView::SetMouseEventsEnabled(bool enabled) {
  if (mouse_events_enabled_ == enabled)
    return;

  mouse_events_enabled_ = enabled;

  if (enabled) {
    UnsubscribeTouchEvents();
    SubscribeMouseEvents();
  } else
    UnsubscribeMouseEvents();
}

namespace {

class JavaScriptCallbackDetails {
 public:
  JavaScriptCallbackDetails(Ewk_View_Script_Execute_Callback callback_func, void *user_data, Evas_Object* view)
    : callback_func_(callback_func)
    , user_data_(user_data)
    , view_(view) {}

  Ewk_View_Script_Execute_Callback callback_func_;
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

bool EWebView::ExecuteJavaScript(const char* script, Ewk_View_Script_Execute_Callback callback, void* userdata) {
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
#warning "[M37] Fix GetUserAgent"
// NOTE: web_contents_delegate_->web_contents()->GetUserAgentOverride();
//       can be used to get an override, what about not verriden UA?
#if 0
  if (overridden_user_agent_.empty()) {
    //return default user agent
    return (content::GetUserAgent(GURL())).c_str();
  } else {
    return overridden_user_agent_.c_str();
  }
#endif
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

void EWebView::SetContentSecurityPolicy(const char* policy, Ewk_CSP_Header_Type type) {
  web_contents_delegate_->SetContentSecurityPolicy((policy ? policy : std::string()), type);
}

void EWebView::LoadHTMLString(const char* html, const char* base_uri, const char* unreachable_uri) {
  LoadData(html, std::string::npos, NULL, NULL, base_uri, unreachable_uri);
}

void EWebView::LoadPlainTextString(const char* plain_text) {
  LoadData(plain_text, std::string::npos, "text/plain", NULL, NULL, NULL);
}

void EWebView::LoadData(const char* data, size_t size, const char* mime_type, const char* encoding, const char* base_uri, const char* unreachable_uri) {
  SET_STRING_IF_NULL(mime_type, "text/html");
  SET_STRING_IF_NULL(encoding, "utf-8");

  if (!base_uri) {
    // Webkit2 compatible
    base_uri = "about:blank";
  }
  SET_EMPTY_STRING_IF_NULL(unreachable_uri);

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

void EWebView::ShowContextMenu(const content::ContextMenuParams& params, content::ContextMenuType type) {
  context_menu_.reset(new content::ContextMenuControllerEfl(evas_object(), type));
  if(!context_menu_->PopulateAndShowContextMenu(params))
    context_menu_.reset();
}

void EWebView::CancelContextMenu(int request_id) {
  context_menu_.reset();
}

void EWebView::Find(const char* text, Ewk_Find_Options ewk_find_options) {
  base::string16 find_text = base::UTF8ToUTF16(text);
  bool find_next = (previous_text_ == find_text);

  if (!find_next) {
    current_find_request_id_ = find_request_id_counter_++;
    previous_text_ = find_text;
  }

  blink::WebFindOptions web_find_options;
  web_find_options.forward = !(ewk_find_options & EWK_FIND_OPTIONS_BACKWARDS);
  web_find_options.matchCase = !(ewk_find_options & EWK_FIND_OPTIONS_CASE_INSENSITIVE);
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
  const gfx::Vector2d scroll_position = rwhv()->host()->GetLastScrollOffset();
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

  Ewk_View_Smart_Data* smart_data = ToSmartData(evas_object());

#ifdef OS_TIZEN
  if (width && contents_area_.width() > smart_data->view.w)
    *width = contents_area_.width() - smart_data->view.w;
  if (height && contents_area_.height() > smart_data->view.h)
    *height = contents_area_.height() - smart_data->view.h;
#else
  if (width && contents_size_.width() > smart_data->view.w)
    *width = contents_size_.width() - smart_data->view.w;
  if (height && contents_size_.height() > smart_data->view.h)
    *height = contents_size_.height() - smart_data->view.h;
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

bool EWebView::ClearSelection() {
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

Ewk_Hit_Test* EWebView::RequestHitTestDataAt(int x, int y, Ewk_Hit_Test_Mode mode) {
  RenderViewHost* render_view_host = web_contents_delegate()->web_contents()->GetRenderViewHost();
  DCHECK(render_view_host);

  render_view_host->Send(new EwkViewMsg_DoHitTest(render_view_host->GetRoutingID(), x, y, mode));

  {
    // We wait on UI thread till hit test data is updated.
    base::ThreadRestrictions::ScopedAllowWait allow_wait;
    hit_test_completion_.Wait();
  }
  return new Ewk_Hit_Test(hit_test_data_);
}

void EWebView::UpdateHitTestData(const Ewk_Hit_Test& hit_test_data, const NodeAttributesMap& node_attributes) {
  DCHECK(content::BrowserThread::CurrentlyOn(content::BrowserThread::IO));
  hit_test_data_ = hit_test_data;
  hit_test_data_.nodeData.PopulateNodeAtributes(node_attributes);
  hit_test_completion_.Signal();
}

void EWebView::OnCopyFromBackingStore(bool success, const SkBitmap& bitmap) {
#warning "[M37] Need proper implementation for OnCopyFromBackingStore"
  // if (selection_controller_->GetSelectionStatus() || selection_controller_->GetCaretSelectionStatus())
  //   selection_controller_->UpdateMagnifierScreen(bitmap);
}

void EWebView::GetSnapShotForRect(const gfx::Rect& display) {
#warning "[M37] Need proper implementation for GetSnapShotForRect"
  // RenderViewHost* render_view_host = web_contents_delegate_->web_contents()->GetRenderViewHost();
  // render_view_host->GetSnapshotFromRenderer(display,
  //                                           base::Bind(&EWebView::OnCopyFromBackingStore,
  //                                           base::Unretained(this)));
  Evas_Object *magnifImg = NULL;
  rwhv()->SaveImage(&magnifImg, display);
  selection_controller_->UpdateMagnifierScreen(magnifImg);
}

bool EWebView::GetSnapshot(Eina_Rectangle rect, Evas_Object *image) {
  DCHECK(image);
#if 0
  content::BackingStore* backing_store = RenderWidgetHostImpl::From(
      rwhv()->GetRenderWidgetHost())->GetBackingStore(false);

  if (!backing_store)
    return false;

  gfx::Rect clip_rect(rect.x, rect.y, rect.w, rect.h);
  skia::PlatformBitmap output;

  if (!backing_store->CopyFromBackingStore(clip_rect, &output))
    return false;

  DCHECK(output.GetBitmap().config() == SkBitmap::kARGB_8888_Config);

  evas_object_image_size_set(image, output.GetBitmap().width(), output.GetBitmap().height());
  evas_object_image_colorspace_set(image, EVAS_COLORSPACE_ARGB8888);
  evas_object_image_data_copy_set(image, output.GetBitmap().getPixels());
  return true;
#else
  #warning "[M37] Backing store was remvoed from content layer"
  return false;
#endif
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

bool IsEWebViewObject(const Evas_Object* evas_object) {
  DCHECK(evas_object);

  const char* object_type = evas_object_type_get(evas_object);
  const Evas_Smart* evas_smart = evas_object_smart_smart_get(evas_object);
  if (!evas_smart) {
    EINA_LOG_CRIT("%p (%s) is not a smart object!", evas_object,
                  object_type ? object_type : "(null)");
    return false;
  }

  const Evas_Smart_Class* smart_class = evas_smart_class_get(evas_smart);
  if (!smart_class) {
    EINA_LOG_CRIT("%p (%s) is not a smart class object!", evas_object,
                  object_type ? object_type : "(null)");
    return false;
  }

  if (smart_class->data != SmartClassName) {
    EINA_LOG_CRIT("%p (%s) is not of an ewk_view (need %p, got %p)!",
                  evas_object, object_type ? object_type : "(null)",
                  SmartClassName, smart_class->data);
    return false;
  }

  return true;
}

EWebView* ToEWebView(const Evas_Object* evas_object) {
  DCHECK(evas_object);
  DCHECK(IsEWebViewObject(evas_object));

  return ToEWebView(static_cast<Ewk_View_Smart_Data*>(evas_object_smart_data_get(evas_object)));
}

void EwkViewPlainTextGetCallback::TriggerCallback(Evas_Object* obj, const std::string& content_text) {
  if(callback_)
    (callback_)(obj, content_text.c_str(), user_data_);
}

int EWebView::SetEwkViewPlainTextGetCallback(Ewk_View_Plain_Text_Get_Callback callback, void* user_data) {
  EwkViewPlainTextGetCallback* view_plain_text_callback_ptr = new EwkViewPlainTextGetCallback(callback, user_data);
  return plain_text_get_callback_map_.Add(view_plain_text_callback_ptr);
}

bool EWebView::PlainTextGet(Ewk_View_Plain_Text_Get_Callback callback, void* user_data) {
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

bool EWebView::GetMHTMLData(Ewk_View_MHTML_Data_Get_Callback callback, void* user_data) {
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

void EWebView::SetJavaScriptAlertCallback(Ewk_View_JavaScript_Alert_Callback callback, void* user_data) {
  GetJavaScriptDialogManagerEfl()->SetAlertCallback(callback, user_data);
}

void EWebView::JavaScriptAlertReply() {
  GetJavaScriptDialogManagerEfl()->ExecuteDialogClosedCallBack(true, std::string());
  SmartCallback<EWebViewCallbacks::PopupReplyWaitFinish>().call(0);
}

void EWebView::SetJavaScriptConfirmCallback(Ewk_View_JavaScript_Confirm_Callback callback, void* user_data) {
  GetJavaScriptDialogManagerEfl()->SetConfirmCallback(callback, user_data);
}

void EWebView::JavaScriptConfirmReply(bool result) {
  GetJavaScriptDialogManagerEfl()->ExecuteDialogClosedCallBack(result, std::string());
  SmartCallback<EWebViewCallbacks::PopupReplyWaitFinish>().call(0);
}

void EWebView::SetJavaScriptPromptCallback(Ewk_View_JavaScript_Prompt_Callback callback, void* user_data) {
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

#ifdef OS_TIZEN
void EWebView::OnMotionEnable(void *data, Evas_Object *obj, void *eventInfo) {
  bool* enable = static_cast<bool*>(eventInfo);
  wkext_motion_tilt_enable_set(obj, *enable, 3);
}

void EWebView::OnMotionMove(void *data, Evas_Object *obj, void *eventInfo) {
}

void EWebView::OnMotionZoom(void *data, Evas_Object *obj, void *eventInfo) {
  EWebView* view = ToEWebView(static_cast<Ewk_View_Smart_Data*>(data));
  view->rwhv()->makePinchZoom(eventInfo);
}

bool EWebView::GetTiltZoomEnabled() {
  int motion_enabled = 0;
  vconf_get_bool(VCONFKEY_SETAPPL_MOTION_ACTIVATION, &motion_enabled);
  if (motion_enabled) {
    int tilt_enabled = 0;
    vconf_get_bool(VCONFKEY_SETAPPL_USE_TILT, &tilt_enabled);
    //BROWSER_LOGD("******* motion_enabled=[%d], tilt_enabled=[%d]", motion_enabled, tilt_enabled);
    if (tilt_enabled)
      return true;
  }
  return false;
}
#endif // OS_TIZEN

void EWebView::ShowFileChooser(const content::FileChooserParams& params) {
  RenderViewHost* render_view_host = web_contents_delegate()->web_contents()->GetRenderViewHost();
  if (!render_view_host)
    return;

  file_chooser_.reset(new content::FileChooserControllerEfl(render_view_host, &params));
  file_chooser_->open();
}
