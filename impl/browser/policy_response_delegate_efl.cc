#include "browser/policy_response_delegate_efl.h"

#include "API/ewk_policy_decision_private.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/browser/render_frame_host.h"
#include "content/public/browser/resource_dispatcher_host.h"
#include "content/public/browser/resource_request_info.h"
#include "content/public/browser/web_contents.h"

#include "web_contents_delegate_efl.h"
#include "common/web_contents_utils.h"

using content::BrowserThread;
using content::RenderViewHost;
using content::RenderFrameHost;
using content::ResourceDispatcherHost;
using content::ResourceRequestInfo;
using content::WebContents;

using web_contents_utils::WebContentsFromFrameID;

namespace {

static content::WebContentsDelegateEfl* WebContentsDelegateFromFrameId(int render_process_id, int render_frame_id) {
  WebContents* web_contents = WebContentsFromFrameID(render_process_id, render_frame_id);
  if (!web_contents)
    return NULL;

  return static_cast<content::WebContentsDelegateEfl*>(web_contents->GetDelegate());
}

}

PolicyResponseDelegateEfl::PolicyResponseDelegateEfl(net::URLRequest* request,
    const net::CompletionCallback& callback,
    const net::HttpResponseHeaders* original_response_headers)
    : policy_decision_(new tizen_webview::PolicyDecision(request->url(), original_response_headers, this)),
      callback_(callback),
      render_process_id_(0),
      render_frame_id_(0),
      render_view_id_(0),
      processed_(false) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::IO));

  const ResourceRequestInfo* info = ResourceRequestInfo::ForRequest(request);

  if (info) {
    info->GetAssociatedRenderFrame(&render_process_id_, &render_frame_id_);
    render_view_id_ = info->GetRouteID();

  } else {
    ResourceRequestInfo::GetRenderFrameForRequest(request, &render_process_id_, &render_frame_id_);
  }

  /*
   * In some situations there is no render_process and render_frame associated with
   * request. Such situation happens in TC utc_blink_ewk_geolocation_permission_request_suspend_func
   */
  //DCHECK(render_process_id_ > 0);
  //DCHECK(render_frame_id_ > 0 || render_view_id_ > 0);
  BrowserThread::PostTask(BrowserThread::UI, FROM_HERE,
        base::Bind(&PolicyResponseDelegateEfl::HandlePolicyResponseOnUIThread, this));
}

void PolicyResponseDelegateEfl::HandlePolicyResponseOnUIThread() {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  DCHECK(policy_decision_.get());

  policy_decision_->GetImpl()->InitializeOnUIThread();
  // Delegate may be retrieved ONLY on UI thread
  content::WebContentsDelegateEfl *delegate = WebContentsDelegateFromFrameId(render_process_id_, render_frame_id_);
  if (!delegate) {
    UseResponse();
    return;
  }

  // web_view_ takes owenership of Ewk_Policy_Decision. This is same as WK2/Tizen
  delegate->web_view()->InvokePolicyResponseCallback(policy_decision_.release());
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
