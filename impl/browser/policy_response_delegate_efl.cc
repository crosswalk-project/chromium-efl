#include "browser/policy_response_delegate_efl.h"

#include "content/public/browser/browser_thread.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/browser/render_frame_host.h"
#include "content/public/browser/resource_dispatcher_host.h"
#include "content/public/browser/resource_request_info.h"
#include "content/public/browser/web_contents.h"
#include "net/url_request/url_request.h"

#include "web_contents_delegate_efl.h"
#include "common/web_contents_utils.h"

using content::BrowserThread;
using content::RenderViewHost;
using content::RenderFrameHost;
using content::ResourceDispatcherHost;
using content::ResourceRequestInfo;
using content::WebContents;

using web_contents_utils::WebContentsFromFrameID;

PolicyResponseDelegateEfl::PolicyResponseDelegateEfl(net::URLRequest* request,
    const net::CompletionCallback& callback,
    const net::HttpResponseHeaders* original_response_headers)
    : original_headers_(original_response_headers),
      callback_(callback),
      url_(request->url()),
      render_process_id_(0),
      render_frame_id_(0),
      url_request_id_(request->identifier()),
      processed_(false) {
  ResourceRequestInfo::GetRenderFrameForRequest(request, &render_process_id_, &render_frame_id_);
  BrowserThread::PostTask(BrowserThread::UI, FROM_HERE,
      base::Bind(&PolicyResponseDelegateEfl::HandlePolicyResponseOnUIThread, this));
}

void PolicyResponseDelegateEfl::HandlePolicyResponseOnUIThread() {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));

  WebContents* web_contents = WebContentsFromFrameID(render_process_id_, render_frame_id_);
  if (!web_contents) {
    UseResponse();
    return;
  }

  content::WebContentsDelegateEfl* delegate =
          static_cast<content::WebContentsDelegateEfl*>(web_contents->GetDelegate());
  DCHECK(delegate);
  // | original_headers_ | is thread safe. We can use it on UI thread.
  delegate->OnHeadersReceived(this, url_, original_headers_);
}

void PolicyResponseDelegateEfl::UseResponse() {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  BrowserThread::PostTask(BrowserThread::IO, FROM_HERE,
        base::Bind(&PolicyResponseDelegateEfl::UseResponseOnIOThread, this));
}

void PolicyResponseDelegateEfl::IgnoreResponse() {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  BrowserThread::PostTask(BrowserThread::IO, FROM_HERE,
      base::Bind(&PolicyResponseDelegateEfl::IgnoreResponseOnIOThread, this));
}

void PolicyResponseDelegateEfl::UseResponseOnIOThread() {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::IO));
  if (processed_)
    return;

  callback_.Run(net::OK);
  processed_ = true;
}

void PolicyResponseDelegateEfl::IgnoreResponseOnIOThread() {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::IO));
  if (processed_)
    return;

  callback_.Run(net::ERR_BLOCKED_BY_CLIENT);
  processed_ = true;
}

void PolicyResponseDelegateEfl::HandleURLRequestDestroyedOnIOThread() {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::IO));
  // The request has gone so don't try to do anything about it further.
  processed_ = true;
}
