// Copyright 2014 Samsung Electronics. All rights reseoved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WEB_CACHE_MANAGER_EFL_H
#define WEB_CACHE_MANAGER_EFL_H

#include <set>

#include "base/compiler_specific.h"
#include "base/memory/singleton.h"
#include "content/public/browser/notification_observer.h"
#include "content/public/browser/notification_registrar.h"
#include "public/ewk_context.h"

namespace content {
class BrowserContext;
}
struct CacheParamsEfl;

class WebCacheManagerEfl : public content::NotificationObserver {
 public:
  explicit WebCacheManagerEfl(content::BrowserContext* browser_context);
  virtual ~WebCacheManagerEfl();

  // content::NotificationObserver implementation:
  virtual void Observe(int type,
                       const content::NotificationSource& source,
                       const content::NotificationDetails& details) OVERRIDE;
  void ClearCache();
  void SetCacheModel(Ewk_Cache_Model model);
  Ewk_Cache_Model GetCacheModel() const { return cache_model_; }
  void SetBrowserContext(content::BrowserContext* browser_context);

 private:
  static void CalculateCacheSizes(Ewk_Cache_Model cache_model, int64 memory_size, int64 disk_free_size,
      int64* cache_total_capacity, int64* cache_min_dead_capacity,
      int64* cache_max_dead_capacity, double* dead_decoded_data_deletion_interval,
      int64* page_cache_capacity, int64* url_cache_memory_capacity,
      int64* url_cache_disk_capacity);

  CacheParamsEfl GetCacheParamsFromModel(Ewk_Cache_Model);
  void SetRenderProcessCacheModel(Ewk_Cache_Model model, int render_process_id);

  content::NotificationRegistrar registrar_;
  std::set<int> renderers_;
  content::BrowserContext* browser_context_;
  Ewk_Cache_Model cache_model_;
  DISALLOW_COPY_AND_ASSIGN(WebCacheManagerEfl);
};

#endif

