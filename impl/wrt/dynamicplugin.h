/*
   Copyright (C) 2014 Samsung Electronics

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

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
