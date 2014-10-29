// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "wrtwidget.h"

#include "wrt/dynamicplugin.h"
#include <base/logging.h>

WrtWidget::WrtWidget()
 : widgetHandle_(-1) {
}

void WrtWidget::SetWidgetInfo(int widgetHandle,
                              double scaleFactor,
                              const std::string& encodedBundle,
                              const std::string& theme) {
  widgetHandle_ = widgetHandle;
  scale_ = scaleFactor;
  theme_ = theme;
  encodedBundle_ = encodedBundle;
}

void WrtWidget::ParseUrl(const GURL& url, GURL& new_url) {
  if (widgetHandle_ != -1) {
    std::string old_url = url.possibly_invalid_spec();
    std::string s_new_url;
    DynamicPlugin::instance().parseURL(&old_url, &s_new_url, widgetHandle_);
    new_url = GURL(s_new_url);
  } else {
    new_url = url;
  }
}

void WrtWidget::StartSession(v8::Handle<v8::Context> context,
                             int routingHandle) {
  if (widgetHandle_ != -1 && !context.IsEmpty()) {
    DynamicPlugin::instance().startSession(widgetHandle_,
                                           context,
                                           routingHandle,
                                           scale_,
                                           encodedBundle_.c_str(),
                                           theme_.c_str());
  }
}

void WrtWidget::StopSession(v8::Handle<v8::Context> context) {
  //TODO: stop session
}

void WrtWidget::messageReceived(const tizen_webview::WrtIpcMessageData& data) {
  if (widgetHandle_ != -1)
    DynamicPlugin::instance().messageReceived(data);
}
