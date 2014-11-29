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

#if defined(XWALK_EFL)
#include "xwalk/runtime/browser/xwalk_browser_context.h"
#endif

class EWebContext;

namespace content {

class BrowserContextEfl
#if defined(XWALK_EFL)
  : public xwalk::XWalkBrowserContext
#else
  : public BrowserContext
#endif
  , public visitedlink::VisitedLinkDelegate {
 public:
  BrowserContextEfl(EWebContext*, bool incognito = false);
  ~BrowserContextEfl();

  virtual bool IsOffTheRecord() const override { return incognito_; }
  virtual net::URLRequestContextGetter* GetRequestContext() override;
  URLRequestContextGetterEfl* GetRequestContextEfl()
  { return request_context_getter_.get(); }
  virtual net::URLRequestContextGetter* GetRequestContextForRenderProcess(int) override
  { return GetRequestContext(); }
  virtual net::URLRequestContextGetter* GetMediaRequestContext() override
  { return GetRequestContext(); }
  virtual net::URLRequestContextGetter* GetMediaRequestContextForRenderProcess(int) override
  { return GetRequestContext(); }
  virtual net::URLRequestContextGetter* GetMediaRequestContextForStoragePartition(
      const base::FilePath&, bool) override
  { return GetRequestContext(); }

  // These methods map to Add methods in visitedlink::VisitedLinkMaster.
  void AddVisitedURLs(const std::vector<GURL>& urls);
  // visitedlink::VisitedLinkDelegate implementation.
  virtual void RebuildTable(
      const scoped_refptr<URLEnumerator>& enumerator) override;
  // Reset visitedlink master and initialize it.
  void InitVisitedLinkMaster();

  virtual ResourceContext* GetResourceContext() override;

  virtual content::DownloadManagerDelegate* GetDownloadManagerDelegate() override
  { return &download_manager_delegate_; }

  virtual BrowserPluginGuestManager* GetGuestManager() override
  { return 0; }

  virtual storage::SpecialStoragePolicy* GetSpecialStoragePolicy() override
  { return 0; }

  virtual PushMessagingService* GetPushMessagingService() override
  { return 0; }

#if defined(XWALK_EFL)
  virtual xwalk::RuntimeURLRequestContextGetter* GetURLRequestContextGetterById(
      const std::string& pkg_id) override;
  virtual net::URLRequestContextGetter* CreateRequestContextForStoragePartition(
      const base::FilePath& partition_path,
      bool in_memory,
      content::ProtocolHandlerMap* protocol_handlers,
      content::URLRequestInterceptorScopedVector request_interceptors) override;
  virtual void InitWhileIOAllowed() override;
#endif // XWALK_EFL

  virtual base::FilePath GetPath() const override;

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

    virtual net::HostResolver* GetHostResolver() override;
    virtual net::URLRequestContext* GetRequestContext() override;
    void set_url_request_context_getter(URLRequestContextGetterEfl* getter);
    BrowserContextEfl* getBrowserContext() { return browser_context_; };

   private:
    URLRequestContextGetterEfl* getter_;
    BrowserContextEfl *browser_context_;

    DISALLOW_COPY_AND_ASSIGN(ResourceContextEfl);
  };

 private:
  static void ReadCertificateAndAdd(base::FilePath* file_path);
  virtual SSLHostStateDelegate* GetSSLHostStateDelegate() override;

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
  const bool incognito_;

  DISALLOW_COPY_AND_ASSIGN(BrowserContextEfl);
};

}

#endif
