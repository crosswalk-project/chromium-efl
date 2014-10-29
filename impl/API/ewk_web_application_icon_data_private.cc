// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ewk_web_application_icon_data_private.h"

const std::string& _Ewk_Web_App_Icon_Data::getSize() const {
  return m_size;
}

const std::string& _Ewk_Web_App_Icon_Data::getUrl() const {
  return m_url;
}

_Ewk_Web_App_Icon_Data* ewkWebAppIconDataCreate(const std::string& size, const std::string& url) {
  return new(std::nothrow) _Ewk_Web_App_Icon_Data(size, url);
}

void ewkWebAppIconDataDelete(_Ewk_Web_App_Icon_Data* iconData) {
  delete iconData;
}
