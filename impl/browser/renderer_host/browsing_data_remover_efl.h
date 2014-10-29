// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BROWSING_DATA_MANAGER_EFL_H
#define BROWSING_DATA_MANAGER_EFL_H

#include <set>

#include "base/compiler_specific.h"
#include "base/memory/ref_counted.h"
#include "base/time/time.h"
#include "content/public/browser/notification_observer.h"
#include "content/public/browser/notification_registrar.h"
#include "storage/common/quota/quota_types.h"
#include "url/gurl.h"

namespace net {
class URLRequestContextGetter;
}
namespace content {
class AppCacheService;
class BrowserContext;
class DOMStorageContext;
struct LocalStorageUsageInfo;
class StoragePartition;
}
namespace disk_cache {
class Backend;
}
namespace storage {
class QuotaManager;
}
class BrowsingDataRemoverEfl {
 public:
  // Mask used for Remove.
  enum RemoveDataMask {
    REMOVE_LOCAL_STORAGE = 1 << 0,
    REMOVE_INDEXEDDB     = 1 << 1,
    REMOVE_WEBSQL        = 1 << 2,
    REMOVE_FILE_SYSTEMS  = 1 << 3,
    REMOVE_APPCACHE      = 1 << 4,
  };

  static BrowsingDataRemoverEfl* CreateForUnboundedRange(content::BrowserContext*);
  static BrowsingDataRemoverEfl* CreateForRange(content::BrowserContext*, base::Time, base::Time);
  void RemoveImpl(int, const GURL&);

  virtual ~BrowsingDataRemoverEfl();
  void ClearNetworkCache();

  // deletes app cache for given origin
  void DeleteAppCachesForOrigin(const GURL& origin);

 protected:
  BrowsingDataRemoverEfl(content::BrowserContext*, base::Time start, base::Time end);

  // Quota managed data uses a different bitmask for types than
  // BrowsingDataRemover uses. This method generates that mask.
  static int GenerateQuotaClientMask(int);

 private:
  void ClearNetworkCacheOnIOThread();

  // Callback when the cache has been cleared.
  void DoClearCache(int);

  // Invoked on the UI thread to delete local storage.
  void ClearLocalStorageOnUIThread();

  // Callback to deal with the list gathered in ClearLocalStorageOnUIThread.
  void OnGotLocalStorageUsageInfo(const std::vector<content::LocalStorageUsageInfo>&);

  // Invoked on the IO thread to delete all storage types managed by the quota
  // system: AppCache, Databases, FileSystems.
  void ClearQuotaManagedDataOnIOThread();

  // Callback to respond to QuotaManager::GetOriginsModifiedSince, which is the
  // core of 'ClearQuotaManagedDataOnIOThread'.
  void OnGotQuotaManagedOrigins(const std::set<GURL>&, storage::StorageType);

  // Callback responding to deletion of a single quota managed origin's
  // persistent data
  void OnQuotaManagedOriginDeletion(const GURL&, storage::StorageType, storage::QuotaStatusCode);

  // Called to check whether all temporary and persistent origin data that
  // should be deleted has been deleted. If everything's good to go, invokes
  // OnQuotaManagedDataDeleted on the UI thread.
  void CheckQuotaManagedDataDeletionStatus();

  // Completion handler that runs on the UI thread once persistent data has been
  // deleted. Updates the waiting flag and invokes NotifyAndDeleteIfDone.
  void OnQuotaManagedDataDeleted();

  void ClearedCache();
  bool AllDone();
  void DeleteIfDone();

  content::BrowserContext* browser_context_;
  content::AppCacheService* app_cache_service_;

  // The QuotaManager is owned by the profile; we can use a raw pointer here,
  // and rely on the profile to destroy the object whenever it's reasonable.
  storage::QuotaManager* quota_manager_;

  // The DOMStorageContext is owned by the profile; we'll store a raw pointer.
  content::DOMStorageContext* dom_storage_context_;

  // Start time to delete from.
  base::Time delete_begin_;

  // End time to delete to.
  base::Time delete_end_;

  enum CacheState {
    STATE_NONE,
    STATE_CREATE_MAIN,
    STATE_CREATE_MEDIA,
    STATE_DELETE_MAIN,
    STATE_DELETE_MEDIA,
    STATE_DONE
  };
  CacheState next_cache_state_;
  disk_cache::Backend* cache_;

  content::NotificationRegistrar registrar_;
  std::set<int> renderers_;

  // Used to delete data from HTTP cache.
  scoped_refptr<net::URLRequestContextGetter> main_context_getter_;
  scoped_refptr<net::URLRequestContextGetter> media_context_getter_;

  bool waiting_for_clear_cache_;
  bool waiting_for_clear_local_storage_;
  bool waiting_for_clear_quota_managed_data_;

  // Tracking how many origins need to be deleted, and whether we're finished
  // gathering origins.
  int quota_managed_origins_to_delete_count_;
  int quota_managed_storage_types_to_delete_count_;

  // The removal mask for the current removal operation.
  int remove_mask_;

  // The origin for the current removal operation.
  GURL remove_origin_;
};
#endif
