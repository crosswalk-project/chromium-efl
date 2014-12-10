// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef RESOURCE_DISPATCHER_HOST_DELEGATE_EFL_H
#define RESOURCE_DISPATCHER_HOST_DELEGATE_EFL_H

#include "base/compiler_specific.h"
#include "content/public/browser/resource_dispatcher_host_delegate.h"

namespace net {
class AuthChallengeInfo;
class URLRequest;
}

namespace content {

class ResourceDispatcherHostLoginDelegate;

class ResourceDispatcherHostDelegateEfl
    : public ResourceDispatcherHostDelegate {
 public:
  ResourceDispatcherHostDelegateEfl()
  {
  }

  // Called after ShouldBeginRequest to allow the embedder to add resource
  // throttles.
  virtual void RequestBeginning(net::URLRequest* request,
      ResourceContext* resource_context,
      AppCacheService* appcache_service,
      ResourceType resource_type,
      ScopedVector<ResourceThrottle>* throttles) override;

  // Create login delegate.
  virtual ResourceDispatcherHostLoginDelegate* CreateLoginDelegate(
      net::AuthChallengeInfo* auth_info, net::URLRequest* request) override;

  // Called to trigger download.
  virtual void DownloadStarting(
      net::URLRequest* request,
      content::ResourceContext* resource_context,
      int child_id,
      int route_id,
      int request_id,
      bool is_content_initiated,
      bool must_download,
      ScopedVector<content::ResourceThrottle>* throttles) override;

#if !defined(EWK_BRINGUP)
  // Return true if it should override mime type, otherwise, return false.
  virtual bool ShouldOverrideMimeType(
      const GURL& url, std::string& mime_type) override;
#endif

 private:
  void TriggerNewDownloadStartCallback(
      net::URLRequest* request,
      const std::string& user_agent,
      const std::string& content_disposition,
      const std::string& mime_type,
      int64 content_length,
      int render_process_id,
      int render_view_id);
};

} // namespace net

#endif // RESOURCE_DISPATCHER_HOST_DELEGATE_EFL_H
