/*
   Copyright (C) 2014 Samsung Electronics

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef EWEB_CONTEXT_H
#define EWEB_CONTEXT_H

#include "base/command_line.h"
#include "base/memory/scoped_ptr.h"
#include "base/run_loop.h"
#include "content/public/app/content_main_runner.h"
#include "content/public/browser/browser_main_runner.h"
#include "content/public/browser/browser_context.h"
#include "browser/renderer_host/web_cache_manager_efl.h"
#include "eweb_object.h"
#include "API/ewk_cookie_manager_private.h"
#include "public/ewk_context.h"
#include "public/ewk_ipc_message.h"
#include "ui/gfx/rect.h"

#include <Evas.h>

typedef std::map<std::string, std::string> HTTPCustomHeadersEflMap;

namespace content {
class BrowserContextEfl;
}

class EwkDidStartDownloadCallback {
 public:
  EwkDidStartDownloadCallback(Ewk_Context_Did_Start_Download_Callback callback,
                              void* user_data)
    : callback_(callback), user_data_(user_data)
    { }
  void TriggerCallback(const std::string& url);
 private:
  Ewk_Context_Did_Start_Download_Callback callback_;
  void* user_data_;
};

class EWebContext : public EWebObject {
 public:
  EWK_OBJECT_DECLARE(EWebContext);

  EWebContext();

  static EWebContext* DefaultContext();
  static void Delete(EWebContext*);

  content::BrowserContextEfl* browser_context() const { return browser_context_.get(); }

  void ClearNetworkCache();
  void ClearWebkitCache();
  void AddExtraPluginDir(const char *path);
  void SetCertificate(const char* certificate_file);

  void SetCacheModel(Ewk_Cache_Model);
  Ewk_Cache_Model GetCacheModel() const;
  void SetNetworkCacheEnable(bool enable);
  bool GetNetworkCacheEnable() const;
  bool HTTPCustomHeaderAdd(const char* name, const char* value);
  bool HTTPCustomHeaderRemove(const char* name);
  void HTTPCustomHeaderClear();
  const HTTPCustomHeadersEflMap& GetHTTPCustomHeadersEflMap() const;

  void NotifyLowMemory();
  Ewk_Cookie_Manager* ewkCookieManager();
  CookieManager* cookieManager()
  { return ewkCookieManager()->cookieManager(); }
  void SetProxyUri(const char* uri);
  const char* GetProxyUri() const
  { return proxy_uri_.c_str(); }
  //download start callback handlers
  void SetDidStartDownloadCallback(Ewk_Context_Did_Start_Download_Callback callback,
                                   void* user_data);
  EwkDidStartDownloadCallback* DidStartDownloadCallback();
  void DeleteAllApplicationCache();
  void DeleteApplicationCacheForSite(const GURL&);
  void GetAllOriginsWithApplicationCache(Ewk_Web_Application_Cache_Origins_Get_Callback callback,
                                         void* user_data);
  void GetApplicationCacheUsage(
      const GURL& url,
      Ewk_Web_Application_Cache_Usage_For_Origin_Get_Callback callback,
      void* user_data);
  void GetAllOriginsWithWebDB(Ewk_Web_Database_Origins_Get_Callback callback, void* user_data);
  void WebDBDelete(const GURL& host);
  void IndexedDBDelete();
  void WebStorageDelete();
  void FileSystemDelete(const GURL& host);
  void GetAllOriginsWithFileSystem(Ewk_Local_File_System_Origins_Get_Callback callback, void* user_data) const;

  void SendWidgetInfo(int widget_id, double scale, const std::string &theme, const std::string &encoded_bundle);
  void SendWrtMessage(const Ewk_IPC_Wrt_Message_Data& message);

  void SetPixmap(int pixmap) { m_pixmap = pixmap; }
  int Pixmap() const { return m_pixmap; }

 private:
  struct GlobalData;

  virtual ~EWebContext();

  static void EnsureGlobalData();

  static EWebContext* default_context_;
  static GlobalData* global_data_;
  scoped_ptr<WebCacheManagerEfl> web_cache_manager_;
  scoped_ptr<content::BrowserContextEfl> browser_context_;
  HTTPCustomHeadersEflMap http_custom_headers_;
  scoped_ptr<Ewk_Cookie_Manager> ewk_cookie_manager_;
  std::string proxy_uri_;
  scoped_ptr<EwkDidStartDownloadCallback> start_download_callback_;
  int m_pixmap;

  static int argc_;
  static const char** argv_;
};

EWebContext* ToEWebContext(Ewk_Context* context);

#endif
