// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "browser/policy_response_delegate_efl.h"
#include "browser/resource_throttle_efl.h"
#include "tizen_webview/public/tw_policy_decision.h"

ResourceThrottleEfl::~ResourceThrottleEfl() {
  if (policy_delegate_.get()) {
    //If resourceThrottleEfl is destroyed, inform delegate that it cannot
    //use controller anymore.
    policy_delegate_->ThrottleDestroyed();
  }
}

void ResourceThrottleEfl::WillProcessResponse(bool* defer) {
  //Stop processing until decision will be granted.
  *defer = true;

  policy_delegate_ = new PolicyResponseDelegateEfl(&request_,
                                                   resource_type_,
                                                   this);
}
