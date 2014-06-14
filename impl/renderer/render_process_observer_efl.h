#ifndef RENDER_PROCESS_OBSERVER_EFL_H
#define RENDER_PROCESS_OBSERVER_EFL_H

#include <string>
#include "base/compiler_specific.h"
#include "content/public/renderer/render_process_observer.h"
#include "common/render_messages_efl.h"
#include "cache_params_efl.h"

namespace IPC {
class Message;
}

class ContentRendererClientEfl;

class RenderProcessObserverEfl : public content::RenderProcessObserver {
public:
  explicit RenderProcessObserverEfl(ContentRendererClientEfl* content_client)
      : content_client_(content_client), webkit_initialized_(false) { }

  virtual bool OnControlMessageReceived(const IPC::Message& message) OVERRIDE;
  virtual void WebKitInitialized() OVERRIDE;
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
