#ifndef RENDER_VIEW_OBSERVER_EFL_H_
#define RENDER_VIEW_OBSERVER_EFL_H_

#include <string>
#include "content/public/renderer/render_view_observer.h"
#include "public/ewk_csp_header_type.h"
#include "renderer/print_web_view_helper_efl.h"
#include "content/public/renderer/render_view_observer.h"
#include "public/ewk_hit_test.h"
#include "API/ewk_hit_test_private.h"
#include "third_party/WebKit/public/platform/WebSize.h"

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

  void DidChangeScrollOffset(blink::WebFrame* frame) OVERRIDE;
  bool OnMessageReceived(const IPC::Message& message) OVERRIDE;
  virtual void DidCreateDocumentElement(blink::WebFrame* frame) OVERRIDE;
  virtual void DidChangeContentsSize(blink::WebFrame* frame, const blink::WebSize& size) OVERRIDE;
  virtual void OrientationChangeEvent(int orientation) OVERRIDE;
  virtual void DidChangePageScaleFactor() OVERRIDE;

  //Changes in PageScaleFactorLimits are applied when layoutUpdated is called
  //So using this notification to update minimum and maximum page scale factor values
  virtual void DidUpdateLayout() OVERRIDE;

 private:
  void OnSetContentSecurityPolicy(const std::string& policy, Ewk_CSP_Header_Type header_type);
  void OnScale(double scale_factor, int x, int y);
  void OnSetScroll(int x, int y);
  void OnUseSettingsFont();
  void OnPlainTextGet(int plain_text_get_callback_id);
  void OnGetSelectionStyle();
  void OnSelectClosestWord(int x, int y);
  void OnDoHitTest(int x, int y, Ewk_Hit_Test_Mode mode);
  void OnPrintToPdf(int width, int height, const base::FilePath& filename);
  void OnGetMHTMLData(int callback_id);
  void OnSetDrawsTransparentBackground(bool enabled);
  void OnWebAppIconUrlGet(int callback_id);
  void OnWebAppIconUrlsGet(int callback_id);
  void OnWebAppCapableGet(int callback_id);
  void WillSubmitForm(blink::WebFrame* frame, const blink::WebFormElement& form);
  void OnSetBrowserFont();

  blink::WebSize max_scroll_offset_;
  float cached_min_page_scale_factor_;
  float cached_max_page_scale_factor_;
};

#endif /* RENDER_VIEW_OBSERVER_EFL_H_ */
