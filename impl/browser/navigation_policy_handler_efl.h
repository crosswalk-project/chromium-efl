// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NAVIGATION_POLICY_HANDLER_EFL_H_
#define NAVIGATION_POLICY_HANDLER_EFL_H_

#include "navigation_policy_params.h"

namespace content {
class RenderViewHost;
}

namespace IPC {
class Message;
}

class NavigationPolicyHandlerEfl {
 public:
  enum Decision {
    Undecied = 0,
    Handled,
    Unhandled
  };

  NavigationPolicyHandlerEfl(content::RenderViewHost* rvh, const NavigationPolicyParams& params);
  virtual ~NavigationPolicyHandlerEfl();
  void DownloadNavigation();
  Decision GetDecision() const { return decision_; }
  bool SetDecision(Decision);

 private:
  content::RenderViewHost* rvh_;
  Decision decision_;
  NavigationPolicyParams params_;
};

#endif /* NAVIGATION_POLICY_HANDLER_EFL_H_ */
