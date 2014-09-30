#ifndef CONTER_RENDERER_CLIENT_EFL_H
#define CONTER_RENDERER_CLIENT_EFL_H

#include "content/public/renderer/content_renderer_client.h"
#include "v8/include/v8.h"
#include "renderer/render_process_observer_efl.h"

namespace content {
class RenderView;
class RenderFrame;
class DocumentState;
}

namespace tizen_webview {
class WrtIpcMessageData;
}

namespace visitedlink {
class VisitedLinkSlave;
}

class WrtWidget;

class ContentRendererClientEfl : public content::ContentRendererClient
{
 public:
  ContentRendererClientEfl();
  ~ContentRendererClientEfl();

  virtual void RenderThreadStarted();

  virtual void RenderViewCreated(content::RenderView* render_view);

  void SetWidgetInfo(int widgetHandle,
                     double scaleFactor,
                     const std::string &encodedBundle,
                     const std::string &theme);

  void WrtMessageReceived(const tizen_webview::WrtIpcMessageData& data);

  virtual void DidCreateScriptContext(blink::WebFrame* frame,
                                      v8::Handle<v8::Context> context,
                                      int extension_group,
                                      int world_id);

  virtual void WillReleaseScriptContext(blink::WebFrame* frame,
                                        v8::Handle<v8::Context>,
                                        int world_id);

#if !defined(EWK_BRINGUP)
  // Note that this API requires a change in
  // content/public/renderer/content_renderer_client.h
  // The base class declares this method for OS_ANDROID only.
  bool HandleNavigation(content::RenderFrame* render_frame,
                        content::DocumentState* document_state,
                        int opener_id,
                        blink::WebFrame* frame,
                        const blink::WebURLRequest& request,
                        blink::WebNavigationType type,
                        blink::WebNavigationPolicy default_policy,
                        bool is_redirect) OVERRIDE;
#endif

  virtual bool WillSendRequest(blink::WebFrame* frame,
                               ui::PageTransition transition_type,
                               const GURL& url,
                               const GURL& first_party_for_cookies,
                               GURL* new_url);

  unsigned long long VisitedLinkHash(const char* canonical_url,
                                     size_t length) OVERRIDE;

  bool IsLinkVisited(unsigned long long link_hash) OVERRIDE;

 private:
  scoped_ptr<WrtWidget> wrt_widget_;
  scoped_ptr<RenderProcessObserverEfl> render_process_observer_;
  scoped_ptr<visitedlink::VisitedLinkSlave> visited_link_slave_;
};

#endif
