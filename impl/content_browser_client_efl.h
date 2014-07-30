/*
   Copyright (C) 2014 Samsung Electronics

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef CONTENT_BROWSER_CLIENT_EFL
#define CONTENT_BROWSER_CLIENT_EFL

#include "content/public/browser/content_browser_client.h"

namespace base {
class CommandLine;
}

class EWebContext;

namespace content {
class ResourceDispatcherHostDelegateEfl;
class BrowserMainPartsEfl;
class RenderViewHostDelegateView;
class WebContents;
class WebContentsView;
struct ShowDesktopNotificationHostMsgParams;

class ContentBrowserClientEfl: public ContentBrowserClient {
 public:
  ContentBrowserClientEfl();

  virtual BrowserMainParts* CreateBrowserMainParts(
      const MainFunctionParams& parameters) OVERRIDE;

  virtual WebContentsView* OverrideCreateWebContentsView(
      WebContents* web_contents,
      RenderViewHostDelegateView** delegate_view) OVERRIDE;

  virtual net::URLRequestContextGetter* CreateRequestContext(
      BrowserContext* browser_context,
      ProtocolHandlerMap* protocol_handlers,
      URLRequestInterceptorScopedVector request_interceptors) OVERRIDE;

  virtual AccessTokenStore* CreateAccessTokenStore() OVERRIDE;
#if defined(OS_TIZEN)
  virtual LocationProvider* OverrideSystemLocationProvider() OVERRIDE;
#endif

  // Allows the embedder to pass extra command line flags.
  // switches::kProcessType will already be set at this point.
  virtual void AppendExtraCommandLineSwitches(base::CommandLine* command_line,
                                              int child_process_id) OVERRIDE;

  virtual void ResourceDispatcherHostCreated() OVERRIDE;

  virtual void AllowCertificateError(int render_process_id,
                                     int render_frame_id,
                                     int cert_error,
                                     const net::SSLInfo& ssl_info,
                                     const GURL& request_url,
                                     ResourceType::Type resource_type,
                                     bool overridable,
                                     bool strict_enforcement,
                                     const base::Callback<void(bool)>& callback,
                                     CertificateRequestResultType* result) OVERRIDE;

  // Asks permission to show desktop notifications.
  virtual void RequestDesktopNotificationPermission(const GURL& source_origin,
                                                    int callback_context,
                                                    int render_process_id,
                                                    int render_view_id) OVERRIDE;

  // Show a desktop notification.  If |worker| is true, the request came from an
  // HTML5 web worker, otherwise, it came from a renderer.
  virtual void ShowDesktopNotification(const ShowDesktopNotificationHostMsgParams& params,
                                       int render_process_id,
                                       int render_view_id,
                                       bool worker) OVERRIDE;

  virtual bool AllowGetCookie(const GURL& url,
                              const GURL& first_party,
                              const net::CookieList& cookie_list,
                              content::ResourceContext* context,
                              int render_process_id,
                              int render_frame_id) OVERRIDE;

  virtual bool AllowSetCookie(const GURL& url,
                              const GURL& first_party,
                              const std::string& cookie_line,
                              content::ResourceContext* context,
                              int render_process_id,
                              int render_frame_id,
                              net::CookieOptions* options) OVERRIDE;

  virtual void RenderProcessWillLaunch(content::RenderProcessHost* host) OVERRIDE;

 private:
  scoped_ptr<ResourceDispatcherHostDelegateEfl> resource_disp_host_del_efl_;
  EWebContext* web_context_;
  BrowserMainPartsEfl* browser_main_parts_efl_;

  DISALLOW_COPY_AND_ASSIGN(ContentBrowserClientEfl);
};

}

#endif // CONTENT_BROWSER_CLIENT_EFL
