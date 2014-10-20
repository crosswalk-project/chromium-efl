#include "devtools_delegate_efl.h"

#include <vector>

#include "base/bind.h"
#include "base/command_line.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/stringprintf.h"
#include "base/strings/utf_string_conversions.h"
#include "content/public/browser/devtools_agent_host.h"
#include "content/public/browser/devtools_http_handler.h"
#include "content/public/browser/devtools_target.h"
#include "content/public/common/content_switches.h"
#include "content/public/common/url_constants.h"
#include "content/public/common/user_agent.h"
#include "content/shell/browser/shell.h"
#include "grit/shell_resources.h"
#include "grit/devtools_resources.h"
#include "net/socket/tcp_server_socket.h"
#include "ui/base/resource/resource_bundle.h"
#include "base/logging.h"
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

namespace {

// Copy of internal class implementation from
// content/shell/browser/shell_devtools_delegate.cc
class TCPServerSocketFactory
    : public content::DevToolsHttpHandler::ServerSocketFactory {
 public:
  TCPServerSocketFactory(const std::string& address, int port, int backlog)
      : content::DevToolsHttpHandler::ServerSocketFactory(
            address, port, backlog) {}

 private:
  // content::DevToolsHttpHandler::ServerSocketFactory.
  virtual scoped_ptr<net::ServerSocket> Create() const OVERRIDE {
    return scoped_ptr<net::ServerSocket>(
        new net::TCPServerSocket(NULL, net::NetLog::Source()));
  }

  DISALLOW_COPY_AND_ASSIGN(TCPServerSocketFactory);
};

scoped_ptr<content::DevToolsHttpHandler::ServerSocketFactory>
CreateSocketFactory(int& port) {
  if (!port) {
    const CommandLine* const command_line = CommandLine::ForCurrentProcess();
    // See if the user specified a port on the command line (useful for
    // automation). If not, use an ephemeral port by specifying 0.
    if (command_line->HasSwitch(switches::kRemoteDebuggingPort)) {
      int temp_port;
      std::string port_str =
          command_line->GetSwitchValueASCII(switches::kRemoteDebuggingPort);
      if (base::StringToInt(port_str, &temp_port) &&
          temp_port > 0 && temp_port < 65535) {
        port = temp_port;
      } else {
        DLOG(WARNING) << "Invalid http debugger port number " << temp_port;
      }
    } else {
      //This random port is for old sdk widget debug.
      srand( (unsigned)time(NULL) + (unsigned)getpid() );
      port = random() % (65535 - 40000) + 40000;
    }
  }
  return scoped_ptr<content::DevToolsHttpHandler::ServerSocketFactory>(
      new TCPServerSocketFactory("0.0.0.0", port, 1));
}

}  // namespace

namespace content {

DevToolsDelegateEfl::DevToolsDelegateEfl(int port)
    : port_(port)
    , browser_context_(NULL)
    , devtools_http_handler_(NULL) {
  std::string frontend_url;
  devtools_http_handler_ =
    content::DevToolsHttpHandler::Start(CreateSocketFactory(port_),
       frontend_url, this, base::FilePath());
}

DevToolsDelegateEfl::~DevToolsDelegateEfl() {
}

void DevToolsDelegateEfl::Stop() {
  // The call below destroys this.
  devtools_http_handler_->Stop();
}

std::string DevToolsDelegateEfl::GetDiscoveryPageHTML() {
  return ResourceBundle::GetSharedInstance().GetRawDataResource(
      IDR_CONTENT_SHELL_DEVTOOLS_DISCOVERY_PAGE).as_string();
}

bool DevToolsDelegateEfl::BundlesFrontendResources() {
  return true;
}

base::FilePath DevToolsDelegateEfl::GetDebugFrontendDir() {
  return base::FilePath();
}

scoped_ptr<net::StreamListenSocket> DevToolsDelegateEfl::CreateSocketForTethering(
net::StreamListenSocket::Delegate* delegate,
    std::string* name) {
  return scoped_ptr<net::StreamListenSocket>();
}

}  // namespace content
