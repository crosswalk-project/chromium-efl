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

#include "ewk_view_private.h"
#include <cassert>
#include <tizen_webview/public/tw_webview_delegate.h>
#include <tizen_webview/public/tw_webview_evas_event_handler.h>
#include "private/ewk_context_private.h"
#include "private/webview_delegate_ewk.h"

//TODO: remove below header dependency
#include "../impl/eweb_view.h"

using tizen_webview::WebView;
using tizen_webview::WebViewDelegate;

// -------- EwkViewImpl
struct EwkViewImpl  {
  explicit EwkViewImpl(tizen_webview::WebView* wv) : wv_(wv) {}
  ~EwkViewImpl() { tizen_webview::WebView::Delete(wv_); }
  tizen_webview::WebView* GetTizenWebView() { return wv_; }
  const tizen_webview::WebView* GetTizenWebView() const { return wv_; }
 private:
  tizen_webview::WebView* wv_;
  DISALLOW_COPY_AND_ASSIGN(EwkViewImpl);
};

Evas_Smart_Class parent_smart_class_ = EVAS_SMART_CLASS_INIT_NULL;

namespace {

Evas_Smart* DefaultSmartClassInstance()
{
  static Ewk_View_Smart_Class api = EWK_VIEW_SMART_CLASS_INIT_NAME_VERSION(EwkViewSmartClassName);
  static Evas_Smart* smart = 0;
  if (!smart) {
    InitSmartClassInterface(api);
    smart = evas_smart_class_new(&api.sc);
  }

  return smart;
}

void SmartDataChanged(Ewk_View_Smart_Data* d)
{
  assert(d);
  if (d->changed.any)
    return;
  d->changed.any = true;
  evas_object_smart_changed(d->self);
}

// Evas_Smart_Class callback interface:
void handleEvasObjectAdd(Evas_Object* evas_object)
{
  const Evas_Smart* smart = evas_object_smart_smart_get(evas_object);
  const Evas_Smart_Class* smart_class = evas_smart_class_get(smart);
  const Ewk_View_Smart_Class* api = reinterpret_cast<const Ewk_View_Smart_Class*>(smart_class);
  assert(api);

  Ewk_View_Smart_Data* d = GetEwkViewSmartDataFromEvasObject(evas_object);

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

// Ewk_View_Smart_Class callback interface:
void handleEvasObjectDelete(Evas_Object* evas_object)
{
  Ewk_View_Smart_Data* smart_data = GetEwkViewSmartDataFromEvasObject(evas_object);
  if (smart_data) {
    delete smart_data->priv;
    smart_data->priv = NULL;
  }
  parent_smart_class_.del(evas_object);
}

void handleEvasObjectShow(Evas_Object* o)
{
  Ewk_View_Smart_Data* d = GetEwkViewSmartDataFromEvasObject(o);
  WebView* wv = GetWebViewFromSmartData(d);
  if (!wv) {
    return;
  }
  // WebKit bails here if widget accelerated compositing is used.
  // TODO: consider this when we will have AC support.
  if (evas_object_clipees_get(d->base.clipper))
    evas_object_show(d->base.clipper);
  evas_object_show(d->image);
  wv->HandleShow();
}

void handleEvasObjectHide(Evas_Object* o)
{
  Ewk_View_Smart_Data* d = GetEwkViewSmartDataFromEvasObject(o);
  WebView* wv = GetWebViewFromSmartData(d);
  if (!wv) {
    return;
  }
  evas_object_hide(d->base.clipper);
  evas_object_hide(d->image);
  // Deleting view by app results in calling hide method.
  // We assert that, RWHV is null only when renderer has crashed.
  /*if (!ToEWebView(d)->rwhv()) {
     DLOG_ASSERT is used because it is controlled by NDEBUG
    DLOG_ASSERT(ToEWebView(d)->renderer_crashed_);
    return;
  }*/
  wv->HandleHide();
}

void handleEvasObjectMove(Evas_Object* o, Evas_Coord x, Evas_Coord y)
{
  Ewk_View_Smart_Data* d = GetEwkViewSmartDataFromEvasObject(o);
  WebView* wv = GetWebViewFromSmartData(d);
  if (!wv) {
    return;
  }
  evas_object_move(d->image, x, y);
  wv->HandleMove(x, y);
  SmartDataChanged(d);
}

void handleEvasObjectResize(Evas_Object* o, Evas_Coord width, Evas_Coord height)
{
  Ewk_View_Smart_Data* d = GetEwkViewSmartDataFromEvasObject(o);
  WebView* wv = GetWebViewFromSmartData(d);
  if (!wv) {
    return;
  }
  evas_object_resize(d->image, width, height);
  evas_object_image_size_set(d->image, width, height);
  evas_object_image_fill_set(d->image, 0, 0, width, height);
  d->view.w = width;
  d->view.h = height;
  wv->HandleResize(width, height);
  SmartDataChanged(d);
}

void handleEvasObjectCalculate(Evas_Object* o)
{
  Ewk_View_Smart_Data* d = GetEwkViewSmartDataFromEvasObject(o);
  WebView* wv = GetWebViewFromSmartData(d);
  if (!wv) {
    return;
  }
  Evas_Coord x, y, width, height;
  evas_object_geometry_get(o, &x, &y, &width, &height);
  d->view.x = x;
  d->view.y = y;
  d->view.w = width;
  d->view.h = height;
}

void handleEvasObjectColorSet(Evas_Object*, int red, int green, int blue, int alpha)
{
  // FIXME: implement.
}

Eina_Bool handleFocusIn(Ewk_View_Smart_Data* d)
{
  WebView* wv = GetWebViewFromSmartData(d);
  if (!wv) {
    return false;
  }
  tizen_webview::WebViewEvasEventHandler* wv_handler = wv->GetEvasEventHandler();
  return wv_handler ? wv_handler->HandleEventDefault_FocusIn() : false;
}

Eina_Bool handleFocusOut(Ewk_View_Smart_Data* d)
{
  tizen_webview::WebViewEvasEventHandler* wv_handler =
      GetWebViewFromSmartData(d)->GetEvasEventHandler();
  return wv_handler ? wv_handler->HandleEventDefault_FocusOut() : false;
}

Eina_Bool handleMouseWheel(Ewk_View_Smart_Data* d, const Evas_Event_Mouse_Wheel* event)
{
  tizen_webview::WebViewEvasEventHandler* wv_handler =
      GetWebViewFromSmartData(d)->GetEvasEventHandler();
  return wv_handler ? wv_handler->HandleEventDefault_MouseWheel(event) : false;
}

Eina_Bool handleMouseDown(Ewk_View_Smart_Data* d, const Evas_Event_Mouse_Down* event)
{
  tizen_webview::WebViewEvasEventHandler* wv_handler =
      GetWebViewFromSmartData(d)->GetEvasEventHandler();
  return wv_handler ?  wv_handler->HandleEventDefault_MouseDown(event) : false;
}

Eina_Bool handleMouseUp(Ewk_View_Smart_Data* d, const Evas_Event_Mouse_Up* event)
{
  tizen_webview::WebViewEvasEventHandler* wv_handler =
      GetWebViewFromSmartData(d)->GetEvasEventHandler();
  return wv_handler ? wv_handler->HandleEventDefault_MouseUp(event) : false;
}

Eina_Bool handleMouseMove(Ewk_View_Smart_Data* d, const Evas_Event_Mouse_Move* event)
{
  tizen_webview::WebViewEvasEventHandler* wv_handler =
      GetWebViewFromSmartData(d)->GetEvasEventHandler();
  return wv_handler ? wv_handler->HandleEventDefault_MouseMove(event) : false;
}

Eina_Bool handleKeyDown(Ewk_View_Smart_Data* d, const Evas_Event_Key_Down* event)
{
   tizen_webview::WebViewEvasEventHandler* wv_handler =
      GetWebViewFromSmartData(d)->GetEvasEventHandler();
  return wv_handler ? wv_handler->HandleEventDefault_KeyDown(event) : false;
}

Eina_Bool handleKeyUp(Ewk_View_Smart_Data* d, const Evas_Event_Key_Up* event)
{
   tizen_webview::WebViewEvasEventHandler* wv_handler =
      GetWebViewFromSmartData(d)->GetEvasEventHandler();
  return wv_handler ? wv_handler->HandleEventDefault_KeyUp(event) : false;
}

Eina_Bool handleTextSelectionDown(Ewk_View_Smart_Data* d, int x, int y)
{
  tizen_webview::WebViewEvasEventHandler* wv_handler =
      GetWebViewFromSmartData(d)->GetEvasEventHandler();
  return wv_handler ? wv_handler->HandleEventDefault_TextSelectionDown(x, y) : false;
}

Eina_Bool handleTextSelectionUp(Ewk_View_Smart_Data* d, int x, int y)
{
  tizen_webview::WebViewEvasEventHandler* wv_handler =
      GetWebViewFromSmartData(d)->GetEvasEventHandler();
  return wv_handler ? wv_handler->HandleEventDefault_TextSelectionUp(x, y) : false;
}


unsigned long long handleExceededDatabaseQuota(Ewk_View_Smart_Data *sd, const char *databaseName, const char *displayName, unsigned long long currentQuota, unsigned long long currentOriginUsage, unsigned long long currentDatabaseUsage, unsigned long long expectedUsage)
{
  // Chromium does not support quota per origin right now, this API can't be implemented
  NOTIMPLEMENTED();
  return EINA_FALSE;
}

} // namespace


// --------- public API --------------

bool IsWebViewObject(const Evas_Object* evas_object)
{
  if (!evas_object) {
    return false;
  }

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

  if (smart_class->data != EwkViewSmartClassName) {
    EINA_LOG_CRIT("%p (%s) is not of an ewk_view (need %p, got %p)!",
                  evas_object, object_type ? object_type : "(null)",
                  EwkViewSmartClassName, smart_class->data);
    return false;
  }
  return true;
}

Ewk_View_Smart_Data* GetEwkViewSmartDataFromEvasObject(const Evas_Object* evas_object)
{
  assert(evas_object);
  assert(IsWebViewObject(evas_object));
  return static_cast<Ewk_View_Smart_Data*>(evas_object_smart_data_get(evas_object));
}

Ewk_View_Smart_Data* GetEwkViewSmartDataFromWebView(tizen_webview::WebView* wv) {
  assert(wv);
  return GetEwkViewSmartDataFromEvasObject(wv->AsEvasObject());
}

WebView* GetWebViewFromSmartData(const Ewk_View_Smart_Data* smartData)
{
  if (smartData && smartData->priv) {
    return smartData->priv->GetTizenWebView();
  }
  return NULL;
}

WebView* GetWebViewFromEvasObject(const Evas_Object* eo) {
  if (!IsWebViewObject(eo)) {
    return NULL;
  }
  Ewk_View_Smart_Data* sd = GetEwkViewSmartDataFromEvasObject(eo);
  return GetWebViewFromSmartData(sd);
}

Evas_Object* CreateWebViewAsEvasObject(Ewk_Context* context,
                                       Evas* canvas,
                                       Evas_Smart* smart /*= 0*/) {
  if(!WebViewDelegate::IsDelegateRegistered()) {
    WebViewDelegate::RegisterDelegate(
      const_cast<WebViewDelegateEwk*>(&WebViewDelegateEwk::GetInstance()));
  }

  smart = smart ? smart : DefaultSmartClassInstance();
  Evas_Object* wv_evas_object = evas_object_smart_add(canvas, smart);
  WebView* view = WebView::Create(context, wv_evas_object);
  if (!view) {
    return NULL;
  }
  Ewk_View_Smart_Data* sd = GetEwkViewSmartDataFromEvasObject(wv_evas_object);
  if (!sd) {
    WebView::Delete(view);
    return NULL;
  }
  // attach webview as a member of smart data
  sd->priv = new EwkViewImpl(view);
  view->Initialize();
  return wv_evas_object;
}


bool InitSmartClassInterface(Ewk_View_Smart_Class& api)
{
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
  api.text_selection_down = &handleTextSelectionDown;
  api.text_selection_up = &handleTextSelectionUp;

  api.exceeded_database_quota = &handleExceededDatabaseQuota;

  // Type identifier.
  api.sc.data = EwkViewSmartClassName;

  return true;
}
