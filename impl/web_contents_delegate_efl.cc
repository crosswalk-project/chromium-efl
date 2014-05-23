#include "web_contents_delegate_efl.h"

#include "API/ewk_console_message_private.h"
#include "API/ewk_error_private.h"
#include "API/ewk_certificate_private.h"
#include "API/ewk_policy_decision_private.h"
#include "API/ewk_user_media_private.h"
#include "API/ewk_view_private.h"
#include "browser/policy_response_delegate_efl.h"
#include "browser/renderer_host/render_widget_host_view_efl.h"
#include "common/render_messages_efl.h"
#include "eweb_view.h"
#include "eweb_view_callbacks.h"
#include "public/ewk_custom_handlers.h"
#include "public/ewk_hit_test.h"
#include "public/ewk_policy_decision.h"

#include "base/strings/utf_string_conversions.h"
#include "content/common/view_messages.h"
#include "content/public/browser/invalidate_type.h"
#include "content/public/browser/browser_thread.h"
#include "net/base/load_states.h"
#include "net/http/http_response_headers.h"
#include "printing/pdf_metafile_skia.h"
#include "url/gurl.h"

using base::string16;

namespace content {

void WritePdfDataToFile(printing::PdfMetafileSkia* metafile, const base::FilePath& filename) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::FILE));
  DCHECK(metafile);
  metafile->SaveTo(filename);
  delete metafile;
}

void FreeResponseHeaders(void* data) {
  EINA_SAFETY_ON_NULL_RETURN(data);
  eina_stringshare_del(static_cast<char*>(data));
}

WebContentsDelegateEfl::WebContentsDelegateEfl(EWebView* view)
    : web_view_(view),
      document_created_(false),
      should_open_new_window_(true),
      forward_backward_list_count_(0) {
  BrowserContext* browser_context = web_view_->context()->browser_context();
  web_contents_.reset(WebContents::Create(WebContents::CreateParams(browser_context)));
  web_contents_->SetDelegate(this);
  Observe(web_contents_.get());
}

WebContentsDelegateEfl::WebContentsDelegateEfl(EWebView* view, WebContents* contents)
  : WebContentsObserver(contents),
    web_view_(view),
    web_contents_(contents),
    document_created_(false),
    should_open_new_window_(true),
    forward_backward_list_count_(0) {
  web_contents_->SetDelegate(this);
}

void WebContentsDelegateEfl::NavigationStateChanged(const WebContents* source, unsigned changed_flags) {
  if (changed_flags & content::INVALIDATE_TYPE_URL) {
    const char* url = source->GetVisibleURL().spec().c_str();
    web_view_->SmartCallback<EWebViewCallbacks::URLChanged>().call(url);
  }
}

void WebContentsDelegateEfl::LoadingStateChanged(WebContents* source) {
  if (source->IsLoading())
    web_view_->SmartCallback<EWebViewCallbacks::LoadProgressStarted>().call();
  else
    web_view_->SmartCallback<EWebViewCallbacks::LoadProgressFinished>().call();
}

void WebContentsDelegateEfl::LoadProgressChanged(WebContents* source, double progress) {
  web_view_->SetProgressValue(progress);
  web_view_->SmartCallback<EWebViewCallbacks::LoadProgress>().call(&progress);
}

bool WebContentsDelegateEfl::ShouldCreateWebContents(
    WebContents* web_contents,
    int route_id,
    WindowContainerType window_container_type,
    const string16& frame_name,
    const GURL& target_url,
    const std::string& partition_id,
    SessionStorageNamespace* session_storage_namespace) {
  // This method is called ONLY when creating a new window, no matter what type.
  web_view_->set_policy_decision(new Ewk_Policy_Decision(this, target_url, frame_name));
  web_view_->SmartCallback<EWebViewCallbacks::NewWindowPolicyDecision>().call(web_view_->get_policy_decision());
  // Chromium has a synchronous API. We cannot block this calls on UI thread.
  CHECK(!web_view_->get_policy_decision()->isSuspended);
  if (web_view_->get_policy_decision()->isDecided)
    return should_open_new_window_;

  // By default we return false. If embedder is not prepared to handle new window creation then we prevent this behaviour.
  return false;
}

void WebContentsDelegateEfl::WebContentsCreated(
    WebContents* source_contents,
    int64 source_frame_id,
    const string16& frame_name,
    const GURL& target_url,
    WebContents* new_contents) {
  // FIXME: we should have the specifications for the new window (size, position, etc.) to set it up correctly.

  RenderWidgetHostViewEfl* source_rwhv = reinterpret_cast<RenderWidgetHostViewEfl*>(source_contents->GetRenderWidgetHostView());
  DCHECK(source_rwhv);
  EWebView* source_view = source_rwhv->eweb_view();
  DCHECK(source_view);
  source_view->CreateNewWindow(new_contents);
}

void WebContentsDelegateEfl::CloseContents(WebContents* source) {
  web_view_->SmartCallback<EWebViewCallbacks::WindowClosed>().call();
}

void WebContentsDelegateEfl::ToggleFullscreenModeForTab(WebContents* web_contents,
      bool enter_fullscreen) {
  if(enter_fullscreen)
    web_view_->SmartCallback<EWebViewCallbacks::EnterFullscreen>().call();
  else
    web_view_->SmartCallback<EWebViewCallbacks::ExitFullscreen>().call();
}
void WebContentsDelegateEfl::RegisterProtocolHandler(WebContents* web_contents,
        const std::string& protocol, const GURL& url, const string16& title, bool user_gesture) {
  scoped_ptr<Ewk_Custom_Handlers_Data> protocol_data(new Ewk_Custom_Handlers_Data(protocol.c_str(), url.host().c_str(),
      url.spec().c_str(), UTF16ToUTF8(title).c_str()));
  web_view_->SmartCallback<EWebViewCallbacks::RegisterProtocolHandler>().call(protocol_data.get());
}

void WebContentsDelegateEfl::RequestMediaAccessPermission(
        WebContents* web_contents,
        const MediaStreamRequest& request,
        const MediaResponseCallback& callback) {
  _Ewk_User_Media_Permission_Request* media_permission_request =
      new _Ewk_User_Media_Permission_Request(web_view_, request);

  web_view_->SmartCallback<EWebViewCallbacks::UserMediaPermission>().call(media_permission_request);
  MediaStreamDevices devices;
  if (media_permission_request->isDecided) {
    // TODO: we have to re-look at this when we actually connect to camera and mic (user media)
    // and how it is used in engine side. sending dummy data for now.
    // How do we construct a MediaStreamDevice?
    MediaStreamDevice audio_device(request.audio_type,
        request.requested_audio_device_id /*id*/,
        request.requested_audio_device_id/*name*/);
    MediaStreamDevice video_device(request.video_type,
            request.requested_video_device_id /*id*/,
            request.requested_video_device_id/*name*/);
    devices.push_back(audio_device);
    devices.push_back(video_device);
    //FIXME: This crashes somewhere inside, so we return an empty list for now.
    callback.Run(MediaStreamDevices(), scoped_ptr<MediaStreamUI>());
  }
  else {
    // Nothing was approved by user, so send an empty list.
    callback.Run(MediaStreamDevices(), scoped_ptr<MediaStreamUI>());
  }
}

void WebContentsDelegateEfl::OnAuthRequired(net::URLRequest* request,
                                            const std::string& realm,
                                            LoginDelegateEfl* login_delegate) {
  web_view_->InvokeAuthCallback(login_delegate, request->url(), realm);
}

void WebContentsDelegateEfl::DidStartProvisionalLoadForFrame(int64 frame_id,
                                                             int64 parent_frame_id,
                                                             bool is_main_frame,
                                                             const GURL& validated_url,
                                                             bool is_error_page,
                                                             bool is_iframe_srcdoc,
                                                             RenderViewHost* render_view_host) {
  web_view_->SmartCallback<EWebViewCallbacks::ProvisionalLoadStarted>().call();
}

void WebContentsDelegateEfl::DidCommitProvisionalLoadForFrame(int64 frame_id,
                                                              bool is_main_frame,
                                                              const GURL& url,
                                                              PageTransition transition_type,
                                                              RenderViewHost* render_view_host) {
  web_view_->SmartCallback<EWebViewCallbacks::LoadCommitted>().call();
}

void WebContentsDelegateEfl::DidNavigateAnyFrame(const LoadCommittedDetails& details, const FrameNavigateParams& params) {
  web_view_->SmartCallback<EWebViewCallbacks::ProvisionalLoadRedirect>().call();
}

void WebContentsDelegateEfl::DidFailProvisionalLoad(int64 frame_id,
                                                    const string16& frame_unique_name,
                                                    bool is_main_frame,
                                                    const GURL& validated_url,
                                                    int error_code,
                                                    const string16& error_description,
                                                    RenderViewHost* render_view_host) {
  DidFailLoad(frame_id, validated_url, is_main_frame, error_code, error_description, render_view_host);
}
void WebContentsDelegateEfl::DidFailLoad(int64 frame_id,
                                         const GURL& validated_url,
                                         bool is_main_frame,
                                         int error_code,
                                         const string16& error_description,
                                         RenderViewHost* render_view_host) {
  if (!is_main_frame)
    return;

  scoped_ptr<_Ewk_Error> error(new _Ewk_Error(error_code,
                                            validated_url.possibly_invalid_spec().c_str(),
                                            error_description.empty() ?
                                                net::ErrorToString(error_code) :
                                                UTF16ToUTF8(error_description).c_str()));

  web_view_->SmartCallback<EWebViewCallbacks::LoadError>().call(error.get());
}

void WebContentsDelegateEfl::DidFinishLoad(int64 frame_id,
                                           const GURL& validated_url,
                                           bool is_main_frame,
                                           RenderViewHost* render_view_host) {
  if (!is_main_frame)
    return;

  web_view_->SmartCallback<EWebViewCallbacks::LoadFinished>().call();
}

void WebContentsDelegateEfl::DidStartLoading(RenderViewHost* render_view_host) {
  web_view_->SmartCallback<EWebViewCallbacks::LoadStarted>().call();
}

void WebContentsDelegateEfl::RequestCertificateConfirm(WebContents* /*web_contents*/,
                                                      int cert_error,
                                                      const net::SSLInfo& ssl_info,
                                                      const GURL& url,
                                                      ResourceType::Type /*resource_type*/,
                                                      bool /*overridable*/,
                                                      bool /*strict_enforcement*/,
                                                      const base::Callback<void(bool)>& callback,
                                                      CertificateRequestResultType* result) {
  DCHECK(result);
  std::string pem_certificate;
  if (!net::X509Certificate::GetPEMEncoded(ssl_info.cert->os_cert_handle(), &pem_certificate)) {
    *result = content::CERTIFICATE_REQUEST_RESULT_TYPE_CANCEL;
    return;
  }
  // |result| can be used to deny/cancel request synchronously.
  // ewk api does not need it. We don't use it.
  scoped_ptr<_Ewk_Certificate_Policy_Decision> policy(new _Ewk_Certificate_Policy_Decision(url,
                                                                                         pem_certificate,
                                                                                         cert_error,
                                                                                         callback));
  web_view_->SmartCallback<EWebViewCallbacks::RequestCertificateConfirm>().call(policy.get());
  if (!policy->isSuspended && !policy->isDecided)
    callback.Run(true);
}

void WebContentsDelegateEfl::OnHeadersReceived(PolicyResponseDelegateEfl* delegate,
                                               const GURL& request_url,
                                               const net::HttpResponseHeaders* original_response_headers) {
  scoped_ptr<Ewk_Policy_Decision> policy_decision(new Ewk_Policy_Decision(delegate));
  policy_decision->requestUrl = request_url;
  policy_decision->responseStatusCode = original_response_headers->response_code();
  std::string mime_type;
  if (original_response_headers->GetMimeType(&mime_type)) {
    policy_decision->responseMime = eina_stringshare_add(mime_type.c_str());
    // In WK2, media player is opened for audio/video. Not handling that case for now.
    // Supported mime type list should also be updated to match platform.
    if (net::IsSupportedMimeType(mime_type))
      policy_decision->decisionType = EWK_POLICY_DECISION_USE;
    else
      policy_decision->decisionType = EWK_POLICY_DECISION_DOWNLOAD;
  }

  std::string cookie;
  if (original_response_headers->EnumerateHeader(NULL, "Set-Cookie", &cookie))
    policy_decision->cookie = eina_stringshare_add(cookie.c_str());

  void* iter = NULL;
  std::string name;
  std::string value;
  policy_decision->responseHeaders = eina_hash_string_small_new(FreeResponseHeaders);
  while (original_response_headers->EnumerateHeaderLines(&iter, &name, &value)) {
    eina_hash_add(policy_decision->responseHeaders, name.c_str(), eina_stringshare_add(value.c_str()));
  }
  // web_view_ takes owenership of Ewk_Policy_Decision. This is the same as WK2/Tizen
  web_view_->InvokePolicyResponseCallback(policy_decision.release());
}

void WebContentsDelegateEfl::SetContentSecurityPolicy(const std::string& policy, Ewk_CSP_Header_Type header_type) {
  if (document_created_) {
    RenderViewHost* rvh = web_contents_->GetRenderViewHost();
    rvh->Send(new EwkViewMsg_SetCSP(rvh->GetRoutingID(), policy, header_type));
  } else {
    DCHECK(!pending_content_security_policy_.get());
    pending_content_security_policy_.reset(new ContentSecurityPolicy(policy, header_type));
  }
}

void WebContentsDelegateEfl::ShowContextMenu(const ContextMenuParams& params) {
  web_view_->ShowContextMenu(params);
}

void WebContentsDelegateEfl::CancelContextMenu(int request_id) {
  web_view_->CancelContextMenu(request_id);
}

void WebContentsDelegateEfl::FindReply(WebContents* web_contents,
                                       int request_id,
                                       int number_of_matches,
                                       const gfx::Rect& selection_rect,
                                       int active_match_ordinal,
                                       bool final_update) {
  if (final_update && request_id == web_view_->current_find_request_id()) {
    unsigned int uint_number_of_matches = static_cast<unsigned int>(number_of_matches);
    web_view_->SmartCallback<EWebViewCallbacks::TextFound>().call(&uint_number_of_matches);
  }
}

JavaScriptDialogManager* WebContentsDelegateEfl::GetJavaScriptDialogManager() {
  if (!dialog_manager_)
    dialog_manager_.reset(new JavaScriptDialogManagerEfl());
  return dialog_manager_.get();
}

bool WebContentsDelegateEfl::OnMessageReceived(const IPC::Message& message) {
  bool handled = true;
  IPC_BEGIN_MESSAGE_MAP(WebContentsDelegateEfl, message)
    IPC_MESSAGE_HANDLER_DELAY_REPLY(EwkHostMsg_GetContentSecurityPolicy, OnGetContentSecurityPolicy)
    IPC_MESSAGE_HANDLER(EwkHostMsg_DidPrintPagesToPdf, OnPrintedMetafileReceived)
    IPC_MESSAGE_HANDLER(EwkHostMsg_WrtMessage, OnWrtPluginMessage)
    IPC_MESSAGE_HANDLER_DELAY_REPLY(EwkHostMsg_WrtSyncMessage, OnWrtPluginSyncMessage)
    IPC_MESSAGE_UNHANDLED(handled = false)
  IPC_END_MESSAGE_MAP()

  return handled;
}

void WebContentsDelegateEfl::OnWrtPluginMessage(const Ewk_IPC_Wrt_Message_Data& data) {
  scoped_ptr<Ewk_IPC_Wrt_Message_Data> p(new Ewk_IPC_Wrt_Message_Data);
  p->type = data.type;
  p->value = data.value;
  p->id = data.id;
  p->reference_id = data.reference_id;

  web_view_->SmartCallback<EWebViewCallbacks::WrtPluginsMessage>().call(p.get());
}

void WebContentsDelegateEfl::OnWrtPluginSyncMessage(const Ewk_IPC_Wrt_Message_Data& data,
                                                    IPC::Message* reply) {
  scoped_ptr<Ewk_IPC_Wrt_Message_Data> tmp(new Ewk_IPC_Wrt_Message_Data);
  tmp->type = data.type;
  web_view_->SmartCallback<EWebViewCallbacks::WrtPluginsMessage>().call(tmp.get());
  EwkHostMsg_WrtSyncMessage::WriteReplyParams(reply, tmp->value);
  Send(reply);
}

void WebContentsDelegateEfl::DidFirstVisuallyNonEmptyPaint(int32 page_id) {
  web_view_->SmartCallback<EWebViewCallbacks::FrameRendered>().call(0);
}

void WebContentsDelegateEfl::OnGetContentSecurityPolicy(IPC::Message* reply_msg) {
  document_created_ = true;
  if (!pending_content_security_policy_.get()) {
    EwkHostMsg_GetContentSecurityPolicy::WriteReplyParams(reply_msg, std::string(), EWK_DEFAULT_POLICY);
  } else {
    EwkHostMsg_GetContentSecurityPolicy::WriteReplyParams(reply_msg,
        pending_content_security_policy_->policy, pending_content_security_policy_->header_type);
    pending_content_security_policy_.reset();
  }
  Send(reply_msg);
}

void WebContentsDelegateEfl::OnPrintedMetafileReceived(const DidPrintPagesParams& params) {
  base::SharedMemory shared_buf(params.metafile_data_handle, true);
  if (!shared_buf.Map(params.data_size)) {
     NOTREACHED() << "couldn't map";
     return;
  }
  scoped_ptr<printing::PdfMetafileSkia> metafile(new printing::PdfMetafileSkia);
  if (!metafile->InitFromData(shared_buf.memory(), params.data_size)) {
    NOTREACHED() << "Invalid metafile header";
    return;
  }
  BrowserThread::PostTask(BrowserThread::FILE, FROM_HERE,
        base::Bind(&WritePdfDataToFile, metafile.release(), params.filename));
}

void WebContentsDelegateEfl::NavigationEntryCommitted(const LoadCommittedDetails& load_details) {
  int forward_backward_list_count = web_contents()->GetController().GetEntryCount();
  if (forward_backward_list_count != forward_backward_list_count_) {
    web_view_->InvokeBackForwardListChangedCallback();
    forward_backward_list_count_ = forward_backward_list_count;
  }
}

void WebContentsDelegateEfl::RenderProcessGone(base::TerminationStatus status) {
  // See RenderWidgetHostViewEfl::RenderProcessGone.
  if (status == base::TERMINATION_STATUS_ABNORMAL_TERMINATION
      || status == base::TERMINATION_STATUS_PROCESS_WAS_KILLED
      || status == base::TERMINATION_STATUS_PROCESS_CRASHED) {
    bool unused = false;
    web_view_->SmartCallback<EWebViewCallbacks::WebProcessCrashed>().call(&unused);
    // A sane app would handle the callback and delete the view immediately.
    // Otherwise, we will most probably crash when a method is called on |web_view_|.
  }
}

bool WebContentsDelegateEfl::AddMessageToConsole(WebContents* source,
                                              int32 level,
                                              const string16& message,
                                              int32 line_no,
                                              const string16& source_id) {
  scoped_ptr<_Ewk_Console_Message> console_message(new _Ewk_Console_Message(level,
                                                                          UTF16ToUTF8(message).c_str(),
                                                                          line_no,
                                                                          source->GetVisibleURL().spec().c_str()));
  web_view_->SmartCallback<EWebViewCallbacks::ConsoleMessage>().call(console_message.get());
  return true;
}

} //namespace content
