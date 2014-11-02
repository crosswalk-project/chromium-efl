// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GEOLOCATION_PERMISSION_CONTEXT_EFL_H
#define GEOLOCATION_PERMISSION_CONTEXT_EFL_H

#include "content/public/browser/geolocation_permission_context.h"

namespace content {

class BrowserContext;

class GeolocationPermissionContextEfl : public GeolocationPermissionContext {
public:
    GeolocationPermissionContextEfl() { }

    virtual void RequestGeolocationPermission(int, int, int, const GURL&, base::Callback<void(bool)>) override;

    // The renderer is cancelling a pending permission request.
    virtual void CancelGeolocationPermissionRequest(int, int, int, const GURL&) override;

private:
    void RequestGeolocationPermissionOnUIThread(int, int, int, const GURL&, base::Callback<void(bool)>);
};

} // namespace
#endif // GEOLOCATION_PERMISSION_CONTEXT_EFL_H
