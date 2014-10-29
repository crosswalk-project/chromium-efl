// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "dynamicplugin.h"

#include <dlfcn.h>
#include <base/logging.h>

namespace {
const char* const INJECTED_BUNDLE_LIBRARY = "/usr/lib/libwrt-injected-bundle.so";
const char* const START_SESSION_FUNCTION = "DynamicPluginStartSession";
const char* const URL_PARSING_FUNCTION = "DynamicUrlParsing";
const char* const DATABASE_ATTACH_FUNCTION = "DynamicDatabaseAttach";
}

DynamicPlugin::DynamicPlugin()
  : m_handle(0),
    m_startSession(0),
    m_parseURL(0),
    m_databaseAttach(0) {
  m_handle = dlopen(INJECTED_BUNDLE_LIBRARY, RTLD_LAZY);
  if (!m_handle) {
    LOG(ERROR) << "No handle to " << INJECTED_BUNDLE_LIBRARY <<  " " << dlerror() << "\n";
    return;
  }

  *reinterpret_cast<void **>(&m_startSession) = dlsym(m_handle, START_SESSION_FUNCTION);
  if (!m_startSession) {
    LOG(ERROR) << "No " << START_SESSION_FUNCTION << " symbol found!\n";
  }
  *reinterpret_cast<void **>(&m_parseURL) = dlsym(m_handle, URL_PARSING_FUNCTION);
  if (!m_parseURL) {
    LOG(ERROR) << "No " << URL_PARSING_FUNCTION << " symbol found!\n";
  }
  *reinterpret_cast<void **>(&m_databaseAttach) = dlsym(m_handle, DATABASE_ATTACH_FUNCTION);
  if (!m_databaseAttach) {
    LOG(ERROR) << "No " << DATABASE_ATTACH_FUNCTION << " symbol found!\n";
    return;
  }
 *reinterpret_cast<void **>(&m_onIPCMessage) = dlsym(m_handle, "DynamicOnIPCMessage");
  if (!m_onIPCMessage) {
      LOG(ERROR) << "No DynamicOnIPCMessage symbol found!\n";
  }
  m_databaseAttach(1);
}

void DynamicPlugin::startSession(int widgetHandle,
                                 v8::Handle<v8::Context> context,
                                 int routingHandle,
                                 double scaleFactor,
                                 const char* encodedBundle,
                                 const char* theme) {
  if (!m_startSession || !m_databaseAttach)
    return;
  m_startSession(widgetHandle, context, routingHandle, scaleFactor, encodedBundle, theme);
}

void DynamicPlugin::parseURL(std::string* old_url, std::string* new_url, int widget_id) {
  if (!m_parseURL || !m_databaseAttach)
    return;
  m_parseURL(old_url, new_url, widget_id);
}

DynamicPlugin::~DynamicPlugin() {
  if (m_databaseAttach)
    m_databaseAttach(0);
  if (m_handle)
    dlclose(m_handle);
}

DynamicPlugin& DynamicPlugin::instance() {
  static DynamicPlugin dynamicPlugin;
  return dynamicPlugin;
}

void DynamicPlugin::messageReceived(const tizen_webview::WrtIpcMessageData& data) {
  if (!m_onIPCMessage)
    return;

  m_onIPCMessage(data);
}
