// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef POLICY_RESPONSE_DELEGATE_EFL_H_
#define POLICY_RESPONSE_DELEGATE_EFL_H_

#include "base/memory/ref_counted.h"
#include "content/public/browser/resource_controller.h"
#include "content/public/common/resource_type.h"
#include "net/base/completion_callback.h"
#include "tizen_webview/public/tw_policy_decision.h"
#include "url/gurl.h"

namespace net {
class URLRequest;
class HttpResponseHeaders;
}

namespace content {
class ResourceController;
}

class ResourceThrottleEfl;

class PolicyResponseDelegateEfl: public base::RefCountedThreadSafe<PolicyResponseDelegateEfl> {
 public:
  PolicyResponseDelegateEfl(net::URLRequest* request,
                            content::ResourceType resource_type,
                            ResourceThrottleEfl* throttle);

  void UseResponse();
  void IgnoreResponse();
  void ThrottleDestroyed();

  int GetRenderProcessId() const { return render_process_id_; }
  int GetRenderFrameId() const { return render_frame_id_; }
  int GetRenderViewId() const { return render_view_id_; }

 private:
  friend class base::RefCountedThreadSafe<PolicyResponseDelegateEfl>;

  void HandlePolicyResponseOnUIThread();
  void UseResponseOnIOThread();
  void IgnoreResponseOnIOThread();

  scoped_ptr<tizen_webview::PolicyDecision> policy_decision_;
  // A throttle which blocks response and invokes policy mechanism.
  // It is used to cancel or resume response processing.
  ResourceThrottleEfl* throttle_;
  int render_process_id_;
  int render_frame_id_;
  int render_view_id_;
};

#endif /* POLICY_RESPONSE_DELEGATE_EFL_H_ */
