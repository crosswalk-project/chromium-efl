// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef LOCATION_PROVIDER_EFL_H_
#define LOCATION_PROVIDER_EFL_H_

#include "base/compiler_specific.h"
#include "content/browser/geolocation/location_provider_base.h"
#include "content/public/common/geoposition.h"

#include <location/locations.h>

namespace base {
class MessageLoop;
}

namespace content {

class LocationProviderEfl : public LocationProviderBase {
 public:
  virtual ~LocationProviderEfl();

  static LocationProvider* Create();

  virtual bool StartProvider(bool high_accuracy) OVERRIDE;
  virtual void StopProvider() OVERRIDE;
  virtual void GetPosition(Geoposition* position) OVERRIDE;
  virtual void RequestRefresh() OVERRIDE;
  virtual void OnPermissionGranted() OVERRIDE;

 private:
  LocationProviderEfl();
  static void GeoPositionChangedCb(double, double, double, time_t, void*);
  void NotifyPositionChanged(double, double, double, time_t);

  Geoposition last_position_;
  location_manager_h location_manager_;
  base::MessageLoop* geolocation_message_loop_;

  DISALLOW_COPY_AND_ASSIGN(LocationProviderEfl);
};

}  // namespace content

#endif  // LOCATION_PROVIDER_EFL_H_
