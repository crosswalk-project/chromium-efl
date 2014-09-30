#ifndef WEB_CONTENTS_DELEGATE_EFL
#define WEB_CONTENTS_DELEGATE_EFL

#include <deque>

#include "base/memory/scoped_ptr.h"
#include "base/memory/weak_ptr.h"
#include "content/public/browser/browser_context.h"
#include "browser/javascript_dialog_manager_efl.h"
#include "browser_context_efl.h"
#include "eweb_view.h"
#include "content/public/browser/web_contents.h"
#include "content/public/browser/web_contents_delegate.h"
#include "content/public/common/frame_navigate_params.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/browser/web_contents_observer.h"
#include "content/browser/web_contents/web_contents_impl.h"
#include "url/gurl.h"
#include "browser/favicon/favicon_downloader.h"
#include "third_party/WebKit/Source/platform/text/TextDirection.h"
#include "ui/base/ime/text_input_type.h"

#include "tizen_webview/public/tw_content_security_policy.h"

class DidPrintPagesParams;

namespace tizen_webview {
class WrtIpcMessageData;
}

namespace content {
struct DateTimeSuggestion;

class WebContentsDelegateEfl
    : public WebContentsDelegate,
      public WebContentsObserver {
 public:
  WebContentsDelegateEfl(EWebView*);
  ~WebContentsDelegateEfl();

  // Reusing the given WebContents instead of creating a new one.
  WebContentsDelegateEfl(EWebView*, WebContents*);

  virtual void NavigationStateChanged(const WebContents* source,
                                      InvalidateTypes changed_flags) OVERRIDE;

  virtual void LoadProgressChanged(WebContents* source, double progress) OVERRIDE;
  virtual void LoadingStateChanged(WebContents* source,
                                   bool to_different_document) OVERRIDE;

  virtual bool ShouldCreateWebContents(
      WebContents*,
      int route_id,
      WindowContainerType,
      const base::string16& frame_name,
      const GURL& target_url,
      const std::string& partition_id,
      SessionStorageNamespace* session_storage_namespace) OVERRIDE;

  virtual void WebContentsCreated(
      WebContents* source_contents,
      int opener_render_frame_id,
      const base::string16& frame_name,
      const GURL& target_url,
      WebContents* new_contents) OVERRIDE;

  virtual void CloseContents(WebContents* source) OVERRIDE;

  virtual void ToggleFullscreenModeForTab(WebContents* web_contents,
                                          bool enter_fullscreen) OVERRIDE;
  virtual bool IsFullscreenForTabOrPending(
      const WebContents* web_contents) const OVERRIDE;
  virtual void RequestMediaAccessPermission(
      WebContents* web_contents,
      const MediaStreamRequest& request,
      const MediaResponseCallback& callback) OVERRIDE;
  void RegisterProtocolHandler(WebContents* web_contents,
                               const std::string& protocol,
                               const GURL& url,
                               bool user_gesture) OVERRIDE;

  void FindReply(WebContents* web_contents,
                 int request_id,
                 int number_of_matches,
                 const gfx::Rect& selection_rect,
                 int active_match_ordinal,
                 bool final_update) OVERRIDE;

  void RequestCertificateConfirm(WebContents* web_contents, int cert_error,
      const net::SSLInfo& ssl_info, const GURL& url, ResourceType resource_type,
      bool overridable,
      bool strict_enforcement,
      const base::Callback<void(bool)>& callback,
      CertificateRequestResultType* result);

  EWebView* web_view() const { return web_view_; }
  WebContents* web_contents() const { return web_contents_; }

  virtual void DidStartProvisionalLoadForFrame(RenderFrameHost* render_frame_host,
                                               const GURL& validated_url,
                                               bool is_error_page,
                                               bool is_iframe_srcdoc) OVERRIDE;

  virtual void DidCommitProvisionalLoadForFrame(RenderFrameHost* render_frame_host,
                                                const GURL& url,
                                                ui::PageTransition transition_type) OVERRIDE;

  virtual void DidNavigateAnyFrame(const LoadCommittedDetails& details, const FrameNavigateParams& params) OVERRIDE;
  void OnAuthRequired(net::URLRequest* request,
                      const std::string& realm,
                      LoginDelegateEfl* login_delegate);
  virtual void DidFailProvisionalLoad(RenderFrameHost* render_frame_host,
                                      const GURL& validated_url,
                                      int error_code,
                                      const base::string16& error_description) OVERRIDE;
  virtual void DidFailLoad(RenderFrameHost* render_frame_host,
                           const GURL& validated_url,
                           int error_code,
                           const base::string16& error_description) OVERRIDE;

  virtual void DidFinishLoad(RenderFrameHost* render_frame_host,
                             const GURL& validated_url) OVERRIDE;

  virtual void DidStartLoading(RenderViewHost* render_view_host) OVERRIDE;
  virtual void DidUpdateFaviconURL(const std::vector<FaviconURL>& candidates) OVERRIDE;
  virtual void DidDownloadFavicon(bool success, const GURL& icon_url, const SkBitmap& bitmap);

  void OnFormSubmit(const GURL&);
  void SetContentSecurityPolicy(const std::string& policy, tizen_webview::ContentSecurityPolicyType header_type);
  void ShowPopupMenu(const gfx::Rect& rect, blink::TextDirection textDirection, double pageScaleFactor, const std::vector<MenuItem>& items, int data, int selectedIndex, bool multiple);
  void HidePopupMenu();

  void set_new_window_policy(bool policy) { should_open_new_window_ = policy; }
  bool get_new_window_policy() const { return should_open_new_window_; }
  JavaScriptDialogManager* GetJavaScriptDialogManager() OVERRIDE;

  void DidFirstVisuallyNonEmptyPaint() OVERRIDE;

  bool OnMessageReceived(const IPC::Message& message) OVERRIDE;
  void OnPrintedMetafileReceived(const DidPrintPagesParams& params);
  virtual void NavigationEntryCommitted(const LoadCommittedDetails& load_details) OVERRIDE;
  virtual void RenderProcessGone(base::TerminationStatus status) OVERRIDE;
  virtual bool AddMessageToConsole(WebContents* source,
                                   int32 level,
                                   const base::string16& message,
                                   int32 line_no,
                                   const base::string16& source_id) OVERRIDE;
  void RunFileChooser(WebContents* web_contents, const FileChooserParams& params);
  ColorChooser* OpenColorChooser(WebContents* web_contents, SkColor color, const std::vector<ColorSuggestion>& suggestions);
  void OnAccessRequestResponse(Eina_Bool allowed);
  void OpenDateTimeDialog(ui::TextInputType dialog_type,
                          double dialog_value,
                          double min,
                          double max,
                          double step,
                          const std::vector<DateTimeSuggestion>& suggestions);

 private:
  void OnGetContentSecurityPolicy(IPC::Message* reply_msg);
  void OnWrtPluginMessage(const tizen_webview::WrtIpcMessageData& data);
  void OnWrtPluginSyncMessage(const tizen_webview::WrtIpcMessageData& data,
                              IPC::Message* reply);

  EWebView* web_view_;
  bool is_fullscreen_;
  WebContents* web_contents_;

  struct ContentSecurityPolicy {
    ContentSecurityPolicy(const std::string& p, tizen_webview::ContentSecurityPolicyType type)
        : policy(p),
          header_type(type) {
    }
    std::string policy;
    tizen_webview::ContentSecurityPolicyType header_type;
  };

  // Structure to hold media request and its callback.
  struct PendingAccessRequest {
    PendingAccessRequest(const content::MediaStreamRequest& request,
                         const content::MediaResponseCallback& callback);
    ~PendingAccessRequest();
    content::MediaStreamRequest request;
    content::MediaResponseCallback callback;
  };

  //Queue to hold all pending request for requesting user permissions.
  std::deque<PendingAccessRequest> requests_Queue_;
  scoped_ptr<ContentSecurityPolicy> pending_content_security_policy_;
  bool document_created_;
  bool should_open_new_window_;
  JavaScriptDialogManagerEfl* dialog_manager_;
  int forward_backward_list_count_;
  scoped_ptr<FaviconDownloader> favicon_downloader_;
  base::WeakPtrFactory<WebContentsDelegateEfl> weak_ptr_factory_;

  DISALLOW_COPY_AND_ASSIGN(WebContentsDelegateEfl);
};

}

#endif
