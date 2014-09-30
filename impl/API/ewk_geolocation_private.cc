// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ewk_geolocation_private.h"
#include "tizen_webview/public/tw_security_origin.h"

#include <content/public/browser/browser_thread.h>

using content::BrowserThread;

_Ewk_Geolocation_Permission_Request::_Ewk_Geolocation_Permission_Request(
    const char* host, const char* protocol, uint16_t port,
    base::Callback<void(bool)> inCallback)
  : origin_(new tizen_webview::Security_Origin(host, protocol, port))
  , isDecided_(false)
  , isSuspended_(false)
  , callback_(inCallback) {
}

_Ewk_Geolocation_Permission_Request::~_Ewk_Geolocation_Permission_Request() {
  CHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  delete origin_;
}

bool _Ewk_Geolocation_Permission_Request::SetDecision(Eina_Bool allow) {
  CHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  // Decision may be done only once.
  if (!isDecided_) {
    isDecided_ = true;
    callback_.Run(allow == EINA_TRUE);

    if (isSuspended_) {
      // If decision was suspended, then it was not deleted by the creator
      // Deletion of this object must be done after decision was made, as
      // this object will no longer be valid. But if decision was not suspended
      // it will be deleted right after permission callbacks are executed.
      BrowserThread::DeleteSoon(BrowserThread::UI, FROM_HERE, this);
    }
    return true;
  }
  return false;
}

void _Ewk_Geolocation_Permission_Request::Suspend() {
  CHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  if (!isDecided_) {
    isSuspended_ = true;
  }
}
