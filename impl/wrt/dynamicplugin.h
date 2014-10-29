// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DYNAMICPLUGIN_H
#define DYNAMICPLUGIN_H

#include <string>
#include "v8/include/v8.h"


namespace tizen_webview {
class WrtIpcMessageData;
}

typedef void (*startSessionFun)(int,
                                v8::Handle<v8::Context>,
                                int,
                                double,
                                const char*,
                                const char*);

typedef void (*onIPCMessageFun)(const tizen_webview::WrtIpcMessageData&);

typedef void (*parseUrlFun)(std::string*, std::string*, int);
typedef void (*databaseAttachFun)(int);

class DynamicPlugin {
 public:
  void startSession(int widgetHandle,
                    v8::Handle<v8::Context> context,
                    int routingHandle,
                    double scaleFactor,
                    const char* encodedBundle,
                    const char* theme);

  void parseURL(std::string* old_url, std::string* new_url, int widget_id);

  void messageReceived(const tizen_webview::WrtIpcMessageData& data);

  ~DynamicPlugin();

  static DynamicPlugin& instance();

 private:
  DynamicPlugin();
  DynamicPlugin(const DynamicPlugin&);
  DynamicPlugin& operator=(const DynamicPlugin&);

  void* m_handle;
  startSessionFun m_startSession;
  parseUrlFun m_parseURL;
  databaseAttachFun m_databaseAttach;
  onIPCMessageFun m_onIPCMessage;
};

#endif // DYNAMICPLUGIN_H
