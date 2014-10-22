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
