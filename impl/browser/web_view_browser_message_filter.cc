// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "browser/web_view_browser_message_filter.h"

#include "web_contents_delegate_efl.h"
#include "API/ewk_hit_test_private.h"
#include "common/render_messages_efl.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/notification_observer.h"
#include "content/public/browser/notification_registrar.h"
#include "content/public/browser/notification_source.h"
#include "content/public/browser/notification_types.h"
#include "content/public/browser/web_contents.h"
#include "tizen_webview/public/tw_error.h"

using content::BrowserThread;
using content::RenderViewHost;
using content::WebContents;
using content::WebContentsDelegateEfl;

namespace tizen_webview {

class WebViewBrowserMessageFilterPrivate
    : public content::NotificationObserver {
 public:
  WebViewBrowserMessageFilterPrivate(WebContents* web_contents) {
    DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
    registrar_.Add(this, content::NOTIFICATION_WEB_CONTENTS_DESTROYED,
        content::Source<content::WebContents>(web_contents));

    WebContentsDelegateEfl* delegate =
        static_cast<WebContentsDelegateEfl*>(web_contents->GetDelegate());
    DCHECK(delegate);
    web_view_ = delegate->web_view();
    CHECK(web_view_);
  }

  void OnReceivedHitTestData(int render_view,
                             const _Ewk_Hit_Test& hit_test_data,
                             const NodeAttributesMap& node_attributes) {
    DCHECK(BrowserThread::CurrentlyOn(BrowserThread::IO));
    if (!web_view_)
      return;

    RenderViewHost* render_view_host =
        web_view_->web_contents().GetRenderViewHost();
    DCHECK(render_view_host);

    if (render_view_host && render_view_host->GetRoutingID() == render_view)
      web_view_->UpdateHitTestData(hit_test_data, node_attributes);
  }

  void OnReceivedDidFailLoadWithError(tizen_webview::Error error) {
    if (web_view_)
      web_view_->InvokeLoadError(error);
  }

  void OnReceivedHitTestAsyncData(int render_view,
                                  const _Ewk_Hit_Test& hit_test_data,
                                  const NodeAttributesMap& node_attributes,
                                  int64_t request_id) {
    DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));

    // Only this method is called from renderer process. In particular case
    // WebContents may be destroyed here, so additional check is needed.
    if (!web_view_)
      return;

    RenderViewHost* render_view_host =
        web_view_->web_contents().GetRenderViewHost();
    DCHECK(render_view_host);

    if (render_view_host && render_view_host->GetRoutingID() == render_view) {
      web_view_->DispatchAsyncHitTestData(hit_test_data,
                                          node_attributes, request_id);
    }
  }

  void Observe(int type, const content::NotificationSource& source,
      const content::NotificationDetails& details) {
    DCHECK_EQ(content::NOTIFICATION_WEB_CONTENTS_DESTROYED, type);

    web_view_ = NULL;
  }

 private:
  EWebView* web_view_;
  content::NotificationRegistrar registrar_;
};


WebViewBrowserMessageFilter::WebViewBrowserMessageFilter(
    content::WebContents* web_contents)
    : BrowserMessageFilter(ChromeMsgStart),
      private_(new WebViewBrowserMessageFilterPrivate(web_contents)) {
}

WebViewBrowserMessageFilter::~WebViewBrowserMessageFilter() {
  // Due to NotificationRegistrar private_ shall be destroyed in UI thread
  BrowserThread::DeleteSoon(BrowserThread::UI, FROM_HERE, private_);
}

void WebViewBrowserMessageFilter::OverrideThreadForMessage(
    const IPC::Message& message, BrowserThread::ID* thread) {
  switch (message.type()) {
    case EwkViewMsg_DidFailLoadWithError::ID:
    case EwkViewHostMsg_HitTestAsyncReply::ID:
      *thread = BrowserThread::UI;
      break;
  }
}

bool WebViewBrowserMessageFilter::OnMessageReceived(
    const IPC::Message& message) {
  bool handled = true;
  IPC_BEGIN_MESSAGE_MAP(WebViewBrowserMessageFilterPrivate, message)
    IPC_MESSAGE_FORWARD(EwkViewHostMsg_HitTestReply, private_,
        WebViewBrowserMessageFilterPrivate::OnReceivedHitTestData)
    IPC_MESSAGE_FORWARD(EwkViewHostMsg_HitTestAsyncReply, private_,
        WebViewBrowserMessageFilterPrivate::OnReceivedHitTestAsyncData)
    IPC_MESSAGE_FORWARD(EwkViewMsg_DidFailLoadWithError, private_,
        WebViewBrowserMessageFilterPrivate::OnReceivedDidFailLoadWithError)
    IPC_MESSAGE_UNHANDLED(handled = false)
  IPC_END_MESSAGE_MAP()
  return handled;
}

} // namespace tizen_webview
