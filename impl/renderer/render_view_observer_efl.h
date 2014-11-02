// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef RENDER_VIEW_OBSERVER_EFL_H_
#define RENDER_VIEW_OBSERVER_EFL_H_

#include <string>
#include "base/timer/timer.h"
#include "content/public/renderer/render_view_observer.h"
#include "renderer/print_web_view_helper_efl.h"
#include "content/public/renderer/render_view_observer.h"
#include "API/ewk_hit_test_private.h"
#include "third_party/WebKit/public/platform/WebSize.h"
#include "tizen_webview/public/tw_content_security_policy.h"
#include "tizen_webview/public/tw_hit_test.h"

namespace base {
class FilePath;
}

namespace content {
class RenderView;
}

namespace WebKit {
class WebHitTestResult;
}

class EwkViewMsg_LoadData_Params;

class RenderViewObserverEfl: public content::RenderViewObserver {
 public:
  explicit RenderViewObserverEfl(content::RenderView* render_view);
  virtual ~RenderViewObserverEfl();

  void DidChangeScrollOffset(blink::WebLocalFrame* frame) override;
  bool OnMessageReceived(const IPC::Message& message) override;
  virtual void DidCreateDocumentElement(blink::WebLocalFrame* frame) override;
  virtual void OrientationChangeEvent() override;
#if !defined(EWK_BRINGUP)
  virtual void DidChangePageScaleFactor() override;
#endif
  //Changes in PageScaleFactorLimits are applied when layoutUpdated is called
  //So using this notification to update minimum and maximum page scale factor values
  virtual void DidUpdateLayout() override;

 private:
  void OnSetContentSecurityPolicy(const std::string& policy, tizen_webview::ContentSecurityPolicyType header_type);
  void OnScale(double scale_factor, int x, int y);
  void OnSetScroll(int x, int y);
  void OnUseSettingsFont();
  void OnPlainTextGet(int plain_text_get_callback_id);
  void OnGetSelectionStyle();
  void OnSelectClosestWord(int x, int y);
  void OnDoHitTest(int x, int y, tizen_webview::Hit_Test_Mode mode);
  void OnDoHitTestAsync(int view_x, int view_y, tizen_webview::Hit_Test_Mode mode, int64_t request_id);
  bool DoHitTest(int view_x, int view_y, tizen_webview::Hit_Test_Mode mode, _Ewk_Hit_Test* hit_test_result, NodeAttributesMap* attributes);
  void OnPrintToPdf(int width, int height, const base::FilePath& filename);
  void OnGetMHTMLData(int callback_id);
  void OnSetDrawsTransparentBackground(bool enabled);
  void OnWebAppIconUrlGet(int callback_id);
  void OnWebAppIconUrlsGet(int callback_id);
  void OnWebAppCapableGet(int callback_id);
  void WillSubmitForm(blink::WebFrame* frame, const blink::WebFormElement& form);
  void OnSetBrowserFont();
  void CheckContentsSize();
  void OnSuspendScheduledTasks();
  void OnResumeScheduledTasks();

  blink::WebSize max_scroll_offset_;
  float cached_min_page_scale_factor_;
  float cached_max_page_scale_factor_;
  gfx::Size last_sent_contents_size_;
  base::OneShotTimer<RenderViewObserverEfl> check_contents_size_timer_;
};

#endif /* RENDER_VIEW_OBSERVER_EFL_H_ */
