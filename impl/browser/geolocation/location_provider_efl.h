/*
   Copyright (C) 2013 Samsung Electronics

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

#ifndef LOCATION_PROVIDER_EFL_H_
#define LOCATION_PROVIDER_EFL_H_

#if defined(OS_TIZEN)
#include "base/compiler_specific.h"
#include "content/browser/geolocation/location_provider_base.h"
#include "content/public/common/geoposition.h"
#include <location/locations.h>

namespace content {

struct Geoposition;

class LocationProviderEfl : public LocationProviderBase {
 public:
  virtual ~LocationProviderEfl();

  static LocationProvider* Create();

  // LocationProvider.
  virtual bool StartProvider(bool high_accuracy) OVERRIDE;
  virtual void StopProvider() OVERRIDE;
  virtual void GetPosition(Geoposition* position) OVERRIDE;
  virtual void RequestRefresh() OVERRIDE;
  virtual void OnPermissionGranted() OVERRIDE;

  void SetGeoPosition(const Geoposition& position) { last_position_ = position; }

  location_manager_h GetManager() { return locationManager_; }
 private:
  LocationProviderEfl();

  Geoposition last_position_;
  location_manager_h locationManager_;
};

}  // namespace content

#endif // OS_TIZEN

#endif  // LOCATION_PROVIDER_EFL_H_