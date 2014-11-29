/*
   Copyright (C) 2013 Samsung Electronics

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

#include "ewk_content_screening_detection.h"
#include "private/ewk_private.h"

void ewk_content_screening_detection_confirmed_set(Ewk_Content_Screening_Detection* content_screening_detection, Eina_Bool confirmed)
{
  // API is postponed. Possibly chromium's mechanism may be reused.
  LOG_EWK_API_MOCKUP("Not Supported yet");
}

void ewk_content_screening_detection_suspend(Ewk_Content_Screening_Detection* content_screening_detection)
{
  // API is postponed. Possibly chromium's mechanism may be reused.
  LOG_EWK_API_MOCKUP("Not Supperted yet");
}

Ewk_Error* ewk_content_screening_detection_error_get(Ewk_Content_Screening_Detection* content_screening_detection)
{
  // API is postponed. Possibly chromium's mechanism may be reused.
  LOG_EWK_API_MOCKUP("Not Supported yet");
  return NULL;
}

