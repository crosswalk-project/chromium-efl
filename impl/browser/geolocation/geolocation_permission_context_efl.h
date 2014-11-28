// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GEOLOCATION_PERMISSION_CONTEXT_EFL_H
#define GEOLOCATION_PERMISSION_CONTEXT_EFL_H

#include "base/callback.h"
#include "base/memory/weak_ptr.h"

class GURL;

namespace content {

// This includes both prompting the user and persisting results, as required.
class GeolocationPermissionContextEfl final {
 public:
  GeolocationPermissionContextEfl();

  // The renderer is requesting permission to use Geolocation.
  // When the answer to a permission request has been determined, |callback|
  // should be called with the result.
  void RequestPermission(int render_process_id,
                         int render_view_id,
                         const GURL& requesting_frame,
                         base::Callback<void(bool)> callback) const;

  // The renderer is cancelling a pending permission request.
  void CancelPermissionRequest(int render_process_id,
                               int render_view_id,
                               int bridge_id,
                               const GURL& requesting_frame) const;

 private:
  void RequestPermissionOnUIThread(int render_process_id,
                                   int render_view_id,
                                   const GURL& requesting_frame,
                                   base::Callback<void(bool)> callback) const;

  mutable base::WeakPtrFactory<GeolocationPermissionContextEfl>
      weak_ptr_factory_;
};

}  // namespace content
#endif // GEOLOCATION_PERMISSION_CONTEXT_EFL_H
