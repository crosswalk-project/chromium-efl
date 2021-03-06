// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef RENDER_PROCESS_OBSERVER_EFL_H
#define RENDER_PROCESS_OBSERVER_EFL_H

#include <string>
#include "base/compiler_specific.h"
#include "content/public/renderer/render_process_observer.h"
#include "cache_params_efl.h"

namespace IPC {
class Message;
}

namespace tizen_webview {
class WrtIpcMessageData;
}

class ContentRendererClientEfl;

class RenderProcessObserverEfl : public content::RenderProcessObserver {
public:
  explicit RenderProcessObserverEfl(ContentRendererClientEfl* content_client);
  bool OnControlMessageReceived(const IPC::Message& message) override;
  void WebKitInitialized() override;
  void OnClearCache();
  void OnPurgeMemory();
  
private:
  void OnWidgetInfo(int widget_id,
                    double scale,
                    const std::string &theme,
                    const std::string &encoded_bundle);
  void OnSetCache(const CacheParamsEfl& params);
  void OnWrtMessage(const tizen_webview::WrtIpcMessageData& data);
  ContentRendererClientEfl* content_client_;
  bool webkit_initialized_;
  CacheParamsEfl pending_cache_params_;
};

#endif
