// Copyright 2012 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

/**
 * @file    ewk_history.h
 * @brief   Describes the history(back & forward list) API of visited page.
 */

#ifndef ewk_history
#define ewk_history

#if !defined(EWK_BRINGUP)

#include <Eina.h>
#include "ewk_export.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Creates a type name for _Ewk_History */
typedef struct _Ewk_History Ewk_History;

/** Creates a type name for _Ewk_History_Item */
typedef struct _Ewk_History_Item Ewk_History_Item;

/**
 * Get the whole length of back list.
 *
 * @param history to get the whole back list
 *
 * @return number of elements in whole list.
 */
EAPI int ewk_history_back_list_length_get(Ewk_History* history);

/**
 * Get the whole length of forward list.
 *
 * @param history to get the whole forward list
 *
 * @return number of elements in whole list.
 */
EAPI int ewk_history_forward_list_length_get(Ewk_History* history);

/**
 * Get the item at a given index relative to the current item.
 *
 * @param history which history instance to query.
 * @param index position of item to get.
 *
 * @note The back list item in case of index < 0, the current item in case of index == 0, the forward list item in case of index > 0.
 *
 * @return the item at a given index relative to the current item.
 */
EAPI Ewk_History_Item* ewk_history_nth_item_get(Ewk_History* history, int index);

/**
 * Query URI for given list item.
 *
 * @param item history item to query.
 *
 * @return the URI pointer, that may be @c NULL.
 */
EAPI const char* ewk_history_item_uri_get(Ewk_History_Item* item);

/**
 * Query title for given list item.
 *
 * @param item history item to query.
 *
 * @return the title pointer, that may be @c NULL.
 */
EAPI const char* ewk_history_item_title_get(Ewk_History_Item* item);

/**
 * Free given history instance.
 *
 * @param history what to free.
 */
EAPI void ewk_history_free(Ewk_History* history);

#ifdef __cplusplus
}
#endif

#endif // EWK_BRINGUP

#endif // ewk_history
