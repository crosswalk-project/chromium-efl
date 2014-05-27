// Copyright 2014 Samsung Electronics. All rights reseoved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "browser/renderer_host/web_cache_manager_efl.h"
#include "common/render_messages_efl.h"
#include "cache_params_efl.h"

#include "base/logging.h"
#include "base/sys_info.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/notification_service.h"
#include "content/public/browser/notification_source.h"
#include "content/public/browser/notification_types.h"
#include "content/public/browser/render_process_host.h"

WebCacheManagerEfl::WebCacheManagerEfl(content::BrowserContext* browser_context)
    : browser_context_(browser_context),
      cache_model_(EWK_CACHE_MODEL_DOCUMENT_VIEWER)
{
  registrar_.Add(this, content::NOTIFICATION_RENDERER_PROCESS_CREATED,
                 content::NotificationService::AllBrowserContextsAndSources());
  registrar_.Add(this, content::NOTIFICATION_RENDERER_PROCESS_TERMINATED,
                 content::NotificationService::AllBrowserContextsAndSources());
}

WebCacheManagerEfl::~WebCacheManagerEfl() { }

void WebCacheManagerEfl::Observe(int type,
                              const content::NotificationSource& source,
                              const content::NotificationDetails& details)
{
  content::RenderProcessHost* process =
            content::Source<content::RenderProcessHost>(source).ptr();
  DCHECK(process);
  if (process->GetBrowserContext() != browser_context_)
    return;

  int renderer_id = process->GetID();
  switch (type) {
    case content::NOTIFICATION_RENDERER_PROCESS_CREATED: {
      renderers_.insert(renderer_id);
      SetRenderProcessCacheModel(cache_model_, renderer_id);
      break;
    }
    case content::NOTIFICATION_RENDERER_PROCESS_TERMINATED: {
      renderers_.erase(renderer_id);
      break;
    }
    default:
      NOTREACHED();
      break;
  }
}

void WebCacheManagerEfl::ClearCache()
{
  for (std::set<int>::const_iterator iter = renderers_.begin();
      iter != renderers_.end(); ++iter) {
    content::RenderProcessHost* host = content::RenderProcessHost::FromID(*iter);
    if (host)
      host->Send(new EflViewMsg_ClearCache());
  }
}

void WebCacheManagerEfl::SetRenderProcessCacheModel(Ewk_Cache_Model model, int render_process_id)
{
  DCHECK(render_process_id);
  DCHECK(renderers_.find(render_process_id) != renderers_.end());
  content::RenderProcessHost* host = content::RenderProcessHost::FromID(render_process_id);
  if (host)
    host->Send(new EflViewMsg_SetCache(GetCacheParamsFromModel(model)));
}

void WebCacheManagerEfl::SetCacheModel(Ewk_Cache_Model model)
{
  cache_model_ = model;
  CacheParamsEfl cache_params = GetCacheParamsFromModel(model);
  for (std::set<int>::const_iterator iter = renderers_.begin();
          iter != renderers_.end(); ++iter) {
    content::RenderProcessHost* host = content::RenderProcessHost::FromID(*iter);
    if (host)
      host->Send(new EflViewMsg_SetCache(cache_params));
  }
}

CacheParamsEfl WebCacheManagerEfl::GetCacheParamsFromModel(Ewk_Cache_Model cache_model)
{
  int64 mem_size = base::SysInfo::AmountOfPhysicalMemory();
  // in chromium the limiting parameter is max file size of network cache
  // which is set at the time of creation of backend.
  // backend could be completely in-memory as well.
  // in WK2/Tizen, it is free space on disk
  // So, we are using free space available on browser_context path
  // we dont set it on renderer side anyway.
  int64 disk_free_size = base::SysInfo::AmountOfFreeDiskSpace(browser_context_->GetPath());
  int64 cache_min_dead_capacity = 0;
  int64 cache_max_dead_capacity = 0;
  int64 cache_total_capacity = 0;
  double dead_decoded_data_deletion_interval = 0;
  int64 page_cache_capacity = 0;

  int64 url_cache_memory_capacity = 0;
  int64 url_cache_disk_capacity = 0;

  // We have taken this calculation from WK2.
  // Except that, we are using pointers instead of reference for output.
  // We don't use page_cache_capacity. NavigationControllerImpl has
  // kMaxSessionHistoryEntries = 50 for back/forward list.
  // url_cache_memory_capacity, url_cache_disk_capacity seem to be parts of
  // network layer. disk_cache::CreateCacheBackend gets size while creation.
  // Could not find anything to modify it on-the-fly.
  // m_deadDecodedDataDeletionInterval is unused variable in MemoryCache.cpp in
  // WK2 CachedResource uses this to start timer.
  // So, We are using only cache_total_capacity, cache_min_dead_capacity, and
  // cache_max_dead_capacity. This is same as Chromium cache manager implementation
  // But we are using following calculation instead of dynamic strategy used in Chromium.
  CalculateCacheSizes(cache_model, mem_size, disk_free_size,
                      &cache_total_capacity, &cache_min_dead_capacity, &cache_max_dead_capacity,
                      &dead_decoded_data_deletion_interval, &page_cache_capacity,
                      &url_cache_memory_capacity, &url_cache_disk_capacity);
  CacheParamsEfl cache_params;
  cache_params.cache_max_dead_capacity = cache_max_dead_capacity;
  cache_params.cache_min_dead_capacity = cache_min_dead_capacity;
  cache_params.cache_total_capacity = cache_total_capacity;
  return cache_params;
}

// taken from WK2/Tizen
// static
void WebCacheManagerEfl::CalculateCacheSizes(Ewk_Cache_Model cache_model, int64 memory_size, int64 disk_free_size,
    int64* cache_total_capacity, int64* cache_min_dead_capacity,
    int64* cache_max_dead_capacity, double* dead_decoded_data_deletion_interval,
    int64* page_cache_capacity, int64* url_cache_memory_capacity,
    int64* url_cache_disk_capacity)
{
  switch (cache_model) {
    case EWK_CACHE_MODEL_DOCUMENT_VIEWER: {
      // Page cache capacity (in pages)
      *page_cache_capacity = 0;

      // Object cache capacities (in bytes)
      if (memory_size >= 2048)
        *cache_total_capacity = 96 * 1024 * 1024;
      else if (memory_size >= 1536)
        *cache_total_capacity = 64 * 1024 * 1024;
      else if (memory_size >= 1024)
        *cache_total_capacity = 32 * 1024 * 1024;
      else if (memory_size >= 512)
        *cache_total_capacity = 16 * 1024 * 1024;

      *cache_min_dead_capacity = 0;
      *cache_max_dead_capacity = 0;

      // Foundation memory cache capacity (in bytes)
      *url_cache_memory_capacity = 0;

      // Foundation disk cache capacity (in bytes)
      *url_cache_disk_capacity = 0;

      break;
    }
    case EWK_CACHE_MODEL_DOCUMENT_BROWSER: {
      // Page cache capacity (in pages)
      if (memory_size >= 1024)
        *page_cache_capacity = 3;
      else if (memory_size >= 512)
        *page_cache_capacity = 2;
      else if (memory_size >= 256)
        *page_cache_capacity = 1;
      else
        *page_cache_capacity = 0;

      // Object cache capacities (in bytes)
      if (memory_size >= 2048)
        *cache_total_capacity = 96 * 1024 * 1024;
      else if (memory_size >= 1536)
        *cache_total_capacity = 64 * 1024 * 1024;
      else if (memory_size >= 1024)
        *cache_total_capacity = 32 * 1024 * 1024;
      else if (memory_size >= 512)
        *cache_total_capacity = 16 * 1024 * 1024;

      *cache_min_dead_capacity = *cache_total_capacity / 8;
      *cache_max_dead_capacity = *cache_total_capacity / 4;

      // Foundation memory cache capacity (in bytes)
      if (memory_size >= 2048)
        *url_cache_memory_capacity = 4 * 1024 * 1024;
      else if (memory_size >= 1024)
        *url_cache_memory_capacity = 2 * 1024 * 1024;
      else if (memory_size >= 512)
        *url_cache_memory_capacity = 1 * 1024 * 1024;
      else
        *url_cache_memory_capacity =      512 * 1024;

      // Foundation disk cache capacity (in bytes)
      if (disk_free_size >= 16384)
        *url_cache_disk_capacity = 50 * 1024 * 1024;
      else if (disk_free_size >= 8192)
        *url_cache_disk_capacity = 40 * 1024 * 1024;
      else if (disk_free_size >= 4096)
        *url_cache_disk_capacity = 30 * 1024 * 1024;
      else
        *url_cache_disk_capacity = 20 * 1024 * 1024;

      break;
    }
    case EWK_CACHE_MODEL_PRIMARY_WEBBROWSER: {
      // Page cache capacity (in pages)
      // (Research indicates that value / page drops substantially after 3 pages.)
      if (memory_size >= 2048)
        *page_cache_capacity = 5;
      else if (memory_size >= 1024)
        *page_cache_capacity = 4;
      else if (memory_size >= 512)
        *page_cache_capacity = 3;
      else if (memory_size >= 256)
        *page_cache_capacity = 2;
      else
        *page_cache_capacity = 1;

      // Object cache capacities (in bytes)
      // (Testing indicates that value / MB depends heavily on content and
      // browsing pattern. Even growth above 128MB can have substantial
      // value / MB for some content / browsing patterns.)
      if (memory_size >= 2048)
        *cache_total_capacity = 128 * 1024 * 1024;
      else if (memory_size >= 1536)
        *cache_total_capacity = 96 * 1024 * 1024;
      else if (memory_size >= 1024)
        *cache_total_capacity = 64 * 1024 * 1024;
      else if (memory_size >= 512)
        *cache_total_capacity = 32 * 1024 * 1024;

      *cache_min_dead_capacity = *cache_total_capacity / 4;
      *cache_max_dead_capacity = *cache_total_capacity / 2;

      // This code is here to avoid a PLT regression. We can remove it if we
      // can prove that the overall system gain would justify the regression.
      *cache_max_dead_capacity = std::max(static_cast<int64>(24), *cache_max_dead_capacity);

      *dead_decoded_data_deletion_interval = 60;

      // Foundation memory cache capacity (in bytes)
      // (These values are small because WebCore does most caching itself.)
      if (memory_size >= 1024)
        *url_cache_memory_capacity = 4 * 1024 * 1024;
      else if (memory_size >= 512)
        *url_cache_memory_capacity = 2 * 1024 * 1024;
      else if (memory_size >= 256)
        *url_cache_memory_capacity = 1 * 1024 * 1024;
      else
        *url_cache_memory_capacity =      512 * 1024;

      // Foundation disk cache capacity (in bytes)
      if (disk_free_size >= 16384)
        *url_cache_disk_capacity = 175 * 1024 * 1024;
      else if (disk_free_size >= 8192)
        *url_cache_disk_capacity = 150 * 1024 * 1024;
      else if (disk_free_size >= 4096)
        *url_cache_disk_capacity = 125 * 1024 * 1024;
      else if (disk_free_size >= 2048)
        *url_cache_disk_capacity = 100 * 1024 * 1024;
      else if (disk_free_size >= 1024)
        *url_cache_disk_capacity = 75 * 1024 * 1024;
      else
        *url_cache_disk_capacity = 50 * 1024 * 1024;

      break;
    }
    default:
       NOTREACHED();
       break;
  };
}
void WebCacheManagerEfl::SetBrowserContext(content::BrowserContext* browser_context) {
  DCHECK(!browser_context_);
  browser_context_ = browser_context;
}
