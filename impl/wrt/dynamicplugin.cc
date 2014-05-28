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

void DynamicPlugin::messageReceived(const Ewk_IPC_Wrt_Message_Data& data) {
  if(!m_onIPCMessage)
    return;

  m_onIPCMessage(data);
}
