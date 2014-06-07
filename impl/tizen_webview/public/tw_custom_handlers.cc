// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "tw_custom_handlers.h"

namespace tizen_webview {

Custom_Handlers_Data::Custom_Handlers_Data()
  : target(0)
  , base_url(0)
  , url(0)
  , title(0)
  , result(TW_CUSTOM_HANDLERS_NEW) {
}

Custom_Handlers_Data::Custom_Handlers_Data(const char* protocol, const char* baseUrl, const char* full_url, const char* handler_title)
  : target(eina_stringshare_add(protocol))
  , base_url(eina_stringshare_add(baseUrl))
  , url(eina_stringshare_add(full_url))
  , title(eina_stringshare_add(handler_title))
  , result(TW_CUSTOM_HANDLERS_NEW) {
}

Custom_Handlers_Data::~Custom_Handlers_Data() {
  eina_stringshare_del(target);
  eina_stringshare_del(base_url);
  eina_stringshare_del(url);
  eina_stringshare_del(title);
}

Eina_Stringshare* Custom_Handlers_Data::getTarget() const {
  return target;
}

Eina_Stringshare* Custom_Handlers_Data::getBaseUrl() const {
  return base_url;
}

Eina_Stringshare* Custom_Handlers_Data::getUrl() const {
  return url;
}

Eina_Stringshare* Custom_Handlers_Data::getTitle() const {
  return title;
}

tizen_webview::Custom_Handlers_State Custom_Handlers_Data::getResult() const {
  return result;
}

void Custom_Handlers_Data::setResult(Custom_Handlers_State result_) {
  result=result_;
}

} // namespace tizen_webview
