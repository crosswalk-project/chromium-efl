// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef RENDER_MESSAGE_FILTER_EFL_H
#define RENDER_MESSAGE_FILTER_EFL_H

#include "API/ewk_hit_test_private.h"
#include "base/compiler_specific.h"
#include "content/public/browser/browser_message_filter.h"
#include "API/ewk_hit_test_private.h"
#include "third_party/WebKit/public/web/WebNavigationPolicy.h"
#include "third_party/WebKit/public/web/WebNavigationType.h"
#include "url/gurl.h"

struct NavigationPolicyParams;

namespace net {
class URLRequestContextGetter;
}

namespace content {
class BrowserContext;
class BrowserThread;
}

class RenderMessageFilterEfl : public content::BrowserMessageFilter {
 public:
  RenderMessageFilterEfl(int);
  ~RenderMessageFilterEfl();
  virtual bool OnMessageReceived(const IPC::Message&) OVERRIDE;
  virtual void OverrideThreadForMessage(const IPC::Message&,
      content::BrowserThread::ID*) OVERRIDE;

 private:
  void OnDecideNavigationPolicy(NavigationPolicyParams, bool* handled);
  void OnReceivedHitTestData(int view, const _Ewk_Hit_Test& hit_test_data,
      const NodeAttributesMap& node_attributes);

  int render_process_id_;
};

#endif
