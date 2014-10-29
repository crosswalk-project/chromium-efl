// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef EWK_GLOBAL_DATA_H_
#define EWK_GLOBAL_DATA_H_

namespace content {
  class BrowserMainRunner;
  class ContentMainRunner;
}

class EwkGlobalData
{
public:
  ~EwkGlobalData();

  static EwkGlobalData* GetInstance();
  static void Ensure();

private:
  EwkGlobalData();

private:
  static EwkGlobalData* instance_;

  content::ContentMainRunner* content_main_runner_;
  content::BrowserMainRunner* browser_main_runner_;
};

#endif // EWK_GLOBAL_DATA_H_
