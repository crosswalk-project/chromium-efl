// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#if defined(OS_TIZEN_TV)

#ifndef ewk_favicon_database_private_h
#define ewk_favicon_database_private_h

#include <Evas.h>

#include <../impl/browser/favicon/favicon_database.h>
#include <tizen_webview/public/tw_web_context.h>
#include "private/ewk_context_private.h"

struct EwkFaviconDatabase {
 public:
  static EwkFaviconDatabase *Instance()
  {
    static EwkFaviconDatabase database;
    return &database;
  }

  EwkFaviconDatabase()
  :favicon_database(NULL)
  {
  }

  ~EwkFaviconDatabase()
  {
    if (favicon_database)
      delete favicon_database;
    favicon_database = NULL;
  }

  void InitEwkFaviconDatabase(const Ewk_Context* ewkContext)
  {
    if (!favicon_database)
      favicon_database = const_cast<Ewk_Context*>(ewkContext)->GetFaviconDatabase();
    return;
  }
 private:
  FaviconDatabase* favicon_database;
};
#endif // ewk_favicon_database_private_h
#endif