// Copyright 2012 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ewk_storage_manager.h"
#include "private/ewk_private.h"

#if defined(OS_TIZEN_TV)
Eina_Bool ewk_storage_manager_entries_clear(Ewk_Storage_Manager* ewkStorageManager)
{
  LOG_EWK_API_MOCKUP("for Tizen TV Browser");
  return false;
}
#endif // OS_TIZEN_TV
