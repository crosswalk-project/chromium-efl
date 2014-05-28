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

#ifndef RESOURCE_DISPATCHER_HOST_DELEGATE_EFL_H
#define RESOURCE_DISPATCHER_HOST_DELEGATE_EFL_H

#include "content/public/browser/resource_dispatcher_host_delegate.h"

namespace content {

class ResourceDispatcherHostDelegateEfl
    : public ResourceDispatcherHostDelegate {
 public:
  ResourceDispatcherHostDelegateEfl() {}

  // Called after ShouldBeginRequest to allow the embedder to add resource
  // throttles.
  virtual void RequestBeginning(
      net::URLRequest* request,
      ResourceContext* resource_context,
      appcache::AppCacheService* appcache_service,
      ResourceType::Type resource_type,
      int child_id,
      int route_id,
      ScopedVector<ResourceThrottle>* throttles) OVERRIDE;

  virtual ResourceDispatcherHostLoginDelegate* CreateLoginDelegate(
      net::AuthChallengeInfo* auth_info,
      net::URLRequest* request) OVERRIDE;

  virtual void DownloadStarting(
      net::URLRequest* request,
      content::ResourceContext* resource_context,
      int child_id,
      int route_id,
      int request_id,
      bool is_content_initiated,
      bool must_download,
      ScopedVector<content::ResourceThrottle>* throttles) OVERRIDE;

 private:
  void TriggerNewDownloadStartCallback(net::URLRequest* request,
                                       const std::string& user_agent,
                                       const std::string& content_disposition,
                                       const std::string& mime_type,
                                       int64 content_length);

};

}

#endif // RESOURCE_DISPATCHER_HOST_DELEGETE_EFL_H
