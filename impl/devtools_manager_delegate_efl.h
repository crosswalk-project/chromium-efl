/*
 * Copyright (C) 2014 Samsung Electronics. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 */

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
                       content::DevToolsAgentHost* agent_host) OVERRIDE {};
  virtual void DevToolsAgentStateChanged(content::DevToolsAgentHost* agent_host,
                                         bool attached) OVERRIDE {};
  virtual base::DictionaryValue* HandleCommand(
      content::DevToolsAgentHost* agent_host,
      base::DictionaryValue* command_dict) OVERRIDE;
  virtual scoped_ptr<content::DevToolsTarget> CreateNewTarget(
      const GURL& url) OVERRIDE;
  virtual void EnumerateTargets(TargetCallback callback) OVERRIDE;
  virtual std::string GetPageThumbnailData(const GURL& url) OVERRIDE;

 private:
  DISALLOW_COPY_AND_ASSIGN(DevToolsManagerDelegateEfl);

};

} // namespace content

#endif  // DEVTOOLS_DELEGATE_EFL_H_
