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

#ifndef BROWSER_CONTEXT_EFL
#define BROWSER_CONTEXT_EFL

#include "base/memory/scoped_ptr.h"
#include "base/files/scoped_temp_dir.h"
#include "content/public/browser/content_browser_client.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/storage_partition.h"
#include "content/public/browser/resource_context.h"
#include "url_request_context_getter_efl.h"
#include "browser/notification/notification_controller_efl.h"
#include "browser/download_manager_delegate_efl.h"
#include "net/url_request/url_request_context.h"

class EWebContext;

namespace content {

class BrowserContextEfl
    : public BrowserContext {
 public:
  BrowserContextEfl(EWebContext*);

  virtual bool IsOffTheRecord() const OVERRIDE { return false; }
  virtual net::URLRequestContextGetter* GetRequestContext() OVERRIDE;
  URLRequestContextGetterEfl* GetRequestContextEfl()
  { return request_context_getter_.get(); }
  virtual net::URLRequestContextGetter* GetRequestContextForRenderProcess(int) OVERRIDE
  { return GetRequestContext(); }
  virtual net::URLRequestContextGetter* GetMediaRequestContext() OVERRIDE
  { return GetRequestContext(); }
  virtual net::URLRequestContextGetter* GetMediaRequestContextForRenderProcess(int) OVERRIDE
  { return GetRequestContext(); }
  virtual net::URLRequestContextGetter* GetMediaRequestContextForStoragePartition(
      const base::FilePath&, bool) OVERRIDE
  { return GetRequestContext(); }

  virtual ResourceContext* GetResourceContext() OVERRIDE;

  virtual content::DownloadManagerDelegate* GetDownloadManagerDelegate() OVERRIDE
  { return &download_manager_delegate_; }

  virtual BrowserPluginGuestManager* GetGuestManager() OVERRIDE
  { return 0; }

  virtual quota::SpecialStoragePolicy* GetSpecialStoragePolicy() OVERRIDE
  { return 0; }

  virtual PushMessagingService* GetPushMessagingService() OVERRIDE
  { return 0; }

  virtual base::FilePath GetPath() const OVERRIDE;

  net::URLRequestContextGetter* CreateRequestContext(
      content::ProtocolHandlerMap* protocol_handlers);
  void SetCertificate(const char* certificate_file);
  EWebContext* WebContext() const
  { return web_context_; }

  content::NotificationControllerEfl* GetNotificationController() const;

  class ResourceContextEfl : public ResourceContext {
   public:
    ResourceContextEfl(BrowserContextEfl*);
    virtual ~ResourceContextEfl();

    virtual net::HostResolver* GetHostResolver() OVERRIDE;
    virtual net::URLRequestContext* GetRequestContext() OVERRIDE;
    virtual bool AllowMicAccess(const GURL& origin) OVERRIDE;
    virtual bool AllowCameraAccess(const GURL& origin) OVERRIDE;
    void set_url_request_context_getter(URLRequestContextGetterEfl* getter);
    BrowserContextEfl* getBrowserContext() { return browser_context_; };

   private:
    URLRequestContextGetterEfl* getter_;
    BrowserContextEfl *browser_context_;

    DISALLOW_COPY_AND_ASSIGN(ResourceContextEfl);
  };

 private:
  static void ReadCertificateAndAdd(base::FilePath* file_path);

  scoped_ptr<ResourceContextEfl> resource_context_;
  scoped_refptr<URLRequestContextGetterEfl> request_context_getter_;
  EWebContext* web_context_;
#if defined(ENABLE_NOTIFICATIONS)
  scoped_ptr<NotificationControllerEfl> notification_controllerefl_;
#endif
  DownloadManagerDelegateEfl download_manager_delegate_;
  base::ScopedTempDir temp_dir_;
  bool temp_dir_creation_attempted_;

  DISALLOW_COPY_AND_ASSIGN(BrowserContextEfl);
};

}

#endif
