// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ewk_web_application_icon_data_private_h
#define ewk_web_application_icon_data_private_h
#include <string>

class _Ewk_Web_App_Icon_Data {
 public:
  _Ewk_Web_App_Icon_Data(const std::string& size, const std::string& url) : m_size(size), m_url(url) { }
  const std::string& getSize() const;
  const std::string& getUrl() const;
 private:
  const std::string m_size;
  const std::string m_url;
};

/**
 * Creates instance of Ewk_Web_App_Icon_Data.
 *
 * @param size of icon url
 *
 * @param url of icon string
 *
 * @return pointer to created Ewk_Web_App_Icon_Data class object
 */
_Ewk_Web_App_Icon_Data* ewkWebAppIconDataCreate(const std::string& size, const std::string& url);

/**
 * Deletes instance of Ewk_Web_App_Icon_Data.
 *
 * @param pointer to Ewk_Web_App_Icon_Data class object.
 */
void ewkWebAppIconDataDelete(_Ewk_Web_App_Icon_Data* iconData);

#endif
