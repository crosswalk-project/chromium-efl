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

#include "browser/geolocation/location_provider_efl.h"

#include "base/bind.h"
#include "base/logging.h"
#include "base/time/time.h"
#include "base/message_loop/message_loop.h"
#include "content/public/common/geoposition.h"

namespace content {

LocationProviderEfl::LocationProviderEfl()
  : LocationProviderBase()
  , location_manager_(NULL)
  , geolocation_message_loop_(base::MessageLoop::current()) {
}

LocationProviderEfl::~LocationProviderEfl() {
  StopProvider();
}

// static
void LocationProviderEfl::GeoPositionChangedCb(
      double latitude, double longitude, double altitude,
      time_t timestamp, void* user_data) {
  LocationProviderEfl* location_provider =
      static_cast<LocationProviderEfl*>(user_data);
  DCHECK(location_provider);
  location_provider->NotifyPositionChanged(
      latitude, longitude, altitude, timestamp);
}

void LocationProviderEfl::NotifyPositionChanged(
      double latitude, double longitude, double altitude, time_t timestamp) {
  location_accuracy_level_e level;

  DCHECK(location_manager_);
  DCHECK(geolocation_message_loop_);

  last_position_.latitude = latitude;
  last_position_.longitude = longitude;
  last_position_.timestamp = base::Time::FromTimeT(timestamp);

  location_manager_get_last_accuracy(location_manager_,
      &level, &last_position_.accuracy, &last_position_.altitude_accuracy);

  base::Closure task = base::Bind(&LocationProviderEfl::NotifyCallback,
                                    base::Unretained(this),
                                    last_position_);
  geolocation_message_loop_->PostTask(FROM_HERE, task);
}

bool LocationProviderEfl::StartProvider(bool high_accuracy) {
  if (location_manager_) {
    // already started!
    return false;
  }

  int ret = location_manager_create(LOCATIONS_METHOD_HYBRID, &location_manager_);
  if (ret != LOCATIONS_ERROR_NONE) {
    LOG(ERROR) << "Failed to create location manager!";
    return false;
  }

  ret = location_manager_set_position_updated_cb(
      location_manager_, GeoPositionChangedCb, 1, this);
  if (ret != LOCATIONS_ERROR_NONE) {
    LOG(ERROR) << "Failed to register position changed callback!";
    location_manager_destroy(location_manager_);
    location_manager_ = NULL;
    return false;
  }

  ret = static_cast<location_error_e>(location_manager_start(location_manager_));
  if (ret != LOCATIONS_ERROR_NONE) {
    LOG(ERROR) << "Failed to start location manager: " << ret;
    location_manager_unset_position_updated_cb(location_manager_);
    location_manager_destroy(location_manager_);
    location_manager_ = NULL;
    return false;
  }

  return true;
}

void LocationProviderEfl::StopProvider() {
  if (location_manager_) {
    int ret = location_manager_stop(location_manager_);
    if (ret == LOCATIONS_ERROR_NONE) {
      location_manager_unset_position_updated_cb(location_manager_);
    }

    // TODO: didn't stop but destroy?
    location_manager_destroy(location_manager_);
    location_manager_ = NULL;
  }
}

void LocationProviderEfl::GetPosition(Geoposition* position) {
   DCHECK(position);
  *position = last_position_;
}

void LocationProviderEfl::RequestRefresh() {
}

void LocationProviderEfl::OnPermissionGranted() {
}

//static
LocationProvider* LocationProviderEfl::Create() {
  return new LocationProviderEfl();
}

}  // namespace content
