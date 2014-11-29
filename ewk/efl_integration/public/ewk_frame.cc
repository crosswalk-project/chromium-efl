/*
   Copyright (C) 2012 Samsung Electronics

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "ewk_frame.h"

#if !defined(EWK_BRINGUP)
// TODO: remove dependency
#include <tizen_webview/public/tw_frame.h>
#endif

#include "private/ewk_private.h"

namespace {
#if !defined(EWK_BRINGUP)
tizen_webview::Frame* ToImpl(void* data)
{
  return static_cast<tizen_webview::Frame*>(data);
}
#endif
} // namespace

Eina_Bool ewk_frame_can_show_mime_type(Ewk_Frame_Ref frame, char* mimeType)
{
  LOG_EWK_API_MOCKUP();
  return false;
}

Eina_Bool ewk_frame_is_main_frame(Ewk_Frame_Ref frame)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(frame, EINA_FALSE);
#if !defined(EWK_BRINGUP)
  return ToImpl(frame)->IsMainFrame() ? EINA_TRUE : EINA_FALSE;
#else
  LOG_EWK_API_MOCKUP();
  return EINA_FALSE;
#endif
}
