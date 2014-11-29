/*
 * Copyright (C) 2013 Samsung Electronics
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef ewk_web_application_icon_data_h
#define ewk_web_application_icon_data_h

#include <Eina.h>

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
