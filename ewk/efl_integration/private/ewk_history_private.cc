// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ewk_history_private.h"

#if !defined(EWK_BRINGUP)

_Ewk_History *chromium_glue::from(tizen_webview::BackForwardList *list) {
  return static_cast<_Ewk_History *>(list);
}

_Ewk_History_Item *chromium_glue::from(tizen_webview::BackForwardListItem *item) {
  return static_cast<_Ewk_History_Item *>(item);
}

#endif
