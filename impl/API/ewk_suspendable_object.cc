// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ewk_suspendable_object.h"

#include "content/public/browser/browser_thread.h"

using content::BrowserThread;

bool Ewk_Suspendable_Object::setDecision(bool allowed) {
  CHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));

  if (!isDecided_) {
    isDecided_ = true;
    callback_.Run(allowed);

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

void Ewk_Suspendable_Object::ignore() {
  isDecided_ = true;
}

bool Ewk_Suspendable_Object::suspend() {
  isSuspended_ = true;
  return true;
}
