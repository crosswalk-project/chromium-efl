// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TIZEN_WEBVIEW_PUBLIC_TW_WEB_CONTEXT_H_
#define TIZEN_WEBVIEW_PUBLIC_TW_WEB_CONTEXT_H_

#include <string>
#include <Evas.h>

#include "tizen_webview/public/tw_callbacks.h"
#include "tizen_webview/public/tw_cache_model.h"
#include "tizen_webview/public/tw_ref_counted.h"

class CookieManager;
class Ewk_Cookie_Manager;
class EWebContext;
//struct Ewk_IPC_Wrt_Message_Data;

namespace content {
// TODO: Remove dependency
class BrowserContextEfl;
}

namespace tizen_webview {
class URL;

struct WebContext : public tizen_webview::RefCounted<WebContext> {
 public:
  static WebContext* DefaultContext();
  static WebContext* Create();
  static void Delete(WebContext*);

  // Get related class
  content::BrowserContextEfl* browser_context() const;
  Ewk_Cookie_Manager* ewkCookieManager() const;
  CookieManager* cookieManager() const;

  // Set Callbacks
  void SetDidStartDownloadCallback(Context_Did_Start_Download_Callback callback,
      void* user_data);
  //EwkDidStartDownloadCallback* DidStartDownloadCallback();

  // Certificate
  void SetCertificate(const char* certificate_file);

  // Proxy URI
  void SetProxyUri(const char* uri);
  const char* GetProxyUri() const;

  // System
  void AddExtraPluginDir(const char *path);
  void NotifyLowMemory();

  // HTTP Custom Header
  bool HTTPCustomHeaderAdd(const char* name, const char* value);
  bool HTTPCustomHeaderRemove(const char* name);
  void HTTPCustomHeaderClear();

  // Cache Model
  void SetCacheModel(tizen_webview::Cache_Model cm);
  tizen_webview::Cache_Model GetCacheModel() const;

  // Network Cache
  void SetNetworkCacheEnable(bool enable);
  bool GetNetworkCacheEnable() const;
  void ClearNetworkCache();
  void ClearWebkitCache();

  // Application Cache
  void GetAllOriginsWithApplicationCache(
      Web_Application_Cache_Origins_Get_Callback callback, void* user_data);
  void GetApplicationCacheUsage(const URL& url,
      Web_Application_Cache_Usage_For_Origin_Get_Callback callback,
      void* user_data);
  void DeleteAllApplicationCache();
  void DeleteApplicationCacheForSite(const tizen_webview::URL& url);

  // Web Database
  void GetAllOriginsWithWebDB(Web_Database_Origins_Get_Callback callback,
      void* user_data);
  void WebDBDelete(const URL& host);

  // Indexed DB
  void IndexedDBDelete();

  // Web Storage
  void WebStorageOriginsAllGet(Web_Storage_Origins_Get_Callback callback,
      void* user_data);
  void WebStorageDelete();
  void WebStorageDelete(const URL& origin);

  // File System
  void GetAllOriginsWithFileSystem(
      Local_File_System_Origins_Get_Callback callback, void* user_data) const;
  void FileSystemDelete(const URL& host);

  // Favicon
  bool SetFaviconDatabasePath(const char *path);
  Evas_Object *AddFaviconObject(const char *uri, Evas *canvas) const;

  // Widget
  void SendWidgetInfo(int widget_id, double scale, const std::string &theme,
      const std::string &encoded_bundle);
  //void SendWrtMessage(const Ewk_IPC_Wrt_Message_Data& message);

  // Pixmap
  int  Pixmap() const;
  void SetPixmap(int pixmap);

  // Set MIME override callback
  static void SetMimeOverrideCallback(Mime_Override_Callback callback);

  // Note: Do not use outside chromium
  EWebContext* GetImpl() { return impl; }

 private:
  EWebContext* impl;

  WebContext();
  ~WebContext();
  friend class tizen_webview::RefCounted<WebContext>;

  DISALLOW_COPY_AND_ASSIGN(WebContext);
};
} // namespace tizen_webview

#endif  // TIZEN_WEBVIEW_PUBLIC_TW_WEB_CONTEXT_H_
