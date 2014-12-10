// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef RESOURCE_THROTTLE__EFL_H_
#define RESOURCE_THROTTLE__EFL_H_

#include <cstddef>  //for NULL
#include "browser/policy_response_delegate_efl.h"
#include "content/public/browser/resource_throttle.h"
#include "content/public/browser/resource_controller.h"
#include "content/public/common/resource_type.h"
#include "net/url_request/url_request.h"

class  ResourceThrottleEfl : public content::ResourceThrottle {
 public:
  ResourceThrottleEfl(net::URLRequest& request,
                      content::ResourceType resource_type)
      : request_(request),
        resource_type_(resource_type),
        policy_delegate_(NULL) {
  }

  ~ResourceThrottleEfl();

  virtual void WillProcessResponse(bool* defer) override;

  virtual const char* GetNameForLogging() const override {
    return "ResourceThrottleEfl";
  }

  void Resume() {
    controller()->Resume();
  }

  void Ignore() {
    controller()->CancelAndIgnore();
  }

 private:
  net::URLRequest& request_;
  content::ResourceType resource_type_;
  scoped_refptr<PolicyResponseDelegateEfl> policy_delegate_;
};
#endif /*RESOURCE_THROTTLE__EFL_H_*/
