// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BROWSER_CONTEXT_EFL
#define BROWSER_CONTEXT_EFL

#include <vector>

#include "base/memory/scoped_ptr.h"
#include "base/files/scoped_temp_dir.h"
#include "components/visitedlink/browser/visitedlink_delegate.h"
#include "components/visitedlink/browser/visitedlink_master.h"
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
  : public BrowserContext,
    public visitedlink::VisitedLinkDelegate {
 public:
  BrowserContextEfl(EWebContext*);
  ~BrowserContextEfl();

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

  // These methods map to Add methods in visitedlink::VisitedLinkMaster.
  void AddVisitedURLs(const std::vector<GURL>& urls);
  // visitedlink::VisitedLinkDelegate implementation.
  virtual void RebuildTable(
      const scoped_refptr<URLEnumerator>& enumerator) OVERRIDE;
  // Reset visitedlink master and initialize it.
  void InitVisitedLinkMaster();

  virtual ResourceContext* GetResourceContext() OVERRIDE;

  virtual content::DownloadManagerDelegate* GetDownloadManagerDelegate() OVERRIDE
  { return &download_manager_delegate_; }

  virtual BrowserPluginGuestManager* GetGuestManager() OVERRIDE
  { return 0; }

  virtual storage::SpecialStoragePolicy* GetSpecialStoragePolicy() OVERRIDE
  { return 0; }

  virtual PushMessagingService* GetPushMessagingService() OVERRIDE
  { return 0; }

  virtual base::FilePath GetPath() const OVERRIDE;

  net::URLRequestContextGetter* CreateRequestContext(
      content::ProtocolHandlerMap* protocol_handlers,
      URLRequestInterceptorScopedVector request_interceptors);
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
    void set_url_request_context_getter(URLRequestContextGetterEfl* getter);
    BrowserContextEfl* getBrowserContext() { return browser_context_; };

   private:
    URLRequestContextGetterEfl* getter_;
    BrowserContextEfl *browser_context_;

    DISALLOW_COPY_AND_ASSIGN(ResourceContextEfl);
  };

 private:
  static void ReadCertificateAndAdd(base::FilePath* file_path);
  virtual SSLHostStateDelegate* GetSSLHostStateDelegate() OVERRIDE;

  scoped_ptr<visitedlink::VisitedLinkMaster> visitedlink_master_;
  ResourceContextEfl* resource_context_;
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
