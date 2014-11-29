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

#ifndef EWK_HISTORY_PRIVATE_H
#define EWK_HISTORY_PRIVATE_H

#if !defined(EWK_BRINGUP)

#include <tizen_webview/public/tw_back_forward_list.h>

struct _Ewk_History : public tizen_webview::BackForwardList {
};

struct _Ewk_History_Item : public tizen_webview::BackForwardListItem {
};

namespace chromium_glue {

_Ewk_History *from(tizen_webview::BackForwardList *);
_Ewk_History_Item *from(tizen_webview::BackForwardListItem *);

}

#endif // EWK_BRINGUP
#endif // EWK_HISTORY_PRIVATE_H
