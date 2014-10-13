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
  virtual std::string GetDiscoveryPageHTML() OVERRIDE;
  virtual bool BundlesFrontendResources() OVERRIDE;
  virtual base::FilePath GetDebugFrontendDir() OVERRIDE;
  virtual scoped_ptr<net::StreamListenSocket> CreateSocketForTethering(
      net::StreamListenSocket::Delegate* delegate,
      std::string* name) OVERRIDE;
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
