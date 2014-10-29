//Copyright (C) 2012 Intel Corporation. All rights reserved.
// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ewk_favicon_database.h"
#include "private/ewk_private.h"

#if defined(OS_TIZEN_TV)
Evas_Object* ewk_favicon_database_icon_get(Ewk_Favicon_Database* ewkIconDatabase, const char* pageURL, Evas* evas)
{
  LOG_EWK_API_MOCKUP("for Tizen TV Browser");
  return NULL;
}

void ewk_favicon_database_icon_change_callback_add(Ewk_Favicon_Database* ewkIconDatabase, Ewk_Favicon_Database_Icon_Change_Cb callback, void* userData)
{
  LOG_EWK_API_MOCKUP("for Tizen TV Browser");
}
#endif // OS_TIZEN_TV
