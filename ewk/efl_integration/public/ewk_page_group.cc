// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ewk_page_group.h"
#include "private/ewk_private.h"

#if defined(OS_TIZEN_TV)
Ewk_Page_Group* ewk_page_group_create(const char* identifier)
{
  LOG_EWK_API_MOCKUP("for Tizen TV Browser");
  return NULL;
}
#endif // OS_TIZEN_TV
