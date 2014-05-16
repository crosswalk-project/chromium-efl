#ifndef WEB_CONTENTS_DELEGATE_EFL
#define WEB_CONTENTS_DELEGATE_EFL

#include "base/memory/scoped_ptr.h"
#include "base/memory/weak_ptr.h"
#include "content/public/browser/browser_context.h"
#include "browser/javascript_dialog_manager_efl.h"
#include "browser_context_efl.h"
#include "eweb_view.h"
#include "public/ewk_ipc_message.h"
#include "public/ewk_hit_test.h"
#include "content/public/browser/web_contents.h"
#include "content/public/browser/web_contents_delegate.h"
#include "content/public/common/frame_navigate_params.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/browser/web_contents_observer.h"
#include "content/browser/web_contents/web_contents_impl.h"
#include "url/gurl.h"
#include "browser/favicon/favicon_downloader.h"

class PolicyResponseDelegateEfl;
class DidPrintPagesParams;

namespace content {

class WebContentsDelegateEfl
    : public WebContentsDelegate,
      public WebContentsObserver {
 public:
  WebContentsDelegateEfl(EWebView*);

  // Reusing the given WebContents instead of creating a new one.
  WebContentsDelegateEfl(EWebView*, WebContents*);

  virtual void NavigationStateChanged(const WebContents* source,
                                      unsigned changed_flags) OVERRIDE;

  virtual void LoadProgressChanged(WebContents* source, double progress) OVERRIDE;
  virtual void LoadingStateChanged(WebContents* source) OVERRIDE;

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
      int64 source_frame_id,
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
                               const base::string16& title,
                               bool user_gesture) OVERRIDE;

  void FindReply(WebContents* web_contents,
                 int request_id,
                 int number_of_matches,
                 const gfx::Rect& selection_rect,
                 int active_match_ordinal,
                 bool final_update) OVERRIDE;

  void RequestCertificateConfirm(WebContents* web_contents, int cert_error,
      const net::SSLInfo& ssl_info, const GURL& url, ResourceType::Type resource_type,
      bool overridable,
      bool strict_enforcement,
      const base::Callback<void(bool)>& callback,
      CertificateRequestResultType* result);

  EWebView* web_view() const { return web_view_; }
  WebContents* web_contents() const { return web_contents_.get(); }

  virtual void DidStartProvisionalLoadForFrame(int64 frame_id,
                                               int64 parent_frame_id,
                                               bool is_main_frame,
                                               const GURL& validated_url,
                                               bool is_error_page,
                                               bool is_iframe_srcdoc,
                                               RenderViewHost* render_view_host) OVERRIDE;

  virtual void DidCommitProvisionalLoadForFrame(int64 frame_id,
                                                bool is_main_frame,
                                                const GURL& url,
                                                PageTransition transition_type,
                                                RenderViewHost* render_view_host) OVERRIDE;

  virtual void DidNavigateAnyFrame(const LoadCommittedDetails& details, const FrameNavigateParams& params) OVERRIDE;
  void OnAuthRequired(net::URLRequest* request,
                      const std::string& realm,
                      LoginDelegateEfl* login_delegate);
  virtual void DidFailProvisionalLoad(int64 frame_id,
                                      const base::string16& frame_unique_name,
                                      bool is_main_frame,
                                      const GURL& validated_url,
                                      int error_code,
                                      const base::string16& error_description,
                                      RenderViewHost* render_view_host) OVERRIDE;
  virtual void DidFailLoad(int64 frame_id,
                           const GURL& validated_url,
                           bool is_main_frame,
                           int error_code,
                           const base::string16& error_description,
                           RenderViewHost* render_view_host) OVERRIDE;

  virtual void DidFinishLoad(int64 frame_id,
                             const GURL& validated_url,
                             bool is_main_frame,
                             RenderViewHost* render_view_host) OVERRIDE;

  virtual void DidStartLoading(RenderViewHost* render_view_host) OVERRIDE;
  virtual void DidUpdateFaviconURL(int32 page_id,
                                   const std::vector<FaviconURL>& candidates) OVERRIDE;
  virtual void DidDownloadFavicon(bool success, const GURL& icon_url, const SkBitmap& bitmap);

  void SetContentSecurityPolicy(const std::string& policy, Ewk_CSP_Header_Type header_type);
  void OnHeadersReceived(PolicyResponseDelegateEfl* delegate,
                         const GURL& request,
                         const net::HttpResponseHeaders* original_response_headers);
  virtual void ShowContextMenu(RenderFrameHost* render_frame_host, const ContextMenuParams& params) OVERRIDE;
  virtual void CancelContextMenu(int request_id) OVERRIDE;
  void set_new_window_policy(bool policy) { should_open_new_window_ = policy; }
  bool get_new_window_policy() const { return should_open_new_window_; }
  JavaScriptDialogManager* GetJavaScriptDialogManager() OVERRIDE;

  void DidFirstVisuallyNonEmptyPaint(int32 page_id) OVERRIDE;

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

 private:
  void OnGetContentSecurityPolicy(IPC::Message* reply_msg);
  void OnWrtPluginMessage(const Ewk_IPC_Wrt_Message_Data& data);
  void OnWrtPluginSyncMessage(const Ewk_IPC_Wrt_Message_Data& data,
                              IPC::Message* reply);

  EWebView* web_view_;
  bool is_fullscreen_;
  scoped_ptr<WebContents> web_contents_;

  struct ContentSecurityPolicy {
    ContentSecurityPolicy(const std::string& p, Ewk_CSP_Header_Type type)
        : policy(p),
          header_type(type) {
    }
    std::string policy;
    Ewk_CSP_Header_Type header_type;
  };

  scoped_ptr<ContentSecurityPolicy> pending_content_security_policy_;
  bool document_created_;
  bool should_open_new_window_;
  scoped_ptr<JavaScriptDialogManagerEfl> dialog_manager_;
  int forward_backward_list_count_;
  scoped_ptr<FaviconDownloader> favicon_downloader_;
  base::WeakPtrFactory<WebContentsDelegateEfl> weak_ptr_factory_;

  DISALLOW_COPY_AND_ASSIGN(WebContentsDelegateEfl);
};

}

#endif
