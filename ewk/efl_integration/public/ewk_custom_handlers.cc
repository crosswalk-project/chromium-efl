// Copyright 2012 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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
