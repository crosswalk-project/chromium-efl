// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef EWK_HISTORY_PRIVATE_H
#define EWK_HISTORY_PRIVATE_H

#include <tizen_webview/public/tw_back_forward_history.h>

struct _Ewk_History : public tizen_webview::BackForwardHistory {
};

struct _Ewk_History_Item : public tizen_webview::BackForwardHistoryItem {
};

namespace chromium_glue {

_Ewk_History* from(tizen_webview::BackForwardHistory*);
_Ewk_History_Item* from(tizen_webview::BackForwardHistoryItem*);

}

#endif // EWK_HISTORY_PRIVATE_H
