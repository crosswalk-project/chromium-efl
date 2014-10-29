// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WRTWIDGET_H
#define WRTWIDGET_H

#include <string>
#include "url/gurl.h"
#include "v8/include/v8.h"

namespace tizen_webview {
class WrtIpcMessageData;
}

class WrtWidget {
 public:
  WrtWidget();

  void SetWidgetInfo(int widgetHandle,
                     double scaleFactor,
                     const std::string& encodedBundle,
                     const std::string& theme);

  void messageReceived(const tizen_webview::WrtIpcMessageData& data);

  void ParseUrl(const GURL& url, GURL& new_url);

  void StartSession(v8::Handle<v8::Context>,
                    int routingHandle);

  void StopSession(v8::Handle<v8::Context>);

 private:
  int widgetHandle_;
  double scale_;
  std::string theme_;
  std::string encodedBundle_;
};

#endif // WRTWIDGET_H
