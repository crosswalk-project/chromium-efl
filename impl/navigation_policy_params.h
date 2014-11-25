// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef POLICY_NAVIGATION_PARAMS_H_
#define POLICY_NAVIGATION_PARAMS_H_

#include "content/public/common/referrer.h"
#include "third_party/WebKit/public/web/WebNavigationPolicy.h"
#include "third_party/WebKit/public/web/WebNavigationType.h"
#include "third_party/WebKit/public/platform/WebString.h"
#include "url/gurl.h"

struct NavigationPolicyParams {
  int render_view_id;
  GURL url;
  std::string httpMethod;
  content::Referrer referrer;
  blink::WebNavigationPolicy policy;
  blink::WebNavigationType type;
  blink::WebString auth;
  std::string cookie;
  bool should_replace_current_entry;
  bool is_main_frame;
  bool is_redirect;
};

#endif /* POLICY_NAVIGATION_PARAMS_H_ */
