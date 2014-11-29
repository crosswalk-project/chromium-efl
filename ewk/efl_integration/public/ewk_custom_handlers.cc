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

#include "ewk_custom_handlers.h"

#include <tizen_webview/public/tw_custom_handlers.h>

#include "private/chromium_glue.h"

struct _Ewk_Custom_Handlers_Data : public tizen_webview::Custom_Handlers_Data
{
};

Eina_Stringshare* ewk_custom_handlers_data_target_get(const Ewk_Custom_Handlers_Data* customHandlersData)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(customHandlersData, 0);
  return customHandlersData->getTarget();
}

Eina_Stringshare* ewk_custom_handlers_data_base_url_get(const Ewk_Custom_Handlers_Data* customHandlersData)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(customHandlersData, 0);
  return customHandlersData->getBaseUrl();
}

Eina_Stringshare* ewk_custom_handlers_data_url_get(const Ewk_Custom_Handlers_Data* customHandlersData)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(customHandlersData, 0);
  return customHandlersData->getUrl();
}

Eina_Stringshare* ewk_custom_handlers_data_title_get(const Ewk_Custom_Handlers_Data* customHandlersData)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(customHandlersData, 0);
  return customHandlersData->getTitle();
}

void ewk_custom_handlers_data_result_set(Ewk_Custom_Handlers_Data* customHandlersData, Ewk_Custom_Handlers_State result)
{
  EINA_SAFETY_ON_NULL_RETURN(customHandlersData);
  customHandlersData->setResult(chromium_glue::to(result));
}
