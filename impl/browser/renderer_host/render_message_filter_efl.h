/*
    Copyright (C) 2014 Samsung Electronics

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB. If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef RENDER_MESSAGE_FILTER_EFL_H
#define RENDER_MESSAGE_FILTER_EFL_H

#include "API/ewk_hit_test_private.h"
#include "base/compiler_specific.h"
#include "content/public/browser/browser_message_filter.h"
#include "public/ewk_hit_test.h"
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
  void OnDecideNavigationPolicy(NavigationPolicyParams, bool*);
  void OnReceivedHitTestData(int, const Ewk_Hit_Test&, const NodeAttributesMap&);

  int render_process_id_;
};

#endif
