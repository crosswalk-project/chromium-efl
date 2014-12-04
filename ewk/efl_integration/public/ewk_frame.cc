// Copyright 2012 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ewk_frame.h"

#include "private/ewk_frame_private.h"
#include "private/ewk_private.h"

Eina_Bool ewk_frame_can_show_mime_type(Ewk_Frame_Ref frame, char* mimeType)
{
  LOG_EWK_API_MOCKUP();
  return false;
}

Eina_Bool ewk_frame_is_main_frame(Ewk_Frame_Ref frame)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(frame, EINA_FALSE);
  return frame->IsMainFrame() ? EINA_TRUE : EINA_FALSE;
}
