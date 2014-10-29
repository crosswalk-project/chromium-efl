// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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
