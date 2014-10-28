// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "renderer/render_process_observer_efl.h"

#include "base/allocator/allocator_extension.h"
#include "base/command_line.h"
#include "common/content_switches_efl.h"
#include "content/public/renderer/render_thread.h"
#include "third_party/WebKit/public/web/WebCache.h"
#include "third_party/sqlite/sqlite3.h"
#include "v8/include/v8.h"
#include "renderer/content_renderer_client_efl.h"

#include "third_party/WebKit/public/web/WebRuntimeFeatures.h"
// XXX:  config.h needs to be included before internal blink headers.
// XXX2: It'd be great if we did not include internal blibk headers.
#include "third_party/WebKit/Source/config.h"
#include "third_party/WebKit/Source/platform/fonts/FontCache.h"


using blink::WebCache;
using blink::WebRuntimeFeatures;
using content::RenderThread;

RenderProcessObserverEfl::RenderProcessObserverEfl(ContentRendererClientEfl* content_client)
    : content_client_(content_client), webkit_initialized_(false) {
  const CommandLine& command_line = *CommandLine::ForCurrentProcess();
  if (command_line.HasSwitch(switches::kEnableViewMode))
    WebRuntimeFeatures::enableCSSViewModeMediaFeature(true);
}

bool RenderProcessObserverEfl::OnControlMessageReceived(const IPC::Message& message)
{
  bool handled = true;
  IPC_BEGIN_MESSAGE_MAP(RenderProcessObserverEfl, message)
    IPC_MESSAGE_HANDLER(EflViewMsg_ClearCache, OnClearCache)
    IPC_MESSAGE_HANDLER(EflViewMsg_SetCache, OnSetCache)
    IPC_MESSAGE_HANDLER(EwkViewMsg_PurgeMemory, OnPurgeMemory)
    IPC_MESSAGE_HANDLER(EwkViewMsg_SetWidgetInfo, OnWidgetInfo)
    IPC_MESSAGE_HANDLER(EwkViewMsg_SendWrtMessage, OnWrtMessage)
    IPC_MESSAGE_UNHANDLED(handled = false)
  IPC_END_MESSAGE_MAP()
  return handled;
}

void RenderProcessObserverEfl::OnWidgetInfo(int widget_id,
                                            double scale,
                                            const std::string &theme,
                                            const std::string &encoded_bundle)
{
  content_client_->SetWidgetInfo(widget_id, scale, encoded_bundle, theme);
}

void RenderProcessObserverEfl::OnWrtMessage(const tizen_webview::WrtIpcMessageData& data)
{
  content_client_->WrtMessageReceived(data);
}

void RenderProcessObserverEfl::WebKitInitialized()
{
  webkit_initialized_ = true;
  OnSetCache(pending_cache_params_);
}
void RenderProcessObserverEfl::OnClearCache()
{
  WebCache::clear();
}

void RenderProcessObserverEfl::OnSetCache(const CacheParamsEfl& params)
{
  pending_cache_params_ = params;
  if(!webkit_initialized_)
    return;

  WebCache::setCapacities(static_cast<size_t>(params.cache_min_dead_capacity),
    static_cast<size_t>(params.cache_max_dead_capacity),
    static_cast<size_t>(params.cache_total_capacity));
}

void RenderProcessObserverEfl::OnPurgeMemory()
{
  RenderThread::Get()->EnsureWebKitInitialized();
  // Clear the object cache (as much as possible; some live objects cannot be
  // freed).
  OnClearCache();
  // Clear the font/glyph cache.
  blink::FontCache::fontCache()->invalidate();
  // TODO(pk): currently web process not linking sqlite. when used this should enable
  // Release all freeable memory from the SQLite process-global page cache (a
  // low-level object which backs the Connection-specific page caches).
  //while (sqlite3_release_memory(std::numeric_limits<int>::max()) > 0) {
  //}
  v8::Isolate::GetCurrent()->LowMemoryNotification();
  // Tell our allocator to release any free pages it's still holding.
  base::allocator::ReleaseFreeMemory();
}
