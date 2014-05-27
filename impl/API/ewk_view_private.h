/*
 * Copyright (C) 2012 Samsung Electronics
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
 *
 */

#ifndef ewk_view_private_h
#define ewk_view_private_h

#include <Evas.h>
#include "public/ewk_custom_handlers.h"
#include "webkit/common/webpreferences.h"

struct _Ewk_Custom_Handlers_Data {
  _Ewk_Custom_Handlers_Data()
      : target(0),
        base_url(0),
        url(0),
        title(0),
        result(EWK_CUSTOM_HANDLERS_NEW) { }

  _Ewk_Custom_Handlers_Data(const char* protocol, const char* baseUrl, const char* full_url,
      const char* handler_title = NULL)
      : target(eina_stringshare_add(protocol)),
        base_url(eina_stringshare_add(baseUrl)),
        url(eina_stringshare_add(full_url)),
        title(eina_stringshare_add(handler_title)),
        result(EWK_CUSTOM_HANDLERS_NEW) { }

  ~_Ewk_Custom_Handlers_Data() {
    eina_stringshare_del(target);
    eina_stringshare_del(base_url);
    eina_stringshare_del(url);
    eina_stringshare_del(title);
  }

  Eina_Stringshare* target;
  Eina_Stringshare* base_url;
  Eina_Stringshare* url;
  Eina_Stringshare* title;
  Ewk_Custom_Handlers_State result;
};

Ewk_Custom_Handlers_Data* ewkCustomHandlersCreateData(const char* target, const char* baseUrl, const char* url, const char* title = NULL);
Ewk_Custom_Handlers_State ewkGetCustomHandlersDataResult(const Ewk_Custom_Handlers_Data* customHandlersData);
void ewkCustomHandlersDeleteData(Ewk_Custom_Handlers_Data* customHandlersData);

void ewkViewRegisterProtocolHandlers(Evas_Object* ewkView, const char* scheme, const char* baseURL, const char* url, const char* title);
Ewk_Custom_Handlers_State ewkViewIsProtocolHandlerRegistered(Evas_Object* ewkView, const char* scheme, const char* baseURL, const char* url);
void ewkViewUnregisterProtocolHandlers(Evas_Object* ewkView, const char* scheme, const char* baseUrl, const char* url);

void ewkViewRegisterContentHandlers(Evas_Object* ewkView, const char* mimeType, const char* baseURL, const char* url, const char* title);
Ewk_Custom_Handlers_State ewkViewIsContentHandlerRegistered(Evas_Object* ewkView, const char* mimeType, const char* baseURL, const char* url);
void ewkViewUnregisterContentHandlers(Evas_Object* ewkView, const char* mimeType, const char* baseUrl, const char* url);

void ewkUpdateWebkitPreferences(Evas_Object* ewkView);

#endif // ewk_view_private_h
