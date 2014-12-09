// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <memory>
#include <libintl.h>
#if defined(OS_TIZEN)
#include <vconf/vconf.h>
#endif

#include "url/gurl.h"
#include "common/render_messages_efl.h"
#include "paths_efl.h"
#include "base/path_service.h"
#include "content/public/renderer/render_thread.h"
#include "content/public/renderer/render_view.h"
#include "content/child/request_extra_data.h"
#include "renderer/render_frame_observer_efl.h"
#include "renderer/render_view_observer_efl.h"
#include "content/public/renderer/content_renderer_client.h"
#include "third_party/WebKit/public/platform/WebURLError.h"
#include "third_party/WebKit/public/platform/WebURLRequest.h"
#include "third_party/WebKit/public/web/WebDataSource.h"
#include "third_party/WebKit/public/web/WebDocument.h"
#include "third_party/WebKit/public/web/WebFrame.h"
#include "third_party/WebKit/public/web/WebView.h"

#include "components/editing/content/renderer/editorclient_agent.h"
#include "components/visitedlink/renderer/visitedlink_slave.h"

#if defined(OS_TIZEN_MOBILE)
#include "common/tts_messages_efl.h"
#include "renderer/tts_dispatcher_efl.h"
#endif

#ifdef TIZEN_AUTOFILL_SUPPORT
#include "components/autofill/content/renderer/autofill_agent.h"
#include "components/autofill/content/renderer/password_autofill_agent.h"
#include "components/autofill/content/renderer/password_generation_agent.h"
#include "components/autofill/core/common/password_generation_util.h"
#endif

#include "renderer/content_renderer_client_efl.h"
#include "navigation_policy_params.h"
#include "wrt/wrtwidget.h"

#ifdef TIZEN_AUTOFILL_SUPPORT
using autofill::AutofillAgent;
using autofill::PasswordAutofillAgent;
using autofill::PasswordGenerationAgent;
#endif

ContentRendererClientEfl::ContentRendererClientEfl() {
  wrt_widget_.reset(new WrtWidget);
}

ContentRendererClientEfl::~ContentRendererClientEfl() {
}

void ContentRendererClientEfl::RenderThreadStarted()
{
  render_process_observer_.reset(new RenderProcessObserverEfl(this));
  visited_link_slave_.reset(new visitedlink::VisitedLinkSlave());
  content::RenderThread* thread = content::RenderThread::Get();
  thread->AddObserver(render_process_observer_.get());
  thread->AddObserver(visited_link_slave_.get());
}

void ContentRendererClientEfl::RenderFrameCreated(content::RenderFrame* render_frame) {
  new content::RenderFrameObserverEfl(render_frame);
}

void ContentRendererClientEfl::RenderViewCreated(content::RenderView* render_view) {
  // Deletes itself when render_view is destroyed.
  new RenderViewObserverEfl(render_view, this);
  new editing::EditorClientAgent(render_view);

#ifdef TIZEN_AUTOFILL_SUPPORT
  PasswordGenerationAgent* password_generation_agent =
      new PasswordGenerationAgent(render_view);
  PasswordAutofillAgent* password_autofill_agent =
      new PasswordAutofillAgent(render_view);
  new AutofillAgent(render_view,
                    password_autofill_agent,
                    password_generation_agent);
#endif
}

bool ContentRendererClientEfl::HandleNavigation(content::RenderFrame* render_frame,
                                                content::DocumentState* document_state,
                                                int opener_id,
                                                blink::WebFrame* frame,
                                                const blink::WebURLRequest& request,
                                                blink::WebNavigationType type,
                                                blink::WebNavigationPolicy default_policy,
                                                bool is_redirect) {
  const content::RenderView* render_view =
      content::RenderView::FromWebView(frame->view());
  bool result = false;
  GURL referrer_url(GURL(request.httpHeaderField(blink::WebString::fromUTF8("Referer"))));
  blink::WebReferrerPolicy referrer_policy =
      request.isNull() ? frame->document().referrerPolicy() : request.referrerPolicy();
  int render_view_id = render_view->GetRoutingID();

  NavigationPolicyParams params;
  params.render_view_id = render_view_id;
  params.url = request.url();
  params.httpMethod = request.httpMethod().utf8();
  params.referrer = content::Referrer(referrer_url, referrer_policy);
  params.auth = request.httpHeaderField(blink::WebString::fromUTF8("Authorization"));
  params.policy = default_policy;
  params.is_main_frame = (frame->view()->mainFrame() == frame);
  params.type = type;
  params.is_redirect = is_redirect;
  params.cookie = request.httpHeaderField(blink::WebString::fromUTF8("Cookie")).utf8();

  blink::WebDataSource* ds = frame->provisionalDataSource();
  params.should_replace_current_entry = (ds ? ds->replacesCurrentHistoryItem() : false);

  // Sync message, renderer is blocked here.
  content::RenderThread::Get()->Send(new EwkHostMsg_DecideNavigationPolicy(
      params, &result));

  return result;
}

bool ContentRendererClientEfl::WillSendRequest(blink::WebFrame* frame,
                                               ui::PageTransition transition_type,
                                               const GURL& url,
                                               const GURL& first_party_for_cookies,
                                               GURL* new_url) {
  wrt_widget_->ParseUrl(url, *new_url);
  return true;
}

void ContentRendererClientEfl::SetWidgetInfo(int widgetHandle,
                                             double scaleFactor,
                                             const std::string &encodedBundle,
                                             const std::string &theme) {
  wrt_widget_->SetWidgetInfo(widgetHandle, scaleFactor, encodedBundle, theme);
}

void ContentRendererClientEfl::WrtMessageReceived(const tizen_webview::WrtIpcMessageData& data) {
  wrt_widget_->messageReceived(data);
}

void ContentRendererClientEfl::DidCreateScriptContext(blink::WebFrame* frame,
                                                      v8::Handle<v8::Context> context,
                                                      int extension_group,
                                                      int world_id) {
  const content::RenderView* render_view = content::RenderView::FromWebView(frame->view());

  wrt_widget_->StartSession(context, render_view->GetRoutingID());
}

void ContentRendererClientEfl::WillReleaseScriptContext(blink::WebFrame* frame,
                                                        v8::Handle<v8::Context> context,
                                                        int world_id) {
  wrt_widget_->StopSession(context);
}

unsigned long long ContentRendererClientEfl::VisitedLinkHash(const char* canonical_url,
                                                             size_t length) {
  return visited_link_slave_->ComputeURLFingerprint(canonical_url, length);
}

bool ContentRendererClientEfl::IsLinkVisited(unsigned long long link_hash) {
  return visited_link_slave_->IsVisited(link_hash);
}

void ContentRendererClientEfl::GetNavigationErrorStrings(
    content::RenderView* render_view,
    blink::WebFrame* frame,
    const blink::WebURLRequest& failed_request,
    const blink::WebURLError& error,
    std::string* error_html,
    base::string16* error_description) {
  if (error_html) {
#if defined(OS_TIZEN)
    base::FilePath locale_dir;
    PathService::Get(PathsEfl::DIR_LOCALE, &locale_dir);

    setlocale(LC_ALL, vconf_get_str(VCONFKEY_LANGSET));
    bindtextdomain("WebKit", locale_dir.value().c_str());
#endif

    std::string errorHead = std::string(dgettext(
        "WebKit", "IDS_WEBVIEW_HEADER_THIS_WEBPAGE_IS_NOT_AVAILABLE"));
    std::string errorMessage = std::string(dgettext(
        "WebKit",
        "IDS_WEBVIEW_BODY_THE_SERVER_AT_PS_CANT_BE_FOUND_BECAUSE_THE_DNS_LOOK_UP_FAILED_MSG"));
    errorMessage = base::StringPrintf(
        errorMessage.c_str(), error.unreachableURL.string().utf8().c_str());

    *error_html =
      "<html>"
        "<head>"
          "<meta name='viewport' content='width=device-width,"
          "initial-scale=1.0, user-scalable=no'>"
          "<meta http-equiv='Content-Type' content='text/html; charset=UTF-8'>"
          "<title>";
    *error_html += error.unreachableURL.string().utf8();
    *error_html +=
          "</title>"
          "<style type=text/css>"
          "#body"
          "{"
          " background-color: #fff;"
          " margin: 0;"
          " padding: 0;"
          "}"
          "#Box"
          "{"
          " background: #fff;"
          " width: 80%%;"
          " min-width: 150px;"
          " max-width: 750px;"
          " margin: auto;"
          " padding: 5px;"
          " border: 1px solid #BFA3A3;"
          " border-radius: 1px;"
          " word-wrap:break-word"
          "}"
          "</style>"
        "</head>"
        "<body bgcolor=\"#CFCFCF\">"
        "<div id=Box>"
        "<h2 align=\"center\">";
    *error_html += errorHead;
    *error_html += "</h2></br>";
    *error_html += errorMessage;
    *error_html +=
        "</div>"
        "</body>"
      "</html>"
      ;
  }
}

blink::WebSpeechSynthesizer* ContentRendererClientEfl::OverrideSpeechSynthesizer(
      blink::WebSpeechSynthesizerClient* client) {
#if defined(OS_TIZEN_MOBILE)
  return new content::TtsDispatcherEfl(client);
#else
  return NULL;
#endif
}
