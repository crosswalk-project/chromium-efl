// Copyright 2014-2015 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef EWEB_CONTEXT_H
#define EWEB_CONTEXT_H

#include "API/ewk_cookie_manager_private.h"
#include "base/memory/scoped_ptr.h"
#include "base/memory/weak_ptr.h"
#include "browser/renderer_host/web_cache_manager_efl.h"
#include "tizen_webview/public/tw_cache_model.h"
#include "tizen_webview/public/tw_callbacks.h"
#include "devtools_delegate_efl.h"

namespace tizen_webview {
class URL;
class WebContext;
class WrtIpcMessageData;
}

class CookieManager;
#if defined(OS_TIZEN_TV)
class FaviconDatabase;
#endif

namespace content {
class BrowserContextEfl;
}

class EwkDidStartDownloadCallback {
 public:
  EwkDidStartDownloadCallback(tizen_webview::Context_Did_Start_Download_Callback callback,
                              void* user_data)
    : callback_(callback), user_data_(user_data)
    { }
  void TriggerCallback(const std::string& url);
 private:
  tizen_webview::Context_Did_Start_Download_Callback callback_;
  void* user_data_;
};

class EWebContext {
 public:
  content::BrowserContextEfl* browser_context() const { return browser_context_.get(); }

  void ClearNetworkCache();
  void ClearWebkitCache();
  void AddExtraPluginDir(const char *path);
  void SetCertificate(const char* certificate_file);

  void SetCacheModel(tizen_webview::Cache_Model);
  tizen_webview::Cache_Model GetCacheModel() const;
  void SetNetworkCacheEnable(bool enable);
  bool GetNetworkCacheEnable() const;

  void NotifyLowMemory();
  Ewk_Cookie_Manager* ewkCookieManager();
  base::WeakPtr<CookieManager> cookieManager()
  { return ewkCookieManager()->cookieManager(); }
  void SetProxyUri(const char* uri);
  const char* GetProxyUri() const
  { return proxy_uri_.c_str(); }
  //download start callback handlers
  void SetDidStartDownloadCallback(tizen_webview::Context_Did_Start_Download_Callback callback,
                                   void* user_data);
  EwkDidStartDownloadCallback* DidStartDownloadCallback();
  void DeleteAllApplicationCache();
  void DeleteApplicationCacheForSite(const tizen_webview::URL&);
  void GetAllOriginsWithApplicationCache(tizen_webview::Web_Application_Cache_Origins_Get_Callback callback,
                                         void* user_data);
  void GetApplicationCacheUsage(
      const tizen_webview::URL& url,
      tizen_webview::Web_Application_Cache_Usage_For_Origin_Get_Callback callback,
      void* user_data);
  void GetAllOriginsWithWebDB(tizen_webview::Web_Database_Origins_Get_Callback callback, void* user_data);
  void WebDBDelete(const tizen_webview::URL& host);
  void IndexedDBDelete();
  void WebStorageDelete();
  void WebStorageDelete(const tizen_webview::URL& origin);
  void WebStorageOriginsAllGet(tizen_webview::Web_Storage_Origins_Get_Callback callback, void* user_data);
  void FileSystemDelete(const tizen_webview::URL& host);
  void GetAllOriginsWithFileSystem(tizen_webview::Local_File_System_Origins_Get_Callback callback, void* user_data) const;
  bool SetFaviconDatabasePath(const char *path);
  Evas_Object *AddFaviconObject(const char *uri, Evas *canvas) const;

  void SendWidgetInfo(int widget_id, double scale, const std::string &theme, const std::string &encoded_bundle);
  void SendWrtMessage(const tizen_webview::WrtIpcMessageData& message);

  static void SetMimeOverrideCallback(tizen_webview::Mime_Override_Callback callback);
  static bool ShouldOverrideMimeForURL(const GURL& url, std::string& mime_type);

  void SetPixmap(int pixmap) { m_pixmap = pixmap; }
  int Pixmap() const { return m_pixmap; }

  void ClearCandidateData();
  void ClearPasswordData();
#if defined(OS_TIZEN_TV)
  FaviconDatabase* GetFaviconDatabase();
#endif

  unsigned int InspectorServerStart(unsigned int port);
  bool InspectorServerStop();

 private:
  EWebContext(bool incognito);
  ~EWebContext();
  friend class tizen_webview::WebContext;

  static EWebContext* default_context_;
  static tizen_webview::Mime_Override_Callback mime_override_callback_;
  scoped_ptr<WebCacheManagerEfl> web_cache_manager_;
  scoped_ptr<content::BrowserContextEfl> browser_context_;
  scoped_ptr<Ewk_Cookie_Manager> ewk_cookie_manager_;
  std::string proxy_uri_;
  scoped_ptr<EwkDidStartDownloadCallback> start_download_callback_;
  int m_pixmap;
  content::DevToolsDelegateEfl* inspector_server_;
  bool incognito_;
};

#endif
