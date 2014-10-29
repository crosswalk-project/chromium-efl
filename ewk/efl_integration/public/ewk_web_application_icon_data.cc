// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ewk_web_application_icon_data.h"

// TODO: remove dependency
#include <../impl/API/ewk_web_application_icon_data_private.h>

const char* ewk_web_application_icon_data_url_get(Ewk_Web_App_Icon_Data* data)
{
  return (NULL == data || data->getUrl().empty()) ? NULL : data->getUrl().c_str();
}

const char* ewk_web_application_icon_data_size_get(Ewk_Web_App_Icon_Data* data)
{
  return (NULL == data || data->getSize().empty()) ? NULL : data->getSize().c_str();
}
