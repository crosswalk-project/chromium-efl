// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "browser_context_efl.h"

#include "base/bind.h"
#include "base/file_util.h"
#include "base/path_service.h"
#include "components/visitedlink/browser/visitedlink_master.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/browser/web_contents.h"
#include "eweb_context.h"

namespace content {

BrowserContextEfl::ResourceContextEfl::ResourceContextEfl(BrowserContextEfl *ctx)
    : getter_(NULL),
      browser_context_(ctx) {
}

BrowserContextEfl::~BrowserContextEfl() {
  if (resource_context_) {
    DCHECK(BrowserThread::DeleteSoon(BrowserThread::IO, FROM_HERE, resource_context_));
    resource_context_ = NULL;
  }
}

BrowserContextEfl::ResourceContextEfl::~ResourceContextEfl() {
}

net::HostResolver* BrowserContextEfl::ResourceContextEfl::GetHostResolver() {
  CHECK(getter_);
  return getter_->host_resolver();
}

net::URLRequestContext* BrowserContextEfl::ResourceContextEfl::GetRequestContext() {
  CHECK(getter_);
  return getter_->GetURLRequestContext();
}

void BrowserContextEfl::ResourceContextEfl::set_url_request_context_getter(
    URLRequestContextGetterEfl* getter) {
  getter_ = getter;
}

BrowserContextEfl::BrowserContextEfl(EWebContext* web_context)
  : resource_context_(NULL),
    web_context_(web_context),
#if defined(ENABLE_NOTIFICATIONS)
    notification_controllerefl_(new NotificationControllerEfl()),
#endif
    temp_dir_creation_attempted_(false) {
  InitVisitedLinkMaster();
}

net::URLRequestContextGetter* BrowserContextEfl::GetRequestContext() {
  return GetStoragePartition(this, NULL)->GetURLRequestContext();
}

ResourceContext* BrowserContextEfl::GetResourceContext() {
  if (!resource_context_)
    resource_context_ = new ResourceContextEfl(this);

  return resource_context_;
}

base::FilePath BrowserContextEfl::GetPath() const {
  // TODO: Figure out something better for data storage.

  static base::FilePath path;
  static bool pathInitialized = false;

  if (!pathInitialized)
    PathService::Get(base::DIR_TEMP, &path);

  return path;
}

content::NotificationControllerEfl*
BrowserContextEfl::GetNotificationController() const {
#if defined(ENABLE_NOTIFICATIONS)
  return notification_controllerefl_.get();
#else
  return NULL;
#endif
}

net::URLRequestContextGetter* BrowserContextEfl::CreateRequestContext(
    content::ProtocolHandlerMap* protocol_handlers,
    URLRequestInterceptorScopedVector request_interceptors) {
  // TODO: Implement support for chromium network log
  request_context_getter_ = new URLRequestContextGetterEfl(
      *web_context_,
      false,
      GetPath(),
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
  visitedlink_master_.reset(new visitedlink::VisitedLinkMaster(this, this, false));
  visitedlink_master_->Init();
}

void BrowserContextEfl::AddVisitedURLs(const std::vector<GURL>& urls) {
  DCHECK(visitedlink_master_);
  visitedlink_master_->AddURLs(urls);
}

void BrowserContextEfl::RebuildTable(const scoped_refptr<URLEnumerator>& enumerator) {
  // WebView rebuilds from WebChromeClient.getVisitedHistory. The client
  // can change in the lifetime of this WebView and may not yet be set here.
  // Therefore this initialization path is not used.
  enumerator->OnComplete(true);
}

SSLHostStateDelegate* BrowserContextEfl::GetSSLHostStateDelegate() {
  // Pure method in BrowserContext class. Need implementation.
  NOTIMPLEMENTED();
}

}
