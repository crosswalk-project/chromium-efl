// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "renderer/render_view_observer_efl.h"

#include <string>
#include <limits.h>

#include "base/files/file_path.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_split.h"
#include "base/strings/string_util.h"
#include "content/public/renderer/render_view.h"
#include "common/render_messages_efl.h"
#include "third_party/WebKit/public/platform/WebCString.h"
#include "third_party/WebKit/public/platform/WebData.h"
#include "third_party/WebKit/public/platform/WebPoint.h"
#include "third_party/WebKit/public/platform/WebSize.h"
#include "third_party/WebKit/public/platform/WebString.h"
#include "third_party/WebKit/public/web/WebDocument.h"
#include "third_party/WebKit/public/web/WebFormElement.h"
#include "third_party/WebKit/public/web/WebFrame.h"
#include "third_party/WebKit/public/web/WebHitTestResult.h"
#include "third_party/WebKit/public/web/WebLocalFrame.h"
#include "third_party/WebKit/public/web/WebPageSerializer.h"
#include "third_party/WebKit/public/web/WebView.h"
#include "third_party/WebKit/public/web/WebNodeList.h"

// XXX: config.h needs to be included before internal blink headers.
//      It'd be great if we did not include not internal blibk headers.
#include "third_party/WebKit/Source/config.h"
#include "third_party/WebKit/Source/platform/fonts/FontCache.h"

using namespace tizen_webview;

namespace {

bool GetGRBAValuesFromString(const std::string& input, int* r, int* g, int* b, int* a)
{
  bool parsing_status = false;
  // for example - rgba(255, 255, 255, 255)
  // for example - rgb(255, 255, 255)
  if (std::string::npos != input.find("rgb")) {
    size_t startPos = input.find("(");
    size_t endPos = input.find(")");

    std::string value = input.substr(startPos + 1, endPos - startPos - 1);
    std::vector<std::string> colorValues;
    base::SplitString(value, ',', &colorValues);
    if (colorValues.size() == 4 || colorValues.size() == 3) {
      base::StringToInt(colorValues[0], r);
      base::StringToInt(colorValues[1], g);
      base::StringToInt(colorValues[2], b);
      if (std::string::npos != input.find("rgba") && colorValues.size() == 4)
        base::StringToInt(colorValues[3], a);
      else
        *a = 255;
      parsing_status = true;
    }
  }
  return parsing_status;
}

void PopulateEwkHitTestData(const blink::WebHitTestResult& web_hit_test, _Ewk_Hit_Test* ewk_hit_test)
{
  DCHECK(ewk_hit_test);
  ewk_hit_test->imageURI = web_hit_test.absoluteImageURL().string().utf8();
  ewk_hit_test->linkURI = web_hit_test.absoluteLinkURL().string().utf8();
  ewk_hit_test->mediaURI = web_hit_test.absoluteMediaURL().string().utf8();
  ewk_hit_test->linkLabel = web_hit_test.textContent().utf8();
  ewk_hit_test->linkTitle = web_hit_test.titleDisplayString().utf8();
  ewk_hit_test->isEditable = web_hit_test.isContentEditable();

  int context = TW_HIT_TEST_RESULT_CONTEXT_DOCUMENT;
  if (!web_hit_test.absoluteLinkURL().isEmpty())
    context |= TW_HIT_TEST_RESULT_CONTEXT_LINK;
  if (!web_hit_test.absoluteImageURL().isEmpty())
    context |= TW_HIT_TEST_RESULT_CONTEXT_IMAGE;
  if (!web_hit_test.absoluteMediaURL().isEmpty())
    context |= TW_HIT_TEST_RESULT_CONTEXT_MEDIA;;
  if (web_hit_test.isSelected())
    context |= TW_HIT_TEST_RESULT_CONTEXT_SELECTION;
  if (web_hit_test.isContentEditable())
    context |= TW_HIT_TEST_RESULT_CONTEXT_EDITABLE;
  if (web_hit_test.node().isTextNode())
    context |= TW_HIT_TEST_RESULT_CONTEXT_TEXT;

  ewk_hit_test->context = static_cast<tizen_webview::Hit_Test_Result_Context>(context);
  ewk_hit_test->nodeData.attributeHash = 0;

  if (ewk_hit_test->mode & TW_HIT_TEST_MODE_NODE_DATA) {
    ewk_hit_test->nodeData.tagName = web_hit_test.node().nodeName().utf8();
    ewk_hit_test->nodeData.nodeValue = web_hit_test.node().nodeValue().utf8();
  }

  if ((ewk_hit_test->mode & TW_HIT_TEST_MODE_IMAGE_DATA) &&
      (ewk_hit_test->context & TW_HIT_TEST_RESULT_CONTEXT_IMAGE)) {
    blink::WebElement hit_element = web_hit_test.node().toConst<blink::WebElement>();
    ewk_hit_test->imageData.imageBitmap = hit_element.imageContents().getSkBitmap();
    ewk_hit_test->imageData.fileNameExtension = hit_element.imageFilenameExtension().utf8();
  }
}

void PopulateNodeAttributesMapFromHitTest(const blink::WebHitTestResult& web_hit_test,
                                          NodeAttributesMap* attributes)
{
  if (!attributes || !web_hit_test.node().isElementNode())
    return;

  blink::WebElement hit_element = web_hit_test.node().toConst<blink::WebElement>();
  for (unsigned int i = 0; i < hit_element.attributeCount(); i++) {
    attributes->insert(std::pair<std::string, std::string>(
        hit_element.attributeLocalName(i).utf8(), hit_element.attributeValue(i).utf8()));
  }
}

}  //namespace

RenderViewObserverEfl::RenderViewObserverEfl(
    content::RenderView* render_view,
    ContentRendererClientEfl* render_client)
  : content::RenderViewObserver(render_view),
    cached_min_page_scale_factor_(-1.0),
    cached_max_page_scale_factor_(-1.0),
    renderer_client_(render_client)
{
}

RenderViewObserverEfl::~RenderViewObserverEfl()
{
}

bool RenderViewObserverEfl::OnMessageReceived(const IPC::Message& message)
{
  bool handled = true;
  IPC_BEGIN_MESSAGE_MAP(RenderViewObserverEfl, message)
    IPC_MESSAGE_HANDLER(EwkViewMsg_SetCSP, OnSetContentSecurityPolicy)
    IPC_MESSAGE_HANDLER(EwkViewMsg_Scale, OnScale)
    IPC_MESSAGE_HANDLER(EwkViewMsg_SetScroll, OnSetScroll)
    IPC_MESSAGE_HANDLER(EwkViewMsg_UseSettingsFont, OnUseSettingsFont)
    IPC_MESSAGE_HANDLER(EwkViewMsg_PlainTextGet, OnPlainTextGet)
    IPC_MESSAGE_HANDLER(EwkViewMsg_GetSelectionStyle, OnGetSelectionStyle);
    IPC_MESSAGE_HANDLER(EwkViewMsg_SelectClosestWord, OnSelectClosestWord);
    IPC_MESSAGE_HANDLER(EwkViewMsg_DoHitTest, OnDoHitTest)
    IPC_MESSAGE_HANDLER(EwkViewMsg_DoHitTestAsync, OnDoHitTestAsync)
    IPC_MESSAGE_HANDLER(EwkViewMsg_PrintToPdf, OnPrintToPdf)
    IPC_MESSAGE_HANDLER(EwkViewMsg_GetMHTMLData, OnGetMHTMLData);
    IPC_MESSAGE_HANDLER(EwkViewMsg_WebAppIconUrlGet, OnWebAppIconUrlGet);
    IPC_MESSAGE_HANDLER(EwkViewMsg_WebAppIconUrlsGet, OnWebAppIconUrlsGet);
    IPC_MESSAGE_HANDLER(EwkViewMsg_WebAppCapableGet, OnWebAppCapableGet);
    IPC_MESSAGE_HANDLER(EwkViewMsg_SetDrawsTransparentBackground, OnSetDrawsTransparentBackground);
    IPC_MESSAGE_HANDLER(EwkViewMsg_SetBrowserFont, OnSetBrowserFont);
    IPC_MESSAGE_HANDLER(EwkViewMsg_SuspendScheduledTask, OnSuspendScheduledTasks);
    IPC_MESSAGE_HANDLER(EwkViewMsg_ResumeScheduledTasks, OnResumeScheduledTasks);
    IPC_MESSAGE_HANDLER(EflViewMsg_UpdateSettings, OnUpdateSettings);

    IPC_MESSAGE_UNHANDLED(handled = false)
  IPC_END_MESSAGE_MAP()
  return handled;
}

void RenderViewObserverEfl::DidCreateDocumentElement(blink::WebLocalFrame* frame)
{
  std::string policy;
  tizen_webview::ContentSecurityPolicyType type = TW_CSP_DEFAULT_POLICY;
  Send(new EwkHostMsg_GetContentSecurityPolicy(render_view()->GetRoutingID(), &policy, &type));

#if !defined(EWK_BRINGUP)
  // Since, Webkit supports some more types and we cast ewk type to Webkit type.
  // We allow only ewk types.
  if (type == TW_CSP_REPORT_ONLY || type == TW_CSP_ENFORCE_POLICY)
    frame->document().setContentSecurityPolicyUsingHeader(blink::WebString::fromUTF8(policy), type);
#endif
}

void RenderViewObserverEfl::OnSetContentSecurityPolicy(const std::string& policy, tizen_webview::ContentSecurityPolicyType header_type)
{
#if !defined(EWK_BRINGUP)
  blink::WebView* view = render_view()->GetWebView();
  DCHECK(view);
  blink::WebDocument document = view->mainFrame()->document();
  document.setContentSecurityPolicyUsingHeader(blink::WebString::fromUTF8(policy), header_type);
#endif
}

void RenderViewObserverEfl::OnScale(double scale_factor, int x, int y)
{
  blink::WebView* view = render_view()->GetWebView();
  if (!view)
    return;

  view->setPageScaleFactor(scale_factor,
                           blink::WebPoint(x, y));
}

void RenderViewObserverEfl::OnSetScroll(int x, int y)
{
  blink::WebFrame* frame = render_view()->GetWebView()->mainFrame();
  if (!frame)
    return;
  frame->setScrollOffset(blink::WebSize(x, y));
}

void RenderViewObserverEfl::OnUseSettingsFont()
{
  blink::FontCache::fontCache()->invalidate();

#if !defined(EWK_BRINGUP)
  blink::WebView* view = render_view()->GetWebView();
  if (view)
    view->sendResizeEventAndForceLayout();
#endif
}

void RenderViewObserverEfl::OnPlainTextGet(int plain_text_get_callback_id)
{
  blink::WebFrame* frame = render_view()->GetWebView()->mainFrame();
  if (!frame)
    return;
  blink::WebString content = frame->contentAsText(INT_MAX);
  Send(new EwkHostMsg_PlainTextGetContents(render_view()->GetRoutingID(), content.utf8(), plain_text_get_callback_id));
}

void RenderViewObserverEfl::DidChangeScrollOffset(blink::WebLocalFrame* frame)
{
  if (!frame || (render_view()->GetWebView()->mainFrame() != frame))
    return;

  if (max_scroll_offset_ != frame->maximumScrollOffset()) {
    max_scroll_offset_ = frame->maximumScrollOffset();
    Send(new EwkHostMsg_DidChangeMaxScrollOffset(render_view()->GetRoutingID(),
                                                 frame->maximumScrollOffset().width,
                                                 frame->maximumScrollOffset().height));
  }

  if(last_scroll_offset_ != frame->scrollOffset()) {
    last_scroll_offset_ = frame->scrollOffset();
    Send(new EwkHostMsg_DidChangeScrollOffset(render_view()->GetRoutingID(),
                                              frame->scrollOffset().width,
                                              frame->scrollOffset().height));
  }
}

void RenderViewObserverEfl::OnGetSelectionStyle()
{
  blink::WebView* view = render_view()->GetWebView();
  if (!view)
    return;

  blink::WebFrame* frame = view->focusedFrame();
  if (!frame)
    return;

  SelectionStylePrams params;
  params.underline_state = frame->commandState(blink::WebString::fromUTF8("underline"));
  params.italic_state = frame->commandState(blink::WebString::fromUTF8("italic"));
  params.bold_state = frame->commandState(blink::WebString::fromUTF8("bold"));
  blink::WebString bg_color = frame->commandValue(blink::WebString::fromUTF8("BackColor"));
  GetGRBAValuesFromString(std::string(bg_color.utf8()), &params.bg_color.r, &params.bg_color.g, &params.bg_color.b, &params.bg_color.a);
  blink::WebString color = frame->commandValue(blink::WebString::fromUTF8("ForeColor"));
  GetGRBAValuesFromString(std::string(color.utf8()), &params.color.r, &params.color.g, &params.color.b, &params.color.a);
  blink::WebString  font_size = frame->commandValue(blink::WebString::fromUTF8("FontSize"));
  params.font_size = font_size.utf8();
  params.order_list_state = frame->commandState(blink::WebString::fromUTF8("InsertOrderedList"));
  params.un_order_list_state = frame->commandState(blink::WebString::fromUTF8("InsertUnorderedList"));
  params.text_align_center_state = frame->commandState(blink::WebString::fromUTF8("JustifyCenter"));
  params.text_align_left_state = frame->commandState(blink::WebString::fromUTF8("JustifyLeft"));
  params.text_align_right_state = frame->commandState(blink::WebString::fromUTF8("JustifyRight"));
  params.text_align_full_state = frame->commandState(blink::WebString::fromUTF8("JustifyFull"));
  params.has_composition = frame->hasMarkedText();
  Send(new EwkViewMsg_SelectionTextStyleState(render_view()->GetRoutingID(), params));
}

void RenderViewObserverEfl::OnSelectClosestWord(int x, int y)
{
  blink::WebView* view = render_view()->GetWebView();
  if (!view)
    return;

  blink::WebFrame* frame = view->focusedFrame();
  if (!frame)
    return;

  frame->selectClosestWord(x, y);
}

void RenderViewObserverEfl::OnDoHitTest(int view_x, int view_y, tizen_webview::Hit_Test_Mode mode)
{
  _Ewk_Hit_Test hit_test_result;
  NodeAttributesMap attributes;

  if (DoHitTest(view_x, view_y, mode, &hit_test_result, &attributes)) {
    Send(new EwkViewHostMsg_HitTestReply(routing_id(), hit_test_result, attributes));
  }
}

void RenderViewObserverEfl::OnDoHitTestAsync(int view_x, int view_y, tizen_webview::Hit_Test_Mode mode, int64_t request_id)
{
  _Ewk_Hit_Test hit_test_result;
  NodeAttributesMap attributes;

  if (DoHitTest(view_x, view_y, mode, &hit_test_result, &attributes)) {
    Send(new EwkViewHostMsg_HitTestAsyncReply(routing_id(), hit_test_result, attributes, request_id));
  }
}

bool RenderViewObserverEfl::DoHitTest(int view_x, int view_y, tizen_webview::Hit_Test_Mode mode, _Ewk_Hit_Test* hit_test_result, NodeAttributesMap* attributes)
{
  DCHECK(hit_test_result);
  DCHECK(attributes);

  if (!render_view() || !render_view()->GetWebView())
    return false;

  const blink::WebHitTestResult web_hit_test_result =
      render_view()->GetWebView()->hitTestResultAt(
          blink::WebPoint(view_x, view_y));

  hit_test_result->mode = mode;

  PopulateEwkHitTestData(web_hit_test_result, hit_test_result);
  if (hit_test_result->mode & TW_HIT_TEST_MODE_NODE_DATA)
    PopulateNodeAttributesMapFromHitTest(web_hit_test_result, attributes);

  return true;
}

void RenderViewObserverEfl::OnPrintToPdf(int width, int height, const base::FilePath& filename)
{
  blink::WebView* web_view = render_view()->GetWebView();
  DCHECK(web_view);
  PrintWebViewHelperEfl print_helper(render_view(), filename);
  print_helper.PrintToPdf(width, height);
}

void RenderViewObserverEfl::OnGetMHTMLData(int callback_id)
{
  blink::WebView* view = render_view()->GetWebView();
  if (!view)
    return;

  std::string content_string;
  blink::WebCString content =  blink::WebPageSerializer::serializeToMHTML(view);
  if (!content.isEmpty())
    content_string = content.data();

  Send(new EwkHostMsg_ReadMHTMLData(render_view()->GetRoutingID(), content_string, callback_id));
}

#if !defined(EWK_BRINGUP)
void RenderViewObserverEfl::DidChangePageScaleFactor()
{
  blink::WebView* view = render_view()->GetWebView();
  if (!view)
    return;

  Send(new EwkHostMsg_DidChangePageScaleFactor(render_view()->GetRoutingID(), view->pageScaleFactor()));
}
#endif

void RenderViewObserverEfl::DidUpdateLayout()
{
  blink::WebView* view = render_view()->GetWebView();
  if (!view)
    return;

  float min_scale = view->minimumPageScaleFactor();
  float max_scale = view->maximumPageScaleFactor();

  // Checking for change in minimum and maximum page scale factors
  if (std::abs(cached_max_page_scale_factor_ - max_scale) < std::numeric_limits<float>::epsilon() &&
        std::abs(cached_min_page_scale_factor_ - min_scale) < std::numeric_limits<float>::epsilon())
    return;

  cached_max_page_scale_factor_ = max_scale;
  cached_min_page_scale_factor_ = min_scale;
  Send(new EwkHostMsg_DidChangePageScaleRange(render_view()->GetRoutingID(), min_scale, max_scale));

  // Check if the timer is already running
  if (check_contents_size_timer_.IsRunning())
    return;

  check_contents_size_timer_.Start(FROM_HERE,
                                   base::TimeDelta::FromMilliseconds(0), this,
                                   &RenderViewObserverEfl::CheckContentsSize);
}

void RenderViewObserverEfl::CheckContentsSize()
{
  blink::WebView* view = render_view()->GetWebView();
  if (!view || !view->mainFrame())
    return;

  gfx::Size contents_size = view->mainFrame()->contentsSize();

  // Fall back to contentsPreferredMinimumSize if the mainFrame is reporting a
  // 0x0 size (this happens during initial load).
  if (contents_size.IsEmpty()) {
    contents_size = render_view()->GetWebView()->contentsPreferredMinimumSize();
  }

  if (contents_size == last_sent_contents_size_)
    return;

  last_sent_contents_size_ = contents_size;
  const blink::WebSize size = static_cast<blink::WebSize>(contents_size);
  Send(new EwkHostMsg_DidChangeContentsSize(render_view()->GetRoutingID(),
                                            size.width,
                                            size.height));
}

void RenderViewObserverEfl::OnSetDrawsTransparentBackground(bool draws_transparent_background)
{
  blink::WebView* view = render_view()->GetWebView();
  if (!view)
    return;

  blink::WebColor backgroundColor = draws_transparent_background ?
      static_cast<blink::WebColor>(0x00000000) : static_cast<blink::WebColor>(0xFFFFFFFF);

  view->setBaseBackgroundColor(backgroundColor);
}

void RenderViewObserverEfl::OnWebAppIconUrlGet(int callback_id)
{
  blink::WebFrame *frame = render_view()->GetWebView()->mainFrame();
  if (!frame) {
    return;
  }

  blink::WebDocument document = frame->document();
  blink::WebElement head = document.head();
  if (head.isNull()) {
    return;
  }

  std::string iconUrl;
  std::string appleIconUrl;
  blink::WebNodeList nodes = head.childNodes();
  // We're looking for Apple style rel ("apple-touch-*")
  // and Google style rel ("icon"), but we prefer the Apple one
  // when both appear, as WebKit-efl was looking only for Apple style rels.
  for (int i = 0; i < nodes.length(); ++i) {
    blink::WebNode node = nodes.item(i);
    if (!node.isElementNode()) {
      continue;
    }
    blink::WebElement elem = node.to<blink::WebElement>();
    if (!elem.hasHTMLTagName("link")) {
      continue;
    }
    std::string rel = elem.getAttribute("rel").utf8();
    if (LowerCaseEqualsASCII(rel, "apple-touch-icon") ||              // Apple's way
        LowerCaseEqualsASCII(rel, "apple-touch-icon-precomposed")) {
      appleIconUrl = document.completeURL(elem.getAttribute("href")).string().utf8();
      break;
    } else if (LowerCaseEqualsASCII(rel, "icon")) {                   // Google's way
      iconUrl = document.completeURL(elem.getAttribute("href")).string().utf8();
    }
  }
  Send(new EwkHostMsg_WebAppIconUrlGet(render_view()->GetRoutingID(), appleIconUrl.empty() ? iconUrl : appleIconUrl, callback_id));
}

void RenderViewObserverEfl::OnWebAppIconUrlsGet(int callback_id) {
  blink::WebFrame *frame = render_view()->GetWebView()->mainFrame();
  if (!frame) {
    return;
  }

  blink::WebDocument document = frame->document();
  blink::WebElement head = document.head();
  if (head.isNull()) {
    return;
  }

  std::map<std::string, std::string> iconUrls;
  blink::WebNodeList nodes = head.childNodes();
  for (int i = 0; i < nodes.length(); ++i) {
    blink::WebNode node = nodes.item(i);
    if (!node.isElementNode()) {
      continue;
    }
    blink::WebElement elem = node.to<blink::WebElement>();
    if (!elem.hasHTMLTagName("link")) {
      continue;
    }
    std::string rel = elem.getAttribute("rel").utf8();
    if (LowerCaseEqualsASCII(rel, "apple-touch-icon") ||              // Apple's way
        LowerCaseEqualsASCII(rel, "apple-touch-icon-precomposed") ||  // same here
        LowerCaseEqualsASCII(rel, "icon")) {                          // Google's way
      std::string iconSize = elem.getAttribute("sizes").utf8();
      std::string iconUrl = document.completeURL(elem.getAttribute("href")).string().utf8();

      iconUrls[iconSize] = iconUrl;
    }
  }
  Send(new EwkHostMsg_WebAppIconUrlsGet(render_view()->GetRoutingID(), iconUrls, callback_id));
}

void RenderViewObserverEfl::OnWebAppCapableGet(int callback_id) {
  blink::WebFrame *frame = render_view()->GetWebView()->mainFrame();
  if (!frame)
    return;

  blink::WebDocument document = frame->document();
  blink::WebElement head = document.head();
  if (head.isNull())
    return;

  bool capable = false;
  blink::WebNodeList nodes = head.childNodes();
  for (int i = 0; i < nodes.length(); ++i) {
    blink::WebNode node = nodes.item(i);
    if (!node.isElementNode())
      continue;

    blink::WebElement elem = node.to<blink::WebElement>();
    if (!elem.hasHTMLTagName("meta"))
      continue;

    std::string name = elem.getAttribute("name").utf8();
    if (LowerCaseEqualsASCII(name, "apple-mobile-web-app-capable") ||   // Apple's way
        LowerCaseEqualsASCII(name, "mobile-web-app-capable")) {         // Google's way
      std::string content = elem.getAttribute("content").utf8();
      if (LowerCaseEqualsASCII(content, "yes")) {
        capable = true;
      }
      break;
    }
  }
  Send(new EwkHostMsg_WebAppCapableGet(render_view()->GetRoutingID(), capable, callback_id));
}

void RenderViewObserverEfl::OrientationChangeEvent()
{
#if !defined(EWK_BRINGUP)
  // Previosly OrientationChangeEvent() had orientation parameter.
  // Now it was removed. Thanks Mounir.
  // Need to figure out how to get orientation.
  Send(new EwkHostMsg_OrientationChangeEvent(render_view()->GetRoutingID(), orientation));
#endif
}

void RenderViewObserverEfl::WillSubmitForm(blink::WebLocalFrame* frame, const blink::WebFormElement& form)
{
  GURL url(form.action());
  Send(new EwkHostMsg_FormSubmit(routing_id(),url));
}

void RenderViewObserverEfl::OnSetBrowserFont()
{
#if !defined(EWK_BRINGUP)
  blink::WebView* view = render_view()->GetWebView();
  if (view) {
    view->setBrowserFont();
  }
#endif
}

void RenderViewObserverEfl::OnSuspendScheduledTasks()
{
  blink::WebView* view = render_view()->GetWebView();
  if (view)
    view->suspendScheduledTasks();
}

void RenderViewObserverEfl::OnResumeScheduledTasks()
{
  blink::WebView* view = render_view()->GetWebView();
  if (view)
    view->resumeScheduledTasks();
}

void RenderViewObserverEfl::OnSetViewMode(blink::WebViewMode view_mode) {
  blink::WebView* view = render_view()->GetWebView();
  if (view)
    view->setViewMode(view_mode);
}

void RenderViewObserverEfl::OnUpdateSettings(const tizen_webview::Settings& settings)
{
  DCHECK(renderer_client_);
  if (renderer_client_) {
    static_cast<ContentRendererClientEfl*>(renderer_client_)->SetWebViewSettings(settings);
  }
}
