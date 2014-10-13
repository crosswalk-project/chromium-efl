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

#include "devtools_manager_delegate_efl.h"

#include "content/public/browser/devtools_agent_host.h"
#include "content/public/browser/web_contents.h"
#include "content/public/browser/navigation_entry.h"
#include "content/public/browser/favicon_status.h"
#include "content/public/browser/web_contents_delegate.h"
#include "content/public/browser/render_view_host.h"

using content::DevToolsAgentHost;
using content::WebContents;
using content::RenderViewHost;

namespace {

const char kTargetTypePage[] = "page";
const char kTargetTypeServiceWorker[] = "service_worker";
const char kTargetTypeOther[] = "other";

class Target : public content::DevToolsTarget {
 public:

  explicit Target(scoped_refptr<DevToolsAgentHost> agent_host);

  virtual std::string GetId() const OVERRIDE { return agent_host_->GetId(); }
  virtual std::string GetParentId() const OVERRIDE { return std::string(); }
  virtual std::string GetType() const OVERRIDE {
    switch (agent_host_->GetType()) {
      case DevToolsAgentHost::TYPE_WEB_CONTENTS:
        return kTargetTypePage;
      case DevToolsAgentHost::TYPE_SERVICE_WORKER:
        return kTargetTypeServiceWorker;
      default:
        break;
    }
    return kTargetTypeOther;
  }
  virtual std::string GetTitle() const OVERRIDE {
    return agent_host_->GetTitle();
  }
  virtual std::string GetDescription() const OVERRIDE { return std::string(); }
  virtual GURL GetURL() const OVERRIDE { return agent_host_->GetURL(); }
  virtual GURL GetFaviconURL() const OVERRIDE { return favicon_url_; }
  virtual base::TimeTicks GetLastActivityTime() const OVERRIDE {
    return last_activity_time_;
  }
  virtual bool IsAttached() const OVERRIDE {
    return agent_host_->IsAttached();
  }
  virtual scoped_refptr<DevToolsAgentHost> GetAgentHost() const OVERRIDE {
    return agent_host_;
  }
  virtual bool Activate() const OVERRIDE;
  virtual bool Close() const OVERRIDE;

 private:

  scoped_refptr<DevToolsAgentHost> agent_host_;
  std::string id_;
  std::string title_;
  GURL url_;
  GURL favicon_url_;
  base::TimeTicks last_activity_time_;
};

Target::Target(scoped_refptr<content::DevToolsAgentHost> agent_host)
    : agent_host_(agent_host) {
  if (WebContents* web_contents = agent_host_->GetWebContents()) {
    last_activity_time_ = web_contents->GetLastActiveTime();
    content::NavigationController& controller = web_contents->GetController();
    content::NavigationEntry* entry = controller.GetActiveEntry();
    if (entry != NULL && entry->GetURL().is_valid())
      favicon_url_ = entry->GetFavicon().url;
  }
}

bool Target::Activate() const {
  WebContents* web_contents = agent_host_->GetWebContents();
  if (!web_contents)
    return false;
  web_contents->GetDelegate()->ActivateContents(web_contents);
  return true;
}

bool Target::Close() const {
  WebContents* web_contents = agent_host_->GetWebContents();
  if (!web_contents)
    return false;
  web_contents->GetRenderViewHost()->ClosePage();
  return true;
}

}

namespace content {

DevToolsManagerDelegateEfl::DevToolsManagerDelegateEfl() {
}

DevToolsManagerDelegateEfl::~DevToolsManagerDelegateEfl() {
}

base::DictionaryValue* DevToolsManagerDelegateEfl::HandleCommand(
    content::DevToolsAgentHost* agent_host,
    base::DictionaryValue* command_dict) {
  return NULL;
}

std::string DevToolsManagerDelegateEfl::GetPageThumbnailData(const GURL& url) {
  return std::string();
}

scoped_ptr<DevToolsTarget> DevToolsManagerDelegateEfl::CreateNewTarget(const GURL& url) {
  return scoped_ptr<DevToolsTarget>();
}

void DevToolsManagerDelegateEfl::EnumerateTargets(TargetCallback callback) {
  TargetList targets;
  DevToolsAgentHost::List agents = DevToolsAgentHost::GetOrCreateAll();
  for (DevToolsAgentHost::List::iterator it = agents.begin();
      it != agents.end();
      ++it) {
    targets.push_back(new Target(*it));
  }
  callback.Run(targets);
}

}  // namespace content
