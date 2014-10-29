// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ewk_view_private_h
#define ewk_view_private_h

#include <Evas.h>
#include "ewk_view.h"
#include <tizen_webview/public/tw_webview.h>

class Ewk_Context;

// Create WebView Evas Object
Evas_Object* CreateWebViewAsEvasObject(Ewk_Context* context, Evas* canvas, Evas_Smart* smart = 0);
bool InitSmartClassInterface(Ewk_View_Smart_Class& api);

// EwkView's Smart Class Name
const char EwkViewSmartClassName[] = "EWebView";

// type conversion utility
bool IsWebViewObject(const Evas_Object* evas_object);
Ewk_View_Smart_Data* GetEwkViewSmartDataFromEvasObject(const Evas_Object* evas_object);
Ewk_View_Smart_Data* GetEwkViewSmartDataFromWebView(tizen_webview::WebView* wv);
tizen_webview::WebView* GetWebViewFromSmartData(const Ewk_View_Smart_Data* smartData);
tizen_webview::WebView* GetWebViewFromEvasObject(const Evas_Object* eo);

// helper macro
#define EWK_VIEW_IMPL_GET_OR_RETURN(evas_object, impl, ...)               \
  tizen_webview::WebView* impl = GetWebViewFromEvasObject(evas_object);   \
  do {                                                                    \
    if (!impl) {                                                          \
      EINA_LOG_CRIT("Evas Object %p is not Ewk WebView", evas_object);    \
      return __VA_ARGS__;                                                 \
    }                                                                     \
  } while (0)

#endif // ewk_view_private_h
