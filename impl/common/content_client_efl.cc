#include "common/content_client_efl.h"

#include "base/basictypes.h"
#include "base/command_line.h"
#include "content/public/common/content_switches.h"
#include "common/version_info.h"
#include "webkit/common/user_agent/user_agent_util.h"

std::string ContentClientEfl::GetProduct() const
{
  return EflWebView::VersionInfo::GetInstance()->ProductNameAndVersionForUserAgent();
}

std::string ContentClientEfl::GetUserAgent() const
{
  std::string product = GetProduct();
  if (CommandLine::ForCurrentProcess()->HasSwitch(
        switches::kUseMobileUserAgent)) {
    product += " Mobile";
  }
  return webkit_glue::BuildUserAgentFromOSAndProduct(EflWebView::VersionInfo::GetInstance()->OSType(),
                                                     product);
}
