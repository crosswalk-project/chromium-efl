// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ewk_application_cache_manager.h"
#include "private/ewk_private.h"
#include "ewk_context.h"

Eina_Bool ewk_application_cache_manager_clear(Ewk_Application_Cache_Manager* manager)
{
#if defined(OS_TIZEN_TV)
  return ewk_context_application_cache_delete_all(ewk_context_default_get());
#else
  return EINA_FALSE;
#endif // OS_TIZEN_TV
}
