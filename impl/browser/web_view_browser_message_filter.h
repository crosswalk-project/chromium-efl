// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WEB_VIEW_BROWSER_MESSAGE_FILTER_H_
#define WEB_VIEW_BROWSER_MESSAGE_FILTER_H_

#include "content/public/browser/browser_message_filter.h"

namespace content {
class WebContents;
}

namespace tizen_webview {

class WebViewBrowserMessageFilterPrivate;

class WebViewBrowserMessageFilter: public content::BrowserMessageFilter {
 public:
  WebViewBrowserMessageFilter(content::WebContents* web_contents);
  ~WebViewBrowserMessageFilter() override;

  void OverrideThreadForMessage(const IPC::Message& message,
                                content::BrowserThread::ID* thread) override;
  bool OnMessageReceived(const IPC::Message& message) override;

 private:
  WebViewBrowserMessageFilterPrivate* private_;
};
} // namespace tizen_webview

#endif // WEB_VIEW_BROWSER_MESSAGE_FILTER_H_
