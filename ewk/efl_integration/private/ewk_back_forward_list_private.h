// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef EWK_BACK_FORWARD_LIST_PRIVATE_H
#define EWK_BACK_FORWARD_LIST_PRIVATE_H

#include <tizen_webview/public/tw_back_forward_list_impl.h>

struct _Ewk_Back_Forward_List : public tizen_webview::BackForwardList {
};

struct _Ewk_Back_Forward_List_Item
    : public tizen_webview::BackForwardListItem {
};

namespace chromium_glue {

_Ewk_Back_Forward_List* from(tizen_webview::BackForwardList*);
_Ewk_Back_Forward_List_Item* from(tizen_webview::BackForwardListItem*);

}

#endif // EWK_BACK_FORWARD_LIST_PRIVATE_H
