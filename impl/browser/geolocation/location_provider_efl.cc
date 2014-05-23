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

#if defined(OS_TIZEN)
#include "base/logging.h"
#include "base/time/time.h"
#include "content/public/common/geoposition.h"

#include "browser/geolocation/location_provider_efl.h"

namespace content {

LocationProviderEfl::LocationProviderEfl()
  : LocationProviderBase()
  , locationManager_(NULL)
{
}

LocationProviderEfl::~LocationProviderEfl()
{
  StopProvider();
}

static void GeolocationPositionChangedCallback(double latitude, double longitude, double altitude, time_t timestamp, void* user_data)
{
  LocationProviderEfl* location_provider = static_cast<LocationProviderEfl*>(user_data);
  DCHECK(location_provider);
  DCHECK(location_provider->GetManager());
  
  Geoposition geo_position;
  location_accuracy_level_e level;

  location_manager_get_last_accuracy(location_provider->GetManager(), &level, &geo_position.accuracy, &geo_position.altitude_accuracy);

  geo_position.latitude = latitude;
  geo_position.longitude = longitude;
  geo_position.timestamp = base::Time::FromTimeT(timestamp);
  location_provider->SetGeoPosition(geo_position);
}

bool LocationProviderEfl::StartProvider(bool high_accuracy)
{
  if (locationManager_) {
    // already started!
    return false;
  }

  location_error_e ret = static_cast<location_error_e>(location_manager_create(LOCATIONS_METHOD_HYBRID, &locationManager_));
  if (ret != LOCATIONS_ERROR_NONE)
    return false;

  ret = static_cast<location_error_e>(location_manager_set_position_updated_cb(locationManager_, GeolocationPositionChangedCallback, 1, this));
  if (ret != LOCATIONS_ERROR_NONE) {
    location_manager_destroy(locationManager_);
    locationManager_ = NULL;
    return false;
  }

  ret = static_cast<location_error_e>(location_manager_start(locationManager_));
  if (ret != LOCATIONS_ERROR_NONE) {
    location_manager_unset_position_updated_cb(locationManager_);
    location_manager_destroy(locationManager_);
    locationManager_ = NULL;
    return false;
  }

  return true;
}

void LocationProviderEfl::StopProvider()
{
  if (locationManager_) {
    location_error_e ret = static_cast<location_error_e>(location_manager_stop(locationManager_));
    if (ret == LOCATIONS_ERROR_NONE) {
      location_manager_unset_position_updated_cb(locationManager_);
    }

    // TODO: didn't stop but destroy?
    location_manager_destroy(locationManager_);
    locationManager_ = NULL;
  }
}

void LocationProviderEfl::GetPosition(Geoposition* position)
{
   DCHECK(position);
  *position = last_position_;
}

void LocationProviderEfl::RequestRefresh()
{
}

void LocationProviderEfl::OnPermissionGranted()
{
}

//static
LocationProvider* LocationProviderEfl::Create()
{
  return new LocationProviderEfl();
}

}  // namespace content
#endif // OS_TIZEN
