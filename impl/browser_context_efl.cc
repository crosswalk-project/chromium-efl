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

#include "browser_context_efl.h"

#include "base/file_util.h"
#include "base/path_service.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/browser/web_contents.h"
#include "eweb_context.h"

namespace content {

BrowserContextEfl::ResourceContextEfl::ResourceContextEfl(BrowserContextEfl *ctx)
    : getter_(NULL),
      browser_context_(ctx) {
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

bool BrowserContextEfl::ResourceContextEfl::AllowMicAccess(const GURL& origin) {
  return false;
}

bool BrowserContextEfl::ResourceContextEfl::AllowCameraAccess(const GURL& origin) {
  return false;
}

void BrowserContextEfl::ResourceContextEfl::set_url_request_context_getter(
    URLRequestContextGetterEfl* getter) {
  getter_ = getter;
}

BrowserContextEfl::BrowserContextEfl(EWebContext* web_context)
  : web_context_(web_context),
#if defined(ENABLE_NOTIFICATIONS)
    notification_controllerefl_(new NotificationControllerEfl()),
#endif
    temp_dir_creation_attempted_(false) {
}

net::URLRequestContextGetter* BrowserContextEfl::GetRequestContext() {
  return GetStoragePartition(this, NULL)->GetURLRequestContext();
}

ResourceContext* BrowserContextEfl::GetResourceContext() {
  if (!resource_context_)
    resource_context_.reset(new ResourceContextEfl(this));

  return resource_context_.get();
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
    content::ProtocolHandlerMap* protocol_handlers) {
  // TODO: Implement support for chromium network log
  request_context_getter_ = new URLRequestContextGetterEfl(
      *web_context_,
      false,
      GetPath(),
      BrowserThread::UnsafeGetMessageLoopForThread(BrowserThread::IO),
      BrowserThread::UnsafeGetMessageLoopForThread(BrowserThread::FILE),
      protocol_handlers,
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
  if (!cert) {
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

}
