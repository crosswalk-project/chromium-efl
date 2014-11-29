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
