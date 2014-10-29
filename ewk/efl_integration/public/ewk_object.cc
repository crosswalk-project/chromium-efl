// Copyright (C) 2012 Intel Corporation. All rights reserved.
// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ewk_object.h"
#include "private/ewk_private.h"

#if defined(OS_TIZEN_TV)
Ewk_Object* ewk_object_ref(Ewk_Object* object)
{
  LOG_EWK_API_MOCKUP("for Tizen TV Browser");
  return NULL;
}

void ewk_object_unref(Ewk_Object* object)
{
  LOG_EWK_API_MOCKUP("for Tizen TV Browser");
}
#endif // OS_TIZEN_TV
