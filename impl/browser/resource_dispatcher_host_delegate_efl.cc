#include "resource_dispatcher_host_delegate_efl.h"

#include "content/public/browser/browser_thread.h"
#include "content/public/browser/web_contents.h"
#include "content/public/browser/resource_request_info.h"
#include "net/url_request/url_request.h"
#include "net/http/http_response_headers.h"
#include "browser/login_delegate_efl.h"
#include "browser_context_efl.h"
#include "common/web_contents_utils.h"
#include "eweb_context.h"

using web_contents_utils::WebContentsFromFrameID;

namespace content {

namespace {

void TriggerNewDownloadStartCallbackOnUIThread(
    int render_process_id, int render_frame_id,
    const GURL& url,
    const std::string& /*user_agent*/,
    const std::string& /*content_disposition*/,
    const std::string& /*mime_type*/,
    int64 /*content_length*/) {
  WebContents* web_contents = WebContentsFromFrameID(
      render_process_id, render_frame_id);
  if (!web_contents)
    return;

  BrowserContextEfl* browser_context = static_cast<BrowserContextEfl*>(
      web_contents->GetBrowserContext());
  if (!browser_context)
    return;

  EwkDidStartDownloadCallback* start_download_callback =
      browser_context->WebContext()->DidStartDownloadCallback();
  if (!start_download_callback)
    return;

  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  start_download_callback->TriggerCallback(url.spec());
}

}

void ResourceDispatcherHostDelegateEfl::RequestBeginning(
    net::URLRequest* request,
    ResourceContext* resource_context,
    AppCacheService* appcache_service,
    ResourceType::Type resource_type,
    int child_id,
    int route_id,
    ScopedVector<ResourceThrottle>* throttles) {
  BrowserContextEfl::ResourceContextEfl *resource_context_efl =
      static_cast<BrowserContextEfl::ResourceContextEfl*>(resource_context);
  if (!resource_context_efl)
    return;

  BrowserContextEfl *browser_context = resource_context_efl->getBrowserContext();
  if (!browser_context)
    return;

  EWebContext* eweb_context = browser_context->WebContext();
  if (!eweb_context)
    return;

  HTTPCustomHeadersEflMap header_map = eweb_context->GetHTTPCustomHeadersEflMap();
  for (HTTPCustomHeadersEflMap::iterator it = header_map.begin(); it != header_map.end(); ++it)
    request->SetExtraRequestHeaderByName(it->first, it->second, true);
}

ResourceDispatcherHostLoginDelegate*
ResourceDispatcherHostDelegateEfl::CreateLoginDelegate(
    net::AuthChallengeInfo* auth_info,
    net::URLRequest* request) {
  return new LoginDelegateEfl(auth_info, request);
}

void ResourceDispatcherHostDelegateEfl::DownloadStarting(
    net::URLRequest* request,
    content::ResourceContext* resource_context,
    int child_id,
    int route_id,
    int request_id,
    bool is_content_initiated,
    bool must_download,
    ScopedVector<content::ResourceThrottle>* throttles) {
  std::string user_agent;
  std::string content_disposition;
  std::string mime_type;
  int64 content_length = request->GetExpectedContentSize();

  request->extra_request_headers().GetHeader(
      net::HttpRequestHeaders::kUserAgent, &user_agent);

  net::HttpResponseHeaders* response_headers = request->response_headers();
  if (response_headers) {
    response_headers->GetNormalizedHeader("content-disposition",
        &content_disposition);
    response_headers->GetMimeType(&mime_type);
  }

  // POST request cannot be repeated in general, so prevent client from
  // retrying the same request, even if it is with a GET.
  if ("GET" == request->method()) {
    TriggerNewDownloadStartCallback(request,
                user_agent,
                content_disposition,
                mime_type,
                content_length);
  }
}

void ResourceDispatcherHostDelegateEfl::TriggerNewDownloadStartCallback(
    net::URLRequest* request,
    const std::string& user_agent,
    const std::string& content_disposition,
    const std::string& mime_type,
    int64 content_length) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::IO));
  int render_process_id = -1;
  int render_frame_id = -1;
  if (!ResourceRequestInfo::GetRenderFrameForRequest(
      request, &render_process_id, &render_frame_id))
    return;

  if (render_process_id == -1 || render_frame_id == -1)
    return;

  // Since called by IO thread callback trigger needs to
  // be posted to UI thread so that IO thread is unblocked
  BrowserThread::PostTask(
    BrowserThread::UI, FROM_HERE,
    base::Bind(TriggerNewDownloadStartCallbackOnUIThread,
               render_process_id,
               render_frame_id,
               request->url(),
               user_agent,
               content_disposition,
               mime_type,
               content_length));
}

bool ResourceDispatcherHostDelegateEfl::ShouldOverrideMimeType(
    const GURL& url, std::string& mime_type) {
  return false;
}

} // namespace content
