// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ewk_quota_permission_request_private_h
#define ewk_quota_permission_request_private_h

#if !defined(EWK_BRINGUP)

#include <tizen_webview/public/tw_quota_permission_request.h>

// chromium-efl implementation for _Ewk_Quota_Permission_Request
struct _Ewk_Quota_Permission_Request : public tizen_webview::Quota_Permission_Request {
};

#endif // EWK_BRINGUP
#endif // ewk_quota_permission_request_private_h
