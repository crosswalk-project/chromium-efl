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
