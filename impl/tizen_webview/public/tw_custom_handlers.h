// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TIZEN_WEBVIEW_PUBLIC_TW_CUSTOM_HANDLERS_H_
#define TIZEN_WEBVIEW_PUBLIC_TW_CUSTOM_HANDLERS_H_

#include <Evas.h>
#include "tizen_webview/public/tw_export.h"

namespace tizen_webview {

enum Custom_Handlers_State {
  TW_CUSTOM_HANDLERS_NEW,        // Indicates that no attempt has been made to register the given handler.
  TW_CUSTOM_HANDLERS_REGISTERED, // Indicates that the given handler has been registered or that the site is blocked from registering the handler.
  TW_CUSTOM_HANDLERS_DECLINED    // Indicates that the given handler has been offered but was rejected.
};

class TW_EXPORT Custom_Handlers_Data {
 public:
  Custom_Handlers_Data();
  Custom_Handlers_Data(const char* protocol, const char* baseUrl,
      const char* full_url, const char* handler_title = NULL);
  ~Custom_Handlers_Data();

  Eina_Stringshare* getTarget() const;
  Eina_Stringshare* getBaseUrl() const;
  Eina_Stringshare* getUrl() const;
  Eina_Stringshare* getTitle() const;
  Custom_Handlers_State getResult() const;
  void setResult(Custom_Handlers_State result_);

 private:
  Eina_Stringshare* target;
  Eina_Stringshare* base_url;
  Eina_Stringshare* url;
  Eina_Stringshare* title;
  Custom_Handlers_State result;
};

} // namespace tizen_webview

#endif  // TIZEN_WEBVIEW_PUBLIC_TW_CUSTOM_HANDLERS_H_
