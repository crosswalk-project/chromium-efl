// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "browser_context_efl.h"

#include "base/bind.h"
#include "base/files/file_util.h"
#include "base/path_service.h"
#include "browser/geolocation/geolocation_permission_context_efl.h"
#include "components/visitedlink/browser/visitedlink_master.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/browser/web_contents.h"
#include "eweb_context.h"
#include "paths_efl.h"

using std::pair;

namespace content {

BrowserContextEfl::ResourceContextEfl::ResourceContextEfl()
    : getter_(NULL) {
}

BrowserContextEfl::~BrowserContextEfl() {
  if (resource_context_) {
    resource_context_->set_url_request_context_getter(NULL);
    bool ok = BrowserThread::DeleteSoon(BrowserThread::IO, FROM_HERE, resource_context_);
    DCHECK(ok);
  }
}

BrowserContextEfl::ResourceContextEfl::~ResourceContextEfl() {
}

bool BrowserContextEfl::ResourceContextEfl::HTTPCustomHeaderAdd(
    const char* name, const char* value) {
  if (!name)
    return false;

  std::string name_string(name);
  if (name_string.empty())
    return false;

  std::string value_string;
  if (value)
    value_string.assign(value);

  base::AutoLock locker(http_custom_headers_lock_);
  return http_custom_headers_.insert(
      std::make_pair(name_string, value_string)).second;
}

bool BrowserContextEfl::ResourceContextEfl::HTTPCustomHeaderRemove(
    const char* name) {
  base::AutoLock locker(http_custom_headers_lock_);
  return http_custom_headers_.erase(string(name));
}

void BrowserContextEfl::ResourceContextEfl::HTTPCustomHeaderClear() {
  base::AutoLock locker(http_custom_headers_lock_);
  http_custom_headers_.clear();
}

const HTTPCustomHeadersEflMap
    BrowserContextEfl::ResourceContextEfl::GetHTTPCustomHeadersEflMap() const {
  base::AutoLock locker(http_custom_headers_lock_);
  return http_custom_headers_;
}


net::HostResolver* BrowserContextEfl::ResourceContextEfl::GetHostResolver() {
  CHECK(getter_.get());
  return getter_->host_resolver();
}

net::URLRequestContext* BrowserContextEfl::ResourceContextEfl::GetRequestContext() {
  CHECK(getter_.get());
  return getter_->GetURLRequestContext();
}

void BrowserContextEfl::ResourceContextEfl::set_url_request_context_getter(
    scoped_refptr<URLRequestContextGetterEfl> getter) {
  getter_ = getter;
}

base::WeakPtr<CookieManager>
BrowserContextEfl::ResourceContextEfl::GetCookieManager() const {
  return getter_->cookieManager();
}

#if defined(ENABLE_NOTIFICATIONS)
scoped_refptr<NotificationControllerEfl>
    BrowserContextEfl::ResourceContextEfl::GetNotificationController() const {
  return notification_controller_efl_;
}

void BrowserContextEfl::ResourceContextEfl::set_notification_controller_efl(
    const scoped_refptr<NotificationControllerEfl>& controller) {
  notification_controller_efl_ = controller;
}
#endif

BrowserContextEfl::BrowserContextEfl(EWebContext* web_context, bool incognito)
  : resource_context_(NULL),
    web_context_(web_context),
#if defined(ENABLE_NOTIFICATIONS)
    notification_controller_efl_(new NotificationControllerEfl()),
#endif
    temp_dir_creation_attempted_(false),
    incognito_(incognito) {
  InitVisitedLinkMaster();
}

net::URLRequestContextGetter* BrowserContextEfl::GetRequestContext() {
  return GetStoragePartition(this, NULL)->GetURLRequestContext();
}

ResourceContext* BrowserContextEfl::GetResourceContext() {
  if (!resource_context_) {
    resource_context_ = new ResourceContextEfl();
#if defined(ENABLE_NOTIFICATIONS)
    resource_context_->set_notification_controller_efl(
        notification_controller_efl_);
#endif
  }

  return resource_context_;
}

BrowserContextEfl::ResourceContextEfl* BrowserContextEfl::GetResourceContextEfl() {
  GetResourceContext();
  return resource_context_;
}

// TODO Can this API be called from multiple threads?
base::FilePath BrowserContextEfl::GetPath() const {
  if (IsOffTheRecord()) {
    // Empty path indicates in memory storage. All data that would be persistent
    // are stored in memory and are gone when closing browser, what is a
    // requirement for the incognito mode (being off the record)
    return base::FilePath();
  }

  // TODO: Figure out something better for data storage.

  static base::FilePath path;
  if (path.empty())
    PathService::Get(PathsEfl::DIR_USER_DATA, &path);
  return path;
}

#if defined(ENABLE_NOTIFICATIONS)
scoped_refptr<content::NotificationControllerEfl>
    BrowserContextEfl::GetNotificationController() const {
  return notification_controller_efl_;
}
#endif

net::URLRequestContextGetter* BrowserContextEfl::CreateRequestContext(
    content::ProtocolHandlerMap* protocol_handlers,
    URLRequestInterceptorScopedVector request_interceptors) {
  // TODO: Implement support for chromium network log

  base::FilePath cache_base_path;
  if (!PathService::Get(base::DIR_CACHE, &cache_base_path)) {
    LOG(ERROR) << "Could not retrieve path to the cache directory";
    return NULL;
  }

  request_context_getter_ = new URLRequestContextGetterEfl(
      *web_context_,
      false,
      cache_base_path,
      BrowserThread::GetMessageLoopProxyForThread(BrowserThread::IO),
      BrowserThread::GetMessageLoopProxyForThread(BrowserThread::FILE),
      protocol_handlers,
      request_interceptors.Pass(),
      NULL);
  GetRequestContext();
  resource_context_->set_url_request_context_getter(request_context_getter_.get());
  return request_context_getter_.get();
}

void BrowserContextEfl::SetCertificate(const char* certificate_file) {
  base::FilePath* certificate_path = new base::FilePath(certificate_file);
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  BrowserThread::PostTask(BrowserThread::FILE, FROM_HERE,
      base::Bind(&BrowserContextEfl::ReadCertificateAndAdd, base::Owned(certificate_path)));
}

void BrowserContextEfl::ReadCertificateAndAdd(base::FilePath* file_path) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::FILE));
  std::string cert_contents;
  base::ReadFileToString(*file_path, &cert_contents);
  scoped_refptr<net::X509Certificate> cert(net::X509Certificate::CreateFromBytes(
      cert_contents.c_str(), cert_contents.size()));
  if (!cert.get()) {
    DLOG(ERROR) << "User certificate could not be parsed.";
    return;
  }
  int err_code = net::CertDatabase::GetInstance()->CheckUserCert(cert.get());
  if (net::OK != err_code) {
    DLOG(ERROR) << "User certificate is not valid. Error code : " << err_code;
    return;
  }
  err_code = net::CertDatabase::GetInstance()->AddUserCert(cert.get());
  if (net::OK != err_code) {
    DLOG(ERROR) << "User certificate could not be added. Error code : " << err_code;
    return;
  }
}

void BrowserContextEfl::InitVisitedLinkMaster() {
  if (!IsOffTheRecord()) {
    visitedlink_master_.reset(new visitedlink::VisitedLinkMaster(this, this, false));
    visitedlink_master_->Init();
  }
}

void BrowserContextEfl::AddVisitedURLs(const std::vector<GURL>& urls) {
  if (!IsOffTheRecord()) {
    DCHECK(visitedlink_master_);
    visitedlink_master_->AddURLs(urls);
  }
}

void BrowserContextEfl::RebuildTable(const scoped_refptr<URLEnumerator>& enumerator) {
  if (!IsOffTheRecord()) {
    // WebView rebuilds from WebChromeClient.getVisitedHistory. The client
    // can change in the lifetime of this WebView and may not yet be set here.
    // Therefore this initialization path is not used.
    enumerator->OnComplete(true);
  }
}

SSLHostStateDelegate* BrowserContextEfl::GetSSLHostStateDelegate() {
  // SSLHostStateDelegate may be left unimplemented to use a default strategy of
  // not remembering certificate decisions at all.
  return NULL;
}

const GeolocationPermissionContextEfl&
    BrowserContextEfl::GetGeolocationPermissionContext() const {
  if (!geolocation_permission_context_.get()) {
    geolocation_permission_context_.reset(
        new GeolocationPermissionContextEfl());
  }

  return *(geolocation_permission_context_.get());
}

}
