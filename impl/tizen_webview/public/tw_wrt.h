// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TIZEN_WEBVIEW_PUBLIC_TW_WRT_H_
#define TIZEN_WEBVIEW_PUBLIC_TW_WRT_H_

#include <string>

namespace tizen_webview {

struct WrtIpcMessageData {
  std::string type;
  std::string value;
  std::string id;
  std::string reference_id;

  const char* GetType() const;
  void SetType(const char* val);
  const char* GetValue() const;
  void SetValue(const char* val);
  const char* GetId() const;
  void SetId(const char* val);
  const char* GetReferenceId() const;
  void SetReferenceId(const char* val);
};

class WebContext;

struct WrtIpc {
  static bool SendMessage(WebContext* ctx, const WrtIpcMessageData& data);
};

struct WrtPluginsIpc {
  static bool SendMessage(int routing_id, const WrtIpcMessageData& data);
  static bool SendSyncMessage(int routing_id, WrtIpcMessageData* data);
};

} // namespace tizen_webview

#endif  // TIZEN_WEBVIEW_PUBLIC_TW_WRT_H_
