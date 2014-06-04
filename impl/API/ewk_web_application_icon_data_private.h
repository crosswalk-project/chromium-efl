/*
 * Copyright (C) 2014 Samsung Electronics
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */
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
