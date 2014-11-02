// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DEVTOOLS_MANAGER_DELEGATE_EFL_H_
#define DEVTOOLS_MANAGER_DELEGATE_EFL_H_


#include "base/basictypes.h"
#include "content/public/browser/devtools_manager_delegate.h"
#include "content/public/browser/devtools_target.h"

namespace content {

class DevToolsManagerDelegateEfl : public DevToolsManagerDelegate {
 public:
  explicit DevToolsManagerDelegateEfl();
  virtual ~DevToolsManagerDelegateEfl();

  // content::DevToolsManagerDelegate implementation.
  virtual void Inspect(content::BrowserContext* browser_context,
                       content::DevToolsAgentHost* agent_host) override {};
  virtual void DevToolsAgentStateChanged(content::DevToolsAgentHost* agent_host,
                                         bool attached) override {};
  virtual base::DictionaryValue* HandleCommand(
      content::DevToolsAgentHost* agent_host,
      base::DictionaryValue* command_dict) override;
  virtual scoped_ptr<content::DevToolsTarget> CreateNewTarget(
      const GURL& url) override;
  virtual void EnumerateTargets(TargetCallback callback) override;
  virtual std::string GetPageThumbnailData(const GURL& url) override;

 private:
  DISALLOW_COPY_AND_ASSIGN(DevToolsManagerDelegateEfl);

};

} // namespace content

#endif  // DEVTOOLS_DELEGATE_EFL_H_
