/*
 * Copyright (C) 2014 Samsung Electronics
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

#include "ewk_history_private.h"

#if !defined(EWK_BRINGUP)

_Ewk_History *chromium_glue::from(tizen_webview::BackForwardList *list) {
  return static_cast<_Ewk_History *>(list);
}

_Ewk_History_Item *chromium_glue::from(tizen_webview::BackForwardListItem *item) {
  return static_cast<_Ewk_History_Item *>(item);
}

#endif
