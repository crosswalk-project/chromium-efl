// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ewk_web_application_icon_data_h
#define ewk_web_application_icon_data_h

#include <Eina.h>
#include "ewk_export.h"

#ifdef __cplusplus
extern "C" {
#endif

/// Creates a type name for _Ewk_Web_App_Icon_Data.
typedef struct _Ewk_Web_App_Icon_Data Ewk_Web_App_Icon_Data;

/**
 * Requests for getting icon size string of Ewk_Web_App_Icon_Data.
 *
 * @param icon_data Ewk_Web_App_Icon_Data object to get icon size
 *
 * @return icon size string of requested icon data
 */
EAPI const char *ewk_web_application_icon_data_size_get(Ewk_Web_App_Icon_Data *data);

/**
 * Requests for getting icon url string of Ewk_Web_App_Icon_Data.
 *
 * @param icon_data Ewk_Web_App_Icon_Data object to get icon url
 *
 * @return icon url string of requested icon data
 */
EAPI const char *ewk_web_application_icon_data_url_get(Ewk_Web_App_Icon_Data *data);


#ifdef __cplusplus
}
#endif
#endif // ewk_web_application_icon_data_h
