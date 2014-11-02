// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DEVTOOLS_DELEGATE_EFL_H_
#define DEVTOOLS_DELEGATE_EFL_H_


#include "base/basictypes.h"
#include "base/compiler_specific.h"
#include "content/public/browser/devtools_http_handler_delegate.h"

namespace content {

class BrowserContext;
class DevToolsHttpHandler;

// This class is to create RemoteInspector Server(Remote Debugger) and return devtools front resources.
// This class implements DevToolsHttpHandlerDelegate interface.
// This class is similar to ShellDevToolsDelegate, which also implements DevToolsHttpHandlerDelegate interface.
class DevToolsDelegateEfl : public DevToolsHttpHandlerDelegate {
 public:
  // explicit ChromiumEflDevToolsDelegate();
  explicit DevToolsDelegateEfl(int = 0);
  virtual ~DevToolsDelegateEfl();

  // Stops http server.
  void Stop();

  // ChromiumDevToolsHttpHandler::Delegate overrides.
  virtual std::string GetDiscoveryPageHTML() override;
  virtual bool BundlesFrontendResources() override;
  virtual base::FilePath GetDebugFrontendDir() override;
  virtual scoped_ptr<net::StreamListenSocket> CreateSocketForTethering(
      net::StreamListenSocket::Delegate* delegate,
      std::string* name) override;
  DevToolsHttpHandler* devtools_http_handler() {
    return devtools_http_handler_;
  }
  int port() const {
    return port_;
  }

 private:
  int port_;
  BrowserContext* browser_context_;
  DevToolsHttpHandler* devtools_http_handler_;

};

} // namespace content

#endif  // DEVTOOLS_DELEGATE_EFL_H_
