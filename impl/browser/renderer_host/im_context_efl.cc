// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "browser/renderer_host/im_context_efl.h"

#include "base/logging.h"
#include "base/strings/string16.h"
#include "base/strings/utf_string_conversions.h"
#include "content/browser/renderer_host/render_widget_host_impl.h"
#include "browser/renderer_host/render_widget_host_view_efl.h"

#include <Ecore_Evas.h>
#include <Ecore_IMF_Evas.h>
#include "wtf/unicode/icu/UnicodeIcu.h"
#include "third_party/icu/source/common/unicode/uchar.h"

#ifdef IM_CTX_DEBUG
#define IM_CTX_LOG_CHANNEL LOG(ERROR)
#else
#define IM_CTX_LOG_CHANNEL VLOG(1)
#endif

#define IM_CTX_LOG IM_CTX_LOG_CHANNEL << "## IMCTX ## " << __FUNCTION__ << " "

// FIXME: we do not handle text compositing correctly yet.
// Limit the functinality of this class to the handling of virtual keyboard for now.
#define USE_IM_COMPOSITING 1

namespace {

Ecore_IMF_Context* CreateIMFContext(Evas* evas) {
  IM_CTX_LOG;
  const char* default_context_id = ecore_imf_context_default_id_get();
  if (!default_context_id) {
    IM_CTX_LOG << "no default context id";
    return NULL;
  }
  Ecore_IMF_Context* context = ecore_imf_context_add(default_context_id);
  if (!context) {
    IM_CTX_LOG << "cannot create context";
    return NULL;
  }

  Ecore_Window window = ecore_evas_window_get(ecore_evas_ecore_evas_get(evas));
  ecore_imf_context_client_window_set(context, reinterpret_cast<void*>(window));
  ecore_imf_context_client_canvas_set(context, evas);

  return context;
}

} // namespace

namespace content {

// static
IMContextEfl* IMContextEfl::Create(RenderWidgetHostViewEfl* view) {
  Ecore_IMF_Context* context = CreateIMFContext(view->evas());
  if (!context)
    return NULL;

  return new IMContextEfl(view, context);
}

IMContextEfl::IMContextEfl(RenderWidgetHostViewEfl* view, Ecore_IMF_Context* context)
    : view_(view),
      context_(context),
      is_focused_(false),
      is_ime_panel_visible_(false),
      is_in_form_tag_(false),
      current_mode_(ui::TEXT_INPUT_MODE_DEFAULT),
      current_type_(ui::TEXT_INPUT_TYPE_NONE),
      can_compose_inline_(false),
      is_handling_keydown_(false),
      is_ime_ctx_reset_(false) {
  IM_CTX_LOG;
  InitializeIMFContext(context_);
}

void IMContextEfl::InitializeIMFContext(Ecore_IMF_Context* context) {
  ecore_imf_context_input_panel_enabled_set(context, false);
  ecore_imf_context_use_preedit_set(context, false);

  ecore_imf_context_event_callback_add(context, ECORE_IMF_CALLBACK_PREEDIT_CHANGED, &IMFPreeditChangedCallback, this);
  ecore_imf_context_event_callback_add(context, ECORE_IMF_CALLBACK_COMMIT, &IMFCommitCallback, this);
  ecore_imf_context_input_panel_event_callback_add(context, ECORE_IMF_INPUT_PANEL_STATE_EVENT, &IMFInputPanelStateChangedCallback, this);
  ecore_imf_context_input_panel_event_callback_add(context, ECORE_IMF_INPUT_PANEL_GEOMETRY_EVENT, &IMFInputPanelGeometryChangedCallback, this);
  ecore_imf_context_input_panel_event_callback_add(context, ECORE_IMF_CANDIDATE_PANEL_STATE_EVENT, &IMFCandidatePanelStateChangedCallback, this);
  ecore_imf_context_input_panel_event_callback_add(context, ECORE_IMF_CANDIDATE_PANEL_GEOMETRY_EVENT, &IMFCandidatePanelGeometryChangedCallback, this);
  ecore_imf_context_input_panel_event_callback_add(context, ECORE_IMF_INPUT_PANEL_LANGUAGE_EVENT, &IMFCandidatePanelLanguageChangedCallback, this);
  ecore_imf_context_event_callback_add(context, ECORE_IMF_CALLBACK_DELETE_SURROUNDING, &IMFDeleteSurroundingCallback, this);
  ecore_imf_context_retrieve_surrounding_callback_set(context, &IMFRetrieveSurroundingCallback, this);
}

IMContextEfl::~IMContextEfl() {
  ecore_imf_context_del(context_);
}

void IMContextEfl::HandleKeyDownEvent(const Evas_Event_Key_Down* event, bool* wasFiltered) {
#if USE_IM_COMPOSITING
  Ecore_IMF_Event im_event;
  is_handling_keydown_ = true;
  ecore_imf_evas_event_key_down_wrap(const_cast<Evas_Event_Key_Down*>(event), &im_event.key_down);
  *wasFiltered = ecore_imf_context_filter_event(context_, ECORE_IMF_EVENT_KEY_DOWN, &im_event);
  is_handling_keydown_ = false;
#endif
}

void IMContextEfl::HandleKeyUpEvent(const Evas_Event_Key_Up* event, bool* wasFiltered) {
#if USE_IM_COMPOSITING
  Ecore_IMF_Event im_event;
  ecore_imf_evas_event_key_up_wrap(const_cast<Evas_Event_Key_Up*>(event), &im_event.key_up);
  *wasFiltered = ecore_imf_context_filter_event(context_, ECORE_IMF_EVENT_KEY_UP, &im_event);
#endif
}

void IMContextEfl::UpdateInputMethodType(ui::TextInputType type,
                                         ui::TextInputMode mode,
                                         bool can_compose_inline) {
  IM_CTX_LOG;

  if (current_type_ == type && current_mode_ == mode &&
      can_compose_inline_ == can_compose_inline) {
    return;
  }

  Ecore_IMF_Input_Panel_Layout layout;
  Ecore_IMF_Input_Panel_Return_Key_Type return_key_type = ECORE_IMF_INPUT_PANEL_RETURN_KEY_TYPE_DEFAULT;
  Ecore_IMF_Autocapital_Type cap_type = ECORE_IMF_AUTOCAPITAL_TYPE_NONE;
  bool allow_prediction = true;

  if (is_in_form_tag_)
    return_key_type = ECORE_IMF_INPUT_PANEL_RETURN_KEY_TYPE_GO;

  switch (type) {
    case ui::TEXT_INPUT_TYPE_TEXT:
      layout = ECORE_IMF_INPUT_PANEL_LAYOUT_NORMAL;
      if (!is_in_form_tag_)
        return_key_type = ECORE_IMF_INPUT_PANEL_RETURN_KEY_TYPE_DONE;
      break;
    case ui::TEXT_INPUT_TYPE_PASSWORD:
      layout = ECORE_IMF_INPUT_PANEL_LAYOUT_PASSWORD;
      allow_prediction = false;
      break;
    case ui::TEXT_INPUT_TYPE_SEARCH:
      layout = ECORE_IMF_INPUT_PANEL_LAYOUT_NORMAL;
      return_key_type = ECORE_IMF_INPUT_PANEL_RETURN_KEY_TYPE_SEARCH;
      break;
    case ui::TEXT_INPUT_TYPE_EMAIL:
      layout = ECORE_IMF_INPUT_PANEL_LAYOUT_EMAIL;
      break;
    case ui::TEXT_INPUT_TYPE_NUMBER:
      layout = ECORE_IMF_INPUT_PANEL_LAYOUT_NUMBER;
      if (!is_in_form_tag_)
        return_key_type = ECORE_IMF_INPUT_PANEL_RETURN_KEY_TYPE_DONE;
      break;
    case ui::TEXT_INPUT_TYPE_TELEPHONE:
      layout = ECORE_IMF_INPUT_PANEL_LAYOUT_PHONENUMBER;
      break;
    case ui::TEXT_INPUT_TYPE_URL:
      layout = ECORE_IMF_INPUT_PANEL_LAYOUT_URL;
      break;
    case ui::TEXT_INPUT_TYPE_MONTH:
      layout = ECORE_IMF_INPUT_PANEL_LAYOUT_MONTH;
      break;
    case ui::TEXT_INPUT_TYPE_TEXT_AREA:
      layout = ECORE_IMF_INPUT_PANEL_LAYOUT_NORMAL;
      cap_type = ECORE_IMF_AUTOCAPITAL_TYPE_SENTENCE;
      break;

    // No direct mapping to Ecore_IMF API, use simple text layout.
    case ui::TEXT_INPUT_TYPE_DATE:
    case ui::TEXT_INPUT_TYPE_DATE_TIME:
    case ui::TEXT_INPUT_TYPE_DATE_TIME_LOCAL:
    case ui::TEXT_INPUT_TYPE_TIME:
    case ui::TEXT_INPUT_TYPE_WEEK:
    case ui::TEXT_INPUT_TYPE_CONTENT_EDITABLE:
    case ui::TEXT_INPUT_TYPE_DATE_TIME_FIELD:
      layout = ECORE_IMF_INPUT_PANEL_LAYOUT_NORMAL;
      break;

    default:
      NOTREACHED();
      return;
  }

  Ecore_IMF_Input_Mode imf_mode;
  switch (mode) {
    case ui::TEXT_INPUT_MODE_NUMERIC:
      imf_mode = ECORE_IMF_INPUT_MODE_NUMERIC;
    default:
      imf_mode = ECORE_IMF_INPUT_MODE_ALPHA;
  }

  ecore_imf_context_input_panel_layout_set(context_, layout);
  ecore_imf_context_input_mode_set(context_, imf_mode);
  ecore_imf_context_input_panel_return_key_type_set(context_, return_key_type);
  ecore_imf_context_autocapital_type_set(context_, cap_type);
  ecore_imf_context_prediction_allow_set(context_, allow_prediction);

  // If the focused element supports inline rendering of composition text,
  // we receive and send related events to it. Otherwise, the events related
  // to the updates of composition text are directed to the candidate window.
  ecore_imf_context_use_preedit_set(context_, can_compose_inline);

  current_type_ = type;
  current_mode_ = mode;
  can_compose_inline_ = can_compose_inline;
}

void IMContextEfl::UpdateInputMethodState(ui::TextInputType type,
                                          bool can_compose_inline,
                                          bool show_if_needed) {
  if (current_type_ != type || can_compose_inline_ != can_compose_inline) {
    UpdateInputMethodType(type, ui::TEXT_INPUT_MODE_DEFAULT, can_compose_inline);
  }

  IM_CTX_LOG;

  bool show = type != ui::TEXT_INPUT_TYPE_NONE;
  if (show && !is_ime_panel_visible_ && show_if_needed) {
    ShowPanel();
  } else if (!show && is_ime_panel_visible_) {
    HidePanel();
  }
}

void IMContextEfl::ShowPanel() {
  IM_CTX_LOG;
  ecore_imf_context_focus_in(context_);
  ecore_imf_context_input_panel_show(context_);
}

void IMContextEfl::HidePanel() {
  IM_CTX_LOG;
  ecore_imf_context_focus_out(context_);
  ecore_imf_context_input_panel_hide(context_);
}

void IMContextEfl::UpdateCaretBounds(const gfx::Rect& caret_bounds) {
  if (is_ime_panel_visible_) {
    int x = caret_bounds.x();
    int y = caret_bounds.y();
    int w = caret_bounds.width();
    int h = caret_bounds.height();
    ecore_imf_context_cursor_location_set(context_, x, y, w, h);
  }
}

void IMContextEfl::OnFocusIn() {
  CancelComposition();

  if (is_focused_)
    return;

  IM_CTX_LOG;
  is_focused_ = true;

  ecore_imf_context_focus_in(context_);

  // Enables RenderWidget's IME related events, so that we can be notified
  // when WebKit wants to enable or disable IME.
  RenderWidgetHostImpl *rwhi = GetRenderWidgetHostImpl();
  if (rwhi)
    rwhi->SetInputMethodActive(true);
}

void IMContextEfl::OnFocusOut() {
  if (!is_focused_)
    return;

  IM_CTX_LOG;
  is_focused_ = false;

  CancelComposition();

  ecore_imf_context_focus_out(context_);

  // Disable RenderWidget's IME related events to save bandwidth.
  RenderWidgetHostImpl *rwhi = GetRenderWidgetHostImpl();
  if (rwhi)
    rwhi->SetInputMethodActive(false);
}

void IMContextEfl::ResetIMFContext() {
  is_ime_ctx_reset_ = true;
  ecore_imf_context_reset(context_);
  is_ime_ctx_reset_ = false;
}

void IMContextEfl::CancelComposition() {
  IM_CTX_LOG;
  ClearQueues();
  view_->ClearQueues();

  ResetIMFContext();
  if (composition_.text.length() > 0) {
    base::string16 empty;
    view_->ConfirmComposition(empty);
    composition_.Clear();
  }
}

void IMContextEfl::ConfirmComposition() {
  // Gtk use it to send the empty string as committed.
  // I'm not sure we need it (kbalazs).
}

void IMContextEfl::SetIsInFormTag(bool is_in_form_tag) {
  is_in_form_tag_ = is_in_form_tag;
  if (!context_ || !is_ime_panel_visible_)
    return;

  if (ecore_imf_context_input_panel_return_key_type_get(context_) ==
      ECORE_IMF_INPUT_PANEL_RETURN_KEY_TYPE_SEARCH)
    return;

  if (is_in_form_tag_)
    ecore_imf_context_input_panel_return_key_type_set(context_,
    ECORE_IMF_INPUT_PANEL_RETURN_KEY_TYPE_GO);
  else {
    if (current_type_ == ui::TEXT_INPUT_TYPE_TEXT ||
        current_type_ == ui::TEXT_INPUT_TYPE_NUMBER)
      ecore_imf_context_input_panel_return_key_type_set(context_,
      ECORE_IMF_INPUT_PANEL_RETURN_KEY_TYPE_DONE);
    else
      ecore_imf_context_input_panel_return_key_type_set(context_,
      ECORE_IMF_INPUT_PANEL_RETURN_KEY_TYPE_DEFAULT);
  }
}

void IMContextEfl::OnCommit(void* event_info) {
  if (is_ime_ctx_reset_) {
    return;
  }

#if USE_IM_COMPOSITING
  IM_CTX_LOG;
  composition_.Clear();

  char* text = static_cast<char*>(event_info);

  base::string16 text16 = base::UTF8ToUTF16(text);
  // Only add commit to queue, till we dont know if key event
  // should be handled. It can be default prevented for exactly.
  commit_queue_.push(text16);

  // sending fake key event if hardware key is not handled as it is
  // in Webkit.
  SendFakeCompositionKeyEvent(text16);
#endif
}

void IMContextEfl::SendFakeCompositionKeyEvent(const base::string16& buf) {
  if (is_handling_keydown_)
    return;

  if (!buf.length())
    return;

  std::string str = base::UTF16ToUTF8(buf.substr(buf.length() -1));

  Evas_Event_Key_Down downEvent;
  memset(&downEvent, 0, sizeof(Evas_Event_Key_Down));
  downEvent.key = str.c_str();
  downEvent.string = str.c_str();

  NativeWebKeyboardEvent n_event = WebEventFactoryEfl::toWebKeyboardEvent(view_->evas(), &downEvent);
  n_event.type = blink::WebInputEvent::KeyDown;

  n_event.isSystemKey = true;

  if (!view_->GetRenderWidgetHost())
    return;

  view_->KeyUpEventQueuePush(n_event.windowsKeyCode);
  view_->GetRenderWidgetHost()->ForwardKeyboardEvent(n_event);
}

void IMContextEfl::OnPreeditChanged(void* data, Ecore_IMF_Context* context, void* event_info) {
  if (is_ime_ctx_reset_)
    return;

#if USE_IM_COMPOSITING
  composition_.Clear();

  char* buffer = 0;

  ecore_imf_context_preedit_string_get(context, &buffer, 0);
  if (!buffer)
      return;

  // Add empty commit to stop processing commits.
  commit_queue_.push(base::string16());

  base::string16 text16 = base::UTF8ToUTF16(buffer);
  if (!text16.empty())
    SendFakeCompositionKeyEvent(text16);
  composition_.Clear();
  composition_.text = text16;

  composition_.underlines.push_back(ui::CompositionUnderline(0, composition_.text.length(), SK_ColorBLACK, false));
  composition_.selection = gfx::Range(composition_.text.length());

  // Only add preedit to queue, till we dont know if key event
  // should be handled. It can be default prevented for exactly.
  if (!text16.empty())
    preedit_queue_.push(composition_);
  else {
    view_->SetComposition(composition_);
    ResetIMFContext();
  }

  free(buffer);

#endif
}

// TODO(kbalazs): figure out what do we need from these callbacks.
// Tizen-WebKit-efl using all of them.

void IMContextEfl::OnInputPanelStateChanged(int state) {
  if (state == ECORE_IMF_INPUT_PANEL_STATE_SHOW) {
    IM_CTX_LOG << ": show";
    is_ime_panel_visible_ = true;
    view_->eweb_view()->SmartCallback<EWebViewCallbacks::IMEInputPanelShow>().call();
  } else if (state == ECORE_IMF_INPUT_PANEL_STATE_HIDE) {
    IM_CTX_LOG << ": hide";
    is_ime_panel_visible_ = false;
    view_->eweb_view()->SmartCallback<EWebViewCallbacks::IMEInputPanelHide>().call();
  }

  // ignore ECORE_IMF_INPUT_PANEL_WILL_SHOW value
}

void IMContextEfl::OnInputPanelGeometryChanged() {
  Eina_Rectangle rect;
  ecore_imf_context_input_panel_geometry_get(context_, &rect.x, &rect.y, &rect.w, &rect.h);
  view_->eweb_view()->SmartCallback<EWebViewCallbacks::IMEInputMethodChanged>().call(&rect);

  SetIMERect(gfx::Rect(rect.x, rect.y, rect.w, rect.h));
}

void IMContextEfl::OnCandidateInputPanelStateChanged(int state) {
  if (state == ECORE_IMF_CANDIDATE_PANEL_SHOW)
    view_->eweb_view()->SmartCallback<EWebViewCallbacks::IMECandidatePanelShow>().call();
  else
    view_->eweb_view()->SmartCallback<EWebViewCallbacks::IMECandidatePanelHide>().call();
}

void IMContextEfl::OnCandidateInputPanelGeometryChanged() {
}

bool IMContextEfl::OnRetrieveSurrounding(char** text, int* offset) {
  return false;
}

void IMContextEfl::OnDeleteSurrounding(void* event_info) {
}

void IMContextEfl::OnCandidateInputPanelLanguageChanged(Ecore_IMF_Context*, int) {
  RenderWidgetHostImpl *rwhi = GetRenderWidgetHostImpl();

  if (!rwhi || composition_.text.length() == 0)
    return;

  CancelComposition();
}

bool IMContextEfl::IsShow() {
  return context_ && is_focused_ && is_ime_panel_visible_;
}

void IMContextEfl::ClearQueues() {
  while (!commit_queue_.empty()) {
    commit_queue_.pop();
  }

  while (!preedit_queue_.empty()) {
    preedit_queue_.pop();
  }
}

void IMContextEfl::IMFCommitCallback(
    void* data, Ecore_IMF_Context*, void* event_info) {
  static_cast<IMContextEfl*>(data)->OnCommit(event_info);
}

void IMContextEfl::IMFPreeditChangedCallback(
    void* data, Ecore_IMF_Context* context, void* event_info) {
  static_cast<IMContextEfl*>(data)->
      OnPreeditChanged(data, context, event_info);
}

void IMContextEfl::IMFInputPanelStateChangedCallback(
    void* data, Ecore_IMF_Context*, int state) {
  static_cast<IMContextEfl*>(data)->OnInputPanelStateChanged(state);
}

void IMContextEfl::IMFInputPanelGeometryChangedCallback(
    void* data, Ecore_IMF_Context*, int state) {
  static_cast<IMContextEfl*>(data)->OnInputPanelGeometryChanged();
}

void IMContextEfl::IMFCandidatePanelStateChangedCallback(
    void* data, Ecore_IMF_Context*, int state) {
  static_cast<IMContextEfl*>(data)->OnCandidateInputPanelStateChanged(state);
}

void IMContextEfl::IMFCandidatePanelGeometryChangedCallback(
    void* data, Ecore_IMF_Context*, int state) {
  static_cast<IMContextEfl*>(data)->OnCandidateInputPanelGeometryChanged();
}

Eina_Bool IMContextEfl::IMFRetrieveSurroundingCallback(
    void* data, Ecore_IMF_Context*, char** text, int* offset) {
  return static_cast<IMContextEfl*>(data)->OnRetrieveSurrounding(text, offset);
}

void IMContextEfl::IMFDeleteSurroundingCallback(
    void* data, Ecore_IMF_Context*, void* event_info) {
  static_cast<IMContextEfl*>(data)->OnDeleteSurrounding(event_info);
}

void IMContextEfl::IMFCandidatePanelLanguageChangedCallback(
    void* data, Ecore_IMF_Context* context, int value) {
  static_cast<IMContextEfl*>(data)->
      OnCandidateInputPanelLanguageChanged(context, value);
}

RenderWidgetHostImpl* IMContextEfl::GetRenderWidgetHostImpl() const {
  RenderWidgetHost *rwh = view_->GetRenderWidgetHost();
  if (!rwh)
    return NULL;
  return RenderWidgetHostImpl::From(rwh);
}

} // namespace content
