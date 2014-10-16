// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "eweb_context.h"

#include "base/synchronization/waitable_event.h"
#include "components/autofill/content/browser/content_autofill_driver.h"
#include "content/public/browser/appcache_service.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/storage_partition.h"
#include "content/public/browser/render_process_host.h"
#include "content/public/browser/local_storage_usage_info.h"
#include "content/public/browser/dom_storage_context.h"
#include "browser/favicon/favicon_service.h"
#include "browser/webdata/web_data_service_factory.h"
#include "gl/gl_shared_context_efl.h"
#include "net/http/http_cache.h"
#include "net/proxy/proxy_config_service_fixed.h"
#include "net/proxy/proxy_service.h"
#include "storage/browser/database/database_quota_client.h"
#include "storage/browser/fileapi/file_system_quota_client.h"
#include "storage/browser/quota/quota_manager.h"

#if defined(ENABLE_PLUGINS)
#include "content/common/plugin_list.h"
#endif

#include "browser_context_efl.h"
#include "ewk_global_data.h"
#include "memory_purger.h"
#include "browser/password_manager/password_manager.h"
#include "browser/password_manager/password_store_factory.h"
#include "browser/renderer_host/browsing_data_remover_efl.h"
#include "browser/vibration/vibration_provider_client.h"
#include "common/render_messages_efl.h"

#include "tizen_webview/public/tw_security_origin.h"
#include "tizen_webview/public/tw_wrt.h"
#include "tizen_webview/tw_misc_utility.h"

using content::BrowserThread;
using content::BrowserContext;
using content::BrowserContextEfl;

using std::string;
using std::pair;
using std::map;

using tizen_webview::Cache_Model;
using tizen_webview::Security_Origin;
using tizen_webview::GetGURL;
using tizen_webview::GetURL;

EWebContext* EWebContext::default_context_ = NULL;
tizen_webview::Mime_Override_Callback EWebContext::mime_override_callback_ = 0;

namespace {

/**
 * @brief Helper class for obtaining WebStorage origins
 */
class WebStorageGetAllOriginsDispatcher: public base::RefCountedThreadSafe<WebStorageGetAllOriginsDispatcher> {
public:
  WebStorageGetAllOriginsDispatcher(tizen_webview::Web_Storage_Origins_Get_Callback callback, void* user_data)
    : callback_(callback)
    , user_data_(user_data) {
    DCHECK(callback_);
    DCHECK(user_data_);
  }

  void Dispatch(const std::vector<content::LocalStorageUsageInfo> &local_storage) {
    DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));

    Eina_List* lorigins = NULL;

    std::vector<content::LocalStorageUsageInfo>::const_iterator it;

    for (it = local_storage.begin(); it != local_storage.end(); it++) {
      Security_Origin* origin = new Security_Origin(GetURL(it->origin));
      lorigins = eina_list_append(lorigins, origin);
    }

    callback_(lorigins, user_data_);
  }

private:
  tizen_webview::Web_Storage_Origins_Get_Callback callback_;
  void* user_data_;
};

void SetProxyConfigCallbackOnIOThread(base::WaitableEvent* done,
                            net::URLRequestContextGetter* url_request_context_getter,
                            const net::ProxyConfig& proxy_config) {
  net::ProxyService* proxy_service =
      url_request_context_getter->GetURLRequestContext()->proxy_service();
  proxy_service->ResetConfigService(
      new net::ProxyConfigServiceFixed(proxy_config));
  done->Signal();
}

void OnOriginsWithApplicationCacheObtained(tizen_webview::Web_Application_Cache_Origins_Get_Callback callback,
                                           void* user_data,
                                           scoped_refptr<content::AppCacheInfoCollection> collection,
                                           int result) {
  Eina_List* origins = 0;
  for (map<GURL, content::AppCacheInfoVector>::iterator iter = collection->infos_by_origin.begin();
      iter != collection->infos_by_origin.end();
      ++iter) {
    Security_Origin* origin = new Security_Origin(GetURL(iter->first));
    origins = eina_list_append(origins, origin);
  }
  callback(origins, user_data);
}

void OnTemporaryUsageAndQuotaObtained(
    tizen_webview::Web_Application_Cache_Usage_For_Origin_Get_Callback callback,
    void* user_data,
    storage::QuotaStatusCode status_code,
    int64 usage,
    int64 quota) {
  if (status_code != storage::kQuotaStatusOk) {
    LOG(ERROR) << "Error in retrieving usage information";
    // We still trigger callback.
    usage = 0;
  }
  BrowserThread::PostTask(BrowserThread::UI,
                          FROM_HERE,
                          base::Bind(callback, usage, user_data));
}

void OnGetWebDBOrigins(
    tizen_webview::Web_Database_Origins_Get_Callback callback,
    void* user_data,
    const std::set<GURL>& origins_ref) {
  Eina_List* origins = 0;
  for (std::set<GURL>::iterator iter =
         origins_ref.begin();
         iter != origins_ref.end(); ++iter) {
      Security_Origin* sec_origin = new Security_Origin(GetURL(*iter));
      origins = eina_list_append(origins, sec_origin);
  }
  BrowserThread::PostTask(BrowserThread::UI,
                          FROM_HERE,
                          base::Bind(callback, origins, user_data));
}

void GetWebDBOriginsOnDBThread(tizen_webview::Web_Database_Origins_Get_Callback callback,
                               void* user_data,
                               content::StoragePartition* partition) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::DB));
  storage::DatabaseQuotaClient client(base::MessageLoopProxy::current().get(), partition->GetDatabaseTracker());
  client.GetOriginsForType(storage::kStorageTypeTemporary,
                           base::Bind(&OnGetWebDBOrigins, callback, user_data));
}

void OnGetFileSystemOrigins(
    tizen_webview::Local_File_System_Origins_Get_Callback callback,
    void* user_data,
    const std::set<GURL>& origins_ref) {
  Eina_List* origins = 0;
  for (std::set<GURL>::iterator iter =
         origins_ref.begin();
         iter != origins_ref.end(); ++iter) {
      Security_Origin* sec_origin = new Security_Origin(GetURL(*iter));
      origins = eina_list_append(origins, sec_origin);
  }
  BrowserThread::PostTask(BrowserThread::UI,
                          FROM_HERE,
                          base::Bind(callback, origins, user_data));
}

void GetFileSystemOriginsOnFILEThread(tizen_webview::Web_Database_Origins_Get_Callback callback,
                                    void* user_data,
                                    content::StoragePartition* partition) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::FILE));
  scoped_ptr<storage::FileSystemQuotaClient> client( new storage::FileSystemQuotaClient(partition->GetFileSystemContext(), false));

  client->GetOriginsForType(storage::kStorageTypeTemporary,
                            base::Bind(&OnGetFileSystemOrigins, callback, user_data));
}

} // namespace

void EwkDidStartDownloadCallback::TriggerCallback(const string& url) {
  if(callback_)
    (*callback_)(url.c_str(),user_data_);
}

void EWebContext::SendWidgetInfo(int widget_id, double scale, const string &theme, const string &encoded_bundle) {
#if !defined(EWK_BRINGUP)
  content::RenderProcessHost::iterator i(content::RenderProcessHost::AllHostsIterator());
  for (; !i.IsAtEnd(); i.Advance()) {
    i.GetCurrentValue()->Send(
      new EwkViewMsg_SetWidgetInfo(widget_id, scale, theme, encoded_bundle));
  }
#endif
}

void EWebContext::SendWrtMessage(const tizen_webview::WrtIpcMessageData& data) {
#if !defined(EWK_BRINGUP)
  content::RenderProcessHost::iterator i(content::RenderProcessHost::AllHostsIterator());
  for (; !i.IsAtEnd(); i.Advance()) {
    i.GetCurrentValue()->Send(new EwkViewMsg_SendWrtMessage(data));
  }
#endif
}

void EWebContext::SetMimeOverrideCallback(
    tizen_webview::Mime_Override_Callback callback) {
  mime_override_callback_ = callback;
}

bool EWebContext::ShouldOverrideMimeForURL(
    const GURL& url, std::string& mime_type) {
  if (mime_override_callback_) {
    char *new_mime = NULL;
    bool overriden = mime_override_callback_(
        url.spec().c_str(), mime_type.c_str(), &new_mime);
    if (overriden) {
      DCHECK(new_mime);
      mime_type.assign(new_mime);
      ::free(new_mime);
      return true;
    }
  }
  return false;
}

EWebContext::EWebContext()
  : m_pixmap(0)
  , initialized_(false) {

}

void EWebContext::Initialize(Evas_Object* object) {
  if (initialized_)
    return;

  initialized_ = true;

  GLSharedContextEfl::Initialize(object);
  EwkGlobalData::Ensure();

  browser_context_.reset(new BrowserContextEfl(this));
  // Notification Service gets init in BrowserMainRunner init,
  // so cache manager can register for notifications only after that.
  web_cache_manager_.reset(new WebCacheManagerEfl(browser_context_.get()));
}

EWebContext::~EWebContext() {
  VibrationProviderClient::DeleteInstance();
}

void EWebContext::ClearNetworkCache() {
  BrowsingDataRemoverEfl* remover = BrowsingDataRemoverEfl::CreateForUnboundedRange(browser_context_.get());
  remover->ClearNetworkCache();
  //remover deletes itself once it is done with clearing operation.
  return;
}

void EWebContext::ClearWebkitCache() {
  web_cache_manager_->ClearCache();
}

void EWebContext::SetCacheModel(Cache_Model model) {
  web_cache_manager_->SetCacheModel(model);
}

Cache_Model EWebContext::GetCacheModel() const {
  return web_cache_manager_->GetCacheModel();
}

void EWebContext::SetNetworkCacheEnable(bool enable) {
  net::URLRequestContext* url_context = browser_context_->GetRequestContext()->GetURLRequestContext();
  if (!url_context)
    return;

  net::HttpTransactionFactory* transaction_factory = url_context->http_transaction_factory();
  if (!transaction_factory)
    return;

  net::HttpCache* http_cache = transaction_factory->GetCache();
  if (!http_cache)
    return;

  if (enable)
    http_cache->set_mode(net::HttpCache::NORMAL);
  else
    http_cache->set_mode(net::HttpCache::DISABLE);
}

bool EWebContext::GetNetworkCacheEnable() const {
  net::HttpCache* http_cache = browser_context_->GetRequestContext()->GetURLRequestContext()->
      http_transaction_factory()->GetCache();
  if (!http_cache)
    return false;

  return (http_cache->mode() != net::HttpCache::DISABLE);
}

bool EWebContext::HTTPCustomHeaderAdd(const char* name, const char* value) {
  pair<map<string, string>::iterator,bool> ret;
  ret = http_custom_headers_.insert(std::make_pair(string(name), string(value)));
  return ret.second;
}

bool EWebContext::HTTPCustomHeaderRemove(const char* name) {
  return http_custom_headers_.erase(string(name));
}

void EWebContext::HTTPCustomHeaderClear() {
  http_custom_headers_.clear();
}

const HTTPCustomHeadersEflMap& EWebContext::GetHTTPCustomHeadersEflMap() const {
  return http_custom_headers_;
}

void EWebContext::AddExtraPluginDir(const char *path) {
#if defined(ENABLE_PLUGINS)
  content::PluginList::Singleton()->AddExtraPluginDir(base::FilePath(path));
#endif
}

void EWebContext::SetCertificate(const char* certificate_file) {
  browser_context_->SetCertificate(certificate_file);
}

void EWebContext::NotifyLowMemory() {
  MemoryPurger::PurgeAll(browser_context_.get());
}

void EWebContext::SetProxyUri(const char* uri) {
  proxy_uri_ = string(uri);
  net::ProxyConfig config;
  config.proxy_rules().ParseFromString(proxy_uri_);
  base::WaitableEvent done(false, false);
  BrowserThread::PostTask(
      BrowserThread::IO, FROM_HERE,
      base::Bind(&SetProxyConfigCallbackOnIOThread, &done,
                 make_scoped_refptr(browser_context()->GetRequestContext()),
                 config));
  done.Wait();
}

void EWebContext::SetDidStartDownloadCallback(tizen_webview::Context_Did_Start_Download_Callback callback,
                                              void* user_data) {
  DCHECK(start_download_callback_.get() == NULL);
  start_download_callback_.reset(new EwkDidStartDownloadCallback(callback,user_data));
}

EwkDidStartDownloadCallback* EWebContext::DidStartDownloadCallback() {
  return start_download_callback_.get();
}

Ewk_Cookie_Manager* EWebContext::ewkCookieManager() {
  if (!ewk_cookie_manager_)
    ewk_cookie_manager_.reset(Ewk_Cookie_Manager::create(browser_context_->GetRequestContextEfl()));
  return ewk_cookie_manager_.get();
}

void EWebContext::DeleteAllApplicationCache() {
  BrowsingDataRemoverEfl* remover = BrowsingDataRemoverEfl::CreateForUnboundedRange(browser_context_.get());
  remover->RemoveImpl(BrowsingDataRemoverEfl::REMOVE_APPCACHE, GURL());
}

void EWebContext::DeleteApplicationCacheForSite(const tizen_webview::URL& site) {
  content::StoragePartition* partition = BrowserContext::GetStoragePartitionForSite(browser_context_.get(),
                                                                                    GetGURL(site));
  partition->ClearDataForOrigin(content::StoragePartition::REMOVE_DATA_MASK_APPCACHE,
                                content::StoragePartition::QUOTA_MANAGED_STORAGE_MASK_ALL,
                                GetGURL(site),
                                partition->GetURLRequestContext(), base::Bind(&base::DoNothing));
}

void EWebContext::GetAllOriginsWithApplicationCache(tizen_webview::Web_Application_Cache_Origins_Get_Callback callback,
                                                    void* user_data) {
  content::StoragePartition* partition =
      BrowserContext::GetStoragePartition(browser_context_.get(), NULL);

  scoped_refptr<content::AppCacheInfoCollection> collection(new content::AppCacheInfoCollection());
  // As per comments on AppCacheService,
  // there is only one instance of AppCacheService per profile.(i.e. context in our case).
  // So, we don't need to iterate over all StoragePartitions.
  partition->GetAppCacheService()->GetAllAppCacheInfo(collection.get(),
      base::Bind(&OnOriginsWithApplicationCacheObtained, callback, user_data, collection));
}

void EWebContext::GetApplicationCacheUsage(
    const tizen_webview::URL& url,
    tizen_webview::Web_Application_Cache_Usage_For_Origin_Get_Callback callback,
    void* user_data) {
  content::StoragePartition* partition =
        BrowserContext::GetStoragePartition(browser_context_.get(), NULL);

  BrowserThread::PostTask(
      BrowserThread::IO,
      FROM_HERE,
      base::Bind(&storage::QuotaManager::GetUsageAndQuota,
                 partition->GetQuotaManager(),
                 GetGURL(url),
                 storage::kStorageTypeTemporary,
                 base::Bind(&OnTemporaryUsageAndQuotaObtained, callback, user_data)));
}

void EWebContext::WebStorageDelete() {
  BrowsingDataRemoverEfl* remover = BrowsingDataRemoverEfl::CreateForUnboundedRange(browser_context_.get());
  remover->RemoveImpl(BrowsingDataRemoverEfl::REMOVE_LOCAL_STORAGE, GURL());
}

void EWebContext::WebStorageDelete(const tizen_webview::URL& origin) {
  content::StoragePartition* partition =
      BrowserContext::GetStoragePartition(browser_context_.get(), NULL);

  partition->GetDOMStorageContext()->DeleteLocalStorage(GetGURL(origin));
}

void EWebContext::WebStorageOriginsAllGet(tizen_webview::Web_Storage_Origins_Get_Callback callback,
                                                    void* user_data) {
  content::StoragePartition* partition =
      BrowserContext::GetStoragePartition(browser_context_.get(), NULL);

  WebStorageGetAllOriginsDispatcher* dispatcher = new WebStorageGetAllOriginsDispatcher(callback, user_data);

  partition->GetDOMStorageContext()->GetLocalStorageUsage(base::Bind(&WebStorageGetAllOriginsDispatcher::Dispatch, dispatcher));
}

void EWebContext::IndexedDBDelete() {
  BrowsingDataRemoverEfl* remover = BrowsingDataRemoverEfl::CreateForUnboundedRange(browser_context_.get());
  remover->RemoveImpl(BrowsingDataRemoverEfl::REMOVE_INDEXEDDB, GURL());
}

void EWebContext::WebDBDelete(const tizen_webview::URL& host) {
  BrowsingDataRemoverEfl* remover = BrowsingDataRemoverEfl::CreateForUnboundedRange(browser_context_.get());
  remover->RemoveImpl(BrowsingDataRemoverEfl::REMOVE_WEBSQL, GetGURL(host));
}

void EWebContext::GetAllOriginsWithWebDB(tizen_webview::Web_Database_Origins_Get_Callback callback, void* user_data) {
  content::StoragePartition* partition = BrowserContext::GetStoragePartition(browser_context_.get(), NULL);
  BrowserThread::PostTask(
      BrowserThread::DB,
      FROM_HERE,
      base::Bind(&GetWebDBOriginsOnDBThread,
                 callback, user_data, partition));
}

void EWebContext::FileSystemDelete(const tizen_webview::URL& host) {
  BrowsingDataRemoverEfl* remover = BrowsingDataRemoverEfl::CreateForUnboundedRange(browser_context_.get());
  remover->RemoveImpl(BrowsingDataRemoverEfl::REMOVE_FILE_SYSTEMS, GetGURL(host));
}

void EWebContext::GetAllOriginsWithFileSystem(tizen_webview::Local_File_System_Origins_Get_Callback callback, void* user_data) const {
  content::StoragePartition* partition = BrowserContext::GetStoragePartition(browser_context_.get(), NULL);
  BrowserThread::PostTask(
      BrowserThread::FILE,
      FROM_HERE,
        base::Bind(&GetFileSystemOriginsOnFILEThread, callback, user_data, partition));
}

bool EWebContext::SetFaviconDatabasePath(const char* path) {
  FaviconService fs;
  return fs.SetDatabasePath(path);
}

#if defined(OS_TIZEN_TV)
FaviconDatabase* EWebContext::GetFaviconDatabase() {
  return FaviconService::GetFaviconDatabase();
}
#endif

Evas_Object *EWebContext::AddFaviconObject(const char* uri, Evas* canvas) const {
  if (uri == NULL || canvas == NULL) {
    return NULL;
  }
  FaviconService fs;
  SkBitmap bitmap = fs.GetBitmapForPageURL(GURL(uri));
  if (bitmap.isNull()) {
    return NULL;
  }

  Evas_Object *favicon = evas_object_image_filled_add(canvas);
  evas_object_image_size_set(favicon, bitmap.width(), bitmap.height());
  evas_object_image_colorspace_set(favicon, EVAS_COLORSPACE_ARGB8888);
  bitmap.copyPixelsTo(evas_object_image_data_get(favicon, EINA_TRUE), bitmap.getSize());

  return favicon;
}

void EWebContext::ClearCandidateData() {
#ifdef TIZEN_AUTOFILL_SUPPORT
  WebDataServiceFactory* webDataServiceInstance = WebDataServiceFactory::GetInstance();
  scoped_refptr<autofill::AutofillWebDataService> autofillWebDataService = webDataServiceInstance->GetAutofillWebDataForProfile();
  if(autofillWebDataService) {
    //RemoveFormElementsAddedBetween will schedule task on proper thread, it is done in WebDatabaseService::ScheduleDBTask
    autofillWebDataService->RemoveFormElementsAddedBetween(base::Time(), base::Time::Max());
  } else {
    DLOG(WARNING) << "AutofillWebDataService is NULL";
  }
#else
  DLOG(WARNING) << "TIZEN_AUTOFILL_SUPPORT is not enabled";
#endif
}

void EWebContext::ClearPasswordData() {
#if defined(TIZEN_AUTOFILL_SUPPORT)
  PasswordStore* store = PasswordStoreFactory::GetPasswordStore();
  if (store)
    store->RemoveLoginsCreatedBetween(base::Time(), base::Time::Max());
#else
  DLOG(WARNING) << "TIZEN_AUTOFILL_SUPPORT is not enabled";
#endif
}

